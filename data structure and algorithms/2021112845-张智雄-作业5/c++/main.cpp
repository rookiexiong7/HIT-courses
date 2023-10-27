#include<iostream>
#include<fstream>
using namespace std;

typedef struct node
{
    int data;
    struct node *lchild,*rchild;
}BSTNode;
typedef BSTNode *BST;
void InsertBST(BST &T,int x)
{
    if(T == NULL){
        T = new BSTNode;
        T->data = x;
        T->lchild = NULL;
        T->rchild = NULL;
    }
    else if(x < T->data)
        InsertBST(T->lchild, x);
    else if(x > T->data)
        InsertBST(T->rchild, x);
}

void createBST(BST &T, char path[])
{
    ifstream infile;
    infile.open(path);
    int x;
    while(infile>>x)
        InsertBST(T,x);
    infile.close();
}

int deletemin(BST &T)
{
    int tmp;BST p;
    if(T->lchild == NULL){
        p = T;
        tmp = T->data;
        T = T->rchild;
        delete p;
        return tmp;
    }
    else   
        return (deletemin(T->lchild));
}

BST search(int k, BST &T, int *count)
{
    BST p = T;
    (*count)++;    
    if((p == NULL)||(k == p->data))
        return p;
    if(k < p->data)
        return (search(k, T->lchild, count));
    else if(k > p->data)
        return (search(k, T->rchild, count));
}

void Sort(BST T)
{
    if(T != NULL){
        Sort(T->lchild);
        cout<<T->data<<endl;
        Sort(T->rchild);
    }
}

int BinSearch(int x[], int low, int up, int k, int *count)
{
    if(low > up) return 0;
    else{
        (*count)++;
        int mid = (low + up)/2;
        if(k < x[mid])
            return BinSearch(x,low,mid-1,k,count);
        else if(k > x[mid])
            return BinSearch(x,mid+1,up,k,count);
        else return mid;    
    }
}
double count_success(int x[])
{
    ifstream infile;
    infile.open("data.txt");
    int y,i = 0;
    while(infile>>y)
        x[i++] = y;
    infile.close();
    int sum = 0;
    for(int j = 1; j < 2048; j = j + 2)
    {
        int count = 0;
        BinSearch(x,0,1023,j,&count);
        sum += count;
    }
    return double(sum)/1024;
}

double count_falth(int x[])
{
    ifstream infile;
    infile.open("data.txt");
    int y,i = 0;
    while(infile>>y)
        x[i++] = y;
    infile.close();
    int sum = 0;
    for(int j = 0; j < 2048; j = j + 2)
    {
        int count = 0;
        BinSearch(x,0,1023,j,&count);
        sum += count;
    }
    return double(sum)/1024;
}
int main()
{
    BST t1 = NULL, t2 = NULL;
    createBST(t1, "data.txt");
    createBST(t2, "redata.txt");
    int q = 0;
    int count1[1024] = {0}, count2[1024] = {0};
    int j = 0, sum1 = 0, sum2 = 0;
    for(int i = 1; i < 2048; i = i + 2)
    {
        BST m = search(i, t1, count1 + j);
        BST n = search(i, t2, count2 + j);
        sum1 += count1[j];
        sum2 += count2[j];
        j++;
    }
    double avl1 = double(sum1)/double(1024);
    double avl2 = double(sum2)/double(1024);
    cout<<"第一组测试数据查找成功的平均查找长度为："<<avl1<<endl;
    cout<<"第二组测试数据查找成功的平均查找长度为："<<avl2<<endl;
    
    j = 0, sum1 = 0, sum2 = 0;
    int count3[1024] = {0}, count4[1024] = {0};

    for(int i = 0; i < 2048; i = i + 2)
    {
        BST m = search(i, t1, count3 + j);
        BST n = search(i, t2, count4 + j);
        sum1 += count3[j];
        sum2 += count4[j];
        j++;
    }
    double f1 = double(sum1)/double(1024);
    double f2 = double(sum2)/double(1024);
    cout<<"第一组测试数据查找失败的平均查找长度为："<<f1<<endl;
    cout<<"第二组测试数据查找失败的平均查找长度为："<<f2<<endl;

    for(int k = 0; k < 1024; k++){
        int x = deletemin(t1);
        int y = deletemin(t2);
    }
    int x[1024];
    double avl3 =  count_success(x);
    cout<<"折半查找测试数据查找成功的平均查找长度为："<<avl3<<endl;
    double f3 = count_falth(x);
    cout<<"折半查找测试数据查找失败的平均查找长度为："<<f3<<endl;
}