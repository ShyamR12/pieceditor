// #define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define TAB_STOP 8
#define QUIT_TIMES 1
#define CTRL_KEY(k) ((k)&0x1f)

#define ABUF_INIT \
    {             \
        NULL, 0   \
    }

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

typedef struct buffer
{
    char *b;
    int len;
} buffer;
