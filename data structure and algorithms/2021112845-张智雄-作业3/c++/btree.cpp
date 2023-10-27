#include <iostream>
#include "seqstack.h"

using namespace std;

struct node
{
    struct node *lchild;
    struct node *rchild;
    char data;
};
typedef struct node *Bnode;

void create(Bnode &T)
{
    char ch;
    cin >> ch;
    if(ch == '#') T = NULL;
    else
    {
        T = new node;
        T->data = ch;
        create(T->lchild);
        create(T->rchild); 
    }
}
//先序递归遍历
void PreOrder(Bnode BT)
{
    if(BT != NULL)
    {
        cout<<BT->data;
        PreOrder(BT->lchild);
        PreOrder(BT->rchild);
    }
}
//先序非递归遍历
void FirstOrder(Bnode root)
{
    Stack<Bnode> s;
    s.stack(); //初始化
    while((root != NULL) || (!s.isEmpty()))
    {
        while(root != NULL)
        {
            cout<<root->data;
            s.push(root);
            root = root->lchild;
        }
        if(!s.isEmpty())
        {
            root = s.pop();
            root = root->rchild;
        }
    }
}
//中序遍历
void InOrder(Bnode BT)
{
    if(BT != NULL)
    {
        InOrder(BT->lchild);
        cout<<BT->data;
        InOrder(BT->rchild);
    }
}
//中序非递归遍历
void SecondOrder(Bnode root)
{
    Stack<Bnode> s;
    s.stack(); //初始化
    while(root != NULL || !s.isEmpty())
    {
        while(root != NULL)
        {
            s.push(root);
            root = root->lchild;
        }
        if(!s.isEmpty())
        {
            root = s.pop();
            cout << root->data;
            root = root->rchild;
        }
    }
}
//后序遍历
void PostOrder(Bnode BT)
{
    if(BT != NULL)
    {
        PostOrder(BT->lchild);
        PostOrder(BT->rchild);
        cout<<BT->data;
    }
}

//后序非递归遍历
void LastOrder(Bnode root)
{
    Stack<Bnode> s;
    Stack<int> t;
    s.stack(); //初始化
    t.stack();
    while(root != NULL || !s.isEmpty())
    {
        while(root != NULL)
        {
            s.push(root);
            t.push(1);
            root = root->lchild;
        }
        while(t.top() == 2 && !s.isEmpty())
        {
            
            int x = t.pop();
            cout<<s.pop()->data;
        }
        if(!s.isEmpty())
        {
            int x = t.pop();
            t.push(2);
            root = s.top()->rchild;
        }
    }
}

//层序遍历
void BFOrder(Bnode root)
{
    //利用队列的思想
    Bnode x[SIZE];
    int pre = 0, now = 0;
    if(root == NULL) return;
    x[++now] = root;
    while(pre != now)
    {
        Bnode q = x[++pre];
        cout<<q->data;
        if(q->lchild != NULL) x[++now] = q->lchild;
        if(q->rchild != NULL) x[++now] = q->rchild;
    }
}


bool isCBT(Bnode root)
{
    Bnode x[SIZE];
    int pre = 0, now = 0;
    bool flag = true;
    if(root == NULL) return false;
    x[++now] = root;
    while(pre != now)
    {
        Bnode q = x[++pre];
        if(q->lchild != NULL) {
            x[++now] = q->lchild;
            flag = false;
        }

        if(q->rchild != NULL) 
        {
            x[++now] = q->rchild;
            if(q->lchild == NULL) flag = false;
            else flag = true;
        }  
    }
    return flag;
}

int Width(Bnode root)
{
    Bnode x[SIZE];
    int level[SIZE] = {-1};
    int width[SIZE] = {0};
    int pre = -1, now = -1, max_w = 0, l = 1;
    if(root == NULL) return 0;
    now++;
    x[now] = root;
    level[now] = l;
    while(pre != now)
    {
        pre++;
        Bnode q = x[pre];
        int   m = level[pre];
        if(q->lchild != NULL)  
        {
            now++;
            x[now] = q->lchild;
            level[now] = m + 1;
        }
        if(q->rchild != NULL)  
        {
            now++;
            x[now] = q->rchild;
            level[now] = m + 1;
        }

    }
    for(int i = 0; i < SIZE;i++)  
    {
        if(level[i] == -1)
            break;
        width[level[i]]++;
    }
    for(int i = 1; i < SIZE;i++)
    {
        if(width[i] == 0)
            break;
        if(width[i] > max_w)
            max_w = width[i];
        cout<<width[i]<<" ";
    }
    return max_w;
}


void clear(Bnode BT)
{
    if(BT != NULL)
    {
        clear(BT->lchild);
        clear(BT->rchild);
        delete BT;
    }
    else return;
}

int main()
{
    Bnode root = NULL;
    cout<<"按照先序序列建立二叉树结构（#表示空）： ";
    create(root);
    cout<<endl;
    cout<<"前序递归遍历序列为： ";
    PreOrder(root);
    cout<<endl;
    cout<<"中序递归遍历序列为： ";
    InOrder(root);
    cout<<endl;
    cout<<"后序递归遍历序列为： ";
    PostOrder(root);
    cout<<endl;
    cout<<"前序非递归遍历序列为： ";
    FirstOrder(root);
    cout<<endl;
    cout<<"中序非递归遍历序列为： ";
    SecondOrder(root);
    cout<<endl;
    cout<<"后序非递归遍历序列为： ";
    LastOrder(root);
    cout<<endl;
    cout<<"层序遍历序列为： ";
    BFOrder(root);
    cout<<endl;
    cout<<"是否为完全二叉树（0表示否，1表示是）： "<< isCBT(root)<<endl;
    cout<<"二叉树各层所含结点数依次为： ";
    int x = Width(root);
    cout<<endl;
    cout<<"二叉树最大宽度为： "<< x << endl;
    clear(root);
}