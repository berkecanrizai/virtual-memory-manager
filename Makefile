all: compile

compile:
	gcc part1.c -o part1.out
	gcc part2.c -o part2.out

first:
	./part1.out BACKING_STORE.bin addresses.txt

FIFO:
	./part2.out BACKING_STORE.bin addresses.txt -p 0
	
LRU:
	./part2.out BACKING_STORE.bin addresses.txt -p 1

clean:
	rm part1.out
	rm part2.out