#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int secondsToAlarm = 3;
int interuptsToCatch = 5;
int sigusr1Count = 0;

/* SIGALRM handler */
void sigalrm_handler(int sig)
{
	time_t t = time(NULL);
	printf("PID: %u. Systime: %s", getpid(), ctime(&t));
	alarm(secondsToAlarm);
}

/* SIGINT handler */
void sigint_handler(int sig)
{
	interuptsToCatch--;
	if (interuptsToCatch == 0)
	{
		printf("\nFinal Ctrl+C caught. SIGUSR1 was handled %d times. "
			"Exiting.\n", sigusr1Count);
		exit(0);
	}
	printf("\nCtrl+C caught. %d more time to program exit\n", interuptsToCatch);
}

/* SIGUSR1 handler */
void sigusr1_handler(int sig)
{
	sigusr1Count++;
	printf("SIGUSR1 handled!\n");
}

int main()
{
	/* install SIGALRM handler */
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = sigalrm_handler;
	if (sigaction(SIGALRM, &act, NULL) == -1)
	{
		fprintf(stderr, "Error:intdate.c:Error while binding SIGALRM handler"
			"\n");
		exit(1);
	}

	/* install SIGINT handler */
	act.sa_handler = sigint_handler;
	if (sigaction(SIGINT, &act, NULL) == -1)
	{
		fprintf(stderr, "Error:intdate.c:Error while binding SIGINT handler\n");
		exit(1);
	}

	/* install SIGUSR1 handler */
	act.sa_handler = sigusr1_handler;
	if (sigaction(SIGUSR1, &act, NULL) == -1)
	{
		fprintf(stderr, "Error:intdate.c:Error while binding SIGUSR1 handler\n");
		exit(1);
	}

	alarm(secondsToAlarm);
	printf("Date will be printed every %d seconds.\n", secondsToAlarm);

	while (1) {  }

	return 0;
}



