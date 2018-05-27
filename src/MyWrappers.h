#ifndef MY_WRAPPERS_H
#define MY_WRAPPERS_H


char* mygetcwd(char *buf, size_t size);
int mykill(pid_t pid, int sig);
int myfflush(FILE* stream);
int myopenwf(const char* pathname, int flags);
int myopenwm(const char* pathname, int flags, mode_t mode);
ssize_t myread(int fd, void *buf, size_t count);
ssize_t mywrite(int fd, const void *buf, size_t count);
int myclose(int fd);

int mychdir(const char *path);

int mypipe(int filedes[2]);
pid_t myfork();
int mydup(int oldfd);

int myremove(const char *filename);
off_t mylseek(int fd, off_t offset, int whence);
FILE *myfopen(const char *filename, const char *mode);

int myfclose(FILE *stream);
int myaccess(const char *pathname, int mode);

int myfseek(FILE *stream, long offset, int whence);

long myftell(FILE *stream);
char* mystrstr(const char *haystack, const char *needle);
int myatoi(const char *str);

#endif