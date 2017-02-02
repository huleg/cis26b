/***
CIS 26B: Homework 2B
********************
NAME: Dat Nguyen
IDE (compiler): Visual Studio 2015
*************************************************************************


*/
#define _CRT_SECURE_NO_WARNINGS			// to avoid using fopen_s and strcpy_s on Visual Studio

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>  
#include <string.h>
#include <errno.h>

#define MAX_LEN        5
#define INPUT_PROMPT   "Enter your input file (or press Enter for default choice):"
#define INPUT_DEFAULT  "./stocks.txt"
#define FORMAT_DESC    "%-7s %5d\n"
#define FLUSH          while(getchar() != '\n')
#define SENTINEL_NODE  "\177"

#ifdef _MSC_VER
#include <crtdbg.h>  // needed to check for memory leaks (Windows only!)
#endif

typedef struct list_node  NODE;
typedef struct stack_node STACK_NODE;

//==========================OBJECT DEFINITIONS============================//
typedef struct {
	char   name[MAX_LEN];
	double quote;
}STOCK;

struct stack_node{
	double		quotes;
	STACK_NODE* next;
};

struct list_node{
	char		name[MAX_LEN];
	int			stack_size;
	STACK_NODE* stack;
	NODE*		next;
	NODE*		prev;
};

typedef struct {
	NODE*  list;
	int    size;
}HEAD;


typedef enum{ Descending, Ascending }Direction;

int main(void){
//==========================FUNCTION PROTOTYPES==========================//
	NODE  *read_file(char  *filename, int *size, NODE **stack);
	char  *get_input(char *message);
	void   display(NODE *list, Direction);
	void   print_manager(NODE *list);
	void   search_manager(NODE *list);
	void   _print(NODE *first, NODE *last, Direction);
//-------------STACK OPERATIONS-----------------//
	STACK_NODE *push(STACK_NODE *stack, double quote);
	STACK_NODE *pop(STACK_NODE **stack);
	void		destroy_stack(STACK_NODE * stack);
//------------LINKED-LIST OPERATIONS -----------//
	NODE *get_node(NODE **stack); 
	NODE *init_list(NODE **stack);
	NODE *locate_node(NODE *list, char *name);
	int   insert(STOCK data, NODE *list, NODE **stack);
	int   delete(NODE *list, NODE **stack);
	void  destroy_list(NODE *list);
//========================== MAIN FUNCTIONS =============================//
	NODE* stack = NULL; // keep track of deleted nodes - for practice
	HEAD  head;
	char  *input = get_input(INPUT_PROMPT);
	head.list = read_file(input, &head.size, &stack);

	display(head.list, Ascending);	// Display in Ascending Order
	display(head.list, Descending); // Display in Desceding Order
	print_manager(head.list);		//Print sub-list
	search_manager(head.list);		//Search manager
	// Free memory
//	destroy_list(head.list);

#ifdef _MSC_VER
	printf(_CrtDumpMemoryLeaks() ? "Memory Leak\n" : "\nNo Memory Leak\n");
#endif

	int wait = scanf("Exiting program: ", &wait);
	return 0;
}
/*================================ FUNCTION DECLARATIONS ======================//

*/
NODE *read_file(char  *filename, int *list_size, NODE **stack){
	FILE  *fp = NULL;
	NODE  *list = NULL;
	//Open a file
	if (strcmp(filename, "") == 0)
		strcpy(filename, INPUT_DEFAULT);  // I assume the maximum buffer is 100
	fp = fopen(filename, "r");

	if (!fp) {
		printf("File %s not found\n", filename); 
		exit(101);
	}

	int count = 0;  								// store the loop counts = number of lines in file
	rewind(fp); 									// Make sure we are at first pos

	// Initialize a list by allocating a sentinel node
	list = init_list(stack);

	// Read file
	int	   size = 1;			//included the sentinel node
	char     stock_name[MAX_LEN];
	double   stock_shares;
	while ((fscanf(fp, "%s  %lf", stock_name, &stock_shares)) != EOF) {   // Stop at the end of file
			STOCK stock = { .quote = stock_shares };
			strcpy(stock.name, stock_name);
			if(insert(stock, list, stack) == 0)
				size++;
	}
	*list_size = size;

	// Close the file
	if (fclose(fp) == EOF) {
		printf("Error closing file %s", filename); exit(201);
	}

	return list;
}
void   _print(NODE *first, NODE *last, Direction direction){
	switch (direction){
		case Descending:{			//Using p->next			
				NODE *tmp = first;
				while (tmp != last){
					if (strcmp(tmp->name, SENTINEL_NODE) != 0)
						printf("%-*s ",MAX_LEN,tmp->name); // using '*' flexibility in format description
					tmp = tmp->next;
				}
				printf("%-*s ", MAX_LEN, tmp->name);
			}
			break;
		case Ascending:{			//Using p->prev			
				NODE *tmp = first;
				while (tmp != last){
					if (strcmp(tmp->name, SENTINEL_NODE)!= 0)
						printf("%-*s ", MAX_LEN, tmp->name);
					tmp = tmp->prev;
				}
				printf("%-*s ", MAX_LEN, tmp->name);
			}
			break;
		default:
			printf("\nWrong Direction EnumType\n");
			break;
	}
	printf("\n");
}
NODE *locate_node(NODE *list, char *target){
	NODE *pWalk = list->next;
	while (strcmp(pWalk->name,SENTINEL_NODE) != 0){
		if (strcmp(pWalk->name, target) == 0)
			return pWalk;
		pWalk = pWalk->next;
	}
	return NULL;
}
void   print_manager(NODE *list){
	char first_stock[MAX_LEN];
	char second_stock[MAX_LEN];
	char print_type[MAX_LEN];
	char condition[MAX_LEN];

	printf("\n\n======================Display Sub-list============================");
	do{
		printf("\n\n");
		// Get user input
		printf("Enter first stock: ");				gets(first_stock);
		printf("Enter second stock: ");				gets(second_stock);
		printf("Descending - 0 or Ascending - 1:"); gets(print_type);

		Direction direction = *print_type - 48; // -48 in order to convert char to numeric value (ASCII table)
		if ((direction != Descending) && (direction != Ascending)){
			printf("\nPlease only enter 0 or 1 for print direction.\n");
		}
		else {
			printf("\nResult\n\n%s to %s: ", first_stock, second_stock);
			//Return null if not found
			NODE *start_node = locate_node(list, first_stock);
			NODE *end_node = locate_node(list, second_stock);

			// Display sublist
			if (start_node && end_node)
				_print(start_node, end_node, direction);
			else {		// One of two stock inputs is not found or both 
				if (start_node == NULL)
					printf("%s ", first_stock);
				if (end_node == NULL)
					printf("%s", second_stock);
				printf(" - Not Found\n");
			}
		}
	} while (printf("\nWould you like to print again? (Y)"), gets(condition),
			 ((toupper(condition[0]) == 'Y') && strlen(condition) == 1));

}
void search_manager(NODE *list){
	char stock[MAX_LEN];
	char condition[MAX_LEN];
	printf("\n\n====================== Search Manager ============================");
	do{
		printf("\n\n");
		// Get user input
		printf("Enter stock name (or quit): "); gets(stock);
		if (strcmp(stock, "quit") == 0) break;
		//Return null if not found
		NODE *node = locate_node(list,stock);
		// Display stock
		if (node){
			int size = 0;
				printf("Enter amount of quotes to display: ");
				scanf("%d", &size);
				while (size > node->stack_size) {
					if (size > node->stack_size) {
						printf("Please enter smaller amount (max is %d): ", node->stack_size);
						scanf("%d", &size);
					}
				}
				printf("Stock %s : ", node->name);
				STACK_NODE *tmp = node->stack;
				for(int i = 0; i < size; i++) {
					printf("$%-5.2f   ", tmp->quotes);
					tmp = tmp->next;
				}
				printf("\n");
		}
		else{		
			printf("%s - not found ",stock);
		}
		FLUSH;
	} while (printf("\nWould you like to search again? (Y)"), gets(condition),
		((toupper(condition[0]) == 'Y') && strlen(condition) == 1));
}
void  display(NODE *list, Direction direction) {
	NODE *tmp = NULL;
	switch (direction){
		case Ascending:{
			printf("\nAscending order\n");
			tmp = list->next;
			while (strcmp(tmp->name, SENTINEL_NODE)) {
				printf("%-5s : %-7.3f\n", tmp->name, tmp->stack->quotes);
				tmp = tmp->next;
				}
			}
			break;
		case Descending:{
				printf("\nDescending order\n");
				tmp = list->prev;
				while (strcmp(tmp->name, SENTINEL_NODE)) {
					printf("%-5s : %-7.2f\n", tmp->name, tmp->stack->quotes);
					tmp = tmp->prev;
				}
			}
			break;
	}
}
char *get_input(char *message){
	char *fname = (char*)malloc(MAX_LEN);
	printf(message);
	fgets(fname, MAX_LEN, stdin);
	fname[strcspn(fname, "\n")] = 0;
	return fname;
}

//-------------STACK OPERATIONS-----------------//
STACK_NODE *push(STACK_NODE *stack, double data){
	STACK_NODE *pnew;

	pnew = (STACK_NODE *)malloc(sizeof (STACK_NODE));
	if (!pnew){
		printf("... error in push!\n");
		exit(1);
	}
	pnew->quotes = data;
	pnew->next = stack;
	stack = pnew;

	return stack;
}
STACK_NODE *pop(STACK_NODE **stack){
	// NOT IMPLEMENTED YET
	return NULL;

}
//------------LINKED-LIST OPERATIONS -----------//
/************************************************
Gets a free node from the free stacl (if not
empty) or from a malloc'ed block of structs
*/
NODE *get_node(NODE **stack){
	#define BLOCKSIZE 5
	static NODE *block = NULL, *blockrear;
	NODE *first;

	if (*stack != NULL){			  // pop node off the free stack
		first = *stack;
		*stack = (*stack)->next;
		first->next = NULL;			 // detach from stack
	}
	else if (block != NULL){		 // get node from block
		first = block;
		if (block == blockrear) block = NULL;
		else block++;   // move to the next struct
	}
	else{  // stack and block are NULL!
		if ((block = (NODE *)malloc(BLOCKSIZE * sizeof(NODE))) == NULL){
			printf("Fatal malloc error in get_node!\n");
			exit(1);
		}
		memset(block, 0, BLOCKSIZE * sizeof(NODE));
		blockrear = block + BLOCKSIZE - 1;
		first = block;
		block++;
	}
	return first;

}
NODE *init_list(NODE **stack){
	NODE *list = NULL;
	// Allocate sentinal node
	list = get_node(stack);
	//Set setinal node
	strcpy(list->name,SENTINEL_NODE);
	list->next = list;
	list->prev = list;
	list->stack_size = -1;
	list->stack = NULL;
	return list;


}
int  insert(STOCK new_stock, NODE *list, NODE **stack){
	NODE *curr = list->next;
	NODE *prev = list;
	NODE *pnew;
	int   duplicate = 1;

	// search
	while (strcmp(new_stock.name, curr->name) > 0){
		prev = curr;
		curr = curr->next;
	}
	if (strcmp(new_stock.name, curr->name)){
		duplicate = 0;
		pnew = get_node(stack);
		// Alloclate name for pnew
		strcpy(pnew->name, new_stock.name);
		pnew->stack = push(pnew->stack, new_stock.quote);
		(pnew->stack_size)++;
		// set prev and next
		pnew->next = curr;
		pnew->prev = curr->prev;
		pnew->next->prev = pnew;
		pnew->prev->next = pnew;
	}
	else if (strcmp(new_stock.name, curr->name) == 0){	// duplicate 
		curr->stack = push(curr->stack, new_stock.quote);
		(curr->stack_size)++;
	}
	return duplicate;

}
int  delete(NODE *list, NODE **stack){
	// NOT IMPLEMENTED YET
	return 0;

}
void  destroy_list(NODE *list) {
	NODE *pWalk;
	NODE *pNext;

	// REMEMBER destroy stack
	pWalk = list->next;
	while (pWalk != list) {
		// Save a pointer to the next node.
		pNext = pWalk->next;
		// Delete the current node.
		free(pWalk);
		// Position nodePtr at the next node.
		pWalk = pNext;
	}
	free(list);
}