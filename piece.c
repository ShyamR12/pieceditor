#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"

void fileInorder(tree root, char *filename)
{
    FILE *fp = fopen(filename, "a+");
    helpInorder(root, fp);
    fclose(fp);
}

void helpInorder(tree root, FILE *fp)
{
    if (root)
    {
        helpInorder(root->left, fp);
        fputs(root->blk->txt, fp);
        helpInorder(root->right, fp);
    }
}

void infoInorder(tree root)
{
    if (root)
    {
        infoInorder(root->left);
        // printf("Inside inorder ");
        printf("%ld %s %ld ", root->size_left, root->blk->txt, root->size_right);
        if (root->parent)
        {
            printf("parent is %s | ", root->parent->blk->txt);
        }
        else
        {
            printf("root | ");
        }

        infoInorder(root->right);
    }
    return;
}

void inorder(tree root)
{
    if (root)
    {
        inorder(root->left);
        printf("%s", root->blk->txt);
        inorder(root->right);
    }
    return;
}

long totalTxtLength(tree root)
{
    if (root == NULL)
    {
        return 0;
    }
    // long left = num_nodes(root->left);
    // long right = num_nodes(root->right);
    return root->size_left + root->size_right + root->blk->length;
}

piece *createPiece(char *msg, long len)
{
    piece *new_msg = (piece *)malloc(sizeof(piece));
    new_msg->length = len;
    new_msg->txt = (char *)malloc(sizeof(char) * (len + 1));
    strncpy(new_msg->txt, msg, len);
    new_msg->txt[len] = '\0';
    return new_msg;
}

long num_nodes(tree root)
{
    if (root == NULL)
    {
        return 0;
    }
    long left = num_nodes(root->left);
    long right = num_nodes(root->right);
    return left + right + 1;
}

node *insert(tree *root, long len, char *msg, long index, stack_ll *undo_st, stack_ll *redo_st, int insert_of_redo)
{
    if (!insert_of_redo)
    {
        while (*redo_st)
        {
            int index = 0;
            pop(redo_st, &index);
        }
    }

    node *p = *root;
    long offset = 0;
    // creating the node of tree
    node *tmp = (node *)malloc(sizeof(node));
    // creating a new piece
    tmp->blk = createPiece(msg, len);
    tmp->left = tmp->right = tmp->parent = NULL;
    tmp->size_left = tmp->size_right = 0;
    tmp->split_part = NULL;

    if (p == NULL)
    {
        // printf("root is NULL\n");
        push(undo_st, tmp, index);
        return tmp;
    }
    else
    {
        offset = p->size_left;
        while (1)
        {
            // printf("Reached here 0\n");
            // printf("Length of p->blk is %ld , index is %ld and offset is %ld.\n", p->blk->length, index, offset);
            if (index == offset)
            {
                // printf("Got index = offset\n");
                node *child = p->left;
                p->left = tmp;
                p->left->parent = p;
                p->left->left = child;
                if (child)
                {
                    child->parent = p->left;
                }
                node *re_offset = tmp;
                while (re_offset)
                {
                    new_offset(re_offset);
                    re_offset = re_offset->parent;
                }
                splay(tmp, root);
                push(undo_st, tmp, index);
                return *root;
            }
            else if (index == p->blk->length + offset)
            {
                // printf("Got index == p->blk->length + offset\n");
                node *child = p->right;
                p->right = tmp;
                p->right->parent = p;
                p->right->right = child;
                if (child)
                {
                    child->parent = p->right;
                }
                node *re_offset = tmp;
                while (re_offset)
                {
                    // printf("%ld %s %ld \n", re_offset->size_left, re_offset->blk->txt, re_offset->size_right);
                    new_offset(re_offset);
                    // printf("txt inside re_offset now is %s\n", re_offset->blk->txt);
                    re_offset = re_offset->parent;
                }
                splay(tmp, root);
                push(undo_st, tmp, index);
                return *root;
            }
            else if (index < offset)
            {
                // printf("Reached at index < offset\n");
                // offset += p->size_left;
                p = p->left;
                // offset = p->size_left;
                // changed here
                offset = offset - p->blk->length - p->size_right;
            }
            else if (index > offset + p->blk->length)
            {
                // printf("Reached index>offset + p->blk->length\n");
                offset = offset + p->blk->length;
                p = p->right;
                offset = offset + p->size_left;
                // move to the right side
            }
            else if (index > offset && (index < offset + p->blk->length))
            {
                // split the current node
                // printf("Splitting\n");
                node *nn = NULL;
                if (p->parent == NULL)
                {
                    // printf("p->parent == NULL\n");
                    *root = split(p, len, msg, index, offset, &nn);
                    // printf("Before splaying1\n");
                    // infoInorder(*root);
                    splay(nn, root);
                }
                else if (p == p->parent->left)
                {
                    // printf("p == p->parent->left\n");
                    p->parent->left = split(p, len, msg, index, offset, &nn);
                    // printf("Before splaying2\n");
                    // infoInorder(*root);
                    splay(nn, root);
                }
                else
                {
                    // printf("p == p->parent->right\n");
                    p->parent->right = split(p, len, msg, index, offset, &nn);
                    // printf("Before splaying3\n");
                    // infoInorder(*root);
                    splay(nn, root);
                }
                push(undo_st, nn, index);

                return *root;
            }
            else
            {
                printf("Entered in else part\n");
            }
            // printf("Looping\n");
        }
    }
}

void new_offset(node *t)
{
    if (t == NULL)
    {
        return;
    }
    node *l, *r;
    l = r = t;
    l = l->left;
    r = r->right;
    long off_left, off_right;
    off_left = off_right = 0;
    if (l)
    {
        off_left = l->size_left + l->size_right + l->blk->length;
    }
    if (r)
    {
        off_right = r->size_left + r->size_right + r->blk->length;
    }
    t->size_left = off_left;
    t->size_right = off_right;
}

node *split(tree parent, long len, char *msg, long index, long offset, tree *nn)
{

    // node *p = parent;

    node *p = (node *)malloc(sizeof(node));
    piece *parent_msg = createPiece(parent->blk->txt, parent->blk->length);
    p->blk = parent_msg;
    p->left = parent->left;
    p->right = parent->right;
    p->parent = parent->parent;
    p->split_part = parent->split_part;
    // int blk_length = p->blk->length;
    // printf("offset - %ld, len - %ld, p->blk->length - %ld and index - %ld\n", offset, len, p->blk->length, index);
    node *first_half_ptr = parent;
    node *child = (node *)malloc(sizeof(node));
    node *half = (node *)malloc(sizeof(node));
    node *grandchild = p->left;
    *nn = child;
    // printf("Creating first half\n");
    // new first half

    piece *first_half = (piece *)malloc(sizeof(piece));
    first_half->length = p->blk->length - index + offset;
    // printf("len is %ld\n", len);
    first_half->txt = (char *)malloc(sizeof(char) * (p->blk->length - index + offset + 1));
    strncpy(first_half->txt, p->blk->txt + index - offset, p->blk->length - index + offset);
    first_half->txt[first_half->length] = '\0';
    // printf("First half text is %s\n", first_half->txt);
    first_half_ptr->blk = first_half;
    first_half_ptr->left = child;
    first_half_ptr->right = p->right;
    first_half_ptr->split_part = half;
    if (first_half_ptr->right)
    {
        first_half_ptr->right->parent = first_half_ptr;
    }

    first_half_ptr->parent = p->parent;

    // new msg
    // printf("Creating new msg\n");
    piece *new_msg = createPiece(msg, len);
    // printf("new_msg->txt = %s\n", new_msg->txt);
    child->blk = new_msg;
    child->left = half;
    child->right = NULL;
    child->parent = first_half_ptr;
    child->size_left = child->size_right = 0;
    child->split_part = NULL;
    // creating remaining half of the split
    // printf("Creating the other half\n");

    piece *half_msg = createPiece(p->blk->txt, index - offset);
    // printf("half_msg->txt = %s\n", half_msg->txt);
    half->blk = half_msg;
    half->left = grandchild;
    half->right = NULL;
    half->parent = child;
    half->size_left = half->size_right = 0;
    half->split_part = NULL;
    // joining parent->tmp->child->grandchild

    if (grandchild)
    {
        grandchild->parent = half;
    }

    // printf("Re offseting\n");
    // re_offsetting
    new_offset(half);
    new_offset(child);
    new_offset(first_half_ptr);
    // new_offset(parent);
    // printf("Reached at the end\n");
    // free(parent);
    return first_half_ptr;
}

void LLrotate(tree *q, tree *root)
{
    node *gp = *q;
    node *p = gp->left;
    if (gp->parent)
    {
        if (gp->parent->left == gp)
        {
            gp->parent->left = p;
        }
        else
        {
            gp->parent->right = p;
        }
    }
    else
    {
        *root = p;
    }
    p->parent = gp->parent;
    gp->left = p->right;
    if (gp->left)
    {

        gp->left->parent = gp;
    }
    p->right = gp;
    gp->parent = p;
    new_offset(gp);
    new_offset(p);
    *q = p;
}

void RRrotate(tree *q, tree *root)
{
    node *gp = *q;
    node *p = gp->right;
    if (gp->parent)
    {
        if (gp->parent->left == gp)
        {
            gp->parent->left = p;
        }
        else
        {
            gp->parent->right = p;
        }
    }
    else
    {
        *root = p;
    }
    p->parent = gp->parent;
    gp->right = p->left;
    if (gp->right)
    {

        gp->right->parent = gp;
    }
    p->left = gp;
    gp->parent = p;
    new_offset(gp);
    new_offset(p);
    *q = p;
}

void splay(tree p, tree *root)
{
    if (p == NULL)
    {
        return;
    }

    while (p != *root)
    {
        if (p == *root)
        {
            return;
        }
        node *critical = NULL;

        // grandparent doesn't exist
        if (p->parent->parent == NULL)
        {
            if (p->parent->left == p)
            {
                // do ll rotation
                // printf("Classified as LL\n");
                critical = p->parent;
                LLrotate(&critical, root);
            }
            else
            {
                // do rr rotation
                // printf("Classified as RR\n");
                critical = p->parent;
                RRrotate(&critical, root);
            }
        }
        else
        {
            if (p->parent->left == p)
            {
                /*
                    p
                  /
                c

                */
                if (p->parent->parent->left == p->parent)
                {
                    /*
                      gp
                     /
                    p
                  /
                c

                */
                    // printf("Classified as LL + LL\n");
                    critical = p->parent->parent;
                    node *critical2 = p->parent;
                    LLrotate(&critical, root);
                    LLrotate(&critical2, root);

                    // perform ll rotation on gp then again 1 ll on p
                }
                else
                {
                    /*
                    gp
                      \
                       p
                      /
                    c

                    */
                    //    ll on p then rr on gp
                    // printf("Classified as RL\n");
                    critical = p->parent;
                    LLrotate(&critical, root);
                    critical = p->parent;
                    RRrotate(&critical, root);
                }
            }
            else
            {
                if (p->parent->parent->left == p->parent)
                {
                    /*
                      gp
                     /
                    p
                     \
                      c

                */
                    // printf("Classified as LR\n");
                    //    rr on p then ll on gp
                    critical = p->parent;
                    RRrotate(&critical, root);
                    critical = p->parent;
                    LLrotate(&critical, root);
                }
                else
                {
                    /*
                  gp
                   \
                    p
                     \
                      c

                */
                    //    rr on p then rr on c
                    // printf("Classified as RR + RR\n");
                    critical = p->parent->parent;
                    node *critical2 = p->parent;
                    RRrotate(&critical, root);
                    RRrotate(&critical2, root);
                }
            }
        }
    }
}

node *joinSplit(tree p)
{
    if (p == NULL)
    {
        return NULL;
    }

    node *combined = (node *)malloc(sizeof(node));
    combined->parent = p->parent;
    combined->left = p->left;
    combined->right = p->right;
    combined->size_left = p->size_left;
    combined->size_right = p->size_right;

    int msg_len = p->blk->length;
    char *str = (char *)malloc(p->blk->length + 1);
    memcpy(str, p->blk->txt, msg_len);
    str[msg_len] = '\0';
    // printf("\ninital str is %s\n", str);
    while (p->split_part)
    {
        msg_len += p->split_part->blk->length;
        char *tmp = (char *)malloc(msg_len + 1);
        // printf("----\n");
        for (int i = 0; i < msg_len - strlen(str); i++)
        {
            tmp[i] = p->split_part->blk->txt[i];
            // printf("%c", tmp[i]);
        }
        // printf("-");
        for (int i = msg_len - strlen(str); i < msg_len; i++)
        {
            tmp[i] = str[i - msg_len + strlen(str)];
            // printf("%c", tmp[i]);
        }

        // printf("\n----\n");

        tmp[msg_len] = '\0';
        str = tmp;
        p = p->split_part;
        // printf("intermediate str is %s\n", str);
    }
    // printf("combined str is %s and length is %d\n\n", str, msg_len);
    // piece *half_msg = createPiece(p->blk->txt, index - offset);

    piece *combined_msg = createPiece(str, strlen(str));
    combined->blk = combined_msg;
    combined->split_part = NULL;
    return combined;
}

node *undo(tree root, stack_ll *undo_st, stack_ll *redo_st)
{
    if (root == NULL)
    {
        return NULL;
    }
    int index = 0;
    node *rem = NULL;
    rem = pop(undo_st, &index);
    // join if it was splitted earlier
    node *p = joinSplit(rem);
    push(redo_st, p, index);
    // if (rem == NULL)
    // {
    //     printf("rem is null\n");
    // }
    // else
    // {
    //     printf("rem is %s\n", rem->blk->txt);
    // }

    // printf("Reached here 0\n");
    // printf("root is %s\n", root->blk->txt);
    splay(rem, &root);
    // printf("Reached here 1\n");
    while (rem)
    {
        splay(rem, &root);
        node *temp;
        if (root->left == NULL)
        {
            temp = root;
            root = root->right;
            if (root)
            {
                root->parent = NULL;
            }
            // printf("Reached\n");
        }
        else
        {
            // printf("Reached in else part\n");
            temp = root;
            root->left->parent = NULL;
            node *pred = root->left;
            while (pred->right)
            {
                pred = pred->right;
            }
            splay(pred, &(root->left));
            // printf("splayed pred to left top\n");
            root = root->left;
            root->right = temp->right;
            if (root->right)
            {
                root->right->parent = root;
            }
            // printf("work done\n");
        }
        free(temp);
        new_offset(root);
        rem = rem->split_part;
    }

    return root;
}

node *redo(tree *root, stack_ll *redo_st, stack_ll *undo_st)
{
    int index = 0;
    node *p = pop(redo_st, &index);
    if (p == NULL)
    {
        return *root;
    }
    *root = insert(root, p->blk->length, p->blk->txt, index, undo_st, redo_st, 1);
    return *root;
}

void push(stack_ll *top, node *key, long pos)
{

    stack *p = NULL;

    if (*top == NULL)
    {
        *top = (stack *)malloc(sizeof(stack));

        (*top)->data = key;
        (*top)->index = pos;
        (*top)->next = NULL;
    }
    else
    {
        p = (stack *)malloc(sizeof(stack));
        p->data = key;
        p->index = pos;
        p->next = *top;
        *top = p;
    }
}

int isEmpty(stack *top)
{
    if (top == NULL)
    {
        return 1;
    }
    return 0;
}

node *pop(stack_ll *top, int *pos)
{
    stack *p = *top;
    if (p == NULL)
    {
        // printf("Stack is Empty\n");
        return NULL;
    }
    node *x = p->data;
    *pos = p->index;
    *top = (*top)->next;
    free(p);
    return x;
}

int isFull()
{
    stack *p = (stack *)malloc(sizeof(stack));

    if (p == NULL)
    {
        return 1;
    }
    return 0;
}
