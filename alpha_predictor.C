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

# define k 12   // bits for hashing local branches
# define s 6    // bits for history
# define t 18   // bits for global/choice history

/*
Stores local history table

Maximum size
k=12 -> 4096  = 2^12
s=8  -> 256   = 2^8
t=14 -> 16384 = 2^14
 */
unsigned int local_pattern_table[4096][256];

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

  // Update pattern_table
  local_histories[i] =  (local_histories[i] << 1) | outcome;
}

void global_step(int i, bool outcome){
  // Strongly taken
  if(global_pattern_table[i] == ST and outcome == true){
    global_pattern_table[i] = ST;
  } else if(global_pattern_table[i] == ST and outcome == false){
    global_pattern_table[i] = WT;
  }
  // Weakly taken
  else if(global_pattern_table[i] == WT and outcome == true){
    global_pattern_table[i] = ST;
  } else if(global_pattern_table[i] == WT and outcome == false){
    global_pattern_table[i] = WNT;
  }
  // Weakly not taken
  else if(global_pattern_table[i] == WNT and outcome == true){
    global_pattern_table[i] = WT;
  } else if(global_pattern_table[i] == WNT and outcome == false){
    global_pattern_table[i] = SNT;
  }
  // Strongly not taken
  else if(global_pattern_table[i] == SNT and outcome == true){
    global_pattern_table[i] = WNT;
  } else if(global_pattern_table[i] == SNT and outcome == false){
    global_pattern_table[i] = SNT;
  }
}


void choice_step(int i, bool outcome){
  // Strongly taken
  if(choice_pattern_table[i] == ST and outcome == true){
    choice_pattern_table[i] = ST;
  } else if(choice_pattern_table[i] == ST and outcome == false){
    choice_pattern_table[i] = WT;
  }
  // Weakly taken
  else if(choice_pattern_table[i] == WT and outcome == true){
    choice_pattern_table[i] = ST;
  } else if(choice_pattern_table[i] == WT and outcome == false){
    choice_pattern_table[i] = WNT;
  }
  // Weakly not taken
  else if(choice_pattern_table[i] == WNT and outcome == true){
    choice_pattern_table[i] = WT;
  } else if(choice_pattern_table[i] == WNT and outcome == false){
    choice_pattern_table[i] = SNT;
  }
  // Strongly not taken
  else if(choice_pattern_table[i] == SNT and outcome == true){
    choice_pattern_table[i] = WNT;
  } else if(choice_pattern_table[i] == SNT and outcome == false){
    choice_pattern_table[i] = SNT;
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
    global_pattern_table[i] = 0;
    choice_pattern_table[i] = 0;
  }
}


bool make_prediction (unsigned int pc)
{

  int branch = (pc % k);  // branch hash
  int global_idx = (pc % t);  // global pc hash
  int history_idx = local_histories[branch] % s;
  int local_taken = local_pattern_table[branch][history_idx];
  int global_taken = global_pattern_table[global_idx];
  int choice_taken = choice_pattern_table[global_idx];

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

  int branch = (pc % k);  // branch hash
  int local_history_idx = local_histories[branch] % s;
  int global_idx = (pc % t);  // global pc hash
  local_step(branch, local_history_idx, outcome);
  global_step(global_idx, outcome);
  choice_step(global_idx, outcome);

}
