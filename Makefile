CC = gcc 
CFLAGS = -W -Wall -g -pthread
TARGET=main

all : client_1 client_2 server_tc

server_tc : server_tc.o error.o strfun.o linked_list.o bootstrap.o csapp.o tc_malloc.o
	$(CC) $(CFLAGS) -o server_tc server_tc.o error.o strfun.o linked_list.o bootstrap.o csapp.o tc_malloc.o

client_1 : client_1.o error.o strfun.o linked_list.o bootstrap.o csapp.o tc_malloc.o
	$(CC) $(CFLAGS) -o client_1 client_1.o error.o strfun.o linked_list.o bootstrap.o csapp.o tc_malloc.o

client_2 : client_2.o error.o strfun.o linked_list.o bootstrap.o csapp.o tc_malloc.o
	$(CC) $(CFLAGS) -o client_2 client_2.o error.o strfun.o linked_list.o bootstrap.o csapp.o tc_malloc.o

client_2.o: strings/strfun.h strings/error.h csapp.h client_2.c
	$(CC) $(CFLAGS) -c -o client_2.o client_2.c

client_1.o: strings/strfun.h strings/error.h csapp.h client_1.c
	$(CC) $(CFLAGS) -c -o client_1.o client_1.c

server_tc.o: strings/tc_malloc.h strings/strfun.h strings/error.h strings/linked_list.h strings/bootstrap.h csapp.h server_tc.c
	$(CC) $(CFLAGS) -c -o server_tc.o server_tc.c

error.o : strings/error.h strings/error.c
	$(CC) $(CFLAGS) -c -o error.o strings/error.c

strfun.o : strings/error.h strings/strfun.h strings/tc_malloc.h strings/strfun.c 
	$(CC) $(CFLAGS) -c -o strfun.o strings/strfun.c

linked_list.o : strings/error.h strings/strfun.h strings/linked_list.h strings/linked_list.c
	$(CC) $(CFLAGS) -c -o linked_list.o strings/linked_list.c

bootstrap.o : strings/error.h strings/strfun.h strings/linked_list.h strings/bootstrap.h strings/bootstrap.c
	$(CC) $(CFLAGS) -c -o bootstrap.o strings/bootstrap.c

csapp.o : csapp.h csapp.c
	$(CC) $(CFLAGS) -c -o csapp.o csapp.c

tc_malloc.o : strings/tc_malloc.h strings/tc_malloc.c 
	$(CC) $(CFLAGS) -c -o tc_malloc.o strings/tc_malloc.c


clean:
	rm -f *.o
	rm -f server_tc
	rm -f client_1
	rm -f client_2