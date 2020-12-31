#include "lifelog.h"


int main () {

    setlocale(LC_ALL, "");

    struct mainMenu* menu = malloc(sizeof(struct mainMenu));

    ncursesInitialize(menu);

    while (getKeyAndEvaluate(menu->form) != 0);

    // file stuff
    FILE* logfile = fopen(LOGFILE_PATH, "a");

    dateTimeHeading(logfile, DATEFILE_PATH);

    // get user input
    form_driver(menu->form, REQ_VALIDATION);

    writeToCurrentEntry(logfile, trim_whitespaces(field_buffer(menu->fields[0], 0)));

    fclose(logfile);

    ncursesTerminate(menu);

    // compile Latex w/ shell command
    //int status = system(LATEX_COMPILE_CMD);
    //printf("Done!, %d\n", status);

    return 0;
}


// sets up ncurses and the main structure
int ncursesInitialize(struct mainMenu* menu) {

    int nrows, ncols;
    int formWidth, formHeight, formX, formY;

    WINDOW* ui = initscr();
    if (ui == NULL) return -1;

    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    start_color();
    use_default_colors();

    getmaxyx(stdscr, nrows, ncols);

    formWidth  = ncols * 0.618;
    formHeight = nrows * 0.618;

    formX = (ncols - formWidth)  / 2;
    formY = (nrows - formHeight) / 2;

    menu->fields[0] = new_field(formHeight, formWidth, formY, formX, 0, 0);
    menu->fields[1] = NULL;

    //init_pair(1, COLOR_BRIGHT_YELLOW, COLOR_BLACK);
    //set_field_fore(field[0], COLOR_PAIR(1));

    set_field_back(menu->fields[0], A_UNDERLINE);
    field_opts_off(menu->fields[0], O_AUTOSKIP | O_STATIC); // O_STATIC - Variable size
    field_opts_on(menu->fields[0], O_EDIT);

    set_max_field(menu->fields[0], MAX_GROWTH);

    menu->form = new_form(menu->fields);
    post_form(menu->form);


    char currentDate[DATE_LENGTH] = {};
    getCurrentDate(currentDate);

    mvprintw(formY + formHeight, formX + formWidth - DATE_LENGTH, "%s", currentDate);

    mvprintw(formY, formX, "Enter your lifelog entry");

    refresh();

    return 0;

}

void ncursesTerminate (struct mainMenu* menu) {

    unpost_form(menu->form);
    free_form(menu->form);
    free_field(menu->fields[0]);
    free(menu);

    endwin();
}

char* trim_whitespaces(char *str)
{
	char *end;

	// trim leading space
	while(isspace(*str))
		str++;

	if(*str == 0) // all spaces?
		return str;

	// trim trailing space
	end = str + strnlen(str, 128) - 1;

	while(end > str && isspace(*end))
		end--;

	// write new null terminator
	*(end+1) = '\0';

	return str;
}

// ncurses key handler
int getKeyAndEvaluate (FORM* form) {

    int ch = getch();

    switch(ch) {

        // regular key
        default:
            form_driver(form, ch);
            return 1;

         case KEY_DOWN:
            form_driver(form, REQ_NEXT_LINE);
            return 1;

        case KEY_UP:
            form_driver(form, REQ_PREV_LINE);
           	return 1;

        case KEY_LEFT:
            form_driver(form, REQ_PREV_CHAR);
            return 1;

        case KEY_RIGHT:
            form_driver(form, REQ_NEXT_CHAR);
            return 1;

        // Delete the char before cursor
        case KEY_BACKSPACE:
        case 127:
            form_driver(form, REQ_DEL_PREV);
            return 1;

        // Delete the char under the cursor
        case KEY_DC:
            form_driver(form, REQ_DEL_CHAR);
            return 1;

        case KEY_HOME:
            form_driver(form, REQ_BEG_LINE);
            return 1;

        case KEY_END:
            form_driver(form, REQ_END_LINE);
            return 1;

        // exit
        case KEY_ENTER:
        case 10:
            return 0;
    }

}

// Time / date


// checks the date file and compares with current date
// if different, put a new heading inside the logfile
void dateTimeHeading (FILE* logfile, char* datefilepath) {

    FILE* datefile;

    char currentDate[DATE_LENGTH] = {};
    char prevDate[DATE_LENGTH]    = {};

    getCurrentDate(currentDate);
    getPrevDateFromFile(datefilepath, prevDate);

    if(strncmp(currentDate, prevDate, DATE_LENGTH) != 0) {

        // new heading
        writeNewEntry(logfile, currentDate);

        // update previous Date
        writeDateToFile(datefilepath, currentDate);

        printf("\nAdded a new entry on %s!\n", currentDate);
    }

}

void getCurrentDate (char* datebuf) {

    time_t t     = time(NULL);
    struct tm tm = *localtime(&t);

    sprintf(
        datebuf,
        "%d.%d.%d",
        tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900
    );

}

void getPrevDateFromFile (char* filepath, char* datebuf) { // maybe don't call by reference?

    FILE* datefile = fopen(filepath, "r+");

    fread(datebuf, DATE_LENGTH, sizeof(char), datefile);

    fclose(datefile);
}

void writeDateToFile (char* filepath, char* date) {

    FILE* datefile = fopen(filepath, "w");

    fseek(datefile, 0, SEEK_SET); //neccessary?

    fprintf(datefile, "%s", date);
}

// File operations

void writeNewEntry (FILE* file, char* date) {
    fprintf(
        file,
        "\n\\entry{%s}\n",
        date
    );
}

void writeToCurrentEntry (FILE* file, char* str) {

    fprintf(
        file,
        "\n\\entryItem{%s\\\\}\n",
        str
    );
}
