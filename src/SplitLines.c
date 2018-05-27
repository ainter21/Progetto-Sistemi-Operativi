#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include "MyWrappers.h"
#include "SplitLines.h"

//used to split commands with pipes. It creates a char *** that divides commands by pipe and by argument
char ***splitCommandLineWithPipes(char* command){

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

//splits the command with the redirection character
char ***splitCommandLineWithRedirection(char* command){

    char ***cmd=malloc(MAXCMDNUM);
    int commandCount = 0;
    int wordCount = 0;

    char *endLine;

    char *actualCommand = strtok_r(command,">", &endLine);

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
        actualCommand = strtok_r(NULL,">", &endLine);
        wordCount=0;
        
    }
    cmd[commandCount] = malloc(MAXCMDNUM);
    cmd[commandCount]  = NULL;

    return cmd;

}


//splitcommand without redirection
char **splitCommandLineNoRedirection(char *command){
    
    char **cmd=malloc(MAXCMDNUM);
    int wordCount = 0;
    char *actualWord = strtok(command, " ");

    while(actualWord != NULL){

        cmd[wordCount] = malloc(MAXLENSTR * sizeof(char));
        cmd[wordCount] = actualWord;
        actualWord = strtok(NULL, " ");
        wordCount++;
    }

    cmd[wordCount] = malloc(MAXLENSTR * sizeof(char));
    cmd[wordCount] = NULL;

    return cmd;
}