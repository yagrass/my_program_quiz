CC = gcc


all: main 

main: clock.c 5_13.c
	$(CC) 5_13.c clock.c  -o  5_13 

run: 5_13
	./5_13

clean:
	rm -f *.o 5_13 *.txt result.png

plot:
	gnuplot plot.gp

