void initEditor();

void insertChar(int c);
void insertNewLine();
void delChar();

void appendBuffer(buffer *ab, const char *s, int len);
void freeBuffer(buffer *ab);
