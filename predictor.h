#ifndef PREDICTOR_H
#define PREDICTOR_H

/*
  Define all your tables and their sizes here.
  All tables must be statically-sized.
  Please don't call malloc () or any of its relatives within your
  program.  The predictor will have a budget, namely (32K + 256) bits
  (not bytes).  That encompasses all storage (all tables, plus GHR, if
  necessary -- but not PC) used for the predictor.  That may not be
  the amount of storage your predictor uses, however -- for example,
  you may implement 2-bit predictors with a table of ints, in which
  case the simulator will use more memory -- that's okay, we're only
  concerned about the memory used by the simulated branch predictor.
*/

#include <math.h>

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

//# define k 12 // number of bits for hashing branches
//# define s 8 // 4096=2^12.  12 bits for history


/*
Stores global history table

Maximum size
k=12 -> 4096 = 2^12
s=8  -> 256  = 2^12
 */
//extern int pattern_table[4096][256];

/*
Histories for each branch

Maximum size
k=12 -> 4096 = 2^12
*/
//extern int histories[4096];

// helper function that updates the global table;
void step(int i, int j, bool outcome);

/*
  Initialize the predictor.
*/
void init_predictor ();

/*
  Make a prediction for conditional branch instruction at PC 'pc'.
  Returning true indicates a prediction of taken; returning false
  indicates a prediction of not taken.
*/
bool make_prediction (unsigned int pc);

/*
  Train the predictor the last executed branch at PC 'pc' and with
  outcome 'outcome' (true indicates that the branch was taken, false
  indicates that the branch was not taken).
*/
void train_predictor (unsigned int pc, bool outcome);


#endif
