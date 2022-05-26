#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"

int main()
{
	tree table = NULL;
	// node *insert(tree *root,long len, char *msg, long index)
	char piece1[] = "HelloWorld";
	table = insert(table, strlen(piece1), piece1, 0);
	printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");
	printf("index of last is %ld\n", totalTxtLength(table));

	char piece2[] = "*";
	table = insert(table, strlen(piece2), piece2, 5);
	printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");
	printf("index of last is %ld\n", totalTxtLength(table));

	// char piece3[] = " fav ";
	// table = insert(table, strlen(piece3), piece3, 2);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	// inorder(table);
	// printf("\n");
	// printf("index of last is %ld\n", totalTxtLength(table));

	// char piece4[] = "blue";
	// table = insert(table, strlen(piece4), piece4, totalTxtLength(table));
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	// inorder(table);
	// printf("\n");
	// printf("index of last is %ld\n", totalTxtLength(table));

	// char piece5[] = " is ";
	// table = insert(table, strlen(piece5), piece5, 13);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	// inorder(table);
	// printf("\n");
	// printf("index of last is %ld\n", totalTxtLength(table));

	// char piece6[] = "*sentence";
	// table = insert(table, strlen(piece6), piece6, totalTxtLength(table));
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	// inorder(table);
	// printf("\n");
	// printf("index of last is %ld\n", totalTxtLength(table));
	return 0;
}
