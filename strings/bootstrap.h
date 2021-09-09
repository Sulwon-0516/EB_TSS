#ifndef BOOTSTRAP_H
#define BOOTSTRAP_H


char * itoa(int input);
int search_from_list(char ** token_input, int token_num, struct term_node * initial, char ** result);
struct term_node * making_inverted_index(char * filepath, struct term_node * initial);
int bootstrapping(char * path_name, struct term_node ** initial);


#endif