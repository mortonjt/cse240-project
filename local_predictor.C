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

# define k 15 // number of bits for hashing branches
# define s 5 // 4096=2^12.  12 bits for history


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

unsigned int kmask;
unsigned int smask;

void init_predictor ()
{
  printf("init");

  kmask = 0;
  smask = 0;
  // initialize all 2bit saturating counts to Strongly Taken
  for (int i = 0; i<pow(2, k); i++){
    // initialize all histories to zero.
    histories[i] = 0;
  }
  for (int i = 0; i < k; i++){
    kmask = (kmask << 1) + 1;
  }

  for (int j = 0; j<pow(2, s); j++){
    pattern_table[j] = 0;
  }
  for (int j = 0; j < s; j++){
    smask = (smask << 1) + 1;
  }
}


bool make_prediction (unsigned int pc)
{
  //printf("predict\n");
  //unsigned int km = pow(2, k);
  //unsigned int sm = pow(2, s);
  //int km = 2;
  unsigned int branch = (pc & kmask);  // branch hash
  //printf ("branch %d\n", branch);
  unsigned int i = histories[branch] & smask;
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
  //unsigned int km = pow(2, k);
  //unsigned int sm = pow(2, s);
  unsigned int branch = (pc & kmask);  // branch hash
  unsigned int i = histories[branch] & smask;
  unsigned int state = pattern_table[i];

  //printf("branch idx:%d, pattern idx: %d, state: %d\n", branch, i, state);
  if(pattern_table[i] == ST and outcome == true){
    pattern_table[i] = ST;
  } else if(pattern_table[i] == ST and outcome == false){
    pattern_table[i] = WT;
  }
  // Weakly taken
  else if(pattern_table[i] == WT and outcome == true){
    pattern_table[i] = ST;
  } else if(pattern_table[i] == WT and outcome == false){
    pattern_table[i] = WNT;
  }
  // Weakly not taken
  else if(pattern_table[i] == WNT and outcome == true){
    pattern_table[i] = WT;
  } else if(pattern_table[i] == WNT and outcome == false){
    pattern_table[i] = SNT;
  }
  // Strongly not taken
  else if(pattern_table[i] == SNT and outcome == true){
    pattern_table[i] = WNT;
  } else if(pattern_table[i] == SNT and outcome == false){
    pattern_table[i] = SNT;
  }

  histories[branch] =  ((histories[branch] << 1) + outcome) & smask;
}
