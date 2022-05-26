#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"

void inorder(tree root)
{
    if (root)
    {
        inorder(root->left);
        // printf("Inside inorder ");
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

node *insert(tree root, long len, char *msg, long index)
{
    node *p = root;
    long offset = 0;
    // creating the node of tree
    node *tmp = (node *)malloc(sizeof(node));
    // creating a new piece
    tmp->blk = createPiece(msg, len);
    tmp->left = tmp->right = tmp->parent = NULL;
    tmp->size_left = tmp->size_right = 0;

    if (p == NULL)
    {
        // printf("root is NULL\n");
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
                    re_offset->size_left = new_offset(re_offset);
                    re_offset = re_offset->parent;
                }
                return root;
            }
            else if (index == p->blk->length + offset)
            {
                // printf("Reached in index == offset +  p->blk->length");
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
                    re_offset->size_left = new_offset(re_offset);
                    // printf("txt inside re_offset now is %s\n", re_offset->blk->txt);
                    re_offset = re_offset->parent;
                }
                return root;
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
                // printf("Reached at index<offset\n");
                offset += p->size_left;
            }
            else if (index > offset + p->blk->length)
            {
                // printf("Reached index>offset + p->blk->length\n");
                offset = offset + p->blk->length;
                p = p->right;
                // move to the right side
            }
            else if (index > offset && (index < offset + p->blk->length))
            {
                // split the current node
                // printf("Splitting\n");
                p = split(p, len, msg, index, offset, &tmp);
                node *re_offset = tmp;
                while (re_offset)
                {
                    re_offset->size_left = new_offset(re_offset);
                    re_offset = re_offset->parent;
                }
                return root;
            }
            else
            {
                // printf("Entered in else part\n");
            }

            // offset = p->size_left;
            // printf("Looping\n");
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

node *split(tree parent, long len, char *msg, long index, long offset, tree *tmp)
{
    node *p = parent;
    node *grandchild = p->left;

    // creating later half of the split
    piece *new_msg = (piece *)malloc(sizeof(piece));
    // new_msg->start = p->left->size_left;
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
