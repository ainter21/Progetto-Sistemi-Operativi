#ifndef SPLITLINES_H
#define SPLILINES_H


//used to alloc strings
#define MAXLENSTR 256
//max number of commands and arguments
#define MAXCMDNUM 64

char ***splitCommandLineWithPipes(char* command);
char ***splitCommandLineWithRedirection(char* command);
char **splitCommandLineNoRedirection(char *command);



#endif