#ifndef FORMAT_UTILS_H
#define FORMAT_UTILS_H

#define HTML_FILE_OFFSET 36

#define DIFFERENT_LOG_FILES 0
#define WRITE_ON_OUTPUT 1
#define WRITE_ON_ERROR 2

bool scriviSuTxt(char * outFile, int id, char * command, char * subcommand, char * output, int* returnCode, int writeFlag);
bool scriviSuCsv(char * outFile, int id, char * command, char * subcommand, char * output, int* returnCode, int writeFlag);
bool scriviSuHtml(char * outFile, int id, char * command, char * subcommand, char * output, int* returnCode, int writeFlag);
void scriviHeaderHTML(FILE* file, int writeFlag);
int getFileDim(FILE* f);
void randString(char *str, size_t size);

#endif