#include<iostream>
#include <cstdlib>
#include <ctime>
#include <fstream> 
#include <time.h>
using namespace std;
void swap(long *a, long *b);
void Bubble_Sort(long x[]);   //冒泡排序算法

long n = 100000;
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
    //计时器
    start = clock();
    Bubble_Sort(x);
    finish = clock();

    Times=(double)(finish-start)/CLOCKS_PER_SEC;
    cout<<"冒泡排序的运行时间(秒)(CLOCKS_PER_SEC): "<<Times<<endl;
    
    //文件写入
    ofstream outfile;
	outfile.open("result.txt", ios::out);
    for(long j=0;j<n;j++)
        outfile<<x[j]<<endl;
	outfile.close();

    return 0;
}


void Bubble_Sort(long x[])
{
    for (long i = 0; i < n; i++)
        for (long j = 0 ; j < n- i ; j++)
            if (x[j] >= x[j+1])
                swap(&x[j],&x[j+1]);
}

void swap(long *a, long *b)
{
    long temp = *a;
    *a = *b;
    *b = temp;
}