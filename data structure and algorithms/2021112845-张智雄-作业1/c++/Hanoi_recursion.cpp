#include<iostream>
using namespace std;
void move(int n,char A,char B, char C);
void display(int n,char a,char b);

int main()
{
    int n;
    char a='A',b='B',c='C';
    cout<<"The number of hanoi is: "<<endl;
    cin>>n;
    cout<<"The solution method is: "<<endl;
    move(n,a,b,c);
}

void move(int n,char A,char B, char C) //递归函数，第n个盘借由B从A移动到C
{
    if(n == 1)
        display(n,A,C);
    else
    {
        move(n-1,A,C,B);
        display(n,A,C);
        move(n-1,B,A,C);
    }
}
void display(int n,char a,char b) //可视化操作
{
    cout<<"No."<<n<<" move from "<<a<<" to "<<b<<endl;
}