#include <stdio.h>
#include <stdlib.h>
double Fibonacci(long x);
int main()
{
    double g;
    long n = 100;
    g = Fibonacci(n)/Fibonacci(n+1);
    printf("When n = 100, the g equals to : %.8f",g);
    return 0;
}

double Fibonacci(long x)
{
    if(x == 0) return 0;
    else if(x == 1) return 1;
    else return Fibonacci(x - 1) + Fibonacci(x - 2);
}
