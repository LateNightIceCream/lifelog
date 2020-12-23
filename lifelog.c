#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#define LOGFILE_PATH "./logfile.tex"
#define DATEFILE_PATH "./datefile.txt"
#define LATEX_COMPILE_CMD "xelatex document.tex"

#define DATE_LENGTH 10 // 10.10.1010

char* inputString (FILE* fp) {

    #define CHUNK_SIZE 16

    char*  string;
    int    character;
    size_t stringLength = 0;
    size_t bufferSize   = CHUNK_SIZE;

    string = malloc(bufferSize * sizeof(char)); // start size

    if(string == NULL) return string;

    while((character = fgetc(fp)) != EOF && character != '\n') {

        string[stringLength] = character;
        stringLength++;

        if(stringLength == bufferSize){

            bufferSize += CHUNK_SIZE;

            string = realloc(string, bufferSize * sizeof(char));

            if(string == NULL) return string;
        }
    }

    string[stringLength] = '\0';

    stringLength++;

    // restrain size to actual length of string
    return realloc(string, stringLength * sizeof(char));
}

void writeNewEntry (FILE* file, char* date) {
    fprintf(
        file,
        "\n\\entry{%s}\n",
        date
    );
}

void writeToCurrentEntry (FILE* file) {
    fprintf(
        file,
        "\\entryItem{%s\\\\}\n",
        inputString(stdin)
    );
}


int main () {

    FILE* logfile;
    FILE* datefile;

    char prevDate[DATE_LENGTH] = {};
    char currentDate[DATE_LENGTH] = {};

    time_t t     = time(NULL);
    struct tm tm = *localtime(&t);

    sprintf(
        currentDate,
        "%d.%d.%d",
        tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900
    );

    logfile  = fopen(LOGFILE_PATH,  "a");
    datefile = fopen(DATEFILE_PATH, "r+");

    fread(prevDate, DATE_LENGTH, sizeof(char), datefile);

    if(strncmp(currentDate, prevDate, DATE_LENGTH) != 0) {

        writeNewEntry(logfile, currentDate);

        fseek(datefile, 0, SEEK_SET);
        fprintf(datefile, "%s", currentDate);

        printf("\nAdded a new entry on %s!\n", currentDate);
    }

    printf("\nWhat do you want to write?\n");
    writeToCurrentEntry(logfile);

    fclose(datefile);
    fclose(logfile);

    // compile Latex w/ shell command
    int status = system(LATEX_COMPILE_CMD);

    printf("\nDone!\n, %d", status);

    return 0;
}
