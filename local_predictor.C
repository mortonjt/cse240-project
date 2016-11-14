# define ST 0
# define WT 1
# define WNT 2
# define SNT 3

// TODO: This will need to be moved to the header file
int pattern[4] = {ST, ST, ST, ST};


void init_predictor ()
{
  // initialize all 4 bit histories to Strongly Taken
  // Note that this is a little redundant
  pattern[0] = ST;
  pattern[1] = ST;
  pattern[2] = ST;
  pattern[3] = ST;
}

bool make_prediction (unsigned int pc)
{
  int i = (pc % 4);  // index in 2 bit prediction scheme
  if(pattern[i] == ST or pattern[i] == WT)
    return true;
  else
    return false;
}

void train_predictor (unsigned int pc, bool outcome)
{
  int i = (pc % 4);  // index in 2 bit prediction scheme
  // Strongly taken
  if(pattern[i] == ST and outcome == true){
    pattern[i] = ST;
  } else if(pattern[i] == ST and outcome == false){
    pattern[i] = WT;
  }
  // Weakly taken
  if(pattern[i] == WT and outcome == true){
    pattern[i] = ST;
  } else if(pattern[i] == WT and outcome == false){
    pattern[i] = WNT;
  }
  // Weakly not taken
  if(pattern[i] == WNT and outcome == true){
    pattern[i] = WT;
  } else if(pattern[i] == WNT and outcome == false){
    pattern[i] = SNT;
  }
  // Strongly not taken
  if(pattern[i] == SNT and outcome == true){
    pattern[i] = WNT;
  } else if(pattern[i] == SNT and outcome == false){
    pattern[i] = SNT;
  }

}
