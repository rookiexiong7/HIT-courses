#include<iostream>
#include <cstdlib>
#include <ctime>
#include <fstream> 
#include <time.h>
using namespace std;
void swap(long *a, long *b);
int Insertion_Sort(long x[],long i,long n);   //插入递归排序算法


int main()
{
    long n = 100;
    long x[n];
    clock_t start,finish;
    double Times, Times1;
    //读取数据
    ifstream infile;
    infile.open("data_recusion.txt");
    for(int i = 0;i < n;i++)
        infile>>x[i];
    infile.close();
    long i = 1;
    start = clock();
    Insertion_Sort(x,i,n);
    finish = clock();

    Times=(double)(finish-start)/CLOCKS_PER_SEC;
    cout<<"选择递归排序的运行时间(秒)(CLOCKS_PER_SEC): "<<Times<<endl;
    for(int i =0;i<100;i++)
        cout<<x[i]<<endl;
    return 0;
}


int Insertion_Sort(long x[],long i, long n)
{

    if(i > n - 1) 
        return 0;
    long key=x[i];
    long j=i-1;
    while((j>=0) && (key<x[j]))
    {
        x[j+1]=x[j];
        j--;
    }
    x[j+1]=key;
    Insertion_Sort(x,i+1,n);

}

void swap(long *a, long *b)
{
    long temp = *a;
    *a = *b;
    *b = temp;
}