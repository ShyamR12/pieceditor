#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include "piece.h"
#include "ext.h"

int main(int argc, char *argv[])
{
    tree table = NULL;
    char *str = (char *)malloc(1);
    strcpy(str, "\0");
    printf("Reached here 1\n");
    setRawMode();
    initEditor();
    if (argc >= 2)
    {
        editorOpen(argv[1]);
    }

    setStatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit");

    while (1)
    {
        refreshScreen();
        processKeypress(&str, &table);
    }

    return 0;
}