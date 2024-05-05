#define __LIBRARY__                             
#include <unistd.h>                              
_syscall2(int, whoami, char*, name, unsigned int, size);  

int main(){
	char result[24];
	int len = 0;
	len = whoami(result, 24);
	printf("%s\n",result);
	return len;
}

  
