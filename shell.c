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
#include "FunctionUtils.h"
#include "FormatUtils.h"
#include "MyWrappers.h"




#define MAXLENSTR 256
#define MAXCMDNUM 20
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define READ 0
#define WRITE 1
#define WELCOME "\x1b[33m   _____ _          _ _   _             __  __ _____  _____          \n  / ____| |        | | | | |           |  \\/  |  __ \\|  __ \\   /\\    \n | (___ | |__   ___| | | | |__  _   _  | \\  / | |  | | |  | | /  \\   \n  \\___ \\| '_ \\ / _ \\ | | | '_ \\| | | | | |\\/| | |  | | |  | |/ /\\ \\  \n  ____) | | | |  __/ | | | |_) | |_| | | |  | | |__| | |__| / ____ \\ \n |_____/|_| |_|\\___|_|_| |_.__/ \\__, | |_|  |_|_____/|_____/_/    \\_\\\n                                 __/ |                               \n                                |___/                                \n\n\x1b[0m"
#define GOODBYE "\e[1;35m  ____                   \n |  _ \\                  \n | |_) |_   _  ___       \n |  _ <| | | |/ _ \\      \n | |_) | |_| |  __/_ _ _ \n |____/ \\__, |\\___(_|_|_)\n         __/ |           \n        |___/            \n\n\x1b[0m"
//VARIABILI GLOBALI
char startCwd[MAXLENSTR]; //starting current working directory
char cwd[MAXLENSTR];    //current working directory
char *absoluteOutFilePath; //absolute path of out file log
char *absoluteErrFilePath; //absolute path of err file log
char *commandLine;  //input string
int fdOutput;   //file descriptor to handle output
int fdError;    //file descriptor to handle error
pid_t child_pid = 0;    //process is of the child
int returnCode;     //return code of the execution of the command
int commandId;

//it get the start current working directory
void takeStartCwd(){

    mygetcwd(startCwd,sizeof(startCwd));

}

//it creates absolute path fles. Used for out and err log files
char *createAbsolutePath(char *relativePath,char *absolutePath){

    strcpy(absolutePath,startCwd);
    strcat(absolutePath,"/");
    strcat(absolutePath,relativePath);
}

bool checkExternCommand(char * cmd){

    bool res = false;
    if(strcmp(cmd,"nano")==0){
        res =  true;
    }
    if(strcmp(cmd,"vim")==0){
         res =  true;
    }
    if(strcmp(cmd,"python3")==0){
         res =  true;
    }
    if(strcmp(cmd,"python")==0){
         res =  true;
    }
    if(strcmp(cmd,"emacs")==0){
         res =  true;
    }
    if(strcmp(cmd,"pico")==0){
         res =  true;
    }
    if(strcmp(cmd,"grep")==0){
         res =  true;
    }
    

    return res;
    
}
void scrivi(char *path,int format,char *output,char *subCommand,int writeFlag){


     bool res = false;
    switch(format){
        case FORMAT_TEXT:
            res = scriviSuTxt(path,commandId,commandLine,subCommand,output,&returnCode, writeFlag); 
            break;
        case FORMAT_CSV:
            res = scriviSuCsv(path,commandId,commandLine,subCommand,output,&returnCode, writeFlag);
            break;
        case FORMAT_HTML:
            res = scriviSuHtml(path,commandId,commandLine,subCommand,output,&returnCode, writeFlag);
            break;
        default: 
            res = scriviSuTxt(path,commandId,commandLine,subCommand,output,&returnCode, writeFlag);

    }

    if(!res)

        fprintf(stderr,"Error printing log file\n");
}
//it prints pwd every cycle
void printCwd(){

    char *shell = malloc(2* MAXLENSTR *sizeof(char));

    mygetcwd(cwd,sizeof(cwd));
    
    strcpy(shell,ANSI_COLOR_GREEN);
    strcat(shell,"\nshell:");
    strcat(shell,ANSI_COLOR_BLUE);
    strcat(shell,cwd);
    strcat(shell,"$");
    strcat(shell,ANSI_COLOR_RESET);
    
    printf("%s",shell);

    free(shell);
    


}

void sigIntHandler(int signum){

    if(child_pid!=0)
        mykill(child_pid,SIGTERM);
    printf("\x1b[31mType 'quit' to terminate\n\x1b[0m");
    // printCwd();
    fflush(stdout);
}
//it closes tmp files
void clearTmpFiles(){
    close(fdOutput);
    close(fdError);
    remove("/tmp/fdError");
    remove("/tmp/fdOutput");
}

char ***splitCommandLine(char* command){

    char ***cmd=malloc(MAXCMDNUM);
    int commandCount = 0;
    int wordCount = 0;

    char *endLine;

    char *actualCommand = strtok_r(command,"|", &endLine);

    while(actualCommand!=NULL){

        cmd[commandCount] =malloc(MAXCMDNUM);
        char *endCommand;
        char *actualWord = strtok_r(actualCommand, " ", &endCommand);

        while(actualWord!=NULL){

            //placeholder for cd check
            cmd[commandCount][wordCount] = malloc(MAXLENSTR * sizeof(char));
            cmd[commandCount][wordCount] = actualWord;
            actualWord = strtok_r(NULL," ", &endCommand);

            wordCount++;
        }
        cmd[commandCount][wordCount] = malloc(MAXLENSTR * sizeof(char));
        cmd[commandCount][wordCount] = NULL;
        commandCount++;
        actualCommand = strtok_r(NULL,"|", &endLine);
        wordCount=0;
        
    }
    cmd[commandCount] = malloc(MAXCMDNUM);
    cmd[commandCount]  = NULL;

    return cmd;

}




bool executeSingleCommand(char ***cmd){
    //int old_stdout = dup(1);
    bool isExternCommand = checkExternCommand(*(cmd)[0]);
    child_pid = fork();

    if(child_pid<0){
        exit(EXIT_FAILURE);
    }else if(child_pid==0){
        
        if(!isExternCommand)
            dup2(fdOutput,1);

        if(execvp(*(cmd)[0],*cmd)==-1){
            fprintf(stderr,"%s\n",strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    else{
        wait(&returnCode);
        char *buf = malloc(maxLength * sizeof(char));

        char *subCommand = malloc(MAXLENSTR *sizeof(char));
        strcpy(subCommand,*(cmd)[0]);
        int i=1;
        while((*cmd)[i]!=NULL){

            strcat(subCommand," ");
            strcat(subCommand,(*cmd)[i]);
            i++;
        }
        if(returnCode==0){

            
            
            if(!isExternCommand){
                lseek(fdOutput,0,0);
                int buflen = myread(fdOutput,buf,maxLength);
                //printf("buflen: %d\n",buflen);
                buf[buflen-1]='\0';
            }else{
                buf[0] = '\0';
            }

            scrivi(absoluteOutFilePath,formatoLog,buf,subCommand,WRITE_ON_OUTPUT);
        }else{
            lseek(fdError,0,0);
            int buflen = myread(fdError,buf,maxLength);
            scrivi(absoluteErrFilePath,formatoLog,buf,subCommand,WRITE_ON_ERROR);
        }
        free(buf);
        free(subCommand);

    }

}



void executeWithPipe(char ***cmd){


    int pToFile[2];     //pipe used to read the subcommand ouput
    int pToChain[2];    //pipe used to pass next input to the chain of pipes

    int nextInput = 0;


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
                    scrivi(absoluteOutFilePath,formatoLog,buf,subCommand,WRITE_ON_OUTPUT);
                }else{
                    mywrite(pToChain[WRITE],"\0",0);
                    lseek(fdError,0,0);
                    int buflen = myread(fdError,buf,maxLength);
                    buf[buflen-1]  ='\0';
                    scrivi(absoluteErrFilePath,formatoLog,buf,subCommand,WRITE_ON_ERROR);
                }
                myclose(pToChain[WRITE]);
                nextInput = pToChain[READ];
                
            }else{
                 if(returnCode==0){

                    lseek(fdOutput,0,0);
                    int buflen = myread(fdOutput,buf,maxLength);
                    buf[buflen-1]  ='\0';
                    scrivi(absoluteOutFilePath,formatoLog,buf,subCommand,WRITE_ON_OUTPUT);

                }else{

                    lseek(fdError,0,0);
                    int buflen = myread(fdError,buf,maxLength);
                    buf[buflen-1]  ='\0';
                    scrivi(absoluteErrFilePath,formatoLog,buf,subCommand,WRITE_ON_ERROR);

                }
            }
            cmd++;
            free(buf);
        }

        free(subCommand);
    }
}

//print output to stdout
void printOutputFileToShell(){

    int fd = open("/tmp/fdOutput", O_RDONLY);
    char *buf = malloc(maxLength*sizeof(char));
    int buflen;
    buflen = read(fd, buf, maxLength);
    write(1, buf, buflen);
    fflush(stdout); 
    free(buf);
    close(fd);

}

//print error to stderr
void printErrorFileToShell(){

    int fd = open("/tmp/fdError", O_RDONLY);
    char *buf = malloc(maxLength*sizeof(char));
    int buflen;
    buflen = read(fd, buf, maxLength);
    write(1,ANSI_COLOR_RED,5);
    write(1, buf, buflen);
    
    fflush(stderr);
    
    free(buf);
    close(fd);

}

bool changeDirectory(char ***cmd){
    bool res =false;
    char *home="/home";
    if(strcmp((*cmd)[0],"cd")==0){
        res = true;

        if((*cmd)[1]==NULL){
            chdir(home);

        }else if((strcmp((*cmd)[1], "~")==0) || (strcmp((*cmd)[1], "~/")==0)){
             chdir(home);
        }else if(chdir((*cmd)[1])<0){
            fprintf(stderr,"cd: %s: No such file or directory\n", (*cmd)[1]);
            returnCode=256;
        }
        char *buf = malloc(maxLength * sizeof(char));
        char *subCommand = malloc(MAXLENSTR *sizeof(char));
        strcpy(subCommand,*(cmd)[0]);
        int i=1;
        while((*cmd)[i]!=NULL){

            strcat(subCommand," ");
            strcat(subCommand,(*cmd)[i]);
            i++;
        }
         if(returnCode==0){

                    
                    scrivi(absoluteOutFilePath,formatoLog,"",subCommand,WRITE_ON_OUTPUT);

        }else{

            lseek(fdError,0,0);
            int buflen = myread(fdError,buf,maxLength);
            buf[buflen-1] = '\0';
            scrivi(absoluteErrFilePath,formatoLog,buf,subCommand,WRITE_ON_ERROR);
        }
        free(buf);
        free(subCommand);
    }
    
    return res;
}


int main(int argc, char **argv){
    srand(time(NULL));
    commandId = 0;
    letturaParametriInput(argc,argv);
    signal(SIGINT,sigIntHandler);
    char *exitCommand ="quit";
    printf("%s",WELCOME);
    //signal to stop child process
    takeStartCwd();


    //create absolute path for err and out file log

    absoluteOutFilePath = malloc(MAXLENSTR *sizeof(char));
    absoluteErrFilePath = malloc(MAXLENSTR * sizeof(char));
    createAbsolutePath(outfilePath,absoluteOutFilePath);
    createAbsolutePath(errfilePath,absoluteErrFilePath);
    //printf("%s\n%s\n",absoluteOutFilePath,absoluteErrFilePath);

    //main loop


    while(1){
        
        printCwd();
        char *tmpCommandLine=malloc(MAXLENSTR * sizeof(char));
        commandLine = malloc(MAXLENSTR *sizeof(char));
        fgets(tmpCommandLine,MAXLENSTR,stdin);
        //search for first alphanum value in the input command
        int i=0;
        int j=0;
        bool found = false;
        commandLine[j]='\0';
        while(i<MAXLENSTR && tmpCommandLine[i]!='\0'){
            if(!found && isalnum(tmpCommandLine[i])){
                found=true;
            }
            if(found){
                commandLine[j]=tmpCommandLine[i];
                j++;
            }
            i++;
            
        }
        free(tmpCommandLine);
       
        // printf("%s\n",commandLine);
        
        if(commandLine[0]!='\0'){
            commandLine[j-1]='\0';
            //printf("Entro qua\n");
            //printf("%s\n",commandLine);
            int len = strlen(commandLine);
            char *toSplitThisCommand = malloc(MAXLENSTR * sizeof(char));
            strcpy(toSplitThisCommand,commandLine);
            fdError = myopenwm("/tmp/fdError",O_RDWR|O_CREAT|O_TRUNC,0777);
            dup2(fdError,2);
            fdOutput = myopenwm("/tmp/fdOutput",O_RDWR|O_CREAT|O_TRUNC,0777);
            if(strcmp(commandLine,exitCommand)==0){
                printf("%s",GOODBYE);
                clearTmpFiles();
                
                exit(EXIT_SUCCESS);
            }

            //counting the number of pipes in the command
            int numberOfPipes = 0;
            i=0;
            while(commandLine[i]!='\0' && i<MAXLENSTR){

                if(commandLine[i]=='|'){
                    numberOfPipes+=1;
                }
                i++;
            }
            //printf("numberOfPipes: %d\n",numberOfPipes);

            // printf("%s\n",commandLine);
            char ***splittedCommand = splitCommandLine(toSplitThisCommand);
            // printf("%s\n",commandLine);

            bool changedDirectory = changeDirectory(splittedCommand);

            if(!changedDirectory){
                if(numberOfPipes==0){

                    executeSingleCommand(splittedCommand);
                }else{
                    executeWithPipe(splittedCommand);
                }
            }
            



            printOutputFileToShell();
            printErrorFileToShell();

            //printf("%s\n",commandLine);
            free(toSplitThisCommand);
        }
    
        free(commandLine);
        commandId+=1;
    }
    
    


    free(absoluteOutFilePath);
    free(absoluteErrFilePath);


    return 0;
}