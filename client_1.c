#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#include "client_lib.h"
#include "csapp.h"
#include "strings/error.h"
#include "strings/strfun.h"
//#define MAXLINE 2048
//maxline defined in csapp as 8k

typedef struct my_pack{
    int total_length;
    int message_type;
    char * content;
}my_prot;
typedef struct my_pool{
    int maxfd;
    fd_set read_set;
    fd_set write_set;
    fd_set ready_set;
    int nready;
    int maxi;
    int clientfd[FD_SETSIZE];
    rio_t clientrio[FD_SETSIZE];

}pool;
struct Worker_arg{
    int req_num;
    char * keyword;
    char * server_ip;
    int server_port;
};


void Worker(struct Worker_arg * args){
    //repeatedly call the server
    int clientfd, port;
    char *host;
    char * command = string_merge("search ",args->keyword);
    char buf[MAXLINE];
    char * temp;
    rio_t * rio = malloc(sizeof(rio_t));
    host = args->server_ip;
    port = args->server_port;
    if((clientfd = Open_clientfd(host,port))<0){
        fprintf(stderr,"Failed to open client. Error code :%d\n",(-clientfd));
        assert(0);
    }
    my_prot* packet = malloc(sizeof(my_prot));
    packet->message_type = 16;
    packet->total_length = string_length(command)+8+1;
    packet->content = malloc(sizeof(char)*(packet->total_length));
    ((int *)packet->content)[0] = packet->total_length;
    ((int *)packet->content)[1] = packet->message_type;
    temp = (char*)packet->content + 8;
    memcpy(temp,command,string_length(command)+1);
    //now, socket is given and connection is maden.
    //initialize the rio IO

    printf("connect fd : %d\n",clientfd);
    fflush(stdout);

    for(int i=0; i<args->req_num; i++){
        rio_readinitb(rio, clientfd);
        Rio_writen(clientfd,packet->content,packet->total_length);
        Rio_readlineb(rio,buf,MAXLINE);
        //검색 결과를 저장하는 부분을 만들어야 한다. 
        if(((my_prot*)buf)->message_type==32){
            //there is no file.
            fprintf(stdout,"No file\n");
        }
        else if(((my_prot*)buf)->message_type==17){
            //int cnt = 8;
            //do{
            //    putc(buf[cnt],stdout);
            //    cnt++;
            //}while(buf[cnt]!=EOF);
        }
        //printf("\n\n");
    }
    //printf("All done\n");  

    free(rio);
    free(packet->content);
    free(packet);
    
}


int main(int argc, char ** argv){
    //the command line input ./client1 [server ip] [server_port] [number_of_threads] [req_per_thread] [word to search]
    char * server_ip_ch= NULL;
    int server_port = 0;
    int num_threads = 0;
    int req_per_thread = 0;
    char * word2search = NULL;
    time_t start, finish;

    if(argc != 6){
        fprintf(stderr," Wrong command line input\n");
        fprintf(stderr," input args [server ip] [server_port] [number_of_threads] [req_per_thread] [word to search]\n");
        assert(0);
    }
    else{
        server_port = atoi(argv[2]);
        num_threads = atoi(argv[3]);
        req_per_thread = atoi(argv[4]);
        word2search = string_copy(argv[5]);

        if(string_compare(argv[1],"localhost")==0){
            server_ip_ch = string_copy("127.0.0.1");
        }
        else{
            server_ip_ch = string_copy(argv[1]);
        }
    }
    struct Worker_arg args_in;
    args_in.req_num = req_per_thread;
    args_in.keyword = word2search;
    args_in.server_ip = server_ip_ch;
    args_in.server_port = server_port;

    pthread_t p_thread[num_threads];
    int thr_id;

    fprintf(stdout,"initializing the thread\n");
    start = time(0);
    for(int i=0;i<num_threads;i++){
        if((thr_id = pthread_create(&p_thread[i],NULL,Worker,&args_in))<0){
            //when thread creation is failed.
            fprintf(stderr,"Main function thread creation error\n");
            assert(0);
        }
    }
    //know, the thread creation is done
    //Join all hreads.
    for(int i=0;i<num_threads;i++){
        pthread_join(p_thread[i],NULL);
        //add some options if status isn't zero.
    }
    fprintf(stdout,"All thread joined.\n");
    finish = time(0);
    fprintf(stdout,"time diff: %f sec\n",difftime(finish,start));


    return 0;
}