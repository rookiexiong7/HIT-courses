#include<iostream>
using namespace std;

//汉诺塔问题的非递归求解主要借助堆栈的思想
void move(int n,char A,char B, char C);
void display(char a,char b);
#define MAX_SIZE 20

//定义顺序栈
typedef struct SeqStack
{
    char a,b,c;
    int n;
}SeqStack;

int step_number = 1;

int top = -1;//栈顶元素的下标  -1 代表是一个空栈
SeqStack step[MAX_SIZE];

void push(int n,char A,char B, char C)
{
    top++;
    step[top].n = n;
    step[top].a = A;
    step[top].b = B;
    step[top].c = C;
}

int main()
{
    int n;
    char a='A',b='B',c='C';
    cout<<"The number of hanoi is: "<<endl;
    cin>>n;
    if(n > MAX_SIZE) 
        return -1;
    cout<<"The solution method is: "<<endl;
    push(n,a,b,c);
    while(top >= 0)
    {
        SeqStack STEP= step[top];
        --top;
        if(STEP.n == 1)
            display(STEP.a,STEP.c);
        else
        {
            push(STEP.n-1,STEP.b,STEP.a,STEP.c);
            push(1       ,STEP.a,STEP.b,STEP.c);
            push(STEP.n-1,STEP.a,STEP.c,STEP.b);
        }

    }
    return 0;
}

void display(char a,char b) //可视化操作
{
    cout<<"No."<<step_number<<" step: move from "<<a<<" to "<<b<<endl;
    step_number++;
}