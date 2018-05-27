#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <stdbool.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>
#include "FunctionUtils.h"
#include "FormatUtils.h"
#include "MyWrappers.h"
#include "SplitLines.h"
#include "ExecuteCommands.h"


pid_t child_pid = 0;    //process is of the child
int fdOutput;   //file descriptor to handle output
int fdError;    //file descriptor to handle error
int returnCode;     //return code of the execution of the command
char *absoluteOutFilePath; //absolute path of out file log
char *absoluteErrFilePath; //absolute path of err file log
int outFlag;    //variable used to set if the out file has the same name of the err file
int errFlag;    //variable used to set if the err file has the same name of the out file

//used to execute a command with no piping or redirection
void executeSingleCommand(char **cmd){
    //int old_stdout = dup(1);
    bool isExternCommand = checkExternCommand(cmd[0]);
    child_pid = myfork();

    if(child_pid==0){
        
        if(!isExternCommand)
            dup2(fdOutput,1);

        if(execvp(cmd[0],cmd)==-1){
            fprintf(stderr,"%s\n",strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    else{
        wait(&returnCode);
        char *buf = malloc(maxLength * sizeof(char));

        char *subCommand = malloc(MAXLENSTR *sizeof(char));
        strcpy(subCommand,cmd[0]);
        int i=1;
        while(cmd[i]!=NULL){

            strcat(subCommand," ");
            strcat(subCommand,cmd[i]);
            i++;
        }
        if(returnCode==0){

            
            
            if(!isExternCommand){
                mylseek(fdOutput,0,0);
                int buflen = myread(fdOutput,buf,maxLength);
                //printf("buflen: %d\n",buflen);
                buf[buflen-1]='\0';
            }else{
                buf[0] = '\0';
            }

            scrivi(absoluteOutFilePath,formatoLog,buf,subCommand,outFlag);
        }else{
            mylseek(fdError,0,0);
            int buflen = myread(fdError,buf,maxLength);
            scrivi(absoluteErrFilePath,formatoLog,buf,subCommand,errFlag);
        }
        free(buf);
        free(subCommand);

    }

}

//used to execute command with piping 
void executeWithPipe(char ***cmd){


    int pToFile[2];     //pipe used to read the subcommand ouput
    int pToChain[2];    //pipe used to pass next input to the chain of pipes

    int nextInput = 0;

    bool isExternCommand = checkExternCommand((*cmd)[0]);
    if(!isExternCommand){

        while(*cmd !=NULL){
            // printf("Entro nel while\n");
            char *subCommand = malloc(MAXLENSTR *sizeof(char));
            strcpy(subCommand,*(cmd)[0]);
            int i=1;
            while((*cmd)[i]!=NULL){

                strcat(subCommand," ");
                strcat(subCommand,(*cmd)[i]);
                i++;
            }
            // printf("Creo il subCommand\n");
            mypipe(pToFile);
            child_pid  = myfork();



            if(child_pid==0){
                
                dup2(nextInput,0);

                if(*(cmd+1)!=NULL){

                    dup2(pToFile[WRITE],1);

                }else{

                    dup2(fdOutput,1);

                }

                myclose(pToFile[READ]);
                
                if(execvp((*cmd)[0],*(cmd))==-1){
                    fprintf(stderr,"%s\n",strerror(errno));
                    exit(EXIT_FAILURE);
                }

            }else{
                wait(&returnCode);
                // printf("Sono il padre\n");
                myclose(pToFile[WRITE]);

                char *buf = malloc(maxLength * sizeof(char));

                
                
                if(*(cmd+1)!=NULL){

                    mypipe(pToChain);

                    if(returnCode==0){

                        int buflen = myread(pToFile[READ],buf,maxLength);
                        //buf[buflen-1]  ='\0';
                        mywrite(pToChain[WRITE],buf,buflen);
                        scrivi(absoluteOutFilePath,formatoLog,buf,subCommand,outFlag);
                    }else{
                        mywrite(pToChain[WRITE],"\0",0);
                        mylseek(fdError,0,0);
                        int buflen = myread(fdError,buf,maxLength);
                        buf[buflen-1]  ='\0';
                        scrivi(absoluteErrFilePath,formatoLog,buf,subCommand,errFlag);
                    }
                    myclose(pToChain[WRITE]);
                    nextInput = pToChain[READ];
                    
                }else{
                    if(returnCode==0){

                        mylseek(fdOutput,0,0);
                        int buflen = myread(fdOutput,buf,maxLength);
                        buf[buflen-1]  ='\0';
                        scrivi(absoluteOutFilePath,formatoLog,buf,subCommand,outFlag);

                    }else{

                        mylseek(fdError,0,0);
                        int buflen = myread(fdError,buf,maxLength);
                        buf[buflen-1]  ='\0';
                        scrivi(absoluteErrFilePath,formatoLog,buf,subCommand,errFlag);

                    }
                }
                cmd++;
                free(buf);
            }

            free(subCommand);
        }
    }else{
        fprintf(stderr,"This type of command is not supported\n");
    }
   
}

//used to execute command with the > redirection command
void executeWithRedirection(char ***command){

    
    bool isExternCommand = checkExternCommand((*command)[0]);
    char ***executable = command;
    int lastFile = -1;
    command++;

    
   
    if(!isExternCommand){
        int pForRedirect[2];
        mypipe(pForRedirect);
        child_pid = myfork();

        if(child_pid==0){

            myclose(pForRedirect[READ]);
            dup2(pForRedirect[WRITE],1);

            if(execvp((*executable)[0],*executable)==-1){
                    fprintf(stderr,"%s\n",strerror(errno));
                    exit(EXIT_FAILURE);
                }

        }else{

            wait(&returnCode);
            myclose(pForRedirect[WRITE]);
            char *subCommand = malloc(MAXLENSTR *sizeof(char));
            strcpy(subCommand,*(executable)[0]);
            int i=1;
            while((*executable)[i]!=NULL){

                strcat(subCommand," ");
                strcat(subCommand,(*executable)[i]);
                i++;
            }
            if(returnCode==0){
                scrivi(absoluteOutFilePath, formatoLog,"\0",subCommand,outFlag);
                
                while(*command!=NULL){

                    int fd = myopenwm((*command)[0],O_RDWR|O_CREAT|O_TRUNC,0666);
                    if(*(command+1)==NULL){
                        
                    
                        char *buf = malloc(maxLength * sizeof(char));
                        int buflen=myread(pForRedirect[READ],buf,maxLength);
                        mywrite(fd,buf,buflen);
                        free(buf);
                        
                    }
                    close(fd);
                    command++;
                }
            }else{
                char *buf = malloc(maxLength * sizeof(char));
                mylseek(fdError,0,0);
                int buflen = myread(fdError,buf,maxLength);
                buf[buflen-1]  ='\0';
                scrivi(absoluteErrFilePath,formatoLog,buf,subCommand,errFlag);
                free(buf);
            }

            free(subCommand);

            myclose(pForRedirect[READ]);
            
            
        }
    }else{
         fprintf(stderr,"This type of command is not supported\n");
    }

}


//it controls if the input is a cd command and it changes directory. It does not support folder with blank spaces
bool changeDirectory(char **cmd){
    bool res =false;
    char *home="/home";
    if(strcmp(cmd[0],"cd")==0){
        res = true;

        
        if(cmd[1]==NULL){
            mychdir(home);
            return res;

        }

        int i = 2;
        char *path = malloc(MAXLENSTR * sizeof(char));
        strcpy(path,cmd[1]);
        while(cmd[i] != NULL){
            
            strcat(path," ");
            strcat(path,cmd[i]);
            i++;
        }

        if((strcmp(cmd[1], "~")==0) || (strcmp(cmd[1], "~/")==0)){
            mychdir(home);
        }else if(mychdir(cmd[1])<0){
            fprintf(stderr,"cd: %s: No such file or directory\n", path);
            returnCode=256;
        }
        char *buf = malloc(maxLength * sizeof(char));
        char *subCommand = malloc(MAXLENSTR *sizeof(char));
        strcpy(subCommand,cmd[0]);
        i=1;
        while(cmd[i]!=NULL){

            strcat(subCommand," ");
            strcat(subCommand,cmd[i]);
            i++;
        }
         if(returnCode==0){

                    
                    scrivi(absoluteOutFilePath,formatoLog,"",subCommand,outFlag);

        }else{

            mylseek(fdError,0,0);
            int buflen = myread(fdError,buf,maxLength);
            buf[buflen-1] = '\0';
            scrivi(absoluteErrFilePath,formatoLog,buf,subCommand,errFlag);
        }
        free(buf);
        free(subCommand);
    }
    
    return res;
}