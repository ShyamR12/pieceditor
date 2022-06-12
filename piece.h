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
    struct node *split_part;
} node;

typedef node *tree;

typedef struct stack
{
    struct node *data;
    struct stack *next;
} stack;

typedef stack *stack_ll;

void push(stack_ll *top, node *data);
int isEmpty(stack *top);
node *pop(stack_ll *top);
int isFull();

void fileInorder(tree root, char *filename);
void helpInorder(tree root, FILE *fp);
void infoInorder(tree root);
piece *createPiece(char *msg, long len);
node *insert(tree *root, long len, char *msg, long index, stack_ll *undo_st);
void inorder(node *root);
void new_offset(node *t);
node *split(tree parent, long len, char *msg, long index, long offset, tree *nn);
long num_nodes(tree root);
long totalTxtLength(tree root);
void LLrotate(tree *q, tree *root);
void RRrotate(tree *q, tree *root);
void splay(tree p, tree *root);
node *undo(tree root, stack_ll *undo_st);
