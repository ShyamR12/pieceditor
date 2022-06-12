#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include "piece.h"
#define EXTERN
#include "ext.h"
char *rowToString(int *buflen)
{
    int totlen = 0;
    int j;
    for (j = 0; j < win.numrows; j++)
        totlen += win.row[j].size + 1;
    *buflen = totlen;

    char *buf = malloc(totlen);
    char *p = buf;
    for (j = 0; j < win.numrows; j++)
    {
        memcpy(p, win.row[j].chars, win.row[j].size);
        p += win.row[j].size;
        *p = '\n';
        p++;
    }

    return buf;
}

void editorOpen(char *filename)
{
    free(win.filename);
    win.filename = strdup(filename);

    FILE *fp = fopen(filename, "r");
    if (!fp)
        die("fopen");

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while ((linelen = getline(&line, &linecap, fp)) != -1)
    {
        while (linelen > 0 && (line[linelen - 1] == '\n' ||
                               line[linelen - 1] == '\r'))
            linelen--;
        insertRow(win.numrows, line, linelen);
    }
    free(line);
    fclose(fp);
    win.change = 0;
}

void editorSave()
{
    if (win.filename == NULL)
    {
        win.filename = prompt("Save as: %s (ESC to cancel)");
        if (win.filename == NULL)
        {
            setStatusMessage("Save aborted");
            return;
        }
    }

    int len;
    char *buf = rowToString(&len);

    int fd = open(win.filename, O_RDWR | O_CREAT, 0644);
    if (fd != -1)
    {
        if (ftruncate(fd, len) != -1)
        {
            if (write(fd, buf, len) == len)
            {
                close(fd);
                free(buf);
                win.change = 0;
                setStatusMessage("%d bytes written to disk", len);
                return;
            }
        }
        close(fd);
    }

    free(buf);
    setStatusMessage("Can't save! I/O error: %s", strerror(errno));
}

/*** output ***/

void scroll()
{
    win.rx = 0;
    if (win.cy < win.numrows)
    {
        win.rx = cxToRx(&win.row[win.cy], win.cx);
    }

    if (win.cy < win.rowoff)
    {
        win.rowoff = win.cy;
    }
    if (win.cy >= win.rowoff + win.screenrows)
    {
        win.rowoff = win.cy - win.screenrows + 1;
    }
    if (win.rx < win.coloff)
    {
        win.coloff = win.rx;
    }
    if (win.rx >= win.coloff + win.screencols)
    {
        win.coloff = win.rx - win.screencols + 1;
    }
}

void drawRows(buffer *ab)
{
    int y;
    for (y = 0; y < win.screenrows; y++)
    {
        int filROW = y + win.rowoff;
        if (filROW >= win.numrows)
        {
            if (win.numrows == 0 && y == win.screenrows / 3)
            {
                char welcome[80];
                int welcomelen = snprintf(welcome, sizeof(welcome),
                                          "Text Editor 0.4.2");
                if (welcomelen > win.screencols)
                    welcomelen = win.screencols;
                int padding = (win.screencols - welcomelen) / 2;
                if (padding)
                {
                    appendBuffer(ab, "<>", 2);
                    padding--;
                }
                while (padding--)
                    appendBuffer(ab, " ", 1);
                appendBuffer(ab, welcome, welcomelen);
            }
            else
            {
                appendBuffer(ab, "<>", 2);
            }
        }
        else
        {
            int len = win.row[filROW].rsize - win.coloff;
            if (len < 0)
                len = 0;
            if (len > win.screencols)
                len = win.screencols;
            appendBuffer(ab, &win.row[filROW].render[win.coloff], len);
        }

        appendBuffer(ab, "\x1b[K", 3);
        appendBuffer(ab, "\r\n", 2);
    }
}

void statusBar(buffer *ab)
{
    appendBuffer(ab, "\x1b[44m", 6); // blue
    // appendBuffer(ab, "\x1b[7m", 4);
    char status[80], rstatus[80];
    int len = snprintf(status, sizeof(status), "%.20s - %d lines %s",
                       win.filename ? win.filename : "[Untitled]", win.numrows,
                       win.change ? "[modified]" : "");
    int rlen = snprintf(rstatus, sizeof(rstatus), "(%d,%d)",
                        win.cy, win.cx);
    if (len > win.screencols)
        len = win.screencols;
    appendBuffer(ab, status, len);
    while (len < win.screencols)
    {
        if (win.screencols - len == rlen)
        {
            appendBuffer(ab, rstatus, rlen);
            break;
        }
        else
        {
            appendBuffer(ab, " ", 1);
            len++;
        }
    }
    appendBuffer(ab, "\x1b[m", 3);
    appendBuffer(ab, "\r\n", 2);
}

void messageBar(buffer *ab)
{
    appendBuffer(ab, "\x1b[K", 3);
    int msglen = strlen(win.statusmsg);
    if (msglen > win.screencols)
        msglen = win.screencols;
    if (msglen && time(NULL) - win.statusmsg_time < 5)
        appendBuffer(ab, win.statusmsg, msglen);
}

void refreshScreen()
{

    scroll();

    buffer ab = ABUF_INIT;

    appendBuffer(&ab, "\x1b[?25l", 6);
    appendBuffer(&ab, "\x1b[H", 3);

    drawRows(&ab);
    statusBar(&ab);
    messageBar(&ab);

    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (win.cy - win.rowoff) + 1,
             (win.rx - win.coloff) + 1);
    appendBuffer(&ab, buf, strlen(buf));

    appendBuffer(&ab, "\x1b[?25h", 6);

    write(STDOUT_FILENO, ab.b, ab.len);
    freeBuffer(&ab);
}

void setStatusMessage(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(win.statusmsg, sizeof(win.statusmsg), fmt, ap);
    va_end(ap);
    win.statusmsg_time = time(NULL);
}

/*** input ***/

char *prompt(char *prompt)
{
    size_t bufsize = 128;
    char *buf = malloc(bufsize);

    size_t buflen = 0;
    buf[0] = '\0';

    while (1)
    {
        setStatusMessage(prompt, buf);
        refreshScreen();

        int c = editorReadKey();
        if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE)
        {
            if (buflen != 0)
                buf[--buflen] = '\0';
        }
        else if (c == '\x1b')
        {
            setStatusMessage("");
            free(buf);
            return NULL;
        }
        else if (c == '\r')
        {
            if (buflen != 0)
            {
                setStatusMessage("");
                return buf;
            }
        }
        else if (!iscntrl(c) && c < 128)
        {
            if (buflen == bufsize - 1)
            {
                bufsize *= 2;
                buf = realloc(buf, bufsize);
            }
            buf[buflen++] = c;
            buf[buflen] = '\0';
        }
    }
}

void moveCursor(int key)
{
    ROW *row = (win.cy >= win.numrows) ? NULL : &win.row[win.cy];

    switch (key)
    {
    case ARROW_LEFT:
        if (win.cx != 0)
        {
            win.cx--;
        }
        else if (win.cy > 0)
        {
            win.cy--;
            win.cx = win.row[win.cy].size;
        }
        break;
    case ARROW_RIGHT:
        if (row && win.cx < row->size)
        {
            win.cx++;
        }
        else if (row && win.cx == row->size)
        {
            win.cy++;
            win.cx = 0;
        }
        break;
    case ARROW_UP:
        if (win.cy != 0)
        {
            win.cy--;
        }
        break;
    case ARROW_DOWN:
        if (win.cy < win.numrows)
        {
            win.cy++;
        }
        break;
    }

    row = (win.cy >= win.numrows) ? NULL : &win.row[win.cy];
    int rowlen = row ? row->size : 0;
    if (win.cx > rowlen)
    {
        win.cx = rowlen;
    }
}

void processKeypress(char **str, tree *table, stack_ll *undo_st, stack_ll *redo_st)
{
    static int start = 0;
    static int end = 0;
    static int quit_times = QUIT_TIMES;
    int c = editorReadKey();
    node *p = NULL;

    switch (c)
    {
    case '\r':
        insertNewLine();
        break;

    case CTRL_KEY('q'):
        if (win.change && quit_times > 0)
        {
            setStatusMessage("WARNING!!! File has unsaved changes. "
                             "Press Ctrl-Q %d more times to quit.",
                             quit_times);
            quit_times--;
            return;
        }
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);
        exit(0);
        break;

    case CTRL_KEY('s'):
        editorSave();
        break;
    case CTRL_KEY('y'):
        if (*redo_st)
        {
            p = (*redo_st)->data;
            int start_redo = (*redo_st)->index;
            int end_redo = start_redo + p->blk->length;
            char *str = (char *)malloc(sizeof(char) * (p->blk->length + 1));
            memcpy(str, p->blk->txt, p->blk->length);
            str[p->blk->length] = '\0';
            if (win.cx > start_redo)
            {
                while (win.cx != start_redo)
                {
                    moveCursor(ARROW_LEFT);
                }
            }
            else
            {
                while (win.cx != start_redo)
                {
                    moveCursor(ARROW_RIGHT);
                }
            }
            for (int i = start_redo; i < end_redo; i++)
            {
                insertChar(str[i - start_redo]);
            }

            *table = redo(table, redo_st, undo_st);
            remove("text_to_piece_table.txt");
            fileInorder(*table, "text_to_piece_table.txt");
        }
        break;
    case CTRL_KEY('z'):
        if (*undo_st)
        {
            p = (*undo_st)->data;
            splay(p, table);

            // printf("Contents of p = %s, length = %ld\n", p->blk->txt, p->blk->length);
            int x_end = p->size_left + p->blk->length;
            int len_del = p->blk->length;
            // printf("x_end before %d\n", x_end);
            while (p->split_part)
            {
                len_del += p->split_part->blk->length;
                p = p->split_part;
            }
            int x_start = x_end - len_del;
            // printf("x_end after %d\n", x_end);
            // printf("x_start = %d, x_end = %d\n", x_start, x_end);
            int j = x_start;
            if (win.cx > x_start)
            {
                while (win.cx != x_start)
                {
                    moveCursor(ARROW_LEFT);
                }
            }
            else
            {
                while (win.cx != x_start)
                {
                    moveCursor(ARROW_RIGHT);
                }
            }

            for (int i = x_start; i < x_end; i++)
            {
                moveCursor(ARROW_RIGHT);
                delChar();
            }
            *table = undo(*table, undo_st, redo_st);
            remove("text_to_piece_table.txt");
            fileInorder(*table, "text_to_piece_table.txt");
        }

        break;
    case CTRL_KEY('i'): // initiate
        start = win.cx;
        // printf("Reached here and x coordinate is %d\n", start);
        break;
    case CTRL_KEY('t'): // terminate
        end = win.cx;
        // printf("At the end string is %s\n", *str);
        // printf("Reached here and x coordinate is %d\n", end);
        *table = insert(table, strlen(*str), *str, start, undo_st, redo_st, 0);
        // inorder(*table);
        remove("text_to_piece_table.txt");
        fileInorder(*table, "text_to_piece_table.txt");
        free(*str);
        *str = (char *)malloc(sizeof(char));
        strcpy(*str, "\0");
        break;
        // case CTRL_KEY('d'): // display
        //     end = win.cx;
        //     printf("start = %d, end = %d\n", start, end);
        //     break;

    case HOME_KEY:
        win.cx = 0;
        break;

    case END_KEY:
        if (win.cy < win.numrows)
            win.cx = win.row[win.cy].size;
        break;

    case BACKSPACE:
    case CTRL_KEY('h'):
    case DEL_KEY:
        if (c == DEL_KEY)
            moveCursor(ARROW_RIGHT);
        delChar();
        break;

    case PAGE_UP:
    case PAGE_DOWN:
    {
        if (c == PAGE_UP)
        {
            win.cy = win.rowoff;
        }
        else if (c == PAGE_DOWN)
        {
            win.cy = win.rowoff + win.screenrows - 1;
            if (win.cy > win.numrows)
                win.cy = win.numrows;
        }

        int times = win.screenrows;
        while (times--)
            moveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
    }
    break;

    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_LEFT:
    case ARROW_RIGHT:
        moveCursor(c);
        break;

    case CTRL_KEY('l'):
    case '\x1b':
        break;

    default:
        insertChar(c);
        char *tmp = (char *)malloc(strlen(*str) + 2);
        memcpy(tmp, *str, strlen(*str));
        tmp[strlen(*str)] = c;
        tmp[strlen(*str) + 1] = '\0';
        *str = tmp;
        // printf("len of str now is %ld\n", strlen(*str));
        // printf("%s\n", *str);
        break;
    }

    quit_times = QUIT_TIMES;
}

// void die(const char *s)
// {
//     write(STDOUT_FILENO, "\x1b[2J", 4);
//     write(STDOUT_FILENO, "\x1b[H", 3);

//     perror(s);
//     exit(1);
// }
