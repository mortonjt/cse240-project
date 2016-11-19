CC=g++
OPTS=-g

all: main.o traceread.o predictor.o
	$(CC) $(OPTS) -lm -o predictor main.o traceread.o predictor.o

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
test_alpha_predictor.o: test_alpha_predictor.C predictor.h
	$(CC) $(OPTS) -c test_alpha_predictor.C

clean:
	rm -f *.o predictor test_alpha_predictor alpha_predictor;
