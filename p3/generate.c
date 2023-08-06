#include <stdio.h>
#include <stdlib.h>

#define Seed 42

/* A program to generate random numbers */
int main( int argc, char* argv[])
{
	int i, randNum, maxNum, totalNum;

	if (argc != 3)
	{
		printf("USAGE: %s <total_nums> <max_nums>\n", argv[0]);
		exit(1);
	}

	totalNum = atoi(argv[1]);
	maxNum = atoi(argv[2]);
	srand(Seed);
	for (i = 0; i < totalNum; i++)
	{
		randNum = rand() % maxNum;
		printf("%d\n", randNum);
	}
	return 0;
}
