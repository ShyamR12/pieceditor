typedef struct piece
{
    // long start;
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

node *insert(tree root,long len, char *msg,long index);
void inorder(node *root);
long new_offset(node *t);
node *split(tree parent,long len, char *msg, long index, long offset, tree *tmp);
