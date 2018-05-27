#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "FormatUtils.h"
#include "FunctionUtils.h"
#include "MyWrappers.h"

bool scriviSuTxt(char * outFile, int id, char * command, char * subcommand, char * output, int* returnCode, int writeFlag){

    //Apro il file in append
    FILE* fOut = fopen(outFile, "a");
    if (fOut == NULL){
        return false;
    }

    //Intestazione
    fprintf(fOut, "----------------------------------------\n");

    if (writeFlag == WRITE_ON_OUTPUT)
        fprintf(fOut, "TIPO:\t\t\tOUTPUT\n");
    else if (writeFlag == WRITE_ON_ERROR)
        fprintf(fOut, "TIPO:\t\t\tERRORE\n");

    //Scrittura dei vari campi
    fprintf(fOut, "ID:\t\t\t\t#%i\n", id);
    fprintf(fOut, "COMANDO:\t\t%s\n", command);

    if (subcommand != NULL){
        fprintf(fOut, "SOTTOCOMANDO:\t%s\n", subcommand);
    }

    //Ottengo la data di sistema e la stampo su file
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    fprintf(fOut, "DATA:\t\t\t%s", asctime (timeinfo));

    fprintf(fOut, "OUTPUT:\n\n%s\n\n", output);

    if (codiceRitorno){
        fprintf(fOut, "CODICE RITORNO:\t%i\n\n", *returnCode);
    }

    fclose(fOut);
    return true;
}

bool scriviSuCsv(char * outFile, int id, char * command, char * subcommand, char * output, int* returnCode, int writeFlag){

    bool scrivereIntestazione = false;

    //Se il file NON esiste
    if( access( outFile, F_OK ) == -1 )
        scrivereIntestazione = true;

    //Apro il file in append
    FILE* fOut = fopen(outFile, "a");
    if (fOut == NULL){
        return false;
    }

    /*if (scrivereIntestazione && writeFlag == DIFFERENT_LOG_FILES)
        fprintf(fOut, "ID;COMANDO;SOTTOCOMANDO;DATA;OUTPUT;CODICE RITORNO\n");
    else
        fprintf(fOut, "ID;TIPO;COMANDO;SOTTOCOMANDO;DATA;OUTPUT;CODICE RITORNO\n");*/

    if (scrivereIntestazione){
        fprintf(fOut, "ID;");

        if (writeFlag != DIFFERENT_LOG_FILES)
            fprintf(fOut, "TIPO;");

        fprintf(fOut, "COMANDO;SOTTOCOMANDO;DATA;OUTPUT;");

        if (codiceRitorno)
            fprintf(fOut, "CODICE RITORNO;");

        fprintf(fOut, "\n");
    }

    //Scrittura dei vari campi
    fprintf(fOut, "#%i;", id);

    if (writeFlag == WRITE_ON_OUTPUT)
        fprintf(fOut, "OUTPUT;");
    else if (writeFlag == WRITE_ON_ERROR)
        fprintf(fOut, "ERRORE;");

    fprintf(fOut, "%s;", command);

    if (subcommand != NULL)
        fprintf(fOut, "%s;", subcommand);
    else
        fprintf(fOut, ";");

    //Ottengo la data di sistema e la stampo su file
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    //Elimino il \n finale dalla stringa altrimenti il csv non viene formattato correttamente
    char* strData = asctime (timeinfo);
    int i=0;
    for (i=0; strData[i] != '\n'; i++);
    strData[i] = '\0';
    fprintf(fOut, "%s;", strData);

    fprintf(fOut, "\"%s\";", output);

    if (codiceRitorno)
        fprintf(fOut, "%i", *returnCode);
    
    fprintf(fOut, "\n");

    fclose(fOut);
    return true;
}

bool scriviSuHtml(char * outFile, int id, char * command, char * subcommand, char * output, int* returnCode, int writeFlag){

    bool scrivereIntestazione = false;

    //Se il file NON esiste
    if( access( outFile, F_OK ) == -1 ){
        scrivereIntestazione = true;

        //Dato che il file non esiste, devo crearlo obbligatoriamente prima di aprirlo in lettura
        FILE* fOut = myfopen(outFile, "w");
        if (fOut == NULL){
            return false;
        }

        myfclose(fOut);
    }

    //Apro il file in lettura/scrittura altrimenti la fseek non funziona (il file non viene sovrascritto)
    FILE* fOut = fopen(outFile, "r+");
    if (fOut == NULL){
        return false;
    }

    if (scrivereIntestazione)
        scriviHeaderHTML(fOut, writeFlag);

    //Mi sposto nel file alla posizione giusta sapendo la dimensione totale
    int dimensioneFile = getFileDim(fOut);
    myfseek(fOut, dimensioneFile - HTML_FILE_OFFSET, SEEK_SET);

    //printf("\n\nTEST OUTPUT 1:\n%s\n\n", output);

    //Sostituisco tutti i \n di output in <br>
    char* tempOutput = (char*) malloc(sizeof(char) * (strlen(output) + 500));
    int indexOutput, indexTempOutput, i;

    for (i=0; i<strlen(output) + 500; i++)
        tempOutput[i] = '\0';

    for (indexOutput=0, indexTempOutput = 0; output[indexOutput]; indexOutput++){
        if (output[indexOutput] == '\n'){
            strcat(tempOutput, "<br>");
            indexTempOutput += 4;
        }
        else{
            tempOutput[indexTempOutput] = output[indexOutput];
            indexTempOutput++;
        }
    }
    tempOutput[indexTempOutput] = '\0';
    output = tempOutput;

    //printf("\n\nTEST OUTPUT 2:\n%s\n%s\n", output, tempOutput);

    //Scrivo le statistiche sotto forma di tabella
    char *str = malloc((1024 + strlen(output)) * sizeof(char));

    sprintf(str, "<tr><td>%i</td>", id);

    if (writeFlag == WRITE_ON_OUTPUT)
        strcat(str, "<td>OUTPUT</td>");
    else if (writeFlag == WRITE_ON_ERROR)
        strcat(str, "<td>ERRORE</td>");
    //else
    //    sprintf(str, "<td></td>");

    

    strcat(str, "<td>");
    strcat(str, command);
    strcat(str, "</td>");

    if (subcommand != NULL){
        strcat(str, "<td>");
        strcat(str, subcommand);
        strcat(str, "</td>");
    }
    else
        strcat(str, "<td></td>");

    //Ottengo la data di sistema
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    
    strcat(str, "<td>");
    strcat(str, asctime(timeinfo));
    strcat(str, "</td>");

    //Scrivo il modal dell'output
    strcat(str, "<td>");
    //strcat(str, output);


    char strID[30];
    randString(strID, 30);

    //printf("%s - %s -> %s\n", command, subcommand, strID);

    strcat(str, "<button type='button' class='btn btn-success' data-toggle='modal' data-target='#");
    strcat(str, strID);
    strcat(str, "'>Vedi l'output</button>");

    strcat(str, "<div class='modal fade' id='");
    strcat(str, strID);
    strcat(str, "'><div class='modal-dialog modal-lg'><div class='modal-content'><div class='modal-header'>");
    strcat(str, "<h4 class='modal-title'>Output</h4><button type='button' class='close' data-dismiss='modal'>&times;</button></div>");
    strcat(str, "<div class='modal-body'>");
    strcat(str, output);
    strcat(str, "</div><div class='modal-footer'><button type='button' class='btn btn-danger' data-dismiss='modal'>Chiudi</button>");
    strcat(str, "</div></div></div></div>");

    strcat(str, "</td>");

    if (codiceRitorno){
        char tmp[10];
        sprintf(tmp, "%i", *returnCode);

        strcat(str, "<td>");
        strcat(str, tmp);
        strcat(str, "</td>");
    }
    /*else{
        strcat(str, "<td></td>");
        printf("Entro2\n");
    }*/

    strcat(str, "</tr>");

    //Riscrivo la coda perchè viene sovrascritta
    strcat(str, "</tbody></table></div></body></html>");

    //Scrivo su file
    fprintf(fOut, "%s", str);

    free(str);

    free(output);
    output = NULL;

    fclose(fOut);
    return true;
}


/*
 * Funzione per scrivere la prima volta sul file HTML.
 * Predispone i vari tag <meta>, <link>, <script>, <table> per il framework Bootstrap 
*/
void scriviHeaderHTML(FILE* file, int writeFlag){

    char header[1000] = "<!DOCTYPE html><html><head>";
    strcat(header, "<meta charset='UTF-8'>");
    strcat(header, "<meta name='description' content='Log file shell custom'>");
    strcat(header, "<meta name='author' content='Alberto, Davide, Davide, Michele'>");
    strcat(header, "<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/4.1.0/css/bootstrap.min.css'>");
    strcat(header, "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js'></script>");
    strcat(header, "<script src='https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.0/umd/popper.min.js'></script>");
    strcat(header, "<script src='https://maxcdn.bootstrapcdn.com/bootstrap/4.1.0/js/bootstrap.min.js'></script>");

    strcat(header, "</head><body class='bg-light'><br>");

    strcat(header, "<div class='container'>");
    strcat(header, "<h2 class='text-danger'>Log file shell custom</h2><br>");
    strcat(header, "<table class='table table-striped table-bordered'>");
    strcat(header, "<thead><tr><th>ID</th>");

    if (writeFlag != DIFFERENT_LOG_FILES)
        strcat(header, "<th>Tipo</th>");
    
    strcat(header, "<th>Comando</th><th>Sottocomando</th><th>Data</th><th>Output</th>");
    
    if (codiceRitorno)
        strcat(header, "<th>Codice ritorno</th>");

    strcat(header, "</tr></thead><tbody></tbody></table></div></body></html>");

    fprintf(file, "%s", header);
}

int getFileDim(FILE* f){

    //Porto il cursore a fine file e chiamo ftell()
    myfseek(f, 0, SEEK_END);
    int sz = myftell(f);

    //Riporto il cursore a inizio file
    myfseek(f, 0, SEEK_SET);

    return sz;
}

void randString(char str[], size_t size) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK1234567890";
    
    if (size) {
        --size;
        int n;
        for (n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
}
