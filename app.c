#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"

int main()
{
	tree table = NULL;
	stack_ll undo_st = NULL;
	node *curr = NULL;
	char piece1[] = "ori";
	table = insert(&table, strlen(piece1), piece1, 0, &undo_st);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");
	curr = pop(&undo_st);
	push(&undo_st, curr);
	// printf("index of last is %ld\n", totalTxtLength(table));
	printf("\n");

	char piece2[] = "My";
	table = insert(&table, strlen(piece2), piece2, 0, &undo_st);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");
	curr = pop(&undo_st);
	push(&undo_st, curr);
	// printf("index of last is %ld\n", totalTxtLength(table));
	printf("\n");

	char piece3[] = "*fv*";
	table = insert(&table, strlen(piece3), piece3, 2, &undo_st);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");
	curr = pop(&undo_st);
	push(&undo_st, curr);
	// printf("index of last is %ld\n", totalTxtLength(table));
	printf("\n");

	char piece4[] = "a";
	table = insert(&table, strlen(piece4), piece4, 4, &undo_st);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");
	curr = pop(&undo_st);
	push(&undo_st, curr);
	// printf("index of last is %ld\n", totalTxtLength(table));
	printf("\n");

	char piece5[] = "bue";
	table = insert(&table, strlen(piece5), piece5, totalTxtLength(table), &undo_st);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");
	curr = pop(&undo_st);
	push(&undo_st, curr);
	// printf("index of last is %ld\n", totalTxtLength(table));
	printf("\n");

	char piece6[] = "l";
	table = insert(&table, strlen(piece6), piece6, totalTxtLength(table) - 2, &undo_st);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");
	curr = pop(&undo_st);
	push(&undo_st, curr);
	// printf("index of last is %ld\n", totalTxtLength(table));
	printf("\n");

	char piece7[] = "*";
	table = insert(&table, strlen(piece7), piece7, 9, &undo_st);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");
	curr = pop(&undo_st);
	push(&undo_st, curr);
	// printf("index of last is %ld\n", totalTxtLength(table));
	printf("\n");

	char piece8[] = "col";
	table = insert(&table, strlen(piece8), piece8, 7, &undo_st);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");
	curr = pop(&undo_st);
	push(&undo_st, curr);
	// printf("index of last is %ld\n", totalTxtLength(table));
	printf("\n");

	char piece9[] = "*";
	table = insert(&table, strlen(piece9), piece9, totalTxtLength(table) - 4, &undo_st);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");
	curr = pop(&undo_st);
	push(&undo_st, curr);
	// printf("index of last is %ld\n", totalTxtLength(table));
	printf("\n");

	char piece10[] = "s";
	table = insert(&table, strlen(piece10), piece10, totalTxtLength(table) - 5, &undo_st);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");
	curr = pop(&undo_st);
	push(&undo_st, curr);
	// printf("index of last is %ld\n", totalTxtLength(table));
	printf("\n");

	for (int i = 0; i < 5; i++)
	{
		// printf("Undo %d\n", i);
		table = undo(table, &undo_st);
		inorder(table);
		printf("\n");
	}
	char piece11[] = "Hello";
	table = insert(&table, strlen(piece11), piece11, 0, &undo_st);
	printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");
	printf("index of last is %ld\n", totalTxtLength(table));

	for (int i = 0; i < 3; i++)
	{
		// printf("Undo %d\n", i);
		table = undo(table, &undo_st);
		inorder(table);
		printf("\n");
	}

	// char piece12[] = "World";
	// table = insert(&table, strlen(piece12), piece12, 0, &undo_st);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	// inorder(table);
	// printf("\n");
	// printf("index of last is %ld\n", totalTxtLength(table));

	// char piece13[] = " ";
	// table = insert(&table, strlen(piece13), piece13, 5, &undo_st);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	// inorder(table);
	// printf("\n");
	// printf("index of last is %ld\n", totalTxtLength(table));

	// char piece14[] = "!";
	// table = insert(&table, strlen(piece14), piece14, totalTxtLength(table), &undo_st);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	// inorder(table);
	// printf("\n");
	// printf("index of last is %ld\n", totalTxtLength(table));
	return 0;
}
