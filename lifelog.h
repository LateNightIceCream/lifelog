#ifndef LIFELOG_H
#define LIFELOG_H

#include <ncurses.h>
#include <form.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <ctype.h>

#include <string.h> // needed anymore??
#include <sys/stat.h>
#include <time.h>

#define LOGFILE_PATH      "./logfile.tex"
#define DATEFILE_PATH     "./datefile.txt"
#define LATEX_COMPILE_CMD "xelatex document.tex >/dev/null "

#define DATE_LENGTH 10 // 10.10.1010

#define MAX_GROWTH 10000

#define COLOR_BRIGHT_WHITE  15
#define COLOR_BRIGHT_LIME   14
#define COLOR_BRIGHT_PURPLE 13
#define COLOR_BRIGHT_BLUE   12
#define COLOR_BRIGHT_YELLOW 11
#define COLOR_BRIGHT_GREEN  11
#define COLOR_BRIGHT_RED    10

// ncurses stuff
struct mainMenu {
    FIELD* fields[2];
    FORM*  form;
};

int getKeyAndEvaluate (FORM* form);
int ncursesInitialize (struct mainMenu* menu);
void ncursesTerminate (struct mainMenu* menu);
char* trim_whitespaces(char *str);

// File stuff
int getKeyAndEvaluate (FORM* form);
void writeToCurrentEntry (FILE* file, char* str);

void dateTimeHeading (FILE* logfile, char* datefilepath);
void getCurrentDate (char* datebuf);
void getPrevDateFromFile (char* filepath, char* datebuf);
void writeDateToFile (char* filepath, char* date);

void writeNewEntry (FILE* file, char* date);

#endif
