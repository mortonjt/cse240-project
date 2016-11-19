CC=g++
OPTS=-g

all: main.o traceread.o predictor.o
	$(CC) $(OPTS) -lm -o predictor.out main.o traceread.o predictor.o

gshare_predictor: main.o traceread.o gshare_predictor.o
	$(CC) $(OPTS) -lm -o gshare_predictor.out main.o traceread.o gshare_predictor.o
percep_predictor: main.o traceread.o percep_predictor.o
	$(CC) $(OPTS) -lm -o percep_predictor.out main.o traceread.o percep_predictor.o
main.o: main.C traceread.h predictor.h
	$(CC) $(OPTS) -c main.C

traceread.o: traceread.h traceread.C
	$(CC) $(OPTS) -c traceread.C

predictor.o: predictor.h predictor.C
	$(CC) $(OPTS) -c predictor.C

local_predictor.o: predictor.h local_predictor.C
	$(CC) $(OPTS) -c local_predictor.C

clean:
	rm -f *.o predictor;
