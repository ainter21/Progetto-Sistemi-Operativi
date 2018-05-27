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



//colors
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"

//welcome and goodbye text
#define WELCOME "\e[1;93m   _____ _          _ _   _             __  __ _____  _____          \n  / ____| |        | | | | |           |  \\/  |  __ \\|  __ \\   /\\    \n | (___ | |__   ___| | | | |__  _   _  | \\  / | |  | | |  | | /  \\   \n  \\___ \\| '_ \\ / _ \\ | | | '_ \\| | | | | |\\/| | |  | | |  | |/ /\\ \\  \n  ____) | | | |  __/ | | | |_) | |_| | | |  | | |__| | |__| / ____ \\ \n |_____/|_| |_|\\___|_|_| |_.__/ \\__, | |_|  |_|_____/|_____/_/    \\_\\\n                                 __/ |                               \n                                |___/                                \n\n\x1b[0m"
#define GOODBYE "\e[1;95m  ____                   \n |  _ \\                  \n | |_) |_   _  ___       \n |  _ <| | | |/ _ \\      \n | |_) | |_| |  __/_ _ _ \n |____/ \\__, |\\___(_|_|_)\n         __/ |           \n        |___/            \n\n\x1b[0m"


//VARIABILI GLOBALI
char startCwd[MAXLENSTR]; //starting current working directory
char cwd[MAXLENSTR];    //current working directory
char *commandLine;  //input string
int commandId;  //incrementing integer indicating the id of the command(subCommand of the same cpmmand has the same commandid)


void takeStartCwd();
char *createAbsolutePath(char *relativePath,char *absolutePath);
bool checkExternCommand(char *cmd);
void printCwd();
void sigIntHandler(int signum);
void clearTmpFiles();
void printOutputFileToShell();
void printErrorFileToShell();


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

//it controls if a command is interactive. Used to not dup the stdout in the exec function
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
     if(strcmp(cmd,"less")==0){
         res =  true;
    }
    

    return res;
    
}
//wrapper to write to log based on the format
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

    if(!res){

        fprintf(stderr,"Error printing log file\n");

    }

        
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
//it kills the child process if exists.
void sigIntHandler(int signum){

    if(child_pid!=0){

        mykill(child_pid,SIGTERM);

    }
        
    printf("\x1b[31mType 'quit' to terminate\n\x1b[0m");
    printCwd();
    myfflush(stdout);

}
//it closes tmp files
void clearTmpFiles(){

    myclose(fdOutput);
    myclose(fdError);
    myremove("/tmp/fdError");
    myremove("/tmp/fdOutput");

}

//print output to stdout
void printOutputFileToShell(){

    int fd = myopenwf("/tmp/fdOutput", O_RDONLY);
    char *buf = malloc(maxLength*sizeof(char));
    int buflen;
    buflen = myread(fd, buf, maxLength);
    mywrite(1, buf, buflen);
    myfflush(stdout); 
    free(buf);
    myclose(fd);

}

//print error to stderr
void printErrorFileToShell(){

    int fd = myopenwf("/tmp/fdError", O_RDONLY);
    char *buf = malloc(maxLength*sizeof(char));
    int buflen;
    buflen = myread(fd, buf, maxLength);
    mywrite(1,ANSI_COLOR_RED,5);
    mywrite(1, buf, buflen);
    
    myfflush(stderr);
    
    free(buf);
    myclose(fd);

}



//main 
int main(int argc, char **argv){
    srand(time(NULL));
    commandId = 0;
    //read input arguments
    letturaParametriInput(argc,argv);

    //control if user foget to enter out or err file log
    if(errfilePath==NULL || outfilePath ==NULL){
        stampaHelp(true);
        exit(EXIT_FAILURE);
    }

    //it sets the the flags if the out and err file are the same or not
    if(strcmp(errfilePath,outfilePath)==0){
        outFlag=WRITE_ON_OUTPUT;
        errFlag=WRITE_ON_ERROR;
    }else{
        outFlag = DIFFERENT_LOG_FILES;
        errFlag = DIFFERENT_LOG_FILES;
    }

    //signal
    signal(SIGINT,sigIntHandler);
    char *exitCommand ="quit";

    //prints the welcome string MDDA = Michele, Davide, Davide, Alberto, our names
    printf("%s",WELCOME);
    
    //we need it to create absolute path
    takeStartCwd();


    //create absolute path for err and out file log

    absoluteOutFilePath = malloc(MAXLENSTR *sizeof(char));
    absoluteErrFilePath = malloc(MAXLENSTR * sizeof(char));
    createAbsolutePath(outfilePath,absoluteOutFilePath);
    createAbsolutePath(errfilePath,absoluteErrFilePath);
    

    //while loop


    while(1){
        
        //prints the shell info every cycle(shell: cwd$)
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
       
        
        //checks if command is not null
        if(commandLine[0]!='\0'){
            commandLine[j-1]='\0';
            
            int len = strlen(commandLine);
            //we will split this string
            char *toSplitThisCommand = malloc(MAXLENSTR * sizeof(char));
            strcpy(toSplitThisCommand,commandLine);

            //we will use this fd as tmp output and error
            fdError = myopenwm("/tmp/fdError",O_RDWR|O_CREAT|O_TRUNC,0777);
            dup2(fdError,2);
            fdOutput = myopenwm("/tmp/fdOutput",O_RDWR|O_CREAT|O_TRUNC,0777);

            //check if it is the quit command
            if(strcmp(commandLine,exitCommand)==0){
                printf("%s",GOODBYE);
                clearTmpFiles();
                
                exit(EXIT_SUCCESS);
            }

            //counts the number of pipes in the command
            int numberOfPipes = 0;
            
            i=0;
            while(commandLine[i]!='\0' && i<MAXLENSTR){

                if(commandLine[i]=='|'){
                    numberOfPipes+=1;
                }
                i++;
            }
            //counts the number of the redirection > command
            int numberOfRedirection  = 0;
            i=0;
            while(commandLine[i]!='\0' && i<MAXLENSTR){

                if(commandLine[i]=='>'){
                    numberOfRedirection+=1;
                }
                i++;
            }
            //this if controls if there are piping or redirection command and it calls different functions. This shell does not support bot redirection and piping. 
            
            if(numberOfPipes == 0 && numberOfRedirection == 0){
                char **splittedSingleCommand = splitCommandLineNoRedirection(toSplitThisCommand);
                bool changedDirectory = changeDirectory(splittedSingleCommand);
                if(!changedDirectory){
                    executeSingleCommand(splittedSingleCommand);
                }
                
                
            }else if(numberOfPipes!=0 && numberOfRedirection==0){
                
                char ***splittedCommand = splitCommandLineWithPipes(toSplitThisCommand);
                executeWithPipe(splittedCommand);
                
                

            }else if(numberOfPipes == 0 && numberOfRedirection!=0){

                char ***splittedCommand = splitCommandLineWithRedirection(toSplitThisCommand);
                executeWithRedirection(splittedCommand);

            }else{

                fprintf(stderr,"This type of command is not supported\n");
            }

            //prints out and errors catched executing the commands.
            printOutputFileToShell();
            printErrorFileToShell();

            
            free(toSplitThisCommand);
        }
    
        free(commandLine);
        //incrementing commandId
        commandId+=1;
    }

    deallocaRisorse();
    free(absoluteOutFilePath);
    free(absoluteErrFilePath);

    return 0;
}