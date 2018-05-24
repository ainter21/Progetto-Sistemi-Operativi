#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "FunctionUtils.h"

bool codiceRitorno;
int maxLength, formatoLog;
char* outfilePath;
char* errfilePath;
bool outAllocatoDinam = false;
bool errAllocatoDinam = false;

void letturaParametriInput(int argc, char* argv[]){

    outfilePath = NULL;
    errfilePath = NULL;
    maxLength = INT_MAX;
    formatoLog = FORMAT_TEXT;

    //Ci devono essere almeno due argomenti inseriti
    if (argc < 3){        
        stampaHelp(true);
        exit(3);
    }
    else {
        int i;
        char* param;

        for (i=1; i<argc; i++){

            param = argv[i];

            if (!strcmp(param, "-o")){
                i++;
                outfilePath = argv[i];
            }
            else if (strstr(param, "--outfile") != NULL){
                char* token;

                token = strtok(param, "=");
                token = strtok(NULL, "=");

                if (token == NULL){
                    //TODO
                    printf("ERRORE NON GESTITO...\n");
                }
                else {
                    outfilePath = token;
                }
            }
            else if (!strcmp(param, "-e") && i < argc - 1){
                i++;
                errfilePath = argv[i];
            }
            else if (strstr(param, "--errfile") != NULL){
                char* token;

                token = strtok(param, "=");
                token = strtok(NULL, "=");

                if (token == NULL){
                    //TODO
                    printf("ERRORE NON GESTITO...\n");
                }
                else {
                    errfilePath = token;
                }
            }
            else if (strstr(param, "--maxlen") != NULL){
                char* token;

                token = strtok(param, "=");
                token = strtok(NULL, "=");

                if (token == NULL){
                    maxLength = INT_MAX;
                }
                else if (isDigit(token) && atoi(token) > 0) {
                    maxLength = atoi(token);
                }
            }
            else if (!strcmp(param, "-m")){

                if (i < argc - 1 && isDigit(argv[i+1]) && atoi(argv[i+1]) > 0){
                    i++;
                    maxLength = atoi(argv[i]);
                }
                else {
                    //TODO
                    printf("ERRORE NON GESTITO (2)\n");
                    i++;
                }
            }
            else if (!strcmp(param, "-c")){

                if (i < argc - 1 && (!strcmp(argv[i+1], "true") || !strcmp(argv[i+1], "false"))){
                    i++;

                    if (!strcmp(argv[i], "true"))
                        codiceRitorno = true;
                    else    
                        codiceRitorno = false;
                }
                else {
                    //TODO
                    printf("ERRORE NON GESTITO (2)\n");
                    i++;
                }
            }
            else if (strstr(param, "--code") != NULL){
                char* token;

                token = strtok(param, "=");
                token = strtok(NULL, "=");

                if (token == NULL)
                    codiceRitorno = false;
                else if (!strcmp(token, "true")) 
                    codiceRitorno = true;
                else
                    codiceRitorno = false;

            }
            else if (strstr(param, "--format") != NULL) {
                char* token;

                token = strtok(param, "=");
                token = strtok(NULL, "=");

                if (token == NULL || !strcmp(token, "txt"))
                    formatoLog = FORMAT_TEXT;
                else if (!strcmp(token, "csv")) 
                    formatoLog = FORMAT_CSV;
                else if (!strcmp(token, "html"))
                    formatoLog = FORMAT_HTML;
                else
                    formatoLog = FORMAT_TEXT;
            }
            else if (!strcmp(param, "-f")){
                if (i < argc - 1 && (!strcmp(argv[i+1], "txt") || !strcmp(argv[i+1], "csv") || !strcmp(argv[i+1], "html"))){
                    i++;

                    if (!strcmp(argv[i], "txt"))
                        formatoLog = FORMAT_TEXT;
                    else if (!strcmp(argv[i], "csv"))
                        formatoLog = FORMAT_CSV;
                    else if (!strcmp(argv[i], "html"))
                        formatoLog = FORMAT_HTML;
                    else
                        formatoLog = FORMAT_TEXT;
                }
                else {
                    //TODO
                    printf("ERRORE NON GESTITO (3)\n");
                    i++;
                }
            }
            else {
                fprintf(stderr, UNDEFINED_COMMAND);
                stampaHelp(true);
                exit(1);
            }

        }
    }

    //Aggiungo l'estensione giusta ai file di log se sono stati specificati

    if (outfilePath != NULL){
        switch (formatoLog){
            case FORMAT_TEXT:
                        if (endWith(outfilePath, ".txt") == false) {

                            char* tempOut = (char*) malloc(sizeof(char) * (strlen(outfilePath) + 4));

                            strcpy(tempOut, outfilePath);
                            strcat(tempOut, ".txt");

                            outfilePath = tempOut;
                            outAllocatoDinam = true;
                        }

                        break;

            case FORMAT_CSV:
                        if (endWith(outfilePath, ".csv") == false){
                            
                            char* tempOut = (char*) malloc(sizeof(char) * (strlen(outfilePath) + 4));
                            strcpy(tempOut, outfilePath);
                            strcat(tempOut, ".csv");

                            outfilePath = tempOut;
                            outAllocatoDinam = true;

                            //strcat(outfilePath, ".csv");
                        }

                        break;

            case FORMAT_HTML:
                        if (endWith(outfilePath, ".html") == false){
                            
                            char* tempOut = (char*) malloc(sizeof(char) * (strlen(outfilePath) + 5));
                            strcpy(tempOut, outfilePath);
                            strcat(tempOut, ".html");

                            outfilePath = tempOut;
                            outAllocatoDinam = true;
                            //strcat(outfilePath, ".html");
                        }

                        break;
        }
    }

    if (errfilePath != NULL){
        switch (formatoLog){
            case FORMAT_TEXT:
                        if (endWith(errfilePath, ".txt") == false){
                            
                            char* tempErr = (char*) malloc(sizeof(char) * (strlen(errfilePath) + 4));
                            strcpy(tempErr, errfilePath);
                            strcat(tempErr, ".txt");

                            errfilePath = tempErr;
                            errAllocatoDinam = true;
                            //strcat(errfilePath, ".txt");
                        }

                        break;

            case FORMAT_CSV:
                        if (endWith(errfilePath, ".csv") == false){
                            
                            char* tempErr = (char*) malloc(sizeof(char) * (strlen(errfilePath) + 4));
                            strcpy(tempErr, errfilePath);
                            strcat(tempErr, ".csv");

                            errfilePath = tempErr;
                            errAllocatoDinam = true;
                            //strcat(errfilePath, ".csv");
                        }

                        break;

            case FORMAT_HTML:
                        if (endWith(errfilePath, ".html") == false){
                            //char tempErr[50];
                            
                            char* tempErr = (char*) malloc(sizeof(char) * (strlen(errfilePath) + 5));
                            strcpy(tempErr, errfilePath);
                            strcat(tempErr, ".html");

                            

                            errfilePath = tempErr;
                            errAllocatoDinam = true;
                            //strcat(errfilePath, ".html");
                        }

                        break;
        }
    }
}

bool isDigit(const char* str){

    int i=0;
    for (i=0; str[i]; i++)
        if (str[i] < '0' || str[i] > '9')
            return false;

    return true;
}

void stampaHelp(bool errore) {

    char str[1000] = "USAGE: ./shell [fileLogOutput] [fileLogError] [parametri opzionali]\n";
    strcat(str, "\tOBBLIGATORI:\n");
    strcat(str, "\t-o <filename>\t\t\tSpecifica il file di log di output\n");
    strcat(str, "\t--outfile=<filename>\t\tSpecifica il file di log di output\n");
    strcat(str, "\t-e <filename>\t\t\tSpecifica il file di log di errore\n");
    strcat(str, "\t--errfile=<filename>\t\tSpecifica il file di log di errore\n");
    strcat(str, "\tNota bene: non è obbligatorio scrivere l'estensione dei file di log!\n\n");

    strcat(str, "\tOPZIONALI:\n");
    strcat(str, "\t--maxlen=<len>\t\t\tSpecifica il massimo numero di byte scrivibili sui file di log\n");
    strcat(str, "\t-m <len>\t\t\tSpecifica il massimo numero di byte scrivibili sui file di log\n");
    strcat(str, "\t--code=<true/false>\t\tSpecifica se bisogna riportare il codice di ritorno nei log\n");
    strcat(str, "\t-c <true/false>\t\t\tSpecifica se bisogna riportare il codice di ritorno nei log\n");
    strcat(str, "\t--format=<txt,csv,html>\t\tSpecifica il formato dei dati del file di log\n");
    strcat(str, "\t-f <txt,csv,html>\t\tSpecifica il formato dei dati del file di log\n");
    strcat(str, "\n");

    if (errore) {
        //Se è stato indicato un file log di errore, vado a scrivere anche su quello
        if (errfilePath != NULL){
            //TODO - scrivere str sul file di log
        }

        fprintf(stderr, "%s", str);
    }
    else {
        if (outfilePath != NULL) {
            //TODO - scrivere str sul file di log
        }

        fprintf(stdout, "%s", str);
    }
}

/*
 * Controlla se str ha alla fine suffix  
*/
bool endWith(const char * str, const char * suffix) {

    int str_len = strlen(str);
    int suffix_len = strlen(suffix);

    return (str_len >= suffix_len) && (0 == strcmp(str + (str_len-suffix_len), suffix));
}

void deallocaRisorse(){

    if (outAllocatoDinam)
        free(outfilePath);

    if (errAllocatoDinam)
        free(errfilePath);
}