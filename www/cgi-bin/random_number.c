#include <stdlib.h>
#include <time.h>
#include <stdio.h>

int main(void)
{
	int random_n;

	srand(time(NULL));
	random_n = rand() % 1001;

	printf("Content-Type: text/plain\r\n\r\n");
	printf("Here is a random number: %d\n", random_n);
	return (0);
}