typedef struct piece
{
    long length;
    char *txt;
} piece;

typedef struct node
{
    piece *blk;
    struct node *left;
    struct node *right;
    struct node *parent;
    long size_left, size_right;
} node;

typedef node *tree;

void fileInorder(tree root, char *filename);
void helpInorder(tree root, FILE *fp);
void infoInorder(tree root);
piece *createPiece(char *msg, long len);
node *insert(tree *root, long len, char *msg, long index);
void inorder(node *root);
void new_offset(node *t);
node *split(tree parent, long len, char *msg, long index, long offset, tree *nn);
long num_nodes(tree root);
long totalTxtLength(tree root);
void LLrotate(tree *q, tree *root);
void RRrotate(tree *q, tree *root);
void splay(tree p, tree *root);
node *undo(node *root);