#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <strings.h>
#include "linked_list.h"
#include "strfun.h"
#include "error.h"
#include "bootstrap.h"


char * itoa(int input){
    char buf[100];
    sprintf(buf,"%d\n",input);
    char * result = string_copy(buf);
    return result;
}

int search_from_list(char ** token_input, int token_num, struct term_node * initial, char ** result){
    
    char* keyword = token_input[1];
    struct term_node * temp=initial;
    struct doc_node * temp_doc;
    struct line_node * temp_line;
    int n_result = 0;
    char * temp_ch;
    while(1){
        if(string_compare(temp->term,keyword)==0){
            //when it meets the term.
            temp_doc=temp->doc_node;
            temp_line=temp_doc->line_node;
            temp_ch = string_merge(temp_doc->doc_name, ": line #");
            result[n_result] = string_merge(temp_ch,itoa(temp_line->line_num));
            free(temp_ch);
            n_result++;
            while(temp_line->behind_node!=NULL){
                temp_line=temp_line->behind_node;
                temp_ch = string_merge(temp_doc->doc_name, ": line #");
                result[n_result] = string_merge(temp_ch,itoa(temp_line->line_num));
                free(temp_ch);
                n_result++;
            }
            while(temp_doc->behind_node!=NULL){
                temp_doc=temp_doc->behind_node;
                temp_ch = string_merge(temp_doc->doc_name, ": line #");
                result[n_result] = string_merge(temp_ch,itoa(temp_line->line_num));
                free(temp_ch);
                n_result++;
                while(temp_line->behind_node!=NULL){
                    temp_line=temp_line->behind_node;
                    temp_ch = string_merge(temp_doc->doc_name, ": line #");
                    result[n_result] = string_merge(temp_ch,itoa(temp_line->line_num));
                    free(temp_ch);
                    n_result++;
                }   
            }
            break;
        }
        else if(temp->behind_node==NULL){
            //printf("There isn't any doc with [%s]\n",keyword);
            break;
        }
        else{
            temp=temp->behind_node;
        }
    }

    return n_result;
}

struct term_node * making_inverted_index(char * filepath, struct term_node * initial){
    //I define the buffer as NULL to use the getline()'s own temporal buffer
    char *buf = NULL;
    char ** parsed_line;
    int token_n=0;
    size_t buf_size = 0;
    int line_num = 0;

    fprintf(stderr,"what the\n");
    //open the file
    FILE *fp = fopen(filepath, "r");

    if(fp==NULL){
        error_process(5);
        return initial;
    }

    

    getline(&buf, &buf_size, fp);
    line_num++;
    if(!feof(fp)){
        parsed_line = string_token(buf);
        token_n = token_num(buf);
        for(int i=0; i< token_n; i++){
            initial = add_node(initial,parsed_line[i], filepath,line_num);
            free(parsed_line[i]);
        }
        free(parsed_line);       
    }
    while(!feof(fp)){
        getline(&buf, &buf_size, fp);
        parsed_line=string_token(buf);
        token_n=token_num(buf);
        line_num++;

        for(int i=0; i< token_n; i++){
            initial = add_node(initial,parsed_line[i], filepath, line_num);
            free(parsed_line[i]);
        }
        free(parsed_line);  
        //read all lines
    }
    

    //I'm not sure it's true.
    //free(buf);

    fclose(fp);
    printf("Successfully added %s \n", filepath);
    free(buf);

    return initial;
}

int bootstrapping(char * path_name, struct term_node ** initial_saved){
    //this is the directory name, what I'll use.
    DIR *dir=NULL;
    struct dirent * file = NULL;
    struct stat info;
    char filepath[1024];


    
    //1st, I have to check the path_name is valid, or not. 
    dir = opendir(path_name);
    if(dir==NULL){
        //when the directory isn't valid.
        error_process(3);
        return -1;
    }

    //2nd search the file, and make list of 
    while((file=readdir(dir))!=NULL){
        
        //first I need to remove the case '.' and '..'
        if(string_compare(file->d_name,".")==0||string_compare(file->d_name,"..")==0){
            continue;
        }

        //get the information of the file.
        //make the absolute path of the file.
        sprintf(filepath,"%s/%s",path_name,file->d_name);
        fprintf(stdout,"%s\n",filepath);
        if(stat(filepath,&info)==-1){
            //when it fail to get the buffer inform.
        }

        if(S_ISDIR(info.st_mode)){
            //when there is sub direcotry
            bootstrapping(filepath,initial_saved);
            continue;
        }
        else if(S_ISREG(info.st_mode)){
            //when it's general file
            //check whether it's text file.
            

            char * ext;
            ext = get_ext(filepath);
            //printf("%s\n",ext);
            //fflush(stdout);
            fprintf(stderr,"%s\n",ext);
            
            if(ext!=NULL){
                
                //printf("%s, the ext : %s", file->d_name,ext);
                if(string_compare(ext,"txt")==0){
                    //when it's text file
                    *initial_saved = making_inverted_index(filepath,*initial_saved);
                }
                free(ext);
            }
        }
    }

    closedir(dir);
    return 0;
}