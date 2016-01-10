CFLAGS += -g -I ../net/ -I ../db/ -I ../log/ -I ../ -I ../log/
testperf1thread.o: testperf1thread.c
	gcc $(CFLAGS) -g -o testperf1thread.o testperf1thread.c -lpthread
testperf5threads.o: testperf5threads.c
	gcc $(CFLAGS) -g -o testperf5threads.o testperf5threads.c -lpthread
testperf10threads.o: testperf10threads.c
	gcc $(CFLAGS) -g -o testperf10threads.o testperf10threads.c -lpthread
testperf15threads.o: testperf15threads.c
	gcc $(CFLAGS) -g -o testperf15threads.o testperf15threads.c -lpthread
testperf20threads.o: testperf20threads.c
	gcc $(CFLAGS) -g -o testperf20threads.o testperf20threads.c -lpthread
testperfthreads.o: testperfthreads.c makecommand.c
	gcc $(CFLAGS) -g -o testperfthreads.o testperfthreads.c makecommand.c -lpthread
clean:
	rm -rf *.o
