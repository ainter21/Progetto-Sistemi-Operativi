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

char* mygetcwd(char *buf, size_t size){
   char *retval;

   retval = getcwd(buf, size);
   if(retval == NULL){
      perror("Error getting current working directory");
   }

   return retval;
}

int mykill(pid_t pid, int sig){
   int retval, errnum;

   retval = kill(pid, sig);
   if(retval == -1){
      errnum = errno;
      printf("Error killing the process: %s\n", strerror(errnum));
   }
}

int myfflush(FILE* stream){
   int retval, errnum;

   retval = fflush(stream);
   if(retval == EOF){
      errnum = errno;
      printf("Error killing the process: %s\n", strerror(errnum));
   }

   return retval;
}

int myopenwf(const char* pathname, int flags){
   int retval;

   retval = open(pathname, flags);
   if(retval == -1){
      perror("Error opening the file");
   }

   return retval;
}

int myopenwm(const char* pathname, int flags, mode_t mode){
   int retval;

   retval = open(pathname, flags, mode);
   if(retval == -1){
      perror("Error opening the file");
   }

   return retval;
}



ssize_t myread(int fd, void *buf, size_t count){
   ssize_t retval;

   retval = read(fd, buf, count);
   if(retval == -1){
      perror("Error reading the file");
   }

   return retval;
}

ssize_t mywrite(int fd, const void *buf, size_t count){
   ssize_t retval;

   retval = write(fd, buf, count);
   if(retval == -1){
      perror("Error writing the file");
   }

   return retval;
}

int myclose(int fd){
   int retval;

   retval = close(fd);
   if(retval == -1){
      perror("Error closing the fila");
   }

   return retval;
}


int mychdir(const char *path){
   int retval;

   retval = chdir(path);
   if(retval == -1){
      perror("Error changing directory");
   }

   return retval;
}

int mypipe(int filedes[2]){
   int retval, errnum;

   retval = pipe(filedes);
   if(retval == -1){
      errnum = errno;
      printf("Error in creating the pipe: %s\n", strerror(errnum));
   }

   return retval;
}

pid_t myfork(){
   pid_t retval;
   int errnum;

   retval = fork();
   if(retval == -1){
      errnum = errno;
      printf("Error in forking the process: %s\n", strerror(errnum));
      exit(EXIT_FAILURE);
   }

   return retval;
}

int mydup(int oldfd){
   int retval;

   retval = dup(oldfd);
   if(retval == -1){
      perror("Error duplicating the file descriptor");
   }

   return retval;
}


int myremove(const char *filename){
   int retval, errnum;

   retval = remove(filename);
   if(retval == -1){
      errnum = errno;
      printf("Error in removing the file: %s\n", strerror(errnum));
   }

   return retval;
}

//Il codice di ritorno dipende dal sistema operativo (127 in linux) ??
int mysystem(const char *string){
   int retval;

   retval = system(string);
   if(retval == -1){
      perror("Error in executing the command");
   }

   return retval;
}

off_t mylseek(int fd, off_t offset, int whence){
   off_t retval;

   retval = lseek(fd, offset, whence);
   if(retval == -1){
      perror("Error in repositioning the cursor");
   }

   return retval;
}

FILE *myfopen(const char *filename, const char *mode){
   FILE  *retval;
   int errnum;

   retval = fopen(filename, mode);
   if(retval == NULL){
      errnum = errno;
      printf("Error in opening the file: %s\n", strerror(errnum));
   }

   return retval;
}

int myfclose(FILE *stream){
   int retval;

   retval = fclose(stream);
   if(retval == EOF){
      perror("Error closing the file");
   }

   return retval;
}

int myaccess(const char *pathname, int mode){
   int retval;

   retval = access(pathname, mode);
   if(retval == -1){
      perror("Error accessing the file, permission denied");
   }

   return retval;
}

int myfseek(FILE *stream, long offset, int whence){
   int retval;

   retval = fseek(stream, offset, whence);
   if(retval == -1){
      perror("Error in repositioning the file");
   }

   return retval;
}

long myftell(FILE *stream){
   long retval;

   retval = ftell(stream);
   if(retval == -1){
      perror("Error analyzing the file");
   }

   return retval;
}

char* mystrstr(const char *haystack, const char *needle){
   char* retval;

   retval = strstr(haystack, needle);
   if(retval == NULL){
      perror("Sequence not present");
   }

   return retval;
}

int myatoi(const char *str){
   int retval;

   retval = atoi(str);
   if(retval == 0){
      perror("No valid conversion could be performed");
   }

   return retval;
}