
/*** file i/o ***/
char *rowToString(int *buflen);
void editorOpen(char *filename);
void editorSave();
/*** output ***/

void scroll();
void drawRows(buffer *ab);
void statusBar(buffer *ab);
void messageBar(buffer *ab);
void refreshScreen();
void setStatusMessage(const char *fmt, ...);
/*** input ***/

char *prompt(char *prompt);
void moveCursor(int key);
void processKeypress(char **str, tree *table);
// void die(const char *s);