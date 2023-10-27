#include<iostream>
#include <cstdlib>
#include <ctime>
#include <fstream> 
#include <time.h>
using namespace std;
void swap(long *a, long *b);
int Bubble_Sort(long x[],long i, long j,long n);   //冒泡递归排序算法


int main()
{
    long n = 100;
    long x[n];
    clock_t start,finish;
    double Times, Times1;
    //读取数据
    ifstream infile;
    infile.open("data_recursion.txt");
    for(int i = 0;i < n;i++)
        infile>>x[i];
    infile.close();
    long i=0,j=0;
    start = clock();
    Bubble_Sort(x,i,j,n);
    finish = clock();

    Times=(double)(finish-start)/CLOCKS_PER_SEC;
    cout<<"冒泡递归排序的运行时间(秒)(CLOCKS_PER_SEC): "<<Times<<endl;
    for(int i =0;i<100;i++)
        cout<<x[i]<<endl;
    return 0;
}


int Bubble_Sort(long x[],long i, long j,long n)
{
    
    if(i >= n - 1) 
        return 0;
    if(j >= n - i)
    {
        i++;
        j = 0;
    }
    if(x[j] >= x[j+1]) 
        swap(&x[j], &x[j+1]);

    Bubble_Sort(x,i,j+1,n);
}

void swap(long *a, long *b)
{
    long temp = *a;
    *a = *b;
    *b = temp;
}