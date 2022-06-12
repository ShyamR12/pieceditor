#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include "piece.h"
#include "ext.h"

int main(int argc, char *argv[])
{
    tree table = NULL;
    stack_ll undo_st = NULL;
    char *str = (char *)malloc(1);
    strcpy(str, "\0");
    // printf("Reached here 1\n");
    setRawMode();
    initEditor();
    if (argc >= 2)
    {
        editorOpen(argv[1]);
    }

    setStatusMessage("Ctrl-I = init pc | Ctrl-T = end pc | Ctrl-Q = quit | usual for undo(Z) & save(S)");

    while (1)
    {
        refreshScreen();
        processKeypress(&str, &table, &undo_st);
    }

    return 0;
}