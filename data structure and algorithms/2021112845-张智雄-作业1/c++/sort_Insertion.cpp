#include<iostream>
#include <cstdlib>
#include <ctime>
#include <fstream> 
#include <time.h>
using namespace std;
void swap(long *a, long *b);
void Insertion_Sort(long x[]);   //插入排序算法

long n=100000;
int main()
{
    long x[n];
    clock_t start,finish;
    double Times, Times1;
    //读取数据
    ifstream infile;
    infile.open("data.txt");
    for(int i = 0;i < n;i++)
        infile>>x[i];
    infile.close();

    start = clock();
    Insertion_Sort(x);
    finish = clock();

    Times=(double)(finish-start)/CLOCKS_PER_SEC;
    cout<<"插入排序的运行时间(秒)(CLOCKS_PER_SEC): "<<Times<<endl;
     return 0;
}



void Insertion_Sort(long x[])
{
    for(long i = 1; i < n;i++)
        {
            long key=x[i];
            long j=i-1;
            while((j>=0) && (key<x[j]))
            {
                x[j+1]=x[j];
                j--;
            }
            x[j+1]=key;
        }

}
void swap(long *a, long *b)
{
    long temp = *a;
    *a = *b;
    *b = temp;
}