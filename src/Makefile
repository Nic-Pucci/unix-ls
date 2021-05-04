CC = gcc
CFLAGS = -g -Wall
PROG = UnixLs
OBJS = UnixLs.o

UnixLs: $(OBJS)
	$(CC) $(CFLAGS) -no-pie $(OBJS) -o $(PROG)

UnixLs.o: UnixLs.c
	$(CC) $(CFLAGS) -c $(PROG).c

clean: 
	rm *.o $(PROG)
