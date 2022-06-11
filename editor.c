/*** includes ***/

#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

/*** defines ***/

#define TAB_STOP 8
#define QUIT_TIMES 1

#define CTRL_KEY(k) ((k)&0x1f)

enum editorKey
{
    BACKSPACE = 127,
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    DEL_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN
};

/*** data ***/

typedef struct ROW
{
    int size;
    int rsize;
    char *chars;
    char *render;
} ROW;

struct editorWin
{
    int cx, cy;
    int rx;
    int rowoff;
    int coloff;
    int screenrows;
    int screencols;
    int numrows;
    ROW *row;
    int change;
    char *filename;
    char statusmsg[80];
    time_t statusmsg_time;
    struct termios orig_termios;
};

struct editorWin window;

/*** prototypes ***/

void setStatusMessage(const char *fmt, ...);
void refreshScreen();
char *prompt(char *prompt);

/*** terminal ***/

void die(const char *s)
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}

void endRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &window.orig_termios) == -1)
        die("tcsetattr");
}

void setRawMode()
{
    if (tcgetattr(STDIN_FILENO, &window.orig_termios) == -1)
        die("tcgetattr");
    atexit(endRawMode);

    struct termios raw = window.orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

int editorReadKey()
{
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
    {
        if (nread == -1 && errno != EAGAIN)
            die("read");
    }

    if (c == '\x1b')
    {
        char seq[3];

        if (read(STDIN_FILENO, &seq[0], 1) != 1)
            return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1)
            return '\x1b';

        if (seq[0] == '[')
        {
            if (seq[1] >= '0' && seq[1] <= '9')
            {
                if (read(STDIN_FILENO, &seq[2], 1) != 1)
                    return '\x1b';
                if (seq[2] == '~')
                {
                    switch (seq[1])
                    {
                    case '1':
                        return HOME_KEY;
                    case '3':
                        return DEL_KEY;
                    case '4':
                        return END_KEY;
                    case '5':
                        return PAGE_UP;
                    case '6':
                        return PAGE_DOWN;
                    case '7':
                        return HOME_KEY;
                    case '8':
                        return END_KEY;
                    }
                }
            }
            else
            {
                switch (seq[1])
                {
                case 'A':
                    return ARROW_UP;
                case 'B':
                    return ARROW_DOWN;
                case 'C':
                    return ARROW_RIGHT;
                case 'D':
                    return ARROW_LEFT;
                case 'H':
                    return HOME_KEY;
                case 'F':
                    return END_KEY;
                }
            }
        }
        else if (seq[0] == 'O')
        {
            switch (seq[1])
            {
            case 'H':
                return HOME_KEY;
            case 'F':
                return END_KEY;
            }
        }

        return '\x1b';
    }
    else
    {
        return c;
    }
}

int getCursorPos(int *rows, int *cols)
{
    char buf[32];
    unsigned int i = 0;

    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
        return -1;

    while (i < sizeof(buf) - 1)
    {
        if (read(STDIN_FILENO, &buf[i], 1) != 1)
            break;
        if (buf[i] == 'R')
            break;
        i++;
    }
    buf[i] = '\0';

    if (buf[0] != '\x1b' || buf[1] != '[')
        return -1;
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2)
        return -1;

    return 0;
}

int getWinSize(int *rows, int *cols)
{
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
    {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
            return -1;
        return getCursorPos(rows, cols);
    }
    else
    {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

/*** row operations ***/

int cxToRx(ROW *row, int cx)
{
    int rx = 0;
    int j;
    for (j = 0; j < cx; j++)
    {
        if (row->chars[j] == '\t')
            rx += (TAB_STOP - 1) - (rx % TAB_STOP);
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
    row->render = malloc(row->size + tabs * (TAB_STOP - 1) + 1);

    int idx = 0;
    for (j = 0; j < row->size; j++)
    {
        if (row->chars[j] == '\t')
        {
            row->render[idx++] = ' ';
            while (idx % TAB_STOP != 0)
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
    if (at < 0 || at > window.numrows)
        return;

    window.row = realloc(window.row, sizeof(ROW) * (window.numrows + 1));
    memmove(&window.row[at + 1], &window.row[at], sizeof(ROW) * (window.numrows - at));

    window.row[at].size = len;
    window.row[at].chars = malloc(len + 1);
    memcpy(window.row[at].chars, s, len);
    window.row[at].chars[len] = '\0';

    window.row[at].rsize = 0;
    window.row[at].render = NULL;
    updateRow(&window.row[at]);

    window.numrows++;
    window.change++;
}

void freeRow(ROW *row)
{
    free(row->render);
    free(row->chars);
}

void delRow(int at)
{
    if (at < 0 || at >= window.numrows)
        return;
    freeRow(&window.row[at]);
    memmove(&window.row[at], &window.row[at + 1], sizeof(ROW) * (window.numrows - at - 1));
    window.numrows--;
    window.change++;
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
    window.change++;
}

void rowAppendString(ROW *row, char *s, size_t len)
{
    row->chars = realloc(row->chars, row->size + len + 1);
    memcpy(&row->chars[row->size], s, len);
    row->size += len;
    row->chars[row->size] = '\0';
    updateRow(row);
    window.change++;
}

void rowDelChar(ROW *row, int at)
{
    if (at < 0 || at >= row->size)
        return;
    memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
    row->size--;
    updateRow(row);
    window.change++;
}

/*** editor operations ***/

void insertChar(int c)
{
    if (window.cy == window.numrows)
    {
        insertRow(window.numrows, "", 0);
    }
    rowInsertChar(&window.row[window.cy], window.cx, c);
    window.cx++;
}

void insertNewLine()
{
    if (window.cx == 0)
    {
        insertRow(window.cy, "", 0);
    }
    else
    {
        ROW *row = &window.row[window.cy];
        insertRow(window.cy + 1, &row->chars[window.cx], row->size - window.cx);
        row = &window.row[window.cy];
        row->size = window.cx;
        row->chars[row->size] = '\0';
        updateRow(row);
    }
    window.cy++;
    window.cx = 0;
}

void delChar()
{
    if (window.cy == window.numrows)
        return;
    if (window.cx == 0 && window.cy == 0)
        return;

    ROW *row = &window.row[window.cy];
    if (window.cx > 0)
    {
        rowDelChar(row, window.cx - 1);
        window.cx--;
    }
    else
    {
        window.cx = window.row[window.cy - 1].size;
        rowAppendString(&window.row[window.cy - 1], row->chars, row->size);
        delRow(window.cy);
        window.cy--;
    }
}

/*** append buffer ***/

typedef struct buffer
{
    char *b;
    int len;
} buffer;

#define ABUF_INIT \
    {             \
        NULL, 0   \
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

/*** file i/o ***/

char *rowToString(int *buflen)
{
    int totlen = 0;
    int j;
    for (j = 0; j < window.numrows; j++)
        totlen += window.row[j].size + 1;
    *buflen = totlen;

    char *buf = malloc(totlen);
    char *p = buf;
    for (j = 0; j < window.numrows; j++)
    {
        memcpy(p, window.row[j].chars, window.row[j].size);
        p += window.row[j].size;
        *p = '\n';
        p++;
    }

    return buf;
}

void editorOpen(char *filename)
{
    free(window.filename);
    window.filename = strdup(filename);

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
        insertRow(window.numrows, line, linelen);
    }
    free(line);
    fclose(fp);
    window.change = 0;
}

void editorSave()
{
    if (window.filename == NULL)
    {
        window.filename = prompt("Save as: %s (ESC to cancel)");
        if (window.filename == NULL)
        {
            setStatusMessage("Save aborted");
            return;
        }
    }

    int len;
    char *buf = rowToString(&len);

    int fd = open(window.filename, O_RDWR | O_CREAT, 0644);
    if (fd != -1)
    {
        if (ftruncate(fd, len) != -1)
        {
            if (write(fd, buf, len) == len)
            {
                close(fd);
                free(buf);
                window.change = 0;
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
    window.rx = 0;
    if (window.cy < window.numrows)
    {
        window.rx = cxToRx(&window.row[window.cy], window.cx);
    }

    if (window.cy < window.rowoff)
    {
        window.rowoff = window.cy;
    }
    if (window.cy >= window.rowoff + window.screenrows)
    {
        window.rowoff = window.cy - window.screenrows + 1;
    }
    if (window.rx < window.coloff)
    {
        window.coloff = window.rx;
    }
    if (window.rx >= window.coloff + window.screencols)
    {
        window.coloff = window.rx - window.screencols + 1;
    }
}

void drawRows(buffer *ab)
{
    int y;
    for (y = 0; y < window.screenrows; y++)
    {
        int filROW = y + window.rowoff;
        if (filROW >= window.numrows)
        {
            if (window.numrows == 0 && y == window.screenrows / 3)
            {
                char welcome[80];
                int welcomelen = snprintf(welcome, sizeof(welcome),
                                          "Text Editor 0.4.2");
                if (welcomelen > window.screencols)
                    welcomelen = window.screencols;
                int padding = (window.screencols - welcomelen) / 2;
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
            int len = window.row[filROW].rsize - window.coloff;
            if (len < 0)
                len = 0;
            if (len > window.screencols)
                len = window.screencols;
            appendBuffer(ab, &window.row[filROW].render[window.coloff], len);
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
                       window.filename ? window.filename : "[Untitled]", window.numrows,
                       window.change ? "[modified]" : "");
    int rlen = snprintf(rstatus, sizeof(rstatus), "(%d,%d)",
                        window.cy, window.cx);
    if (len > window.screencols)
        len = window.screencols;
    appendBuffer(ab, status, len);
    while (len < window.screencols)
    {
        if (window.screencols - len == rlen)
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
    int msglen = strlen(window.statusmsg);
    if (msglen > window.screencols)
        msglen = window.screencols;
    if (msglen && time(NULL) - window.statusmsg_time < 5)
        appendBuffer(ab, window.statusmsg, msglen);
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
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (window.cy - window.rowoff) + 1,
             (window.rx - window.coloff) + 1);
    appendBuffer(&ab, buf, strlen(buf));

    appendBuffer(&ab, "\x1b[?25h", 6);

    write(STDOUT_FILENO, ab.b, ab.len);
    freeBuffer(&ab);
}

void setStatusMessage(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(window.statusmsg, sizeof(window.statusmsg), fmt, ap);
    va_end(ap);
    window.statusmsg_time = time(NULL);
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
    ROW *row = (window.cy >= window.numrows) ? NULL : &window.row[window.cy];

    switch (key)
    {
    case ARROW_LEFT:
        if (window.cx != 0)
        {
            window.cx--;
        }
        else if (window.cy > 0)
        {
            window.cy--;
            window.cx = window.row[window.cy].size;
        }
        break;
    case ARROW_RIGHT:
        if (row && window.cx < row->size)
        {
            window.cx++;
        }
        else if (row && window.cx == row->size)
        {
            window.cy++;
            window.cx = 0;
        }
        break;
    case ARROW_UP:
        if (window.cy != 0)
        {
            window.cy--;
        }
        break;
    case ARROW_DOWN:
        if (window.cy < window.numrows)
        {
            window.cy++;
        }
        break;
    }

    row = (window.cy >= window.numrows) ? NULL : &window.row[window.cy];
    int rowlen = row ? row->size : 0;
    if (window.cx > rowlen)
    {
        window.cx = rowlen;
    }
}

void processKeyPress()
{
    static int quit_times = QUIT_TIMES;

    int c = editorReadKey();

    switch (c)
    {
    case '\r':
        insertNewLine();
        break;

    case CTRL_KEY('q'):
        if (window.change && quit_times > 0)
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

    case HOME_KEY:
        window.cx = 0;
        break;

    case END_KEY:
        if (window.cy < window.numrows)
            window.cx = window.row[window.cy].size;
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
            window.cy = window.rowoff;
        }
        else if (c == PAGE_DOWN)
        {
            window.cy = window.rowoff + window.screenrows - 1;
            if (window.cy > window.numrows)
                window.cy = window.numrows;
        }

        int times = window.screenrows;
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
        break;
    }

    quit_times = QUIT_TIMES;
}

/*** init ***/

void initEditor()
{
    window.cx = 0;
    window.cy = 0;
    window.rx = 0;
    window.rowoff = 0;
    window.coloff = 0;
    window.numrows = 0;
    window.row = NULL;
    window.change = 0;
    window.filename = NULL;
    window.statusmsg[0] = '\0';
    window.statusmsg_time = 0;

    if (getWinSize(&window.screenrows, &window.screencols) == -1)
        die("getWinSize");
    window.screenrows -= 2;
}

int main(int argc, char *argv[])
{
    setRawMode();
    initEditor();
    if (argc >= 2)
    {
        editorOpen(argv[1]);
    }

    setStatusMessage("HELP: Ctrl + S = Save | Ctrl + Q = Quit");

    while (1)
    {
        refreshScreen();
        processKeyPress();
    }

    return 0;
}