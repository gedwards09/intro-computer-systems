#include <stdio.h>

int main()
{
	int input;
	printf("Enter an integer between 1 to 100 (1 and 100 excluded):\n");
	scanf("%d", &input);
	if (input <= 1 || input >= 100)
	{
		printf("not in range\n");
	}
	else
	{
		printf("0x%x\n", input);
		printf("0%o\n", input);
	
		int i = 2;
		int done = 0;
		while (i*i < input && !done)
		{
			if (input % i == 0)
			{
				done = 1;
			}
			else
			{
				i++;
			}
		}

		if (!done)
		{
			printf("prime\n");
		}
		else
		{
			printf("composite\n");
		}
	}
	return 0;
}

