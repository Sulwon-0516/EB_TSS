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



void Worker(struct Worker_arg * args){
    //repeatedly call the server
    
    
}


int main(int argc, char ** argv){
    //the command line input ./client1 [server ip] [server_port] [number_of_threads] [req_per_thread] [word to search]
    char * server_ip_ch= NULL;
    int server_port = 0;
    time_t start, finish;

    if(argc != 3){
        fprintf(stderr," Wrong command line input\n");
        fprintf(stderr," input args [server ip] [server_port]\n");
        assert(0);
    }
    else{
        server_port = atoi(argv[2]);

        if(string_compare(argv[1],"localhost")==0){
            server_ip_ch = string_copy("127.0.0.1");
        }
        else{
            server_ip_ch = string_copy(argv[1]);
        }
    }

    int clientfd;
    char *host;
    int port;
    char * command;
    char buf[MAXLINE];
    char * temp;
    char * server_name;
    char * input = NULL;
    size_t input_size = 0;
    rio_t * rio = malloc(sizeof(rio_t));
    host = server_ip_ch;
    port = server_port;
    if((clientfd = Open_clientfd(host,port))<0){
        fprintf(stderr,"Failed to open client. Error code :%d\n",(-clientfd));
        assert(0);
    }
    printf("connected\n");
    my_prot* packet = malloc(sizeof(my_prot));


    packet->message_type = 18;
    packet->total_length = 8+1;
    packet->content = malloc(sizeof(char)*(packet->total_length));
    ((int *)packet->content)[0] = packet->total_length;
    ((int *)packet->content)[1] = packet->message_type;
    packet->content[8] = EOF;


    rio_readinitb(rio, clientfd);
    Rio_writen(clientfd,packet->content,packet->total_length);
    Rio_readlineb(rio,buf,MAXLINE);
    //검색 결과를 저장하는 부분을 만들어야 한다. 
    if(((my_prot*)buf)->message_type==17){
        server_name = string_copy_wo_EOF(&(buf[8]));
    }
    else{
        fprintf(stderr,"failed to receive program name\n");
        return 0;
    }
    free(packet->content);
    printf("Got the server name\n");


    while(1){
        printf("%s>", server_name);
        getline(&command,&input_size,stdin);

        //check whether it's quit or not.
        if(string_compare(command,"quit")==0){
            printf("terminating the searching engine\n");
            break;
        }


        packet->message_type = 16;
        packet->total_length = string_length(command)+8+1;
        packet->content = malloc(sizeof(char)*(packet->total_length));
        ((int *)packet->content)[0] = packet->total_length;
        ((int *)packet->content)[1] = packet->message_type;
        temp = (char*)packet->content + 8;
        memcpy(temp,command,string_length(command)+1);
        //now, socket is given and connection is maden.
        //initialize the rio IO
        rio_readinitb(rio, clientfd);
        Rio_writen(clientfd,packet->content,packet->total_length);
        Rio_readlineb(rio,buf,MAXLINE);
        //검색 결과를 저장하는 부분을 만들어야 한다. 
        if(((my_prot*)buf)->message_type==32){
            //there is no file.
            fprintf(stdout,"No file");
        }
        else if(((my_prot*)buf)->message_type==17){
            int cnt = 8;
            do{
                putc(buf[cnt],stdout);
                cnt++;
            }while(buf[cnt]!=EOF); 
        }
        printf("\n"); 
        free(packet->content);
    }
    
    free(rio);
    free(packet);


    return 0;
}