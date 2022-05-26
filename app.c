#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"

int main()
{
	tree table = NULL;
	// node *insert(tree *root,long len, char *msg, long index)
	char piece1[] = "Hello";
	printf("Length of Hello is %ld\n", strlen(piece1));
	table = insert(table, strlen(piece1), piece1, 0);
	printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");

	char piece2[] = "World";
	table = insert(table, strlen(piece2), piece2, 5);
	printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");

	char piece3[] = " ";
	// char piece3[] = "Good Morning";
	table = insert(table, strlen(piece3), piece3, 5);
	printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");

	char piece4[] = "!";
	printf("index of last is %ld\n", totalTxtLength(table));
	table = insert(table, strlen(piece4), piece4, totalTxtLength(table));
	printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");
	return 0;
}
