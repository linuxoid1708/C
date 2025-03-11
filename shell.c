// gcc shell.c ./lib/linenoise.c -o shell

#include <stdio.h>
#include <stdlib.h>
#include "./lib/linenoise.h"
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define PROMPT "$ "
#define HISTORY_LENGHT 1024
#define MAX_ARGS 2048
#define TOKEN_SEPS " \t"

int s_read(char *input, char **args){
    int i = 0;
    char *token = strtok(input,TOKEN_SEPS);
    while(token != NULL && i < (MAX_ARGS -1)){
        args[i] = token; // args[i++] = token
        i++;
        token = strtok(NULL,TOKEN_SEPS);
    }
    args[i] = NULL;
    return i;
}


int s_execute(char *cmd, char **cmd_args){
    fprintf(stdout,"Executing '%s' !\n",cmd);

    int status;
    pid_t pid;

    pid = fork();
    if(pid < 0){
        fprintf(stderr,"Cound not execute\n");
        return -1;
    }
    if(pid == 0){
        execv(cmd, cmd_args);
    } else {
        if(waitpid(pid, &status, 0) != pid){
            fprintf(stderr,"Could not wait for kiddo!\n");
            return -1;
        }
    }
    return status;
}

int main(void){
    if(!linenoiseHistorySetMaxLen(HISTORY_LENGHT)) {
        fprintf(stderr,"Could not set lineoise history!");
        exit(1);
    }

    char *line;
    char *args[MAX_ARGS];
    while((line = linenoise(PROMPT)) != NULL){

        //read step
        int args_read = s_read(line,args);

        fprintf(stdout,"Read %d args\n",args_read);
        for(int i=0 ; i < args_read;i++){
            fprintf(stdout,"arg[%d] = %s\n",i,args[i]);
        }
        //skip empty lines 
        if(args_read == 0){
            linenoiseFree(line);
            continue;
        }

        // TODO : eval + print step
        char *cmd = args[0];
        char **cmd_args = args;
        s_execute(cmd,cmd_args);

        linenoiseHistoryAdd(line);
        linenoiseFree(line);
    }
    return 0;
}