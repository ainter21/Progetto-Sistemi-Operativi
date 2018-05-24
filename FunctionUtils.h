#ifndef FUNCTION_UTILS_H
#define FUNCTION_UTILS_H

#define DEFAULT_OUTFILEPATH "/tmp/shell.out"
#define DEFAULT_ERRFILEPATH "/tmp/shell.err"
#define UNDEFINED_COMMAND "Comando non supportato o non riconosciuto\n\n"

#define FORMAT_CSV 1111
#define FORMAT_TEXT 2222
#define FORMAT_HTML 3333

extern bool codiceRitorno;
extern int maxLength, formatoLog;
extern char* outfilePath;
extern char* errfilePath;
extern bool outAllocatoDinam, errAllocatoDinam;

void letturaParametriInput(int argc, char* argv[]);
void stampaHelp(bool errore);
bool isDigit(const char* str);
bool endWith(const char * str, const char * suffix);
void deallocaRisorse();

#endif