#include <stdio.h>
// k bits used from branch address
unsigned int bitCount = 13;
unsigned int predictorCount = 8192;
unsigned int mask = 0;
unsigned int counters[8192];
unsigned int BHR = 0;
unsigned int current_ps = 0;
unsigned int current_p = 0;
unsigned int current_map_key = 0;

void init_predictor ()
{
  for (int i = 0; i < bitCount; i++)
  {
    mask = mask << 1;
    mask += 1;
  }
  for (int i = 0; i < predictorCount; i++)
  {
    counters[i] = 0;
  }
}


bool make_prediction (unsigned int pc)
{
  unsigned int pcLow = pc & mask;
  current_map_key = (~pcLow & ~ BHR);
  current_map_key = current_map_key & mask;
  current_ps = counters[current_map_key];
  current_p = (current_ps > 1);
  return current_p;
}

void train_predictor (unsigned int pc, bool outcome)
{
  BHR = BHR << 1;
  BHR += (int)(outcome == true);
  unsigned int next_ps = current_ps;
  if (current_p == outcome)
  {
    if (current_ps == 1)
    {
      next_ps = 0;
    }
    if (current_ps == 2)
    {
      next_ps = 4;
    }
  }
  else
  {
    if (current_ps <= 1)
    {
      next_ps = current_ps + 1;
    }
    else
    {
      next_ps = current_ps - 1;
    }
  }
  counters[current_map_key] = next_ps;

}
