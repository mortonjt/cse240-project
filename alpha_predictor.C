// #include "predictor.h"
#include <math.h>
#include <stdio.h>

# define ST 0
# define WT 1
# define WNT 2
# define SNT 3

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/*
Budget sizes

8K   + 64  -> k=9,  s=7,  t=10
16K  + 128 -> k=10, s=7,  t=11
32K  + 256 -> k=,  s=,  t=
64K  + 512 -> k=,  s=,  t=
128K + 1K  -> k=, s=,  t=
1M   + 4K  -> k=12, s=9,  t=14
*/

# define k 12   // bits for hashing local branches
# define s 9    // bits for history
# define t 14   // bits for global/choice history

/*
Stores local history table

Maximum size: 2^20
 */
unsigned int local_pattern_table[1048576];

/*
Histories for each branch

Maximum size: 2^20
*/
unsigned int local_histories[1048576];

/*
2-bit saturating counters for global history

Maximum size: 2^20
*/
unsigned int global_pattern_table[1048576];

/*
2-bit saturating counters for choice pattern_table

Maximum size: 2^20
*/
unsigned int choice_pattern_table[1048576];

/*
path history
*/
unsigned int path_history;


// mask for k bits
unsigned int kmask;
// mask for s bits
unsigned int smask;
// mask for t bits
unsigned int tmask;


// Helper functions
void local_step(int i, bool outcome){
  /*
    This method will update the 2bit saturating counter at branch i
    and pattern_table j.

    Parameters
    ----------
    i : row in the global pattern_table table.
	i.e. the branch that is being inspected
    j : column in the global pattern_table table.
	i.e. the pattern_table that is being inspected for branch i.
    outcome : outcome of specific branch at that given time
   */
  int state = local_pattern_table[i];
  if (state < 2 and outcome){
    local_pattern_table[i] = ST;
  } else if (state >= 2 and not outcome){
    local_pattern_table[i] = SNT;
  } else if (state >= 2 and outcome){
    local_pattern_table[i] = MAX(0, state-1);
  } else if (state < 2 and not outcome){
    local_pattern_table[i] = MIN(3, state+1);
  } else{
    printf("warning\n");
  }
}


void global_step(int i, bool outcome){
  /*
    Updates global pattern table.

    Parameters
    ----------
    i : the hash of the pattern corresponding to the last t occurences
   */
  int state = global_pattern_table[i];
  if (state < 2 and outcome){
    global_pattern_table[i] = ST;
  } else if (state >= 2 and not outcome){
    global_pattern_table[i] = SNT;
  } else if (state >= 2 and outcome){
    global_pattern_table[i] = MAX(0, state-1);
  } else if (state < 2 and not outcome){
    global_pattern_table[i] = MIN(3, state+1);
  } else{
    printf("warning\n");
  }
}


void choice_step(int i,
		 bool global_outcome,
		 bool local_outcome,
		 bool outcome){
  /*
    Updates global pattern table.

    Parameters
    ----------
    i : the hash of the pattern corresponding to the last t occurences
   */
  int state = global_pattern_table[i];

  if (state < 2 and outcome==local_outcome){
    choice_pattern_table[i] = ST;
  } else if (state >= 2 and outcome==global_outcome){
    choice_pattern_table[i] = SNT;
  } else if (state >= 2 and outcome!=global_outcome){
    choice_pattern_table[i] = MAX(0, state-1);
  } else if (state < 2 and outcome!=local_outcome){
    choice_pattern_table[i] = MIN(3, state+1);
  } else{
    printf("state:%d, outcome:%d, local_outcome:%d, global_outcome%d\n",
	   state, outcome, local_outcome, global_outcome);
  }


  // if (choice_pattern_table[i] < 2 and (outcome!=local_outcome)){
  //   choice_pattern_table[i] = MIN(3, choice_pattern_table[i]+1);
  // } else if (choice_pattern_table[i] < 2 and (outcome==local_outcome)){
  //   choice_pattern_table[i] = MAX(3, choice_pattern_table[i]-1);
  // } else if (choice_pattern_table[i] > 2 and (outcome!=global_outcome)){
  //   choice_pattern_table[i] = MIN(0, choice_pattern_table[i]-1);
  // } else if (choice_pattern_table[i] > 2 and (outcome==global_outcome)){
  //   choice_pattern_table[i] = MAX(0, choice_pattern_table[i]+1);
  // }
}


// actual code
void init_predictor ()
{
  // initialize all 2bit saturating counts to Strongly Taken
  for (int i = 0; i<pow(2, k); i++){
    local_pattern_table[i] = 0;
  }
  for (int j = 0; j<pow(2, s); j++){
    // initialize all histories to zero.
    local_histories[j] = 0;
  }
  for (int i = 0; i<pow(2, t); i++){
    global_pattern_table[i] = 0;
    choice_pattern_table[i] = 0;
  }
  for (int i = 0; i < k; i++){
    kmask = (kmask << 1) + 1;
  }
  for (int j = 0; j < s; j++){
    smask = (smask << 1) + 1;
  }
  for (int j = 0; j < t; j++){
    tmask = (tmask << 1) + 1;
  }

  path_history = 0;
}


bool make_prediction (unsigned int pc)
{
  int branch = (pc & kmask);  // branch hash
  int history_idx  = local_histories[branch] & smask;
  int path_idx = path_history & tmask;
  int local_taken = local_pattern_table[history_idx];

  int global_taken = global_pattern_table[path_idx];
  int choice_taken = choice_pattern_table[path_idx];

  int taken = 0;
  if(choice_taken == ST or choice_taken == WT){
    taken = local_taken;
  }else{
    taken = global_taken;
  }
  if(taken == ST or taken == WT)
    return true;
  else
    return false;
}


void train_predictor (unsigned int pc, bool outcome)
{
  int branch = (pc & kmask);  // branch hash
  int history_idx  = local_histories[branch] & smask;
  int path_idx = path_history & tmask;
  int local_taken  = local_pattern_table[history_idx];
  int global_taken = global_pattern_table[path_idx];
  int choice_taken = choice_pattern_table[path_idx];
  //printf ("branch:%d, path_idx:%d, history:%d, outcome:%d\n", branch,  path_idx, history_idx, outcome);
  choice_step(path_idx, global_taken, local_taken, outcome);
  global_step(path_idx, outcome);
  local_step(history_idx, outcome);

  local_histories[branch] =  ((local_histories[branch] << 1) + outcome);
  // Update overall path history
  path_history =  (path_history << 1) + outcome;
}
