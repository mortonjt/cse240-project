#include <stdio.h>
#define g 5
#define l 26
#define N 32
#define hashCount 5  
#define integerBit 8
signed int biasW[N];
signed int globalW[N][g];
signed int localW[N][l];
unsigned int percep_index;
unsigned int globalBHR = 0;
unsigned int localBHR[N];
unsigned int hashMask = 0;
signed int current_t = 0;
unsigned int leastOneMask = 1;
unsigned int integerMask = 0;

void init_predictor ()
{
	for (int i = 0; i < hashCount; i++)
	{
		hashMask = hashMask << 1;
		hashMask += 1;
	}
	for (int i = 0; i < integerBit; i++)
	{
		integerMask = integerMask << 1;
		integerMask += 1;
	}
}

bool make_prediction (unsigned int pc)
{
  percep_index = pc & hashMask;

  signed int y = biasW[percep_index];

  unsigned int shifterTemp = localBHR[percep_index];

  for (int i = 0 ; i < l; i++)
  {
  	 if ((shifterTemp & leastOneMask) > 0)
  	 	y += localW[percep_index][i];
  	 else
  	 	y -= localW[percep_index][i];
  	 shifterTemp = shifterTemp >> 1;
  }
  shifterTemp = globalBHR;
  for (int i = 0 ; i < g; i++)
  {
 	 if ((shifterTemp & leastOneMask) > 0)
  	 	y += globalW[percep_index][i];
  	 else
  	 	y -= globalW[percep_index][i];
  	shifterTemp = shifterTemp >> 1;
  }

  if (y >= 0)
  {
  	current_t = 1;
  }
  else
  {
  	current_t = -1;
  }
  return current_t == 1;
}

void train_predictor (unsigned int pc, bool outcome)
{
	unsigned int shifterTemp = localBHR[percep_index];

	unsigned int trueOutcome = 0;
	if (outcome)
  	{
	  	trueOutcome = 1;
	}
	else
	{
	  	trueOutcome = -1;
	}
	//update weight
	for (int i = 0; i < l; i ++)
	{
		if ((shifterTemp & leastOneMask) > 0)
			localW[percep_index][i] += trueOutcome * 1;
		else
			localW[percep_index][i] += trueOutcome * (-1);
		localW[percep_index][i] = integerMask & localW[percep_index][i];
		shifterTemp = shifterTemp >> 1;
	}
	shifterTemp = globalBHR;
	for (int i = 0; i < g; i ++)
	{
		if ((shifterTemp & leastOneMask) > 0)
			globalW[percep_index][i] += trueOutcome * 1;
		else
			globalW[percep_index][i] += trueOutcome * (-1);
		globalW[percep_index][i] = integerMask & globalW[percep_index][i];
		shifterTemp = shifterTemp >> 1;
	}
	biasW[percep_index] += trueOutcome;
	biasW[percep_index] = integerMask & biasW[percep_index];

	//update history
	globalBHR = globalBHR << 1;
    globalBHR += (int)(outcome == true);

    localBHR[percep_index] = localBHR[percep_index] << 1;
    localBHR[percep_index] += (int)(outcome == true);
}
