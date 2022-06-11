#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ext.h"

void initEditor()
{
    win.cx = 0;
    win.cy = 0;
    win.rx = 0;
    win.rowoff = 0;
    win.coloff = 0;
    win.numrows = 0;
    win.row = NULL;
    win.change = 0;
    win.filename = NULL;
    win.statusmsg[0] = '\0';
    win.statusmsg_time = 0;

    if (getWinSize(&win.screenrows, &win.screencols) == -1)
        die("getWinSize");
    win.screenrows -= 2;
}

void insertChar(int c)
{
    if (win.cy == win.numrows)
    {
        insertRow(win.numrows, "", 0);
    }
    rowInsertChar(&win.row[win.cy], win.cx, c);
    win.cx++;
}

void insertNewLine()
{
    if (win.cx == 0)
    {
        insertRow(win.cy, "", 0);
    }
    else
    {
        ROW *row = &win.row[win.cy];
        insertRow(win.cy + 1, &row->chars[win.cx], row->size - win.cx);
        row = &win.row[win.cy];
        row->size = win.cx;
        row->chars[row->size] = '\0';
        updateRow(row);
    }
    win.cy++;
    win.cx = 0;
}

void delChar()
{
    if (win.cy == win.numrows)
        return;
    if (win.cx == 0 && win.cy == 0)
        return;

    ROW *row = &win.row[win.cy];
    if (win.cx > 0)
    {
        rowDelChar(row, win.cx - 1);
        win.cx--;
    }
    else
    {
        win.cx = win.row[win.cy - 1].size;
        rowAppendString(&win.row[win.cy - 1], row->chars, row->size);
        delRow(win.cy);
        win.cy--;
    }
}

void appendBuffer(buffer *ab, const char *s, int len)
{
    char *new = realloc(ab->b, ab->len + len);

    if (new == NULL)
        return;
    memcpy(&new[ab->len], s, len);
    ab->b = new;
    ab->len += len;
}

void freeBuffer(buffer *ab)
{
    free(ab->b);
}
