#include<iostream>
#include<stdio.h>
#define N 150
using namespace std;

int step=1;//用于记录 step 
int jiaogu(int n) 
{
 	if(n==1)
	{
	printf("%d ",n);
 	return step;
	}
 	else if(n % 2==0) { 
 		printf("%d->",n);
  		jiaogu(n/2); 
  		step++; 
  }
   else { 
   		printf("%d->",n); 
  		jiaogu(3*n+1); 
   		step++; 
   }
} 
int main(void) { 
	int n,step; 
	cout<<"需要验证的数字为:"; 
	cin >> n;
	step=jiaogu(n); 
	printf("\n"); 
	printf("序列长度为%d \n",step); 
	return 0; 
}
