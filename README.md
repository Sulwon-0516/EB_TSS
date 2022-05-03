# EE488-HW3

This is the repository of Final Project conducted in Fall 2020.

It's implementation of text search server, which can handle more 100k requests/sec based on multi-thread

It includes thread-cached memory allocation(tcmalloc) and text bootstrapping implementation.


## Quick Start
compile files

` ` ` 
make all
` ` `

run server first

` ` `
./server_tc [server_name] [directory] [server_port]
` ` `

test with clients
client1 is to test extreme case. (1000 threads, 100 requests each)
client2 is for actual user. 
` ` `
./client_1 [server ip] [server_port] [number_of_threads] [req_per_thread] [word to search]
./client_2 [server ip] [server_port]
` ` `

## Details
- strings/tc_malloc.c : implementation of tcmalloc
- strings/strfunc.c : implementation of string relevant library. (overflow-safe)
- server_tc.c : implementation of event-based server

## Etcs.
I've checked it working well in Ubuntu18.04. 
To handle 100k requests/sec, need to prepare big multi-core system which can handle hundreds of threads simultaneously for both client and server.

last modified : 220504
