#include <iostream>
#include <fstream>
#include "seqstack.h"

using namespace std;
//中缀转后缀
void translate(char x[], char y[]);
//后缀的计算
float calculate(char y[]);
//四则运算
float operate(float x, float y, char c)
{
    switch (c)
    {
    case '*': return (x * y); break;
    case '/': return (y / x); break;
    case '+': return (x + y); break;
    case '-': return (y - x); break;
    default:  break;
    }
}
int main()
{
    char x[SIZE],y[SIZE];
    ifstream infile;
    int i = 0;
    //文件读取
    infile.open("data.txt");
    while(!infile.eof()) //判断文件是否读取完毕
    {
        i++;
        infile.getline(x,SIZE);
        cout<<"============================================="<<endl;
        cout<<"读取的第"<<i<<"个中缀表达式为： "<<x<<endl;
        cout<<"============================================="<<endl;
        cout<<"转换过程中栈的变化过程为："<<endl;
        translate(x,y);
        cout<<"============================================="<<endl;
        cout<<"后缀表达式为： "<<y<<endl;
        cout<<"============================================="<<endl;
        cout<<"计算过程为： "<<endl;
        float m = calculate(y);
        cout<<"============================================="<<endl;
        cout<<"最后中表达式结果为： "<<m<<endl;
        cout<<"============================================="<<endl;
    }
    infile.close();

}

void translate(char x[], char y[])
{
    Stack<char> s; //定义运算符栈
    s.stack();
    int i = 0, j = 0,number = 0;
    //初始化字符数组
    for(int k = 0; k < SIZE; k++)
        y[k] = ' ';
    //逐个扫描
    while (x[i] != '\0')
    {
        //数字字符
        if (x[i] >= '0' && x[i] <= '9')
        {
            while((x[i + 1] >= '0' && x[i + 1] <= '9' ) || (x[i + 1] == '.'))
                {
                    y[j] = x[i];
                    i++;j++;
                }
            y[j] = x[i]; y[j + 1] = ' ';
            j += 2;
        }
        //运算符处理
        if(x[i] == '#')
        {   
            number++;
            if(number == 1) 
                s.push(x[i]);
            //当第二次遇到#时结束转换
            if(number == 2)
            {
                while(s.top() != '#')
                {
                    char x = s.pop();
                    y[j] = x;
                    y[j + 1] = ' ';
                    j += 2;
                }
                s.show();cout<<'\n';
                char x = s.pop();
                s.show();cout<<'\n';
                return;
            }
        }
        if (x[i] == '(' )
            s.push(x[i]);
        if (x[i] == '*' || x[i] == '/')
        {
            while(s.top() == '*' || s.top() == '/')
            {
                char x = s.pop();
                y[j] = x;
                y[j + 1] = ' ';
                j += 2;
            }
            s.push(x[i]);
        }
        if (x[i] == '+' || x[i] == '-')
        {
            while(s.top() != '#' && s.top() != '(')
            {
                char x = s.pop();
                y[j] = x;
                y[j + 1] = ' ';
                j += 2;
            } 
            s.push(x[i]);
        }
        if(x[i] == ')')
        {
            while(s.top() != '(')
            {
                char x = s.pop();
                y[j] = x;
                y[j + 1] = ' ';
                j += 2;
            }
            char q = s.pop();
        }
        i++;
        //展示当前栈状态
        s.show();
        cout<<'\n';
    }
    // 加入字符串的结尾
    y[j+1] = '\0';
}

float calculate(char y[])
{
    Stack<float> numbers;
    numbers.stack();
    int i = 0;
    while(y[i] != '\0')
    {
        //数字字符处理
        if(y[i] >= '0' && y[i] <= '9')
        {
            float m = 0;
            //整数部分
            while(y[i + 1] >= '0' && y[i + 1] <= '9')
            {
                m += (y[i] - '0');
                m *= 10;
                i++;
            }
            m += (y[i] - '0');
            //小数部分
            if(y[i + 1] == '.')
            {
                i = i + 2;
                float j = 1;
                while(y[i + 1] >= '0' && y[i + 1] <= '9')
                {
                    m += ((float)(y[i] - '0')/(10*j));
                    j *= 10;
                    i++;
                }
                m += ((y[i] - '0')/(10*j));
            }
            numbers.push(m);
        }
        //运算符处理
        if(y[i] == '*' || y[i] == '/' || y[i] == '+' || y[i] == '-' )
        {
            numbers.show();
            cout<<y[i]<<endl;
            float x1,x2;
            x1 = numbers.pop();
            x2 = numbers.pop();
            float x = operate(x1,x2,y[i]);
            //cout<<x<<endl;
            numbers.push(x);
        }
        i++;

    }
    return numbers.pop();
}