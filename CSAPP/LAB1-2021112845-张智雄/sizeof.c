#include <stdio.h>

int main(void)
{
    printf("The size of char %d\n", sizeof(char));
    printf("The size of short %d\n", sizeof(short));
    printf("The size of int %d\n", sizeof(int));
    printf("The size of long %d\n", sizeof(long));
    printf("The size of long long %d\n", sizeof(long long));
    printf("The size of float %d\n", sizeof(float));
    printf("The size of double %d\n", sizeof(double));
    printf("The size of long double %d\n", sizeof(long double));
    printf("The size of pointer %d\n", sizeof(int*));
    return 0;
}
