// #include "predictor.h"
#include <math.h>
#include <stdio.h>

# define ST 0
# define WT 1
# define WNT 2
# define SNT 3


/*
Budget sizes

8K   + 64  -> k=6,  s=7
16K  + 128 -> k=7,  s=7
32K  + 256 -> k=8,  s=7
64K  + 512 -> k=9,  s=7
128K + 1K  -> k=10, s=7
1M   + 4K  -> k=12, s=8
*/

# define k 12 // number of bits for hashing branches
# define s 8 // 4096=2^12.  12 bits for history


/*
Stores global history table

Maximum size
k=12 -> 4096 = 2^12
s=8  -> 256  = 2^8
 */
unsigned int pattern_table[4096][256];

/*
Histories for each branch

Maximum size
k=12 -> 4096 = 2^12
*/
unsigned int histories[4096];
//int histories[64];

// Helper functions
void step(int i, int j, bool outcome){
  /*
    This method will update the 2bit saturating counter at branch i
    and history j.

    Parameters
    ----------
    i : row in the global history table.
	i.e. the branch that is being inspected
    j : column in the global history table.
	i.e. the history that is being inspected for branch i.
    outcome : outcome of specific branch at that given time
   */

  // Strongly taken
  if(pattern_table[i][j] == ST and outcome == true){
    pattern_table[i][j] = ST;
  } else if(pattern_table[i][j] == ST and outcome == false){
    pattern_table[i][j] = WT;
  }
  // Weakly taken
  else if(pattern_table[i][j] == WT and outcome == true){
    pattern_table[i][j] = ST;
  } else if(pattern_table[i][j] == WT and outcome == false){
    pattern_table[i][j] = WNT;
  }
  // Weakly not taken
  else if(pattern_table[i][j] == WNT and outcome == true){
    pattern_table[i][j] = WT;
  } else if(pattern_table[i][j] == WNT and outcome == false){
    pattern_table[i][j] = SNT;
  }
  // Strongly not taken
  else if(pattern_table[i][j] == SNT and outcome == true){
    pattern_table[i][j] = WNT;
  } else if(pattern_table[i][j] == SNT and outcome == false){
    pattern_table[i][j] = SNT;
  }

  // Update history
  histories[i] =  (histories[i] << 1) | outcome;
}


void init_predictor ()
{
  // initialize all 2bit saturating counts to Strongly Taken
  for (int i = 0; i<pow(2, k); i++){
    for (int j = 0; j<pow(2, s); j++){
      pattern_table[i][j] = 0;
    }
    // initialize all histories to zero.
    histories[i] = 0;
  }
}


bool make_prediction (unsigned int pc)
{

  int branch = (pc % k);  // branch hash
  int history_idx = histories[branch] % s;
  int taken = pattern_table[branch][history_idx];
  if(taken == ST or taken == WT)
    return true;
  else
    return false;
}


void train_predictor (unsigned int pc, bool outcome)
{

  int branch = (pc % k);  // branch hash
  int history_idx = histories[branch] % s;
  step(branch, history_idx, outcome);

}
