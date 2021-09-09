#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
//#include "client_lib.h"
#include "csapp.h"
#include "strings/error.h"
#include "strings/strfun.h"
#include "strings/bootstrap.h"
#include "strings/linked_list.h"
#include "strings/tc_malloc.h"

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


int rio_read_custom(rio_t *rp, size_t n)
{
    int cnt;
    int rc;
    //printf("%ud, %ud\n", rp->rio_buf, rp->rio_bufptr);

      /* Refill if buf is empty */
    cnt = read(rp->rio_fd, rp->rio_bufptr,(n-rp->rio_cnt));
    if (cnt < 0) {
        if (errno != EINTR || errno != EWOULDBLOCK) /* Interrupted by sig handler return */
            return -1;
    }
    else if (cnt == 0 && rp->rio_cnt == 0)  /* EOF */
        return 0;
	//else 
	//    rp->rio_bufptr = rp->rio_buf; /* Reset buffer ptr */

    //printf("cur:%s\n",&(rp->rio_buf[8]));
    /* Copy min(n, rp->rio_cnt) bytes from internal buf to user buf */
    rp->rio_bufptr += cnt;
    cnt += rp->rio_cnt;          
    rp->rio_cnt = cnt;
    return cnt;
}

int global_called = 0;
char* thread_stat = NULL;
int* thread_fd = NULL;
char ** thread_keyword = NULL;
const int thr_num = 10;
struct term_node ** term_saved = NULL;
struct req_node * req_init = NULL;
struct req_node * req_last = NULL;
int * req_num ;
char * server_name;
pthread_mutex_t req_lock;

void pop_req_node(){
    if(*req_num < 1){
        fprintf(stderr,"wrong call. req_node is empty\n");
        return;
    }
    else if(*req_num == 1){
        free(req_init);
        req_init = NULL;
        req_last = NULL;
        (*req_num)--;
        return;
    }
    else{
        (*req_num)--;
        req_init=req_init->back;
        free(req_init->front);
        req_init->front = NULL;
        return;
    }
}
void push_req_node(struct req_node * add_node){
    if(*req_num==0){
        req_init = add_node;
        req_last = add_node;
        add_node->front = NULL;
        add_node->back = NULL;
    }
    else{
        req_last->back = add_node;
        add_node->front = req_last;
        add_node->back = NULL;
        req_last = add_node;
    }
    (*req_num)++;
    return;
}


void finding(int * thread_i){
    int called_time = 0;
    char * keyword;
    int connfd, rc;
    char ** result = malloc(sizeof(char*)*1000);
    int n_result;
    int tot_size;
    int result_len;
    my_prot * send_packet = malloc(sizeof(my_prot));
    struct str_w_len * merged_str;

    char ** token_buf;
    char * temp;
    int token_buf_length = 0;

    tc_thread_init();
    //waiting input.
    while(1){
        if(thread_stat[*thread_i]==1&&(thread_keyword[*thread_i]!=NULL)){
            called_time++;
            global_called++;
            
            //printf("now the work is given\n");
            //check whether it's search or not.
            keyword = thread_keyword[*thread_i];
            if(keyword == NULL)
                assert(0);

            token_buf_length = token_num(keyword);
            if((token_buf = string_token(keyword))==NULL){
                temp= "wrong input\n";
                result[0] = string_copy(temp);
                result_len = string_length(result[0]);
                n_result = 1;
            }
            else if((rc=string_compare("search",token_buf[0]))!=0){
                if(string_compare("name",token_buf[0])==0){
                    temp= server_name;
                    result[0] = string_copy(temp);
                    result_len = string_length(result[0]); 
                    n_result = 1;  
                }
                else{
                    temp= "wrong input\n";
                    result[0] = string_copy(temp);
                    result_len = string_length(result[0]); 
                    n_result = 1;   
                }
                         
            }
            else{
                n_result = search_from_list(token_buf, token_buf_length, *term_saved, result);
            }
            //now, bootstraping cases.
            //I will check the only first keyword from the result.
            if(n_result==0){
                send_packet->total_length=(8+1);
                send_packet->message_type =(32);
                send_packet->content = tc_malloc(sizeof(char)*send_packet->total_length);
                ((int *)send_packet->content)[0]=send_packet->total_length;
                ((int *)send_packet->content)[1]=send_packet->message_type;
                send_packet->content[8] = EOF;
                Rio_writen(thread_fd[*thread_i],(char*)send_packet->content,send_packet->total_length);
                tc_free(send_packet->content);
            }
            else{
                merged_str = string_merge_lots(result,n_result);
                send_packet->content = merged_str->str;
                send_packet->total_length = (8+merged_str->tot_len+1);
                send_packet->message_type = (17);
                ((int *)send_packet->content)[0]=send_packet->total_length;
                ((int *)send_packet->content)[1]=send_packet->message_type;
                Rio_writen(thread_fd[*thread_i],send_packet->content,send_packet->total_length);
                tc_free(merged_str->str);
                tc_free(merged_str); 
            }

            //printf("I sent the data\n");
            //fflush(stdout);

            //result free
            
            for(int i=0;i<n_result;i++){
                free(result[i]);
            }
            //이제 다시 대기 상태로 설정.
            pthread_mutex_lock(&req_lock);
            thread_stat[*thread_i]=0;
            thread_keyword[*thread_i]=NULL;
            thread_fd[*thread_i] = 0;
            pthread_mutex_unlock(&req_lock);
            free(keyword);
            
        }
        //checking whether new work is given or not.
        //Mutex를 통해서 구현하자.
        //printf("I got the lock %d\n",*thread_i);
        if((rc = pthread_mutex_trylock(&req_lock))==0){
            if(*req_num>0){
                for(int i=0;i<thr_num;i++)
                    if((*req_num>0)&&(thread_stat[i]==0)){
                        thread_fd[i] = req_init->connfd;
                        thread_keyword[i] = req_init->input;
                        thread_stat[i]=1;
                        pop_req_node();
                    }
            }
            pthread_mutex_unlock(&req_lock);
        }
    }
}

int server_init(char * path){
    int n_thr = thr_num;
    if(n_thr<0||n_thr>1000){
        fprintf(stdout,"to small or big thread num. please set between 1~999\n");
        return -1;
    }
    //setting bootstrapping
    term_saved = malloc(sizeof(struct term_node *));
    *term_saved = NULL;
    if(bootstrapping(path,term_saved)<0){
        fprintf(stderr,"bootstrapping failed\n");
        return -1;
    }
    printf("bootstrapping is done\n");

    pthread_mutex_init(&req_lock,NULL);
    thread_stat = malloc(sizeof(char)*n_thr);
    thread_fd = malloc(sizeof(int)*n_thr);
    thread_keyword = malloc(sizeof(char*)*n_thr);
    for(int i=0;i<n_thr;i++){
        thread_stat[i] = 0;
    }
    req_num = malloc(sizeof(int));
    *req_num=0;
    tc_central_init();


    pthread_t p_thread[n_thr];
    int thr_id;
    int status;
    int i;
    int * thread_arg=malloc(sizeof(int)*n_thr);
    for(i=0;i<n_thr;i++){
        thread_arg[i]=i;
        pthread_mutex_lock(&req_lock);
        if((thr_id = pthread_create(&p_thread[i],NULL,finding,&(thread_arg[i])))<0){
            fprintf(stderr,"thread pool creation error\n");
            return -1;
        }
        pthread_mutex_unlock(&req_lock);
    }
    return 0;
    
}


////////server funcs
void init_pool(int sockfd, pool *p){
    int i;
    p-> maxfd = -1;
    for(i=0; i<FD_SETSIZE; i++){
        p->clientfd[i] = -1;
    }
    p->maxfd = sockfd;
    FD_ZERO(&(p->read_set));
    FD_SET(sockfd, &p->read_set);

    //이 친구들도 초기화를 해줘야 할 것 같은데, 딱히 언급이 없어서....
    FD_ZERO(&(p->write_set));
    FD_ZERO(&(p->ready_set));

}

int add_client(int connfd, pool *p){
    //accept를 통해서 할당된 fd를 pool에 추가하는 과정이다.
    int i;
    //reduce the counter
    p->nready--;
    for(i=0; i<FD_SETSIZE;i++){
        //searching the right place.
        if(p->clientfd[i]==-1){
            //add the descriptor to the pool
            p->clientfd[i]= connfd;
            Rio_readinitb(&(p->clientrio[i]),connfd);
            //add to the descriptor set.
            FD_SET(connfd,&p->read_set);

            if(connfd>p->maxfd)
                p->maxfd = connfd;
            if(i>p->maxi)
                p->maxi = i;
            break;
        }
    }
    //check whether it's breaked for loop or finished for loop
    if(i==FD_SETSIZE){
        fprintf(stderr,"There isn't empty slot\n");
        return -1;
    }

    //printf("%ud, %ud\n", p->clientrio[i].rio_buf, p->clientrio[i].rio_bufptr);
    return 0;
}

int check_clients(pool *p){
    int i, connfd, n;
    char buf[MAXLINE];
    rio_t * rio;
    
    for(i=0; (i<=p->maxi)&&(p->nready>0);i++){
        connfd = p->clientfd[i];
        rio = &p->clientrio[i];
        //check whether the descriptor is ready or not.
        if((connfd>0)&&(FD_ISSET(connfd, &p->ready_set))){
            p->nready--;
            if((n= rio_read_custom(rio,RIO_BUFSIZE))<0){
                //skip.
                if(errno != EWOULDBLOCK)
                {
                    fprintf(stderr,"read error, closing socket %d, err : %s\n",connfd, strerror(errno));
                    Close(connfd);
                    assert(0);
                    FD_CLR(connfd,&p->read_set);
                    p->clientfd[i]= -1;
                }                
            }
            
            if(n==0){
                //이경우 NULL 을 보냄으로 clos하겠다는 거니까 
                Close(connfd);
                FD_CLR(connfd,&p->read_set);
                p->clientfd[i]= -1;
            }
            else{
                my_prot* temp = rio->rio_buf;
                if(n<8){
                    printf("less data\n");
                }
                else if(temp->total_length>n){
                    //skip해야한다.
                    //이 부분이 호출된다면 어떻게 될지 잘 모르겠다. 
                    printf("need to receive more data\n");
                    printf("cur in : %d, expected : %d\n",n,temp->total_length);
                }
                else{
                    rio->rio_cnt = 0;
                    rio->rio_bufptr = rio->rio_buf;
                    if(temp->message_type!=16&& temp->message_type!=18){
                        printf("it's not request\n");
                        assert(0);
                    }
                    else if(temp->message_type==16){
                        //push in to the request list.
                        struct req_node * new_task = malloc(sizeof(struct req_node));
                        new_task->front = NULL;
                        new_task->back = NULL;
                        new_task->connfd = connfd;
                        new_task->input = string_copy(((char *) rio->rio_buf + 8));
                        if(new_task->input == NULL)
                            assert(0); //skip the packet set.
                        new_task->input_len = temp->total_length;
                        int rc = pthread_mutex_lock(&req_lock);
                        if(rc<0){
                            fprintf(stderr,"mutex lock failed\n");
                            assert(0);
                        }
                        push_req_node(new_task);
                        rc = pthread_mutex_unlock(&req_lock);
                    }
                    else{
                        struct req_node * new_task = malloc(sizeof(struct req_node));
                        new_task->front = NULL;
                        new_task->back = NULL;
                        new_task->connfd = connfd;
                        new_task->input = string_copy("name");
                        if(new_task->input == NULL)
                            assert(0); //skip the packet set.
                        new_task->input_len = temp->total_length;
                        int rc = pthread_mutex_lock(&req_lock);
                        if(rc<0){
                            fprintf(stderr,"mutex lock failed\n");
                            assert(0);
                        }
                        push_req_node(new_task);
                        rc = pthread_mutex_unlock(&req_lock);
                    }
                }
            }

        }
    }
    return 0;
}


int open_listenfd_NB(int port){
    int listenfd, opts=1,rc;
    struct sockaddr_in serveraddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd < 0){
        fprintf(stderr,"Listening socket() failed.\n");
        return -1;
    }

    if((rc = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opts, sizeof(opts)))!=0){
        fprintf(stderr, "setting socket option as reusable error\n");
        return -1;
    }

    //now set the socket as non-blocking
    if((opts = fcntl(listenfd, F_GETFL))<0){
        fprintf(stderr, "getting socket option error\n");
        return -1;
    }
    opts = (opts|O_NONBLOCK);
    if((rc=fcntl(listenfd,F_SETFL,opts))<0){
        fprintf(stderr, "setting socket error\n");
        return -1;
    }
    //I used memset insetad of bzero().
    memset(&serveraddr,0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(port);

    //bind하자
    rc = bind(listenfd, (SA *)&serveraddr, sizeof(serveraddr));
    if(rc<0){
        fprintf(stderr, "binding socket failed\n");
        return -1;
    }
    if((rc=listen(listenfd,LISTENQ))<0){
        fprintf(stderr, "failed listen()\n");
        return -1;
    }

    return listenfd;

}

int main(int argc, char * argv[]){
    //get input
    char * path_name;
    int port_num;
    if(argc == 4){
        server_name = string_copy(argv[1]);
        path_name = string_copy(argv[2]);
        port_num = atoi(argv[3]);
    }
    else{
        fprintf(stderr,"The command argument is like this \n");
        fprintf(stderr,"[server_name] [absolute path to target folder] [port number]\n");
        return 0;
    }

    /*
    bootstrapping initialization.
    */
    if(server_init(path_name)<0){
        fprintf(stderr,"server initilaize failed\n");
        return -1;
    }
    printf("server initalizing is done\n");

    /*
    server making.
    */
    int listenfd, connfd, clientlen,opts=1,rc;
    struct sockaddr_in clientaddr;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1;
    clientlen = sizeof(clientaddr);
    //struct hostent * hp;
    pool * s_pool = malloc(sizeof(pool));
    //char * haddrp;

    //listen
    listenfd = open_listenfd_NB(port_num);

    init_pool(listenfd,s_pool);
    while(1){
        memcpy(&(s_pool->ready_set), &(s_pool->read_set), sizeof(s_pool->read_set));
        s_pool->nready = select(s_pool->maxfd+1,&s_pool->ready_set,(fd_set *)0,(fd_set *)0,&tv);
        //printf("cur req : %d\n",s_pool->nready);

        if(FD_ISSET(listenfd,&(s_pool->ready_set))){
            //when new connection is required.
            //check all accept calls.
            do{
                connfd = accept(listenfd, (SA *)&clientaddr, &clientlen);
                //printf("connect fd : %d\n",connfd);
                fflush(stdout);
                if(connfd<0){
                    if(errno != EWOULDBLOCK){
                        fprintf(stderr,"acceptance error : %s\n",strerror(errno));
                        assert(0);
                    }
                    break;
                }
                else{
                    if((opts = fcntl(connfd, F_GETFL))<0){
                        fprintf(stderr, "getting socket option error\n");
                        return -1;
                    }
                    opts = (opts|O_NONBLOCK);
                    if((rc=fcntl(connfd,F_SETFL,opts))<0){
                        fprintf(stderr, "setting socket error\n");
                        return -1;
                    }
                    if((rc = add_client(connfd, s_pool))<0){
                        fprintf(stderr,"error in add_client()\n");
                    }
                }
            }while(connfd!=-1);
        }
        if((rc = check_clients(s_pool))<0){
            fprintf(stderr,"error in check_clients\n");
        }
    }

    /*

    호출될일 없는 구역. 서버 종료 관련 설정을 만들떄 쓰자.

    */
}
