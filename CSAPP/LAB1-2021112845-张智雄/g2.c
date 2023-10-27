#include <stdio.h>
#include <stdlib.h>

int main()
{
    double g;
    long n = 101;
    double f[n+1];
    f[0] = 0; f[1] = 1;
    for(long i = 2; i < n + 1; i++)
    {
        f[i] = f[i - 1] + f[i - 2];
    }
    g = f[n-1]/f[n];
    printf("When n = 100, the g equals to : %.8f\n",g);
    return 0;
}
