#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>


int main(int argv, char* argc[])
{
	if (argv != 3)
	{
		printf("Usage:./sendsig [option] <pid>\n");
		exit(1);
	}

	int signal;
	pid_t pid;
	if (strcmp(argc[1], "-u") == 0)
	{
		signal = SIGUSR1;
	}
	else if (strcmp(argc[1], "-i") == 0)
	{
		signal = SIGINT;
	}
	else
	{
		printf("Error: option %s not recognized\n", argc[1]);
		exit(1);
	}

	pid = (pid_t)atoi(argc[2]);
	if (pid == 0)
	{
		printf("Error:sendsig.c: Error parsing PID\n");
	}

	kill(pid, signal);
	return 0;
}
