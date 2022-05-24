#include <stdio.h>
#include <stdlib.h>

void inorder(tree root)
{
    if (root)
    {
        inorder(root->left);
        printf("%ld  %s\n", root->blk->start, root->blk->txt);
        inorder(root->right);
    }
    return;
}

node *insert(tree *root, long st, long len, char *msg, long index)
{
    node *p = *root;
    long offset = p->size_left;
    // creating a new piece
    piece *new_msg = (piece *)malloc(sizeof(piece));
    new_msg->start = index;
    new_msg->length = len;
    new_msg->txt = (char *)malloc(sizeof(char) * (len + 1));
    new_msg->txt = msg;

    // creating the node of tree
    node *tmp = (node *)malloc(sizeof(node));
    tmp->blk = new_msg;
    tmp->left = tmp->right = tmp->parent = NULL;
    tmp->size_left = tmp->size_right = 0;

    if (p == NULL)
    {
        return tmp;
    }
    else
    {
        while (1)
        {
            if (index == offset)
            {
                node *child = p->left;
                p->left = tmp;
                p->left->parent = p->left;
                p->left->left = child;
                child->parent = p->left;
                node *re_offset = tmp;
                while (re_offset)
                {
                    re_offset->size_left = new_offset(re_offset);
                    re_offset = re_offset->parent;
                }
                return *root;
            }
            else if (index < offset)
            {
                // if (index < offset + p->blk->length)
                // {
                //     p = p->left;
                // }
                // else
                // {
                //     // split the current node
                // }
                // // check node_length + offset if > index then split
                offset = p->size_left;
            }
            else if (index > offset + p->blk->length)
            {
                offset = offset + p->blk->length;
                p = p->right;
                // move to the right side
            }
            else if (index > offset)
            {
                // split the current node
            }

            // offset = p->size_left;
        }
    }
}

long new_offset(node *t)
{
    if (t == NULL || t->left == NULL)
    {
        return 0;
    }
    t = t->left;
    long off = t->size_left + t->size_right + t->blk->length;
    return off;
}

node *split(tree parent, long st, long len, char *msg, long index, long offset, tree *tmp)
{
    node *p = parent;
    node *grandchild = p->left;

    // creating later half of the split
    piece *new_msg = (piece *)malloc(sizeof(piece));
    new_msg->start = p->left->size_left;
    new_msg->length = index;
    new_msg->txt = (char *)malloc(sizeof(char) * (new_msg->length + 1));
    strncpy(new_msg->txt, msg, new_msg->length);
    new_msg->txt[new_msg->length] = '\0';

    node *child = (node *)malloc(sizeof(node));
    child->blk = new_msg;
    child->left = grandchild;
    child->right = child->parent = NULL;
    child->size_left = child->size_right = 0;

    // joining parent->tmp->child->grandchild

    if (grandchild)
    {
        grandchild->parent = child;
    }
    (*tmp)->parent = p;
    p->left = *tmp;
    (*tmp)->left = child;
    child->parent = (*tmp);

    // re_offsetting
    child->size_left = new_offset(child);
    (*tmp)->size_left = new_offset(*tmp);
    parent->size_left = new_offset(parent);
    return parent;
}
