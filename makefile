CC=g++
OPTS=-g

all: main.o traceread.o predictor.o
	$(CC) $(OPTS) -lm -o predictor.out main.o traceread.o predictor.o

main.o: main.C traceread.h predictor.h
	$(CC) $(OPTS) -c main.C

gshare_predictor: main.o traceread.o gshare_predictor.o
	$(CC) $(OPTS) -lm -o gshare_predictor.out main.o traceread.o gshare_predictor.o

percep_predictor: main.o traceread.o percep_predictor.o
	$(CC) $(OPTS) -lm -o percep_predictor.out main.o traceread.o percep_predictor.o

local_predictor: main.o traceread.o local_predictor.o
	$(CC) $(OPTS) -lm -o local_predictor.out main.o traceread.o local_predictor.o

alpha_predictor: main.o traceread.o alpha_predictor.o
	$(CC) $(OPTS) -lm -o alpha_predictor.out main.o traceread.o alpha_predictor.o

traceread.o: traceread.h traceread.C
	$(CC) $(OPTS) -c traceread.C

predictor.o: predictor.h predictor.C
	$(CC) $(OPTS) -c predictor.C

alpha_predictor.o: predictor.h alpha_predictor.C
	$(CC) $(OPTS) -c alpha_predictor.C

local_predictor.o: predictor.h local_predictor.C
	$(CC) $(OPTS) -c local_predictor.C

gshare_predictor.o: predictor.h gshare_predictor.C

	$(CC) $(OPTS) -c gshare_predictor.C
percep_predictor.o: predictor.h percep_predictor.C
	$(CC) $(OPTS) -c percep_predictor.C

clean:
	rm -f *.o predictor local_predictor alpha_predictor gshare_predictor percep_predictor;

