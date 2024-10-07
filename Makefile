CC = gcc
CFLAGS = -g

scheduler: src/scheduler.c
	$(CC) -Isrc -o scheduler src/scheduler.c
	
test01:
	./scheduler sample_io/input/input-1

test02:
	./scheduler sample_io/input/input-2

test03:
	./scheduler sample_io/input/input-3

clean:
	rm -f scheduler *.o *~