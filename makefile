CC:=gcc
FLAGS:=-g -c
PROG:=hosts
F1:=main
F2:=ping
F3:=utilities
F4:=udp

all:$(F1).o $(F2).o $(F3).o $(F4).o
	$(CC) -g $(F1).o $(F2).o $(F3).o $(F4).o -o $(PROG) 

$(F1).o:$(F1).c
	$(CC) $(FLAGS) $(F1).c -o $(F1).o

$(F2).o:$(F2).c
	$(CC) $(FLAGS) $(F2).c -o $(F2).o

$(F3).o:$(F3).c
	$(CC) $(FLAGS) $(F3).c -o $(F3).o

$(F4).o:$(F4).c
	$(CC) $(FLAGS) $(F4).c -o $(F4).o

clean:
	rm *.o