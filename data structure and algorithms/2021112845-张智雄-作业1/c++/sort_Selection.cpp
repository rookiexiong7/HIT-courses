#include<iostream>
#include <cstdlib>
#include <ctime>
#include <fstream> 
#include <time.h>
using namespace std;
void swap(long *a, long *b);
void Selection_Sort(long x[]);   //选择排序算法

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
    Selection_Sort(x);
    finish = clock();

    Times=(double)(finish-start)/CLOCKS_PER_SEC;
    cout<<"选择排序的运行时间(秒)(CLOCKS_PER_SEC): "<<Times<<endl;

    return 0;
}



void Selection_Sort(long x[])
{
    for (long i = 0; i < n; i++)
    {
        long min = i;
        for (long j = i + 1 ; j < n; j++)
            if (x[j] < x[min])
                min = j;
        swap(&x[i], &x[min]);
    }



}
void swap(long *a, long *b)
{
    long temp = *a;
    *a = *b;
    *b = temp;
}