CC=g++
OPTS=-g

all: main.o traceread.o predictor.o
	$(CC) $(OPTS) -lm -o predictor.out main.o traceread.o predictor.o

local_predictor: main.o traceread.o local_predictor.o
	$(CC) $(OPTS) -lm -o local_predictor.out main.o traceread.o local_predictor.o


alpha_predictor: main.o traceread.o alpha_predictor.o
	$(CC) $(OPTS) -lm -o alpha_predictor.out main.o traceread.o alpha_predictor.o

main.o: main.C traceread.h predictor.h
	$(CC) $(OPTS) -c main.C

traceread.o: traceread.h traceread.C
	$(CC) $(OPTS) -c traceread.C

predictor.o: predictor.h predictor.C
	$(CC) $(OPTS) -c predictor.C

alpha_predictor.o: predictor.h alpha_predictor.C
	$(CC) $(OPTS) -c alpha_predictor.C


local_predictor.o: predictor.h local_predictor.C
	$(CC) $(OPTS) -c local_predictor.C

clean:
	rm -f *.o predictor local_predictor alpha_predictor;

