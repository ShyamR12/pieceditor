#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"

int main()
{
	tree table = NULL;
	char piece1[] = "ori";
	table = insert(&table, strlen(piece1), piece1, 0);
	inorder(table);
	printf("\n");
	printf("\n");

	char piece2[] = "My";
	table = insert(&table, strlen(piece2), piece2, 0);
	inorder(table);
	printf("\n");
	printf("\n");

	char piece3[] = "*fv*";
	table = insert(&table, strlen(piece3), piece3, 2);
	inorder(table);
	printf("\n");
	printf("\n");

	char piece4[] = "a";
	table = insert(&table, strlen(piece4), piece4, 4);
	inorder(table);
	printf("\n");
	printf("\n");

	char piece5[] = "bue";
	table = insert(&table, strlen(piece5), piece5, totalTxtLength(table));
	inorder(table);
	printf("\n");
	printf("\n");

	char piece6[] = "l";
	table = insert(&table, strlen(piece6), piece6, totalTxtLength(table) - 2);
	inorder(table);
	printf("\n");
	printf("\n");

	char piece7[] = "*";
	table = insert(&table, strlen(piece7), piece7, 9);
	inorder(table);
	printf("\n");
	printf("\n");

	char piece8[] = "col";
	table = insert(&table, strlen(piece8), piece8, 7);
	inorder(table);
	printf("\n");
	printf("\n");

	char piece9[] = "*";
	table = insert(&table, strlen(piece9), piece9, totalTxtLength(table) - 4);
	infoInorder(table);
	printf("\n");
	printf("\n");

	char piece10[] = "s";
	table = insert(&table, strlen(piece10), piece10, totalTxtLength(table) - 5);
	infoInorder(table);
	printf("\n");
	printf("\n");

	printf("Testing undo operation\n\n");
	table = undo(table);
	infoInorder(table);
	printf("\n");
	printf("\n");

	table = undo(table);
	infoInorder(table);
	printf("\n");
	printf("\n");
	return 0;
}
