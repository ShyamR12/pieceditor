void die(const char *s);
void endRawMode();
void setRawMode();
int editorReadKey();
int getCursorPos(int *rows, int *cols);
int getWinSize(int *rows, int *cols);