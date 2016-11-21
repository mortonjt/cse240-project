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

# define k 10 // number of bits for hashing branches
# define s 19 // 4096=2^12.  12 bits for history


/*
Stores global history table

Maximum size
k=12 -> 4096 = 2^12
s=8  -> 256  = 2^8
 */
unsigned int pattern_table[1048576];

/*
Histories for each branch

Maximum size
k=12 -> 4096 = 2^12
*/
unsigned int histories[1048576];


void init_predictor ()
{
  printf("init");
  // initialize all 2bit saturating counts to Strongly Taken
  for (int i = 0; i<pow(2, k); i++){
    // initialize all histories to zero.
    histories[i] = 1;
  }
  for (int j = 0; j<pow(2, s); j++){
    pattern_table[j] = 3;
  }
}


bool make_prediction (unsigned int pc)
{
  //printf("predict\n");
  unsigned int km = pow(2, k);
  unsigned int sm = pow(2, s);
  //int km = 2;
  unsigned int branch = (pc % km);  // branch hash
  //printf ("branch %d\n", branch);
  unsigned int i = histories[branch] % sm;
  //printf ("histories %d\n", i);
  if(pattern_table[i] < 2)
    return true;
  else
    return false;
}


void train_predictor (unsigned int pc, bool outcome)
{
  //printf("train\n");
  //int km = 2;
  //int sm = 2;
  unsigned int km = pow(2, k);
  unsigned int sm = pow(2, s);
  unsigned int branch = (pc % km);  // branch hash
  unsigned int i = histories[branch] % sm;
  unsigned int state = pattern_table[i];
  if (state < 2 and outcome){
    pattern_table[i] = ST;
  } else if (state >= 2 and not outcome){
    pattern_table[i] = SNT;
  } else if (state >= 2 and outcome){
    pattern_table[i] = MAX(0, state-1);
  } else if (state < 2 and not outcome){
    pattern_table[i] = MIN(3, state+1);
  } else{
    printf("warning\n");
  }

  histories[i] =  ((histories[i] << 1) | outcome) % sm;
}
