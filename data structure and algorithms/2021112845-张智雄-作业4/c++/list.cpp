#include<iostream>
#include<stack>
#include<queue>
#define N 15    //最大顶点数
using namespace std;

struct EdgeNode
{
    int adjvex; //邻接点域
    int weight; //边权重
    EdgeNode *next; //指针域
};

struct vertexNode
{
    int vertax; //数据域
    EdgeNode *phead; //边链表头指针
};

struct graph
{
    int n,e;//图的顶点数与边数
    int edge[N][N] = {0};//邻接矩阵
    int vertex[N];//顶点表
    struct vertexNode vertexlist[N]; //顶点表
};
bool visited[N];
int  dfn[N] = {0};
int  bfn[N] = {0};

//邻接矩阵的展示
void show_matrix(struct graph *G);
//邻接表的展示
void show_list(struct graph *G);
//深(广)度优先编号展示
void show_search(int fn[]);
//无向图的邻接表生成
void create_list(struct graph *G);
//邻接表转换为邻接矩阵
void list_to_matrix(struct graph *G, struct graph *g);
//邻接矩阵转换为邻接表
void matrix_to_list(struct graph *G, struct graph *g);
//邻接表的非递归深度优先搜索
void DFS_list(struct graph G);
//邻接表的递归深度优先搜索
void DFS_list_recursion(struct graph G);
//从第i个顶点出发的一次深度优先搜索
void DFS1(struct graph G, int i, int *count, int search_tree[][N]);
//邻接表的广度优先搜索
void BFS_list(struct graph G);
//计算每个顶点度
void degree(struct graph G, int D[]);
//清理内存
void clear(struct graph *G);

int main()
{
    struct graph G, g;
    int D[N];
    cout<<"以邻接表方式构建无向图"<<endl;
    cout<<"==============================="<<endl;
    create_list(&G);
    list_to_matrix(&G, &g);
    DFS_list_recursion(G);
    DFS_list(G);
    BFS_list(G);
    degree(G, D);
    clear(&G);
    clear(&g);
}
void show_matrix(struct graph *G)
{
    cout<<"此无向图的邻接矩阵为："<<endl;
    for(int i = 0; i < G->n; i++)
    {
        for(int j = 0; j < G->n; j++)
        {
            cout<<G->edge[i][j]<<" ";
        }
        cout<<endl;
    } 
}

void show_list(struct graph *G)
{
    cout<<"此无向图的邻接表为："<<endl;
    for(int i = 0; i < G->n; i++)
    {
        EdgeNode *p;
        p = G->vertexlist[i].phead;
        cout<<G->vertexlist[i].vertax;
        while(p != NULL)
        {
            cout<<"->"<<p->adjvex;
            p = p->next;
        }
        cout<<endl;
    }
}

void create_list(struct graph *G)
{
    cout<<"图的顶点数为：";
    cin>>G->n;
    cout<<"图的边数为：";
    cin>>G->e;
    cout<<"此无向图的顶点为：";
    for(int i = 0; i < G->n; i++)
    {
        G->vertex[i] = i;
        cout<<i<<" ";
        G->vertexlist[i].vertax = i;
        G->vertexlist[i].phead = NULL;
    }
    cout<<endl;
    for(int i = 0; i < G->e; i++)
    {
        int head,tail,w;//边依附的两个顶点序号m，n；边权值w
        cout<<"第"<<i+1<<"条边的起止结点以及权值为：";
        cin>>head>>tail>>w;
        EdgeNode *p = new EdgeNode;
        p->adjvex = tail; p->weight = w;
        p->next = G->vertexlist[head].phead;
        G->vertexlist[head].phead = p;
        EdgeNode *q = new EdgeNode;
        q->adjvex = head; q->weight = w;
        q->next = G->vertexlist[tail].phead;
        G->vertexlist[tail].phead = q;
    }
    show_list(G);
}

void list_to_matrix(struct graph *G, struct graph *g)
{
    for(int i = 0; i < G->n; i++)
        g->vertex[i] = G->vertexlist[i].vertax;   
    g->n = G->n;
    g->e = G->e;
    for(int i = 0; i < G->n; i++)
    {
        EdgeNode *p;
        p = G->vertexlist[i].phead;
        while(p != NULL)
        {        
            int m = i, n = p->adjvex;
            g->edge[m][n] = p->weight;
            p = p->next;
        }
    }
    cout<<"邻接表转换为邻接矩阵......"<<endl;
    show_matrix(g);
}

void clear(struct graph *G)
{
    for(int i = 0; i < G->n; i++)
    {
        EdgeNode *p;
        p = G->vertexlist[i].phead;
        while(p != NULL)
        {
            EdgeNode *q = p->next;        
            delete p;
            p = q;
            delete q;
        }
    }
}

void DFS_list(struct graph G)
{
    int i, count = 1;
    stack<int> s;
    for(int j = 0; j < G.n; j++)
        visited[j] = false;
    cout<<"深度优先序列为（非递归）： ";
    int search_tree[N][N] = {0};
    for(int i = 0; i < G.n; i++)
    {
        if(!visited[i])
        {
            cout<<G.vertexlist[i].vertax<<" ";
            dfn[i] = count++;
            visited[i] = true;
            s.push(i);
        }
        
        while(!s.empty())
        {
            EdgeNode *p = G.vertexlist[s.top()].phead;
            int m = G.vertexlist[s.top()].vertax;
            while(p != NULL)
            {
                if(!visited[p->adjvex])
                {
                    search_tree[m][p->adjvex] = p->weight;
                    search_tree[p->adjvex][m] = p->weight;
                    cout<<p->adjvex<<" ";
                    dfn[p->adjvex] = count++;
                    visited[p->adjvex] = true;
                    s.push(p->adjvex);
                    m = G.vertexlist[p->adjvex].vertax;
                    p = G.vertexlist[p->adjvex].phead;
                }
                else p = p->next;
            }
            p = G.vertexlist[s.top()].phead;
            m = G.vertexlist[s.top()].vertax;
            s.pop();
        }
    }
    cout<<endl;
    cout<<"深度优先编号为（非递归）： "<<endl;
    show_search(dfn);
    cout<<"深度优先生成树的邻接表为："<<endl;
    for(int i = 0; i < G.n; i++)
        for(int j = 0; j < G.n; j++)
            G.edge[i][j] = search_tree[i][j];
    struct graph g;
    matrix_to_list(&G, &g);
    //clear(&g);
}

void DFS1(struct graph G, int i, int *count, int search_tree[][N])
{
    EdgeNode *p = G.vertexlist[i].phead;
    if(!visited[i])
    {
        cout<<G.vertexlist[i].vertax<<" ";
        dfn[i] = (*count)++;
        visited[i] = true;
    }
    while(p != NULL)
    {
        if(!visited[p->adjvex])
        {
            search_tree[i][p->adjvex] = p->weight;
            search_tree[p->adjvex][i] = p->weight;
            DFS1(G,p->adjvex,count,search_tree);
        }      
        p = p->next;
    }
}

void DFS_list_recursion(struct graph G)
{
    int i, count = 1;
    for(int j = 0; j < G.n; j++)
        visited[j] = false;
    cout<<"深度优先序列为（递归）： ";
    int search_tree[N][N] = {0};
    for(int i = 0; i < G.n; i++)
        if(!visited[i]) DFS1(G,i,&count,search_tree);
    cout<<endl;
    cout<<"深度优先编号为（递归）： "<<endl;
    show_search(dfn);
    cout<<"深度优先生成树的邻接表为："<<endl;
    for(int i = 0; i < G.n; i++)
        for(int j = 0; j < G.n; j++)
            G.edge[i][j] = search_tree[i][j];
    struct graph g;
    matrix_to_list(&G, &g);
    //clear(&g);
}

void BFS_list(struct graph G)
{
    int i, count = 1;
    queue<int> q;
    for(int j = 0; j < G.n; j++)
        visited[j] = false;
    cout<<"广度优先序列为： ";
    int search_tree[N][N] = {0};
    for(int i = 0; i < G.n; i++)
    {
        if(!visited[i])
        {
            cout<<G.vertexlist[i].vertax<<" ";
            bfn[i] = count++;
            visited[i] = true;
            q.push(i);
        }
        while(!q.empty())
        {
            int i = q.front();
            q.pop();
            EdgeNode *p = G.vertexlist[i].phead;
            while(p != NULL)
            {
                if(!visited[p->adjvex])
                {
                    search_tree[i][p->adjvex] = p->weight;
                    search_tree[p->adjvex][i] = p->weight;
                    cout<<p->adjvex<<" ";
                    bfn[p->adjvex] = count++;
                    visited[p->adjvex] = true;
                    q.push(p->adjvex);
                }
                p = p->next;
            }
        }
    }
    cout<<endl;
    cout<<"广度优先编号为： "<<endl;
    show_search(bfn);
    cout<<"广度优先生成树的邻接表为："<<endl;
    for(int i = 0; i < G.n; i++)
        for(int j = 0; j < G.n; j++)
            G.edge[i][j] = search_tree[i][j];
    struct graph g;
    matrix_to_list(&G, &g);
    //clear(&g);
}
void degree(struct graph G, int D[])
{
    for(int i = 0; i < G.n; i++)
    {
        D[i] = 0;
        EdgeNode *p;
        p = G.vertexlist[i].phead;
        while(p != NULL)
        {
            D[i]++;
            p = p->next;
        }
    }
    for(int i = 0; i < G.n; i++)
        cout<<"第"<<i<<"个顶点的度为："<<D[i]<<endl;
}
void show_search(int fn[])
{
    for(int i = 0; i < N; i++)
        if(fn[i] != 0)
            cout<<i<<" ";
    cout<<endl;
    for(int i = 0; i < N; i++)
        if(fn[i] != 0)
            cout<<fn[i]<<" ";
    cout<<endl;  
}

void matrix_to_list(struct graph *G, struct graph *g)
{
    for(int i = 0; i < G->n; i++)
    {
        g->vertexlist[i].vertax = G->vertex[i];
        g->vertexlist[i].phead  = NULL;
    }
    g->n = G->n;
    g->e = G->e;
    for(int i = 0; i < G->n; i++)
        for(int j = 0; j < G->n; j++)
            if(G->edge[i][j] != 0)
            {
                int w = G->edge[i][j];
                EdgeNode *p = new EdgeNode;
                p->adjvex = j; p->weight = w;
                p->next = g->vertexlist[i].phead;
                g->vertexlist[i].phead = p;
            }
    for(int i = 0; i < G->n; i++)
    {
        EdgeNode *p;
        p = g->vertexlist[i].phead;
        cout<<g->vertexlist[i].vertax;
        while(p != NULL)
        {
            cout<<"->"<<p->adjvex;
            p = p->next;
        }
        cout<<endl;
    }
}