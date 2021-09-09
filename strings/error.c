#include <stdio.h>
#include <stdlib.h>
#include "error.h"

void memory_free(){
    fprintf(stderr,"freeing all allocated memory");
    

}


void log_skipped_process(int code){
    FILE *fp= fopen("log.txt","a");


    switch(code){
        case 1:{
            fprintf(fp,"Code : %d, It's failed to get stat of the file\n",code);
            break;
        }



    }


}



void error_process(int error_code){
    switch(error_code){
        case 1:{
            fprintf(stderr,"Code : %d, string length comparison error. NULL pointer input\n", error_code);
            memory_free();
            break;
        }
        case 2:{
            fprintf(stderr,"Code : %d, NULL pointer input\n", error_code);
            memory_free();
            break;
        }
        case 3:{
            fprintf(stderr,"Code : %d, the int put directory is not valid. or possible error on opendir\n",error_code);
            memory_free();
            break;
        }
        case 4:{
            fprintf(stderr,"Code : %d, the input node isn't first node\n",error_code);
            break;

        }
        case 5:{
            fprintf(stderr,"Code : %d, failed to open file. check the file name\n",error_code);
            memory_free();
            break;
        }
        case 6:{
            fprintf(stderr,"Code : %d, Invalid input. Input should follow following structure \"search\" [terms]. If you want to quit, input \"quit\".\n",error_code);
            break;
        }
        



    }




}