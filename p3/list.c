#include <stdio.h>
#include <stdlib.h>

#define MaxInts 1000
#define MaxLine 1000

/* a node in the linked list */
struct node
{
	int data;
	struct node* next;
};

struct node* CreateList(int intArray[], int len);
struct node* AddItemAtStart(struct node* head, int data);
int SearchArray(int integers[], int numInts, int element);
int SearchList(struct node* head, int element);
struct node* CreateSortedList(struct node* head);
struct node* AddItemSorted(struct node* head, int data);
int CopyListToArray(struct node* head, int* array);
void PrintList(struct node* head);
void PrintArray(int integers[], int len);

int main(int argc, char* argv[])
{
	struct node* head;
	struct node* sortedHead;
	int nums[MaxInts];
	int length;
	int i = 0;
	int searchInt;
	char input[MaxLine];
	char* line = input;
	int index;
	struct node* current;
	char* filename;
	FILE* fp;
	int sortedArray[MaxInts];
	int* pn = sortedArray;

	if (argc != 2)
	{
		printf("Usage: ./list <input_file>\n");
		return 1;
	}

	filename = argv[1];

	//read integers from file
	fp = fopen(filename,"r");
	if (fp == NULL)
	{
		printf("ERROR: cannot open file for reading!");
		return 1;
	}
	printf("Reading integers from a file to an array...\n\n");
	while (i < MaxInts && fgets(line, MaxLine, fp))
	{
		nums[i++] = atoi(line);	
	}
	length = i;
	
	printf("ARRAY: ");
	PrintArray(nums, length);
	printf("\n");

	head = CreateList(nums, length);

	printf("LINKED LIST: ");
	PrintList(head);
	printf("\n");

	while(1)
	{
		printf("Enter an element to be search in the list and array: ");
		fgets(line, MaxInts, stdin);
		printf("\n");

		if (input[0] == 'q' && input[1] == '\n' &&  input[2] == '\0')
		{
			break;
		}

		searchInt = atoi(input);

		index = SearchArray(nums, length, searchInt);
		if (index == -1)
		{
			printf("Element %d not found in the array\n\n", searchInt);
		}
		else
		{
			printf("Element %d found in the array at index %d\n\n", 
				searchInt, index);
		}

		index = SearchList(head, searchInt);
		if (index == -1)
		{
			printf("Element %d not found in the linked list\n\n", searchInt);
		}
		else
		{
			printf("Element %d found in the linked list at index %d\n\n", 
				searchInt, index);
		}
	}

	sortedHead = CreateSortedList(head);

	printf("SORTED LIST: ");
	PrintList(sortedHead);
	printf("\n");

	CopyListToArray(sortedHead, pn);

	printf("SORTED ARRAY: ");
	PrintArray(sortedArray, length);
	printf("\n");

	printf("ORIGINAL LIST: ");
	PrintList(head);
	printf("\n");

	printf("ORIGINAL ARRAY: ");
	PrintArray(nums, length);
	printf("\n");

	//write sorted array to file

	fp = fopen("sorted_numbers.txt","w");
	if (fp == NULL)
	{
		printf("ERROR: Cannot open file for writing\n");
	}

	for (i=0; i < length; i++)
	{
		fprintf(fp, "%d\n", sortedArray[i]);
	}

	fclose(fp);

	//cleanup
	current = head;
	while (current != NULL)
	{
		head = head->next;
		free(current);
		current = head;
	}
	current = sortedHead;
	while (current != NULL)
	{
		sortedHead = sortedHead->next;
		free(current);
		current = sortedHead;
	}
	
	return 0;
}

struct node* CreateList(int intArray[], int len)
{
	struct node* head = NULL;
	
	for (int i=0; i<len; i++)
	{
		head = AddItemAtStart(head, intArray[i]); 
	}

	return head;
}

struct node* AddItemAtStart(struct node* head, int data)
{
	struct node* pNode = (struct node*) malloc(sizeof(struct node));
	if (pNode == NULL)
	{
		return head;
	}

	pNode->data = data;

	if (head == NULL)
	{
		head = pNode;
		pNode->next = NULL;
	}
	else
	{
		pNode->next = head;
		head = pNode;
	}

	return head;
}

int SearchArray(int integers[], int numInts, int element)
{
	int i=0;
	while(i < numInts && integers[i] != element)
	{
		i++;
	}

	if (i == numInts)
	{
		return -1;
	}
	else
	{
		return i;
	}
}


int SearchList(struct node* head, int element)
{
	int index = 1;

	struct node* current = head;
	while (current != NULL && current->data !=element)
	{
		current = current->next;
		index++;
	}
	
	if (current == NULL)
	{
		return -1;
	}
	else
	{
		return index;
	}
}

struct node* CreateSortedList(struct node* head)
{
	struct node* sortedHead = NULL;
	struct node* current = head;
	while (current != NULL)
	{
		sortedHead = AddItemSorted(sortedHead, current->data);
		current = current->next;
	}

	return sortedHead;
}

struct node* AddItemSorted(struct node* head, int data)
{
	struct node* pNode = malloc(sizeof(struct node));
	if (pNode == NULL)
	{
		return head;
	}
	pNode->data = data;
	
	if (head == NULL)
	{
		head = pNode;
		pNode->next = NULL;
	}
	else if (data < head->data)
	{
		pNode->next = head;
		head = pNode;
	}
	else
	{
		struct node* current = head;
		while (current->next != NULL && current->next->data < data)
		{
			current = current->next;
		}
		
		pNode->next = current->next;
		current->next = pNode;
	}

	return head;
}

int CopyListToArray(struct node* head, int* array)
{
	struct node* current = head;
	int i = 0;
	while (current != NULL)
	{
		array[i++] = current->data;
		current = current->next;
	}

	return i;
}

void PrintList(struct node* head)
{
	if (head == NULL)
	{
		printf("Linked List is Empty.\n");
	}
	else
	{
		struct node* temp = head;
		printf("head->");
		while (temp != NULL)
		{
			printf("|%d|->", temp->data);
			temp = temp->next;
		}
		printf("NULL\n");
	}
}

void PrintArray(int integers[], int len)
{
	int i;
	for (i = 0; i < len; i++)
	{
		printf("| %d ",integers[i]);
	}
	printf("|\n");
}
