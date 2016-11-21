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

8K   + 64  -> k=6,  s=7
16K  + 128 -> k=7,  s=7
32K  + 256 -> k=8,  s=7
64K  + 512 -> k=9,  s=7
128K + 1K  -> k=10, s=7
1M   + 4K  -> k=12, s=8
*/

# define k 8 // number of bits for hashing branches
# define s 12 // 4096=2^12.  12 bits for history


/*
Stores global history table

Maximum size
k=12 -> 4096 = 2^12
s=8  -> 256  = 2^8
 */
unsigned int pattern_table[4096][4096];

/*
Histories for each branch

Maximum size
k=12 -> 4096 = 2^12
*/
unsigned int histories[4096];
unsigned int history;

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
  int i = histories[branch];
  if(pattern_table[branch][i] < 2)
    return true;
  else
    return false;
}


void train_predictor (unsigned int pc, bool outcome)
{

  int branch = (pc % k);  // branch hash
  unsigned int i = histories[branch];
  if (pattern_table[branch][i] < 2 and outcome){
    pattern_table[branch][i] = ST;
  }else if (pattern_table[branch][i] > 2 and not outcome){
    pattern_table[branch][i] = SNT;
  }else if (outcome){
    pattern_table[branch][i] = MAX(0, pattern_table[branch][i]-1);
  }else{
    pattern_table[branch][i] = MIN(3, pattern_table[branch][i]+1);
  }

  histories[i] =  ((histories[i] << 1) | outcome) % s;
}