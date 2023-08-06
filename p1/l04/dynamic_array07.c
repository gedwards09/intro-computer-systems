#include <stdio.h>
#include <stdlib.h>

#define Rows 3
#define Cols 3

int** Transpose(int a[][Cols]);

int main()
{
	int a[Rows][Cols] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

	for (int i = 0; i < Rows; ++i)
	{
		for (int j = 0; j < Cols; ++j)
		{
			printf("a[%d][%d] = %d\t", i, j, a[i][j]);
		}
		printf("\n");
	}

	int** t = Transpose(a);
	
	for (int i = 0; i < Rows; ++i)
	{
		for (int j = 0; j < Cols; ++j)
		{
			printf("t[%d][%d] = %d\t", i, j, t[i][j]);
		}
		printf("\n");
	}

return 0;
}

int** Transpose(int a[][Cols])
{
	int** t = malloc(Cols * sizeof(int*));

	for (int i = 0; i < Cols; i++)
	{
		*(t+i) = malloc(Rows * sizeof(int)); //t[i]
		for (int j = 0; j < Rows; j++)
		{
			*(t[i]+j) = a[j][i]; // t[i][j]
		}
	}	

	return t;
}
