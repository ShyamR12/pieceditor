#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include "piece.h"
#include "ext.h"

int cxToRx(ROW *row, int cx)
{
    int rx = 0;
    int j;
    for (j = 0; j < cx; j++)
    {
        if (row->chars[j] == '\t')
            rx += (8 - 1) - (rx % 8);
        rx++;
    }
    return rx;
}

void updateRow(ROW *row)
{
    int tabs = 0;
    int j;
    for (j = 0; j < row->size; j++)
        if (row->chars[j] == '\t')
            tabs++;

    free(row->render);
    row->render = malloc(row->size + tabs * (8 - 1) + 1);

    int idx = 0;
    for (j = 0; j < row->size; j++)
    {
        if (row->chars[j] == '\t')
        {
            row->render[idx++] = ' ';
            while (idx % 8 != 0)
                row->render[idx++] = ' ';
        }
        else
        {
            row->render[idx++] = row->chars[j];
        }
    }
    row->render[idx] = '\0';
    row->rsize = idx;
}

void insertRow(int at, char *s, size_t len)
{
    if (at < 0 || at > win.numrows)
        return;

    win.row = realloc(win.row, sizeof(ROW) * (win.numrows + 1));
    memmove(&win.row[at + 1], &win.row[at], sizeof(ROW) * (win.numrows - at));

    win.row[at].size = len;
    win.row[at].chars = malloc(len + 1);
    memcpy(win.row[at].chars, s, len);
    win.row[at].chars[len] = '\0';

    win.row[at].rsize = 0;
    win.row[at].render = NULL;
    updateRow(&win.row[at]);

    win.numrows++;
    win.change++;
}

void freeRow(ROW *row)
{
    free(row->render);
    free(row->chars);
}

void delRow(int at)
{
    if (at < 0 || at >= win.numrows)
        return;
    freeRow(&win.row[at]);
    memmove(&win.row[at], &win.row[at + 1], sizeof(ROW) * (win.numrows - at - 1));
    win.numrows--;
    win.change++;
}

void rowInsertChar(ROW *row, int at, int c)
{
    if (at < 0 || at > row->size)
        at = row->size;
    row->chars = realloc(row->chars, row->size + 2);
    memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
    row->size++;
    row->chars[at] = c;
    updateRow(row);
    win.change++;
}

void rowAppendString(ROW *row, char *s, size_t len)
{
    row->chars = realloc(row->chars, row->size + len + 1);
    memcpy(&row->chars[row->size], s, len);
    row->size += len;
    row->chars[row->size] = '\0';
    updateRow(row);
    win.change++;
}

void rowDelChar(ROW *row, int at)
{
    if (at < 0 || at >= row->size)
        return;
    memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
    row->size--;
    updateRow(row);
    win.change++;
}
