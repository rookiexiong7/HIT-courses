#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#define SIZE 200
#define MAX 42 //文本所用字符个数

using namespace std;
struct data 
{
    char alphabet;
    double frequency = 0;
};//读取数据临时储存

typedef struct node  
{
    int self;
    int lchild = -1;
    int rchild = -1;
    int parent = -1;
    char alphabet;
    double weight = 0;
}HTNODE;//哈夫曼树节点

struct code
{
    char ch;
    char bits[MAX+1];
};//编码结构

typedef struct heap
{
    HTNODE heap[2*MAX]; //顺序储存堆结构
    int n; //堆内数据个数
}HEAP; //堆结构

//最小堆结构初始化
void MinHeap(HEAP *minheap)
{
    minheap->n = 0;
}

//插入新节点
void insert(HEAP *minheap, HTNODE x)
{
    int i;
    if((minheap->n < MAX) && (minheap->n >= 0))
    {
        i = minheap->n + 1;
        while(((i != 1) && (x.weight < minheap->heap[i/2].weight)) || ((i != 1) && (x.weight == minheap->heap[i/2].weight) && (x.self < minheap->heap[i/2].self)))
        {
            minheap->heap[i] = minheap->heap[i/2];
            i = i / 2;
        }
        minheap->heap[i] = x;
        minheap->n++;
    }
}

//删除最小节点
HTNODE deleteMin(HEAP *minheap)
{
    int par = 1, child = 2;
    HTNODE ele, tmp;
    if(minheap->n > 0)
    {
        ele = minheap->heap[1];
        tmp = minheap->heap[minheap->n--];
        while(child < minheap->n)
        {
            if((child < minheap->n) && (minheap->heap[child].weight > minheap->heap[child + 1].weight))
                child++;
            if(tmp.weight <= minheap->heap[child].weight) break;
            minheap->heap[par] = minheap->heap[child];
            par = child;
            child *= 2;
        }
        minheap->heap[par] = tmp;
        return ele;
    }
}

//读取文件信息，返回等长编码文件长度
int file(int x[], struct data Youth[]);
//文件信息转移到树节点，并初始化堆
void data_read(struct data Youth[], HTNODE T[], HEAP *minheap);
//寻找最小的两个节点（不使用堆）
void Select(HTNODE T[], int i, int &p1, int &p2);
//建立哈夫曼树
void createtree(HTNODE T[], HEAP *minheap);
//哈夫曼编码
void Encoding(HTNODE T[], struct code HAff[]);
//输出编码文件，返回压缩后的长度
int transfer(struct code HAff[], char path[]);
//解码文件
void check(HTNODE T[], char path[]);

int main()
{
    struct data Youth[MAX];
    struct code HAFF[MAX];
    HTNODE T[2*MAX - 1];
    HEAP min;
    MinHeap(&min);
    int x[SIZE] = {0};
    int pre = file(x,Youth);
    cout<<"================================="<<endl;
    cout<<"文本出现字符及其使用频率： "<<endl;
    data_read(Youth, T, &min);
    cout<<"================================="<<endl;
    createtree(T, &min);
    cout<<'\n';
    cout<<"================================="<<endl;
    cout<<"各字符哈夫曼编码：  "<<endl;
    Encoding(T, HAFF);
    cout<<"================================="<<endl;
    int now = transfer(HAFF,"result.txt");
    cout<<"原文件按照6位等长编码所得长度为："<<pre<<endl;
    cout<<"按照哈夫曼编码所得长度为："<<now<<endl;
    cout<<"压缩率为: "<<double(now)/double(pre)<<endl;
    cout<<"================================="<<endl;
    check(T, "check.txt");
}

int file(int x[], struct data Youth[])
{
    char data;
    ifstream infile;
    infile.open("Youth.txt");
    int sum = 0;
    while(infile.get(data))
    {
        x[data]++;
        sum++;
    }
    int i = 0;
    for(int j = 0; j < SIZE; j++)
    {
        if(x[j] != 0) 
        {
            Youth[i].alphabet = char(j);
            Youth[i].frequency = double(x[j]) / double(sum);
            i++;
        }
    }
    infile.close();
    return sum*6;
}

void data_read(struct data Youth[], HTNODE T[], HEAP *minheap)
{   
    //节点初始化
    for(int i = 0; i < MAX; i++)
    {
        if(Youth[i].alphabet == '\n') cout <<"换行符"<<" "<< Youth[i].frequency<<endl;
        else if(Youth[i].alphabet == ' ')  cout <<"空格符"<<" "<< Youth[i].frequency<<endl;
        else  cout<<Youth[i].alphabet <<" "<< Youth[i].frequency<<endl;
        T[i].alphabet = Youth[i].alphabet;
        T[i].weight = Youth[i].frequency;
        T[i].self = i;
        insert(minheap, T[i]);
    }
}

void Select(HTNODE T[], int n, int &p1, int &p2)
{
    int i, j;
    for(i = 0; i < n; i++)
        if(T[i].parent == -1) {p1 = i; break;}
    for(j = i + 1; j < n; j++)
        if(T[j].parent == -1) {p2 = j; break;}
    for(i = 0; i < n; i++)
        if((T[p1].weight > T[i].weight) && (T[i].parent == -1) && ((i-p2) != 0))  p1 = i;
    for(j = 0; j < n; j++)
        if((T[p2].weight > T[j].weight) && (T[j].parent == -1) && (j != p1))  p2 = j;
    //cout<<p1<<p2;
}

void createtree(HTNODE T[], HEAP *minheap)
{
    int i, p1, p2,j,k;
    for(i = MAX; i < (2*MAX-1); i++)
    {
        j = deleteMin(minheap).self;
        k = deleteMin(minheap).self;
        p1 = min(j,k);
        p2 = max(j,k);
        T[p1].parent = T[p2].parent = i;
        T[i].lchild = p1;
        T[i].rchild = p2;
        T[i].weight = T[p1].weight + T[p2].weight;
        T[i].self   = i;
        insert(minheap, T[i]);
        
    }
    cout << "树建立完毕！";
}

void Encoding(HTNODE T[], struct code HAff[])
{
    for(int i = 0; i < MAX; i++)
    {
        HAff[i].ch = T[i].alphabet;
        int start = MAX;
        char x[MAX + 1];
        x[MAX] = '\0';
        int p, q = i;
        while((p = T[q].parent) != -1)
        {
            if(T[p].lchild == q) x[--start] = '0';
            else if(T[p].rchild == q) x[--start] = '1';
            q = p;
        }
        strcpy(HAff[i].bits, &x[start]);
        if(HAff[i].ch == '\n') cout <<"换行符"<<" "<<HAff[i].bits<<endl;
        else if(HAff[i].ch == ' ') cout<<"空格符"<<" "<<HAff[i].bits<<endl;
        else cout << HAff[i].ch <<" " <<HAff[i].bits<<endl;
    }
}

int transfer(struct code HAff[], char path[])
{
    ofstream fout(path);
    char x;
    int number = 0;
    if(fout)
    {
        ifstream infile;
        infile.open("Youth.txt");
        while(infile.get(x))
            for(int i = 0; i < MAX; i++)
                if(HAff[i].ch == x)
                {
                    fout<<HAff[i].bits;
                    number += strlen(HAff[i].bits);
                }
    }
    fout.close();
    return number;
}

void check(HTNODE T[], char path[])
{
    char x;
    ifstream infile;
    ofstream fout(path);
    if(fout)
    {
        int p,q;
        infile.open("result.txt");
        for(int i = 0; i < 2*MAX-1;i++)
            if(T[i].weight == 1)  {p = i;q = p;}
        while(infile.get(x))
        {
            if(x == '0') p = T[p].lchild;
            else if(x == '1') p = T[p].rchild;

            if((T[p].lchild == -1) && (T[p].rchild == -1))
            {
                fout<<T[p].alphabet;
                p = q;
            }
        }
    }
    fout.close();
    infile.close();
}