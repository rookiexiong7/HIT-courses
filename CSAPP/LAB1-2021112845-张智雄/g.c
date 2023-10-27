#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
void Add(char x[],char y[],char z[]);
double value(char x[]);
int main()
{
    int n = 100;
    double g;
    char x[30]="00000000000000000000000000001",y[30]="00000000000000000000000000001",z[30]="00000000000000000000000000000";
    for(int i = 2; i < n + 1; i++)
    {
        Add(x,y,z);
        strcpy(x,y);
        strcpy(y,z);

    }
    g = value(x)/value(y);
	printf("When n = 100, the g equals to : %.8f\n",g);
    return 0;
}

void Add(char x[],char y[],char z[])
{
  int add[29] = {0};
  for(int i = 28; i >= 0 ; i--)
  {
    if(x[i] + y[i] + add[i] - '0'>='0' && x[i] + y[i] + add[i] - '0'<='9')
      z[i] = x[i] + y[i] + add[i] - '0';
    else
	{
      z[i] = (x[i] + y[i] + add[i] - '0' - '0')%10 + '0';
      add[i-1] = (x[i] + y[i] + add[i] - '0' - '0')/10;
    }
  }
}

double value(char x[])
{
	double value = 0;
	for(int i = 28; i >= 0; i--)
	{
		value += (x[i]-'0')*pow(10,29-i);
	}
	return value;
}
