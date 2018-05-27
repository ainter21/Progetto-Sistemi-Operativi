#ifndef EXECUTECOMMANDS_H
#define EXECUTECOMMANDS_H





//used in pipes for better understanding
#define READ 0
#define WRITE 1

extern pid_t child_pid;    //process is of the child
extern int fdOutput;   //file descriptor to handle output
extern int fdError;    //file descriptor to handle error
extern int returnCode;     //return code of the execution of the command
extern char *absoluteOutFilePath; //absolute path of out file log
extern char *absoluteErrFilePath; //absolute path of err file log
extern int outFlag;    //variable used to set if the out file has the same name of the err file
extern int errFlag;    //variable used to set if the err file has the same name of the out file

void executeSingleCommand(char **cmd);
void executeWithPipe(char ***cmd);
void executeWithRedirection(char ***command);
bool changeDirectory(char **cmd);

#endif