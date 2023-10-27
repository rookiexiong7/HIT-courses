#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

int main()
{
    ofstream fout("data.txt");
    int x[1024],j = 0;
    if(fout){
        for(int i = 1; i < 2048; i = i+2){
            fout<<i<<endl;
            x[j++] = i;
        }    
    }
    fout.close();

    random_shuffle(x, x+1024);
    ofstream infile("redata.txt");
    if(infile){
        for(int j = 0; j < 1024; j++)
            infile<<x[j]<<endl;
    }
    infile.close();
}