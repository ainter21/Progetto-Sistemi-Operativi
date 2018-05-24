#ifndef MY_WRAPPERS_H
#define MY_WRAPPERS_H

// void mystrcat(); ??
// void mystrcmp(); ??
// void mystrcpy(); ??
char* mygetcwd(char *buf, size_t size);
int mykill(pid_t pid, int sig);
int myfflush(FILE* stream);
int myopenwf(const char* pathname, int flags);
int myopenwm(const char* pathname, int flags, mode_t mode);
void* mymalloc(size_t size);
ssize_t myread(int fd, void *buf, size_t count);
ssize_t mywrite(int fd, const void *buf, size_t count);
int myclose(int fd);
// void myfree(void *ptr); ??
char* mystrtok(char *str, const char *delim);
char* mystrtok_r(char *str, const char *delim, char **saveptr);
int mychdir(const char *path);
// void myfprintf(); ??
int mypipe(int filedes[2]);
pid_t myfork();
int dup(int oldfd);
// int dup2(int oldfd, int newfd);    Errore
// void mysignal(); ??
// void fgets(); ??
// void strlen(); ??
int myremove(const char *filename);
int mysystem(const char *string);
off_t mylseek(int fd, off_t offset, int whence);
FILE *myfopen(const char *filename, const char *mode);
// void mytime(time_t *t); ??
// void localtime(); ??
int myfclose(FILE *stream);
int myaccess(const char *pathname, int mode);
// void myasctime(); ??
int myfseek(FILE *stream, long offset, int whence);
// void sprintf(); ??
long myftell(FILE *stream);
char* mystrstr(const char *haystack, const char *needle);
int myatoi(const char *str);

#endif