#include<iostream>
#include <cstdlib>
#include <ctime>
#include <fstream> 
#include <time.h>
using namespace std;
void swap(long *a, long *b);
int Selection_Sort(long x[],long i, long j,long n,long min);   //选择递归排序算法


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
    long min = i;
    start = clock();
    Selection_Sort(x,i,j,n,min);
    finish = clock();

    Times=(double)(finish-start)/CLOCKS_PER_SEC;
    cout<<"选择递归排序的运行时间(秒)(CLOCKS_PER_SEC): "<<Times<<endl;
    for(int i =0;i<100;i++)
        cout<<x[i]<<endl;
    return 0;
}


int Selection_Sort(long x[],long i, long j,long n,long min)
{

    if(i > n - 1) 
        return 0;
    if(j > n - 1)
    {
        swap(&x[i], &x[min]);
        i++;
        min = i;
        j = i + 1;
    }
    if(x[j] < x[min])
        min = j;

    Selection_Sort(x,i,j+1,n,min);
}

void swap(long *a, long *b)
{
    long temp = *a;
    *a = *b;
    *b = temp;
}