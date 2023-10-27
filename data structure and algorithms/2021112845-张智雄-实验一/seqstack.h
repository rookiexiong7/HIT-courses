#include<iostream>
#define SIZE 50
using namespace std;

template<class T>class Stack
{
    private:
            T data[SIZE]; //顺序储存数据
            int Top; //栈顶下标
    public:
        void stack() //栈的初始化
        { 
            Top = -1;
        }

        void push(T n) //压栈
        {        
            data[Top + 1]  = n;
            Top++;
        }

        T pop() //出栈
        {
            if(Top <= -1)
                abort();
            T d = data[Top];
            Top--;
            return d;
        }

        int size() //查看栈长度
        {
            return Top + 1;
        }

        T top() //读取栈顶元素
        {
            return data[Top];
        }

        bool isEmpty() //判断是否栈空
        {
            if(Top == -1)
                return true;
            else 
                return false;
        }

        void show() //打印当前栈中元素
        {
            for(int i = 0; i<=Top; i++)
                cout<<data[i]<<" ";
            if(Top < 0)
                cout<<"栈空";
        }
};