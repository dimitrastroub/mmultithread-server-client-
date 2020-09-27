OBJS = master.o
OBJS2 = worker.o 
OBJS3 = whoServer.o
OBJS4 = whoClient.o
SOURCE = master.c
HEADER = structs.h 
OUT = master
OUT2 = worker
OUT3 = whoServer
OUT4 = whoClient
CC = gcc
FLAGS = -g -c 

all: $(OUT) $(OUT2) $(OUT3) $(OUT4)

$(OUT): $(OBJS)
	$(CC) -g $(OBJS) -o $@

$(OUT2): $(OBJS2)
	$(CC) -g $(OBJS2) -o $@

$(OUT3): $(OBJS3)
	$(CC) -g $(OBJS3) -o $@ -lpthread

$(OUT4): $(OBJS4)
	$(CC) -g $(OBJS4) -o $@ -lpthread

whoClient.o: whoClient.c
	$(CC) $(FLAGS) whoClient.c

whoServer.o: whoServer.c
	$(CC) $(FLAGS) whoServer.c

master.o: master.c
	$(CC) $(FLAGS) master.c

my_worker.o: worker.c
	$(CC) $(FLAGS) worker.c


clean: 
	rm -f $(OBJS) $(OUT)
	rm -f $(OBJS2) $(OUT2)
	rm -f $(OBJS3) $(OUT3)
	rm -f $(OBJS4) $(OUT4)
	rm -f output*
	rm -f input*

