#include <stdio.h>
#include <stdlib.h>
#include "strfun.h"
#include "error.h"
#include "linked_list.h"




struct line_node * new_line_node(int line_num){
    struct line_node * initial_node = malloc(sizeof(struct line_node));
    initial_node->front_node = NULL;
    initial_node->behind_node = NULL;
    initial_node->line_num= line_num;
    initial_node->line_freq=1;

    
    initial_node->total_length = 1;
    initial_node->is_initial = 1;

    return initial_node;
};

struct line_node * add_line_node(struct line_node * initial, int line_num){
    //I'll add the line nodes with smaller line number order
    //It includes the adding new line node.
    
    struct line_node* temp = initial;

    //when the first node is NULL
    if(initial==NULL){
        struct line_node* adding_node = new_line_node(line_num);

        return adding_node;
    }
    //check whether it's in the list or not.
    else{
        for(int i=0;i<initial->total_length;i++){
            //when the document is same
            if(temp->line_num==line_num){
                temp->line_freq++;

                return initial;
            }
            temp=temp->behind_node;
        }
    }

    struct line_node* adding_node = new_line_node(line_num);

    //just add the node, at the end of the list.
    //Because the input will come with line order, there isn't need of sorting
    temp=initial;
    for(int i=1;i<initial->total_length;i++){
        temp=temp->behind_node;
    }
    temp->behind_node=adding_node;
    adding_node->front_node=temp;

    initial->total_length++;
    return initial;
};

struct doc_node * new_doc_node(char *doc_name, int line_num){
    // the doc_name will be shown frequently, so I'll save it specific area, to save the memory.
    struct doc_node * initial_node = malloc(sizeof(struct doc_node));
    initial_node->front_node = NULL;
    initial_node->behind_node = NULL;
    initial_node->doc_name= make_lower(doc_name);
    initial_node->doc_freq=1;

    
    initial_node->total_length = 1;
    initial_node->is_initial = 1;
    initial_node->line_node = add_line_node(NULL,line_num);

    return initial_node;
}

struct doc_node * add_doc_node(struct doc_node * initial, char *doc_name, int line_num){
    //I'll add the document nodes with higher frequency order
    //It includes the adding new document node.
    struct doc_node* temp = initial;

    //when the first node is NULL
    if(initial==NULL){
        struct doc_node* adding_node = new_doc_node(doc_name,line_num);

        return adding_node;
    }
    //check whether it's in the list or not.
    else{
        for(int i=0;i<initial->total_length;i++){
            //when the document is same
            if(string_compare(temp->doc_name,doc_name)==0){
                //printf("It's already in the list, %s\n",doc_name);
                temp->doc_freq++;
                temp->line_node = add_line_node(temp->line_node,line_num);

                return initial;
            }
            temp=temp->behind_node;
        }
    }

    struct doc_node* adding_node = new_doc_node(doc_name,line_num);
    //just add the node, at the end of the list.

    temp=initial;
    for(int i=1;i<initial->total_length;i++){
        temp=temp->behind_node;
    }
    temp->behind_node=adding_node;
    adding_node->front_node=temp;

    initial->total_length++;
    return initial;
}

struct term_node * new_term_node(char * content, char * doc_name, int line_num){
    //create first node.
    //printf("In term adder, %s\n", content);

    struct term_node* initial_node = malloc(sizeof(struct term_node));
    initial_node->front_node = NULL;
    initial_node->behind_node = NULL;
    initial_node->term = make_lower(content);
    initial_node->is_initial = 1;
    initial_node->term_freq = 1;
    //Only meaningful when it's first node
    initial_node->total_length = 1;
    //doc_node를 추가해줘야함. 
    initial_node->doc_node = add_doc_node(NULL,doc_name,line_num);

    return initial_node;
}

struct term_node * add_node(struct term_node * first, char * content, char * doc_name, int line_num){
    //I'll add the nodes with dictionary order
    //It includes the adding new term node.
    
    struct term_node* temp = first;

    //when the first node is NULL
    if(first==NULL){
        struct term_node* adding_node = new_term_node(content,doc_name,line_num);
        return adding_node;
    }
    //check whether it's in the list or not.
    else{
        for(int i=0;i<first->total_length;i++){
            //when the term is same
            if(string_compare(temp->term,content)==0){
                //printf("It's already in the list, %s\n",content);
                temp->term_freq++;
                temp->doc_node=add_doc_node(temp->doc_node,doc_name,line_num);

                return first;
            }
            temp=temp->behind_node;
        }
    }

    struct term_node* adding_node  = new_term_node(content,doc_name,line_num);


    //when the input term is earlier than first node
    if(string_compare(content,first->term)==2){
        adding_node->total_length=first->total_length+1;
        first->is_initial=0;
        adding_node->behind_node=first;
        first->front_node=adding_node;

        return adding_node;
    }
    else{
        adding_node->is_initial=0;
        //if length is one
        if(first->total_length==1){
            first->behind_node=adding_node;
            adding_node->front_node=first;
            
            first->total_length++;
            return first;
        }
        temp=first;
        //search the proper position of new term node. 
        for(int i=1;i<first->total_length;i++){
            temp=temp->behind_node;
            if(string_compare(content,temp->term)==2){
                struct term_node * front_save = temp->front_node;
                front_save->behind_node=adding_node;
                temp->front_node=adding_node;
                adding_node->front_node=front_save;
                adding_node->behind_node=temp;

                first->total_length++;
                return first;
            }
        }
        //when the new input should be the last node
        temp->behind_node=adding_node;
        adding_node->front_node=temp;

        first->total_length++;
        return first;
    }
    
}



int delete_line_node(struct line_node * first){
    if(first==NULL){
        error_process(2);
        return -1;
    }
    if(first->is_initial!=1){
        error_process(4);
        return -1;
    }
    struct line_node * temp = first;
    int length=first->total_length-1;
    for(int i=0;i<length;i++){
        temp=temp->behind_node;
        free(temp->front_node);
    }
    free(temp);

    return 0;
}

int delete_doc_node(struct doc_node * first){
    if(first==NULL){
        error_process(2);
        return -1;
    }
    if(first->is_initial!=1){
        error_process(4);
        return -1;
    }
    struct doc_node * temp = first;
    int length=first->total_length-1;
    for(int i=0;i<length;i++){
        //free(temp->doc_name);
        delete_line_node(temp->line_node);
        temp=temp->behind_node;
        free(temp->doc_name);
        temp->doc_name=NULL;
        free(temp->front_node);
    }
    //free(temp->doc_name);
    delete_line_node(temp->line_node);
    free(temp->doc_name);
    temp->doc_name=NULL;
    free(temp);

    return 0;

}

int delete_term_node(struct term_node * first){
    if(first==NULL){
        error_process(2);
        return -1;
    }
    if(first->is_initial!=1){
        error_process(4);
        return -1;
    }
    struct term_node * temp = first;
    int length=first->total_length-1;
    for(int i=0;i<length;i++){
        free(temp->term);
        delete_doc_node(temp->doc_node);
        temp=temp->behind_node;
        free(temp->front_node);
    }
    free(temp->term);
    delete_doc_node(temp->doc_node);
    free(temp);

    printf("freeing all term node is done\n");
    return 0;
}
