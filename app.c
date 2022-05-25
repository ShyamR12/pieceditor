#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"

int main()
{
	tree table = NULL;
	// node *insert(tree *root,long len, char *msg, long index)
	char piece1[5] = "Hello";
	table = insert(table,5,piece1,0);
	printf("Inserted\n");
	inorder(table);
	printf("\n");
	char piece2[5] = "World";
	table = insert(table,5,piece2,5);
	printf("Inserted\n");
	printf("diplaying inorder\n");
	inorder(table);
	/*char piece3[] = "Good Morning";
	table = insert(table,12,piece3,5);
	printf("diplaying inorder\n");
	inorder(table);*/
	printf("\n");		
    return 0;
}
