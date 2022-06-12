#ifndef EXT_H
#define EXT_H 1
#include "piece.h"
#endif

#include "struct.h"

#ifndef EXTERN
#define EXTERN extern
#endif

EXTERN struct editorWin win;

// edit
void initEditor();
void insertChar(int c);
void insertNewLine();
void delChar();
void appendBuffer(buffer *ab, const char *s, int len);
void freeBuffer(buffer *ab);

// io

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
void processKeypress(char **str, tree *table, stack_ll *undo_st, stack_ll *redo_st);

// raw
void die(const char *s);
void endRawMode();
void setRawMode();
int editorReadKey();
int getCursorPos(int *rows, int *cols);
int getWinSize(int *rows, int *cols);

// row
int cxToRx(ROW *row, int cx);
void updateRow(ROW *row);
void insertRow(int at, char *s, size_t len);
void freeRow(ROW *row);
void delRow(int at);
void rowInsertChar(ROW *row, int at, int c);
void rowAppendString(ROW *row, char *s, size_t len);
void rowDelChar(ROW *row, int at);
