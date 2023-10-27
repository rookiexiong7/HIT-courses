#include<iostream>
#include<fstream>
using namespace std;

#define MaxSize 15 //顺序表最大长度
#define n 10 //数据长度

//定义顺序表
typedef struct SeqList
{
    int data[MaxSize];
    int length;
}SeqList;

//打印数据
void Print(SeqList *seqlist)
{
    for(int i = 0; i < MaxSize; i++)
    {
        if(seqlist->data[i] == -1)  
            break;
        cout<<seqlist->data[i]<<endl;
    }
    cout<<"The length of SeqList is :"<<seqlist->length<<endl;
}
//初始化 
void initialize(SeqList *seqlist)
{
    seqlist->length = 0;
    ifstream infile;
    infile.open("data_test_s.txt");
    for(int i = 0;i < MaxSize; i++)
    {
        infile>>seqlist->data[i];
        if(seqlist->data[i] == -1)  
            break;
        seqlist->length++;
    }
    infile.close();
    cout<<"The length of SeqList is :"<<seqlist->length<<endl;
    Print(seqlist);
}
//查找数据
void Search(SeqList *seqlist, int x)
{
    for(int i = 0; i < MaxSize; i++)
    {
        if(seqlist->data[i] == -1)  
            break;
        if(seqlist->data[i] == x)
            cout<<"您查找的数据为"<<seqlist->data[i]<<",在顺序表中位置为"<<i+1<<endl;
    }
}
//修改数据
void Change(SeqList *seqlist, int x)
{
    if(x > seqlist->length || x < 0)
        cout<<"The wrong input"<<endl;
    else
    {
        cout<<"将被修改的数据为:"<<seqlist->data[x-1]<<endl;
        int m;
        cout<<"要将其修改为："<<endl;
        cin>>m;
        seqlist->data[x-1] = m;
    }
    Print(seqlist);
}

void Insert(SeqList *seqlist, int x)
{
    if(x > seqlist->length || x < 0)
        cout<<"The wrong input"<<endl;
    else if(seqlist->length == MaxSize)
        cout<<"当前顺序表空间已满"<<endl;
    else
    {
        int m;
        cout<<"插入的数据为："<<endl;
        cin>>m;
        for(int i = seqlist->length; i > x - 1; i--)
            seqlist->data[i]=seqlist->data[i-1];

        seqlist->data[x-1] = m;
        seqlist->length++;
        seqlist->data[seqlist->length] = -1;
    }
    Print(seqlist);
}

//删除第k个数据
void Delete(SeqList *seqlist, int k)
{
    //排除非法输入
    if(k > seqlist->length || k < 0)
        cout<<"The wrong input!"<<endl;
    else
    {
        for(int i = k - 1; i < MaxSize; i++)
        {
            seqlist->data[i]=seqlist->data[i+1];
            if(seqlist->data[i] == -1)  
                break;
        }
    //长度变化
        seqlist->length--;
        Print(seqlist);
    }
}

//选择效率最高的插入排序
void sort(SeqList *seqlist)
{
    for(int i = 1;i < seqlist->length; i++)
    {
        int key = seqlist->data[i];
        int j = i - 1;
        while((j >= 0) && (key < seqlist->data[j]))
        {
            seqlist->data[j+1] = seqlist->data[j];
            j--;
        }
        seqlist->data[j+1] = key;
    }
    Print(seqlist);
}

//对于已排好序的线性表,删除重复元素
void Delete_repeat(SeqList *seqlist)
{
    sort(seqlist);
    for(int i = 0; i < seqlist->length; i++)
        if(seqlist->data[i] == seqlist->data[i+1])
        {
            Delete(seqlist, i+1);
            i--;
        }
    //Print(seqlist);
}

//逆序
void Invert_order(SeqList *seqlist)
{
    int tail = seqlist->length - 1;
    int head = 0;
    while(tail >= (seqlist->length/2))
    {
        int temp = seqlist->data[head];
        seqlist->data[head] = seqlist->data[tail];
        seqlist->data[tail] = temp;
        head++;
        tail--;
    }
    Print(seqlist);
}

//循环右移k位
void move_k_right(SeqList *seqlist, int k)
{   
    while(k > 0)
    {
        int temp = seqlist->data[seqlist->length-1];
        for(int i = seqlist->length-1; i >= 0; i--)
            seqlist->data[i]=seqlist->data[i-1];
        seqlist->data[0] = temp;
        k--;
    }
    for(int i = 0; i < MaxSize; i++)
    {
        if(seqlist->data[i] == -1)  
            break;
        cout<<seqlist->data[i]<<endl;
    }
}
//循环左移k位
void move_k_left(SeqList *seqlist, int k)
{   
    while(k > 0)
    {
        int temp = seqlist->data[0];
        for(int i = 0; i < seqlist->length; i++)
            seqlist->data[i]=seqlist->data[i+1];
        seqlist->data[seqlist->length-1] = temp;
        k--;
    }
    for(int i = 0; i < MaxSize; i++)
    {
        if(seqlist->data[i] == -1)  
            break;
        cout<<seqlist->data[i]<<endl;
    }
}

//合并两个已排好序的线性表的算法。
void combine(SeqList *seqlist1, SeqList *seqlist2)
{
    //排除非法输入
    if(seqlist1->length + seqlist2->length > MaxSize)
        cout<<"The wrong input"<<endl;
    int head = 0;
    for(int i = seqlist1->length ; i < MaxSize; i++)
    {
        seqlist1->data[i] = seqlist2->data[head];
        head++;
        if(seqlist1->data[i] == -1)  
            break;
        seqlist1->length++;
    }
    for(int i = 0; i < MaxSize; i++)
    {
        if(seqlist1->data[i] == -1)  
            break;
        cout<<seqlist1->data[i]<<endl;
    }
    cout<<"The length of SeqList is :"<<seqlist1->length<<endl;

}

int main()
{
	int Item;//保存操作命令
    SeqList seqlist;
    SeqList seqlist1, seqlist2;

	while(1)
	{
        system("cls");
        cout<<"线性表的顺序存储结构实现"<<endl;
	    cout<<"==================================="<<endl;
	    cout<<"请选择要操作的命令："<<endl;
	    cout<<" 1  数据信息初始化"<<endl;
	    cout<<" 2  输出当前数据信息"<<endl;
	    cout<<" 3  删除顺序表中第k个数据"<<endl;
	    cout<<" 4  查找某一数据信息"<<endl;
	    cout<<" 5  修改数据信息"<<endl;
	    cout<<" 6  插入数据信息"<<endl;
	    cout<<" 7  排序"<<endl;
        cout<<" 8  删除所有重复元素"<<endl;
        cout<<" 9  线性表“逆置”"<<endl;
        cout<<" 10 线性表循环左移k位"<<endl;
        cout<<" 11 线性表循环右移k位"<<endl;
        cout<<" 12 合并两线性表"<<endl;
	    cout<<"==================================="<<endl;
		cout<<"请选择操作命令:"<<endl;
		cin>>Item;
 
 
		switch(Item)
		{
		    case 1://导入数据信息
			{
				initialize(&seqlist);//文件导入
			}
			break;
 
			case 2://输出数据信息
			{
				Print(&seqlist);
			}
			break;
 
			case 3://删除数据信息
			{
                int k;
                cout<<"要删除的数据位置为"<<endl;
                cin>>k;
				Delete(&seqlist,k);
			}
			break;
 
			case 4://查找数据信息
			{
                int k;
                cout<<"查找的数据为"<<endl;
                cin>>k;
				Search(&seqlist,k);
			}
			break;
 
			case 5://修改数据信息
			{
                int k;
                cout<<"被修改的数据位置为"<<endl;
                cin>>k;
				Change(&seqlist,k);
			}
			break;
 
			case 6://插入数据信息
			{
                int k;
                cout<<"插入的数据位置为"<<endl;
                cin>>k;
				Insert(&seqlist,k);
			}
			break;
            case 7:
            {
                sort(&seqlist);
            }
            break;
            case 8:
            {
                Delete_repeat(&seqlist);
            }
            break;
            case 9:
            {
                Invert_order(&seqlist);
            }
            break;
            case 10:
            {
                int k;
                cout<<"左移的步长为："<<endl;
                cin>>k;
                move_k_left(&seqlist,k);
            }
            break;
            case 11:
            {
                int k;
                cout<<"右移的步长为："<<endl;
                cin>>k;
                move_k_right(&seqlist,k);
            }
            break;
            case 12:
            {
                cout<<"线性表1为：（以-1结束输入）"<<endl;
                (&seqlist1)->length = 0;
                for(int i = 0;i < MaxSize; i++)
                {
                    cin>>(&seqlist1)->data[i];
                    if((&seqlist1)->data[i] == -1)  
                    break;
                    (&seqlist1)->length++;
                }
                cout<<"线性表2为：（以-1结束输入）"<<endl;
                (&seqlist2)->length = 0;
                for(int i = 0;i < MaxSize; i++)
                {
                    cin>>(&seqlist2)->data[i];
                    if((&seqlist2)->data[i] == -1)  
                    break;
                    (&seqlist2)->length++;
                }
                combine(&seqlist1,&seqlist2);
            }
			default:   break;
	    }
 
        system("pause");
    }
	return 0;
}
