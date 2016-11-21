// #include "predictor.h"
#include <math.h>
#include <stdio.h>

# define ST 0
# define WT 1
# define WNT 2
# define SNT 3


/*
Budget sizes

8K   + 64  -> k=6,  s=6,  t=11
16K  + 128 -> k=7,  s=6,  t=12
32K  + 256 -> k=8,  s=6,  t=13
64K  + 512 -> k=9,  s=6,  t=14
128K + 1K  -> k=10, s=6,  t=15
1M   + 4K  -> k=12, s=7,  t=18
*/

# define k 10   // bits for hashing local branches
# define s 3    // bits for history
# define t 12   // bits for global/choice history

/*
Stores local history table

Maximum size
k=12 -> 4096  = 2^12
s=8  -> 256   = 2^8
 */
unsigned int local_pattern_table[4096][4096];

/*
Histories for each branch

Maximum size
t=18 -> 2^12
*/
unsigned int local_histories[4096];

/*
2-bit saturating counters for global history

Maximum size
t=18 -> 2^18
*/
unsigned int global_pattern_table[262144];

/*
2-bit saturating counters for choice pattern_table

Maximum size
t=18 -> 2^18
*/
unsigned int choice_pattern_table[262144];

/*
path history
*/
unsigned int path_history;

// Helper functions
void local_step(int i, int j, bool outcome){
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

  // Strongly taken
  if(local_pattern_table[i][j] == ST and outcome == true){
    local_pattern_table[i][j] = ST;
  } else if(local_pattern_table[i][j] == ST and outcome == false){
    local_pattern_table[i][j] = WT;
  }
  // Weakly taken
  else if(local_pattern_table[i][j] == WT and outcome == true){
    local_pattern_table[i][j] = ST;
  } else if(local_pattern_table[i][j] == WT and outcome == false){
    local_pattern_table[i][j] = WNT;
  }
  // Weakly not taken
  else if(local_pattern_table[i][j] == WNT and outcome == true){
    local_pattern_table[i][j] = WT;
  } else if(local_pattern_table[i][j] == WNT and outcome == false){
    local_pattern_table[i][j] = SNT;
  }
  // Strongly not taken
  else if(local_pattern_table[i][j] == SNT and outcome == true){
    local_pattern_table[i][j] = WNT;
  } else if(local_pattern_table[i][j] == SNT and outcome == false){
    local_pattern_table[i][j] = SNT;
  }
  // Update local histories table
  local_histories[i] =  (local_histories[i] << 1) | outcome;
}

void global_step(int i, bool outcome){
  /*
    Updates global pattern table.

    Parameters
    ----------
    i : the hash of the pattern corresponding to the last t occurences
   */
  // Strongly taken
  // if(global_pattern_table[i] == ST and outcome == true){
  //   global_pattern_table[i] = ST;
  // } else if(global_pattern_table[i] == ST and outcome == false){
  //   global_pattern_table[i] = WT;
  // }
  // // Weakly taken
  // else if(global_pattern_table[i] == WT and outcome == true){
  //   global_pattern_table[i] = ST;
  // } else if(global_pattern_table[i] == WT and outcome == false){
  //   global_pattern_table[i] = WNT;
  // }
  // // Weakly not taken
  // else if(global_pattern_table[i] == WNT and outcome == true){
  //   global_pattern_table[i] = WT;
  // } else if(global_pattern_table[i] == WNT and outcome == false){
  //   global_pattern_table[i] = SNT;
  // }
  // // Strongly not taken
  // else if(global_pattern_table[i] == SNT and outcome == true){
  //   global_pattern_table[i] = WNT;
  // } else if(global_pattern_table[i] == SNT and outcome == false){
  //   global_pattern_table[i] = SNT;
  // }

  if (global_pattern_table[i] < 2 and (outcome == false)){
    global_pattern_table[i] = MIN(3, global_pattern_table[i]+1);
  } else if (global_pattern_table[i] < 2 and (outcome == true)){
    global_pattern_table[i] = MAX(3, global_pattern_table[i]-1);
  } else if (global_pattern_table[i] > 2 and (outcome != false)){
    global_pattern_table[i] = MAX(0, global_pattern_table[i]+1);
  } else if (global_pattern_table[i] > 2 and (outcome == true)){
    global_pattern_table[i] = MIN(3, global_pattern_table[i]-1);
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

  // // Strongly taken (local)
  // if(choice_pattern_table[i] == ST and
  //    ((local_outcome == true and outcome == false) or
  //     (local_outcome == false and outcome == true))){
  //   choice_pattern_table[i] = WT;
  // }
  // // Weakly taken (local)
  // else if(choice_pattern_table[i] == WT and
  // 	  ((local_outcome == false and outcome == true) or
  // 	   (local_outcome == true and outcome == false))){
  //   choice_pattern_table[i] = WNT;
  // }
  // else if(choice_pattern_table[i] == WT and
  // 	  ((local_outcome == true and outcome == true) or
  // 	   (local_outcome == false and outcome == false))){
  //   choice_pattern_table[i] = ST;
  // }
  // // Weakly not taken (global)
  // else if(choice_pattern_table[i] == WNT and
  // 	  ((global_outcome == false and outcome == true) or
  // 	   (global_outcome == true and outcome == false))){
  //   choice_pattern_table[i] = WT;
  // }
  // else if(choice_pattern_table[i] == WNT and
  // 	  ((global_outcome == false and outcome == false) or
  // 	   (global_outcome == true and outcome == true))){
  //   choice_pattern_table[i] = SNT;
  // }
  // // Strongly not taken (global)
  // else if(choice_pattern_table[i] == SNT and
  // 	  ((global_outcome == true and outcome == false) or
  // 	   (global_outcome == false and outcome == true))){
  //   choice_pattern_table[i] = WNT;
  // }

  if (choice_pattern_table[i] < 2 and (outcome!=local_outcome)){
    choice_pattern_table[i] = MIN(3, choice_pattern_table[i]+1);
  } else if (choice_pattern_table[i] < 2 and (outcome==local_outcome)){
    choice_pattern_table[i] = MAX(3, choice_pattern_table[i]-1);
  } else if (choice_pattern_table[i] > 2 and (outcome!=global_outcome)){
    choice_pattern_table[i] = MIN(0, choice_pattern_table[i]-1);
  } else if (choice_pattern_table[i] > 2 and (outcome==global_outcome)){
    choice_pattern_table[i] = MAX(0, choice_pattern_table[i]+1);
  }

}


// actual code
void init_predictor ()
{
  // initialize all 2bit saturating counts to Strongly Taken
  for (int i = 0; i<pow(2, k); i++){
    for (int j = 0; j<pow(2, s); j++){
      local_pattern_table[i][j] = 0;
    }
    // initialize all histories to zero.
    local_histories[i] = 0;
  }
  for (int i = 0; i<pow(2, t); i++){
    global_pattern_table[i] = 0;
    choice_pattern_table[i] = 0;
  }
  path_history = 0;
}


bool make_prediction (unsigned int pc)
{
  //printf ("Predicting\n");
  int branch = (pc % k);  // branch hash
  int history_idx = local_histories[branch] % s;
  int local_taken = local_pattern_table[branch][history_idx];
  int path_idx = path_history % t;
  int global_taken = global_pattern_table[path_idx];
  int choice_taken = choice_pattern_table[path_idx];

  int taken = 0;
  if(choice_taken == ST or choice_taken == WT){
    taken = local_taken;
  }else{
    taken = global_taken;
  }

  taken = global_taken;

  if(taken == ST or taken == WT)
    return true;
  else
    return false;
}


void train_predictor (unsigned int pc, bool outcome)
{
  int branch = (pc % k);  // branch hash
  int history_idx  = local_histories[branch] % s;
  int path_idx = path_history % t;
  int local_taken  = local_pattern_table[branch][history_idx];
  int global_taken = global_pattern_table[path_idx];
  int choice_taken = choice_pattern_table[path_idx];

  choice_step(path_idx, global_taken, local_taken, outcome);
  global_step(path_idx, outcome);
  local_step(branch, history_idx, outcome);


  // Update overall path history
  path_history =  (path_history << 1) | outcome;
}
