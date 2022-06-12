#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"

int main()
{
	tree table = NULL;
	stack_ll undo_st = NULL;
	stack_ll redo_st = NULL;
	node *curr = NULL;
	char piece1[] = "ori";
	table = insert(&table, strlen(piece1), piece1, 0, &undo_st, &redo_st, 0);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");

	// printf("index of last is %ld\n", totalTxtLength(table));
	printf("\n");

	char piece2[] = "My";
	table = insert(&table, strlen(piece2), piece2, 0, &undo_st, &redo_st, 0);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");

	// printf("index of last is %ld\n", totalTxtLength(table));
	printf("\n");

	char piece3[] = "*fv*";
	table = insert(&table, strlen(piece3), piece3, 2, &undo_st, &redo_st, 0);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");

	// printf("index of last is %ld\n", totalTxtLength(table));
	printf("\n");

	printf("undo\n");
	table = undo(table, &undo_st, &redo_st);
	inorder(table);
	printf("\n");
	char piece12[] = "O";
	table = insert(&table, strlen(piece12), piece12, totalTxtLength(table) - 3, &undo_st, &redo_st, 0);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");
	printf("Redo\n");
	table = redo(&table, &redo_st, &undo_st);
	inorder(table);
	printf("\n");
	printf("Redo\n");
	table = redo(&table, &redo_st, &undo_st);
	inorder(table);
	printf("\n");

	char piece4[] = "a";
	table = insert(&table, strlen(piece4), piece4, 4, &undo_st, &redo_st, 0);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");

	// printf("index of last is %ld\n", totalTxtLength(table));
	printf("\n");

	char piece5[] = "bue";
	table = insert(&table, strlen(piece5), piece5, totalTxtLength(table), &undo_st, &redo_st, 0);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");

	// printf("index of last is %ld\n", totalTxtLength(table));
	printf("\n");

	char piece6[] = "l";
	table = insert(&table, strlen(piece6), piece6, totalTxtLength(table) - 2, &undo_st, &redo_st, 0);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");

	// printf("index of last is %ld\n", totalTxtLength(table));
	printf("\n");

	char piece7[] = "*";
	table = insert(&table, strlen(piece7), piece7, 9, &undo_st, &redo_st, 0);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");

	// printf("index of last is %ld\n", totalTxtLength(table));
	printf("\n");

	char piece8[] = "col";
	table = insert(&table, strlen(piece8), piece8, 7, &undo_st, &redo_st, 0);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");

	// printf("index of last is %ld\n", totalTxtLength(table));
	printf("\n");

	char piece9[] = "*";
	table = insert(&table, strlen(piece9), piece9, totalTxtLength(table) - 4, &undo_st, &redo_st, 0);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");

	// printf("index of last is %ld\n", totalTxtLength(table));
	printf("\n");

	char piece10[] = "s";
	table = insert(&table, strlen(piece10), piece10, totalTxtLength(table) - 5, &undo_st, &redo_st, 0);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");

	// printf("index of last is %ld\n", totalTxtLength(table));
	printf("\n");
	printf("undo\n");
	table = undo(table, &undo_st, &redo_st);
	inorder(table);
	printf("\n");
	char piece11[] = "O";
	table = insert(&table, strlen(piece11), piece11, totalTxtLength(table) - 5, &undo_st, &redo_st, 0);
	// printf("Inserted and num_nodes = %ld\n", num_nodes(table));
	inorder(table);
	printf("\n");
	printf("Redo\n");
	table = redo(&table, &redo_st, &undo_st);
	inorder(table);
	printf("\n");
	printf("\n");

	for (int i = 0; i < 9; i++)
	{
		printf("Undo %d\n", i);
		table = undo(table, &undo_st, &redo_st);
		// node *in_redo = NULL;
		// if (redo_st)
		// {
		// 	printf("Reached here 0\n");
		// 	in_redo = redo_st->data;
		// 	printf("content is %s\n", redo_st->data->blk->txt);
		// }
		// if (in_redo == NULL)
		// {
		// 	printf("Reached here 1\n");
		// 	printf("redo is empty\n");
		// }
		// else
		// {
		// 	printf("Reached here 1\n");
		// 	printf("Data in redo now is %s\n", in_redo->blk->txt);
		// }
		inorder(table);
		printf("\n");
	}
	for (int i = 0; i < 10; i++)
	{
		printf("Redo %d\n", i);
		table = redo(&table, &redo_st, &undo_st);
		inorder(table);
		printf("\n");
	}

	return 0;
}
