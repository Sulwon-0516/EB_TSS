#ifndef LINKED_LIST_H
#define LINKED_LIST_H


#include <stdio.h>
#include <stdlib.h>

struct req_node{
    struct req_node * front;
    struct req_node * back;

    char * input;
    int input_len;
    int connfd;
};


struct line_node{
    struct line_node * front_node;
    struct line_node * behind_node;

    char is_initial;
    int total_length;

    int line_num;
    int line_freq;

};

struct doc_node{
    struct doc_node * front_node;
    struct doc_node * behind_node;
    char * doc_name;
    int doc_freq;
    //about the initial.
    char is_initial;
    //change the total_length in the initial node.
    //others have total_length = -1;
    int total_length;

    struct line_node * line_node;
};


struct term_node{
    struct term_node * front_node;
    struct term_node * behind_node;
    char * term;
    int term_freq;
    char is_initial;
    //change the total_length in the initial node.
    //others have total_length = -1;
    int total_length;
    
    struct doc_node * doc_node;

};


struct line_node * new_line_node(int line_num);
struct line_node * add_line_node(struct line_node * initial, int line_num);
struct doc_node * new_doc_node(char *doc_name, int line_num);
struct doc_node * add_doc_node(struct doc_node * initial, char *doc_name, int line_num);
int delete_line_node(struct line_node * first);
int delete_term_node(struct term_node * first);
int delete_doc_node(struct doc_node * first);
struct term_node * new_term_node(char * content, char * doc_name, int line_num);
struct term_node * add_node(struct term_node * initial,char * content,char * doc_name, int line_num);




#endif