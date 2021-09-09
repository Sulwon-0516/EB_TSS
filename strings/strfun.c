#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "strfun.h"
#include "error.h"
#include "tc_malloc.h"




struct str_w_len * string_merge_lots(char ** str_ary, int n_str){
    struct str_w_len * result = tc_malloc(sizeof(struct str_w_len));
    int cnt=0;
    result->tot_len = 0;
    for(int i=0;i<n_str;i++){
        result->tot_len += (string_length(str_ary[i])+1);
    }
    result->str = tc_malloc((result->tot_len+8)*sizeof(char));
    cnt = 8;
    for(int i=0;i<n_str;i++){
        for(int j=0;j<=string_length(str_ary[i]);j++){
            result->str[cnt] = str_ary[i][j];
            cnt++;
        }
    }
    result->str[cnt-1] = EOF;
    return result;
}


char * string_merge(char * str_1, char * str_2){
    int str_1_len = string_length(str_1);
    int str_2_len = string_length(str_2);
    char * result = malloc(sizeof(char)*(str_1_len+str_2_len+1));
    int i;
    for(i=0;i<str_1_len;i++){
        result[i]=str_1[i];
    }
    for(int j=0;j<str_2_len;j++){
        result[i]=str_2[j];
        i++;
    }
    result[i] = '\n';
    
    return result;
}

char * get_ext(char * file_name){
    //I want to get the type of the file. 
    int length= string_length(file_name);
    int i=length-1;

    
    
    while(1){
        if(i<0||file_name[i]=='/'){
            //there is no dot.
            return NULL;
        }
        //when it meets the '.'
        if(file_name[i]=='.'){
            break;
        }
        i--;
    }
    //now, checkk the length.
    char * ext = malloc(sizeof(char)*(length-i));
    i++;
    for(int j=0;i<=length;i++){
        ext[j]=file_name[i];
        j++;
    }

    return ext;
}


int string_length(char * str){
    if(str==NULL){
        //severe error. exit()
        error_process(1);
        assert(0);
        return -1;
    }
    int length=0;
    while(1){
        if(str[length]=='\0' || str[length]=='\n' || str[length]==EOF)break;
        length++;
    }
    return length;

}


char * string_copy_wo_EOF(char * str_i){
    if(str_i==NULL){
        //severe error. exit()
        error_process(2);
        return NULL;
    }
    int i;
    int length= string_length(str_i);
    char * str_o=(char *) malloc((length+1)*sizeof(char));
    for( i=0; i<length; i++){
        *(str_o+i)=*(str_i+i);
    }
    *(str_o+i) = '\0';

    return str_o;

}

char * string_copy(char * str_i){
    if(str_i==NULL){
        //severe error. exit()
        error_process(2);
        return NULL;
    }
    
    int length= string_length(str_i);
    char * str_o=(char *) malloc((length+1)*sizeof(char));
    for(int i=0; i<=length; i++){
        *(str_o+i)=*(str_i+i);
    }

    return str_o;

}


char * make_lower(char * str){
    //it keeps the original string. 
    //there is posibility of unfreeing issue.
    //it should be used instead of the str_copy.
    int length = string_length(str);
    char * lower_str=(char *) malloc(sizeof(char)*(length+1));
    for(int i=0;i<length;i++){
        if(0x40<str[i]&&str[i]<0x5B){
            lower_str[i] = str[i] + 32;
        }
        else{
            lower_str[i]=str[i];
        }
    }
    lower_str[length]='\0';
    return lower_str;
}


char string_compare(char * str_1, char * str_2){
    //if input str_1 is larger than str_2, return 1, 
    //str_2 is larger than str_1,return 2,
    //if they are fully same, return 0
    char * lower_str_1 = make_lower(str_1);
    char * lower_str_2 = make_lower(str_2);
    int len_1=string_length(str_1);
    int len_2=string_length(str_2);
    int i=0;
    if(len_1>len_2){
        while(i<len_2){
            if(lower_str_1[i]>lower_str_2[i]){
                //when str_1 is larger.
                free(lower_str_1);
                free(lower_str_2);
                return 1;
            }
            else if(lower_str_1[i]<lower_str_2[i]){
                //when str_2 is larger.
                free(lower_str_1);
                free(lower_str_2);
                return 2;
            }
            i++;
        }
        //Only the length of str_1 is longer

        free(lower_str_1);
        free(lower_str_2);
        return 1; 
    }
    else if(len_1<len_2){
        while(i<len_1){
            if(lower_str_1[i]>lower_str_2[i]){
                //when str_1 is larger.
                free(lower_str_1);
                free(lower_str_2);
                return 1;
            }
            else if(lower_str_1[i]<lower_str_2[i]){
                //when str_2 is larger.
                free(lower_str_1);
                free(lower_str_2);
                return 2;
            }
            i++;
        }
        //Only the length of str_1 is longer

        free(lower_str_1);
        free(lower_str_2);
        return 2; 
    }
    else{
        //when two has same length;
        while(i<len_2){
            if(lower_str_1[i]>lower_str_2[i]){
                //when str_1 is larger.
                free(lower_str_1);
                free(lower_str_2); 
                return 1;
            }
            else if(lower_str_1[i]<lower_str_2[i]){
                //when str_2 is larger.
                free(lower_str_1);
                free(lower_str_2); 
                return 2;
            }
            i++;
        }
        free(lower_str_1);
        free(lower_str_2); 
        return 0;
    }
}


int token_num(char * str_i){
    int length=string_length(str_i);
    int tok_len=0;
    int tok_int=0;
    char * lower_str_i = make_lower(str_i);

    int tok_num=0;
    //first, find the number of token.
    while(tok_int<length+1){
        if(0x60<*(lower_str_i+tok_int)&&*(lower_str_i+tok_int)<0x7B){
            tok_len++;
            
        }
        else{
            if(tok_len!=0){
                tok_num++;
            }
            tok_len=0;
            //when
        }
        tok_int++;
    }

    free(lower_str_i);
    return tok_num;
}

char ** string_token(char * str_i){
    int length=string_length(str_i);
    int tok_len=0;
    int tok_int=0;
    char * lower_str_i = make_lower(str_i);

    int tok_num=token_num(lower_str_i);
    if(tok_num==0){
        return NULL;
    }
    //second define the list of token array.
    char ** tok_list = (char**) malloc(sizeof(char *)*tok_num);
    //let's save the tokens

    tok_num=0;
    tok_int=0;
    while(tok_int<length+1){
        if(0x60<*(lower_str_i+tok_int)&&*(lower_str_i+tok_int)<0x7B){
            tok_len++;
        }
        else{
            if(tok_len!=0){
                char * temp =(char*) malloc(sizeof(char)*(tok_len+1));
                for(int i=0;i<tok_len;i++){
                    *(temp+i)=*(lower_str_i+tok_int-tok_len+i);
                } 
                *(temp+tok_len)='\0';
                *(tok_list+tok_num)=temp;
                tok_num++;
            }
            tok_len=0;
            //when
        }
        tok_int++;
    }
    free(lower_str_i);

    return tok_list;

}
