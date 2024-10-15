#include <stdio.h>
#include<unistd.h>

int main()
{
	printf("Hello, world! my pid is %d.\n", getpid());
	sleep(10);
	return 0;
}
