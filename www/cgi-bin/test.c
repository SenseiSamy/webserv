#include <cstdio>
#include <stdio.h>

int	main(int ac, char **av, char **env)
{
	printf("Content-Type: text/plain\n\n");
	for (int i = 0; env[i]; ++i)
		printf("%s\n", env[i]);

	void *p = malloc(1024);
	return (0);
}