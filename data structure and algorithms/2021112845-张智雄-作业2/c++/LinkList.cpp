#include<iostream>
#include<fstream>
using namespace std;
#define MaxSize 15 //顺序表最大长度

typedef struct LinkList
{
    int data;
    LinkList *next;
}*pLinkList;

//为新节点申请内存
pLinkList applicate_memory()
{
    pLinkList p = (pLinkList)malloc(sizeof(struct LinkList));
    if (p == NULL) {
        cout<<"No enough memory to allocate!\n"<<endl;
        exit(0);
    }
    return p;
}

//打印数据
void Print(struct LinkList *phead)
{
    struct LinkList *p;
    p = phead->next;
    int i = 1;
    while(p != NULL)
    {
        cout<<p->data<<endl;
        p = p->next;
        i++;
    }
   cout<<"The length of SeqList is :"<<i-1<<endl;
}

//初始化
pLinkList initialize(struct LinkList *phead)
{
    int length = 0;
    ifstream infile;
    struct LinkList *p;
    p = applicate_memory();
    phead = p;
    p->next = NULL;
    infile.open("data_test_l.txt");
    while(!infile.eof())
    {
        struct LinkList *pNew;
        pNew = applicate_memory();
        if (pNew == NULL)
        {
            cout<<"No enough memory to import!\n"<<endl;
            exit(0);
        }
        infile>>pNew->data;
        pNew->next = NULL;
        p->next = pNew;
        p = p->next;
        length++;
    }
    infile.close();
    cout<<"The length of LinkList is :"<<length<<endl;
    Print(phead);
    return phead;
}

//查找数据
void Search(struct LinkList *phead, int x)
{
    struct LinkList *p;
    p = phead->next;
    int i = 1;//计数器
    while(p != NULL)
    {
        if(p->data == x)
            cout<<"您查找的数据为"<<p->data<<",在顺序表中位置为"<<i<<endl;
        i++;
        p = p->next;
    }
}

//修改数据
void Change(struct LinkList *phead, int x)
{
    struct LinkList *p;
    p = phead->next;
    int i = 1;//计数器
    while(p != NULL)
    {
        if(i == x)
        {
            cout<<"将被修改的数据为:"<<p->data<<endl;
            int m;
            cout<<"要将其修改为："<<endl;
            cin>>m;
            p->data = m;
        }
        i++;
        p = p->next;
    }
    Print(phead);
}

//插入数据
void Insert(struct LinkList *phead, int x)
{
    struct LinkList *p;
    p = phead->next;
    int i = 1;//计数器

    int m;
    cout<<"在位置"<<x<<"插入的数据为："<<endl;
    cin>>m;
    struct LinkList *pNew;
    pNew = applicate_memory();
    pNew->data = m;
    while(p != NULL)
    {
        if(i + 1 == x)
        {
            struct LinkList *q;
            q = p->next;
            p->next = pNew;
            pNew->next = q;
            break;
        }
        i++;
        p = p->next;
    }
    Print(phead);
}

//删除第k个数据
void Delete(struct LinkList *phead, int k)
{
    struct LinkList *p;
    p = phead;
    int i = 1;//计数器
    while(p != NULL)
    {
        if(i == k)
        {
            struct LinkList *q;
            q = p->next;
            p->next = q->next;
            break;
        }
        i++;
        p = p->next;
    }
    Print(phead);
}

//选择排序
void sort(struct LinkList *phead)
{
    struct LinkList *p,*q;
    for (p = phead->next; p != NULL; p = p->next)
        for (q = p->next; q != NULL; q = q->next)
        {
            if(p->data > q->data){
                int temp = p->data;
                p->data = q->data;
                q->data = temp;
            }
        }
    Print(phead); 
}

//对于已排好序的线性表,删除重复元素
void Delete_repeat(struct LinkList *phead)
{
    sort(phead);
    struct LinkList *p = phead->next;
    int i = 1;
    while(p->next != NULL)
    {
        if(p->data == p->next->data)
            Delete(phead, i);
        p = p->next;
        i++;
    }
}

//逆序
void Invert_order(struct LinkList *phead)
{
    struct LinkList *p,*q;
    p = phead->next->next;
    q = p->next;
    phead->next->next = NULL;
    while(p != NULL)
    {
        p->next = phead->next;
        phead->next = p;
        p = q;
        if(q->next != NULL)
            q = q->next;
        else  //遍历至最后一个节点
        {
            p->next = phead->next;
            phead->next = q;
            break;
        }
    }
    Print(phead);
}

//循环右移k位
void move_k_right(struct LinkList *phead, int k)
{   
    struct LinkList *p, *q;
    while(k > 0)
    {
        p = phead->next;
        while(p != NULL)
        {
            if(p->next->next == NULL)//遍历至尾节点
            {
                q = phead->next;
                phead->next = p->next;
                p->next->next = q;
                p->next = NULL;
            }
            p = p->next;
        }
        k--;
    }
    Print(phead);
}

//循环左移k位
void move_k_left(struct LinkList *phead, int k)
{   
    struct LinkList *p, *q;
    while(k > 0)
    {
        q = phead->next;
        phead->next = q->next;
        q->next = NULL;
        p = phead->next;
        while(p != NULL)
        {
            if(p->next == NULL)//遍历至尾节点
            {
                p->next = q;
                break;
            }
            p = p->next;
        }
        k--;
    }
    Print(phead);
}


//合并两个已排好序的线性表的算法。
void combine(struct LinkList *phead1,struct LinkList *phead2)
{
    struct LinkList *p;
    p = phead1->next;
    while(p != NULL)
    {
        if(p->next == NULL)//遍历至尾节点
        {
            p->next = phead2->next;
            break;
        }
        p = p->next;
    }
    return ;
}

void clear(struct LinkList *phead)
{
    struct LinkList *p, *q;
    p = phead;

    if (phead == NULL)
        return;
    while(p != NULL)
    {
        q = p;
        free(p);
        p = q->next;
    }
    free(q);
}

int main()
{
int Item;//保存操作命令
    struct LinkList *linklist;
    struct LinkList *linklist1, *linklist2;

	while(1)
	{
        system("cls");
        cout<<"线性表的链式存储结构实现"<<endl;
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
        cout<<" 13 退出程序"<<endl;
	    cout<<"==================================="<<endl;
		cout<<"请选择操作命令:"<<endl;
		cin>>Item;
 
		switch(Item)
		{
		    case 1://导入数据信息
			{
				linklist = initialize(linklist);//文件导入
			}
			break;
 
			case 2://输出数据信息
			{
				Print(linklist);
			}
			break;
 
			case 3://删除数据信息
			{
                int k;
                cout<<"要删除的数据位置为"<<endl;
                cin>>k;
				Delete(linklist,k);
			}
			break;
 
			case 4://查找数据信息
			{
                int k;
                cout<<"查找的数据为"<<endl;
                cin>>k;
				Search(linklist,k);
			}
			break;
 
			case 5://修改数据信息
			{
                int k;
                cout<<"被修改的数据位置为"<<endl;
                cin>>k;
				Change(linklist,k);
			}
			break;
 
			case 6://插入数据信息
			{
                int k;
                cout<<"插入的数据位置为"<<endl;
                cin>>k;
				Insert(linklist,k);
			}
			break;
            case 7:
            {
                sort(linklist);
            }
            break;
            case 8:
            {
                Delete_repeat(linklist);
            }
            break;
            case 9:
            {
                Invert_order(linklist);
            }
            break;
            case 10:
            {
                int k;
                cout<<"左移的步长为："<<endl;
                cin>>k;
                move_k_left(linklist,k);
            }
            break;
            case 11:
            {
                int k;
                cout<<"右移的步长为："<<endl;
                cin>>k;
                move_k_right(linklist,k);
            }
            break;
            case 12:
            {
                struct LinkList *pNew,*p1,*p2;
                p1 = applicate_memory();
                linklist1 = p1;
                p1->next = NULL;
                p2 = applicate_memory();
                linklist2 = p2;
                p2->next = NULL;
                cout<<"线性表1为：（以-1结束输入）"<<endl;
                for(int i = 0;i < MaxSize; i++)
                {
                    pNew = applicate_memory();
                    if (pNew == NULL)
                    {
                        cout<<"No enough memory to import!\n"<<endl;
                        exit(0);
                    }
                    int m;
                    cin>>m;
                    if(m == -1)
                        break;
                    else
                    {
                        pNew->data = m;
                        pNew->next = NULL;
                        p1->next = pNew;
                        p1 = p1->next;
                    }
                }
                cout<<"线性表2为：（以-1结束输入）"<<endl;
                for(int i = 0;i < MaxSize; i++)
                {
                    pNew = applicate_memory();
                    if (pNew == NULL)
                    {
                        cout<<"No enough memory to import!\n"<<endl;
                        exit(0);
                    }
                    int m;
                    cin>>m;
                    if(m == -1)
                        break;
                    else
                    {
                        pNew->data = m;
                        pNew->next = NULL;
                        p2->next = pNew;
                        p2 = p2->next;
                    }
                }
                combine(linklist1,linklist2);
                Print(linklist1);
                clear(linklist1);
                clear(linklist2);
                Print(linklist1);
            }
            break;
            case 13:{
                clear(linklist);
                clear(linklist1);
                clear(linklist2);
            }
            break;
			default:   break;
	    }
 
        system("pause");
    }
	return 0;
}
