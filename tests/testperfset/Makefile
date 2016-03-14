CFLAGS += -g -I ../net/ -I ../db/ -I ../log/ -I ../ -I ../log/
testperfthreads.o: testperfthreads.c makecommand.c
	gcc $(CFLAGS) -g -o testperfthreads.o testperfthreads.c makecommand.c -lpthread
clean:
	rm -rf *.o
