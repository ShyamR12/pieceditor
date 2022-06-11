int cxToRx(ROW *row, int cx);
void updateRow(ROW *row);
void insertRow(int at, char *s, size_t len);
void freeRow(ROW *row);
void delRow(int at);
void rowInsertChar(ROW *row, int at, int c);
void rowAppendString(ROW *row, char *s, size_t len);
void rowDelChar(ROW *row, int at);
