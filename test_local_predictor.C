#include <stdio.h>
#include <netinet/in.h>
#include "predictor.h"


bool test_init_predictor()
{
  int t = 10;

  pattern_table[0][0] = 1;
  init_predictor ();
  if(pattern_table[0][0] != 0)
    return false;

  return true;
}

bool test_step()
{
  //
  init_predictor ();
  step(0, 0, false);

  if(pattern_table[0][0] != WT)
    return false;


  if(histories[0] != 0)
    return false;

  init_predictor ();
  step(0, 0, true);

  if(pattern_table[0][0] != ST)
    return false;
  if(histories[0] != 1)
    return false;

  return true;
}

// TODO: Add in a test where
// the history_idx is larger than
// the table size

int main (int argc, char * argv[])
{
  if(!test_init_predictor())
      printf ("init_predictor failed!\n");

  if(!test_step())
      printf ("step failed!\n");

  return 0;
}
