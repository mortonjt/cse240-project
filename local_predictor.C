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

8K   + 64  -> k=10,  s=7
16K  + 128 -> k=11,  s=7
32K  + 256 -> k=12,  s=7
64K  + 512 -> k=13,  s=7
128K + 1K  -> k=14,  s=8
1M   + 4K  -> k=17,  s=8
*/

# define k 17 // number of bits for hashing branches
# define s 8  // 4096=2^12.  12 bits for history


/*
Stores global history table

Maximum size: 2^20
 */
unsigned int pattern_table[1048576];

/*
Histories for each branch

Maximum size: 2^20
*/
unsigned int histories[1048576];

// mask for k bits
unsigned int kmask;
// mask for s bits
unsigned int smask;

void init_predictor ()
{

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
  unsigned int branch = (pc & kmask);  // branch hash
  unsigned int i = histories[branch] & smask;

  if(pattern_table[i] < 2)
    return true;
  else
    return false;
}


void train_predictor (unsigned int pc, bool outcome)
{
  unsigned int branch = (pc & kmask);  // branch hash
  unsigned int i = histories[branch] & smask;
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

  histories[branch] =  ((histories[branch] << 1) + outcome) & smask;
}
