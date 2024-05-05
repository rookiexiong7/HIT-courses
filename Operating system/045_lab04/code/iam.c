#define __LIBRARY__
#include <unistd.h>
_syscall1(int,iam,const char*,name); 

int main(int argc, char ** argv)
{
	int err = 0;
	if(argc < 1){
		printf("not enough arguments!\n");
		return -1;
	}
	err = iam(argv[1]);
	return err;
}

