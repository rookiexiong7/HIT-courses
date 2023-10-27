#include<iostream>
#include <cstdlib>
#include <ctime>
#include <fstream> 
#include <time.h>
using namespace std;
int main()
{
    long x[100000];
    clock_t start,finish;
    double Times, Times1;
    srand((long)time(NULL));
    ofstream outfile;
    outfile.open("data.txt",ios::app);
    //生成在0-999999之间的100000个随机数
    for (long i = 0; i < 100000; i++)
    {
        x[i] = ((double)rand() / 32767) * 1000000;
        outfile<<x[i]<<"\n";
    }
	outfile.close();		
	return 0;	

}
