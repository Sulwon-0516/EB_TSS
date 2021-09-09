#ifndef STRFUN_H
#define STRFUN_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct str_w_len{
    char * str;
    int tot_len;
};

struct str_w_len * string_merge_lots(char ** str_ary, int n_str);
char * string_merge(char * str_1, char * str_2);
char * get_ext(char * file_name);
int string_length(char * str);
char * string_copy_wo_EOF(char * str_i);
char * string_copy(char * str_i);
char * make_lower(char * str);
char string_compare(char * str_1, char * str_2);
int token_num(char * );
char ** string_token(char * str_i);

#endif