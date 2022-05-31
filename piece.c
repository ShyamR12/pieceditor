#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"

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

node *insert(tree *root, long len, char *msg, long index)
{
    node *p = *root;
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
            printf("Length of p->blk is %ld , index is %ld and offset is %ld.\n", p->blk->length, index, offset);
            if (index == offset)
            {
                printf("Got index = offset\n");
                node *child = p->left;
                p->left = tmp;
                p->left->parent = p;
                p->left->left = child;
                if (child)
                {
                    child->parent = p->left;
                }
                // splay(tmp, root);
                node *re_offset = tmp;
                while (re_offset)
                {
                    new_offset(re_offset);
                    re_offset = re_offset->parent;
                }
                return *root;
            }
            else if (index == p->blk->length + offset)
            {
                printf("Got index == p->blk->length + offset\n");
                node *child = p->right;
                p->right = tmp;
                p->right->parent = p;
                p->right->right = child;
                if (child)
                {
                    child->parent = p->right;
                }
                // splay(tmp, root);
                node *re_offset = tmp;
                while (re_offset)
                {
                    // printf("%ld %s %ld \n", re_offset->size_left, re_offset->blk->txt, re_offset->size_right);
                    new_offset(re_offset);
                    // printf("txt inside re_offset now is %s\n", re_offset->blk->txt);
                    re_offset = re_offset->parent;
                }
                return *root;
            }
            else if (index < offset)
            {
                printf("Reached at index < offset\n");
                // offset += p->size_left;
                p = p->left;
                offset = p->size_left;
            }
            else if (index > offset + p->blk->length)
            {
                printf("Reached index>offset + p->blk->length\n");
                offset = offset + p->blk->length;
                p = p->right;
                // move to the right side
            }
            else if (index > offset && (index < offset + p->blk->length))
            {
                // split the current node
                printf("Splitting\n");
                node *nn = NULL;
                if (p->parent == NULL)
                {
                    printf("p->parent == NULL\n");
                    *root = split(p, len, msg, index, offset, &nn);
                    // splay(nn, root);
                }
                else if (p == p->parent->left)
                {
                    printf("p == p->parent->left\n");
                    p->parent->left = split(p, len, msg, index, offset, &nn);
                    // splay(nn, root);
                }
                else
                {
                    printf("p == p->parent->right\n");
                    p->parent->right = split(p, len, msg, index, offset, &nn);
                    // splay(nn, root);
                }
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
    // printf("offset - %ld, len - %ld and index - %ld\n", offset, len, index);
    node *p = parent;
    node *first_half_ptr = (node *)malloc(sizeof(node));
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
    // strncpy(first_half->txt, p->blk->txt + p->blk->length - index + offset, p->blk->length);
    strncpy(first_half->txt, p->blk->txt + index - offset, p->blk->length - index + offset);
    // piece *first_half = createPiece(p->blk->txt, parent->blk->length + offset - index);
    // printf("first_half->txt = %s\n", first_half->txt);
    // printf("first_half created\n");
    first_half_ptr->blk = first_half;
    first_half_ptr->left = child;
    first_half_ptr->right = p->right;
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

    // creating remaining half of the split
    // printf("Creating the other half\n");

    piece *half_msg = createPiece(p->blk->txt, index - offset);
    // printf("half_msg->txt = %s\n", half_msg->txt);
    half->blk = half_msg;
    half->left = grandchild;
    half->right = NULL;
    half->parent = child;
    half->size_left = half->size_right = 0;

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
                critical = p->parent;
                LLrotate(&critical, root);
            }
            else
            {
                // do rr rotation
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
                    critical = p->parent->parent;
                    node *critical2 = p->parent;
                    RRrotate(&critical, root);
                    RRrotate(&critical2, root);
                }
            }
        }
    }
}
