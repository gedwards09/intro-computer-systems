
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MaxInput 80

int DivisionCount = 0;

void printDivisions()
{
	printf("Number of sucessful divisions: %d\n", DivisionCount);
}

void sigfpe_handler(int sig)
{
	printf("A divide by zero error occured.\n");
	printDivisions();
	exit(0);
}

void sigint_handler(int sig)
{
	printDivisions();
	exit(0);
}

int main()
{
	char input[MaxInput+1];
	int n1, n2, quot, rem;

	/* install SIGFPE handler */
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = sigfpe_handler;
	if (sigaction(SIGFPE, &act, NULL) == -1)
	{
		fprintf(stderr, "Error:division.c:Error while binding SIGFPE handler"
			"\n");
		exit(1);
	}

	act.sa_handler = sigint_handler;
	if (sigaction(SIGINT, &act, NULL) == -1)
	{
		fprintf(stderr, "Error:division.c:Error while binding SIGINT handler"
			"\n");
		exit(1);
	}
	while (1)
	{
		printf("Enter first integer: ");
		fgets(input, MaxInput, stdin);
		n1 = atoi(input);
		printf("Enter second integer: ");
		fgets(input, MaxInput, stdin);
		n2 = atoi(input);
		quot = n1 / n2;
		DivisionCount++;
		rem = n1 % n2;
		printf("%d / %d is %d with remainder %d\n", n1, n2, quot, rem);
	}
}
