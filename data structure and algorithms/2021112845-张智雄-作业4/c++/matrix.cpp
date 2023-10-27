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
//无向图的邻接矩阵构建
void create_matrix(struct graph *G);
//邻接矩阵转换为邻接表
void matrix_to_list(struct graph *G, struct graph *g);
//邻接矩阵的非递归深度优先搜索
void DFS_matrix(struct graph G);
//邻接矩阵的递归深度优先搜索
void DFS_matrix_recursion(struct graph G);
//从第i个顶点出发的一次深度优先搜索
void DFS2(struct graph G, int i, int *count, int search_tree[][N]);
//邻接矩阵的广度优先搜索
void BFS_matrix(struct graph G);
//清理内存
void clear(struct graph *G);

int main()
{
    struct graph G, g;
    cout<<"以邻接矩阵方式构建无向图"<<endl;
    cout<<"==============================="<<endl;
    create_matrix(&G);
    matrix_to_list(&G, &g);
    DFS_matrix(G);
    DFS_matrix_recursion(G);
    BFS_matrix(G);
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

void create_matrix(struct graph *G)
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
    }
    cout<<endl;
    for(int i = 0; i < G->e; i++)
    {
        int m,n,w;//边依附的两个顶点序号m，n；边权值w
        cout<<"第"<<i+1<<"条边的起止结点以及权值为：";
        cin>>m>>n>>w;
        G->edge[m][n] = w;
        G->edge[n][m] = w;
    }
    show_matrix(G); 
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
    cout<<"邻接矩阵转换为邻接表......"<<endl;
    show_list(g);
}

void DFS_matrix(struct graph G)
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
            cout<<G.vertex[i]<<" ";
            dfn[i] = count++;
            visited[i] = true;
            s.push(i);
        }
        int j = 0, f = i;
        while(!s.empty())
        {
            while((j >= 0) && (j < G.n))
            {            
                if((G.edge[f][j] != 0) && (!visited[j]))
                {
                    search_tree[f][j] = G.edge[f][j];
                    search_tree[j][f] = G.edge[f][j];
                    cout<<G.vertex[j]<<" ";
                    dfn[j] = count++;
                    visited[j] = true;
                    s.push(j);
                    f = j; j = 0;
                }
                else j++;
            }
            f = s.top();s.pop();
            j = 0;
        }
    }
    cout<<endl;
    cout<<"深度优先编号为（非递归）： "<<endl;
    show_search(dfn);
    cout<<"深度优先生成树矩阵为："<<endl;
    for(int i = 0; i < G.n; i++)
    {
        for(int j = 0; j < G.n; j++)
            cout<<search_tree[i][j]<<" ";
        cout<<endl;
    }
}
void DFS2(struct graph G, int i, int *count, int search_tree[][N])
{
    if(!visited[i])
    {
        cout<<G.vertex[i]<<" ";
        dfn[i] = (*count)++;
        visited[i] = true;
    }
    for(int j = 0; j < G.n; j++)
    {            
        if((G.edge[i][j] != 0) && (!visited[j]))
        {
            search_tree[i][j] = G.edge[i][j];
            search_tree[j][i] = G.edge[i][j];
            DFS2(G,j,count,search_tree);
        }
            
    }
}
void DFS_matrix_recursion(struct graph G)
{
    int i, count = 1;
    for(int j = 0; j < G.n; j++)
        visited[j] = false;
    cout<<"深度优先序列为（递归）： ";
    int search_tree[N][N] = {0};
    for(int i = 0; i < G.n; i++)
        if(!visited[i]) DFS2(G,i,&count,search_tree);
    cout<<endl;
    cout<<"深度优先编号为（递归）： "<<endl;
    show_search(dfn);
    cout<<"深度优先生成树矩阵为："<<endl;
    for(int i = 0; i < G.n; i++)
    {
        for(int j = 0; j < G.n; j++)
            cout<<search_tree[i][j]<<" ";
        cout<<endl;
    }
}

void BFS_matrix(struct graph G)
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
            cout<<G.vertex[i]<<" ";
            bfn[i] = count++;
            visited[i] = true;
            q.push(i);
        }
        while(!q.empty())
        {
            int j = 0, f = q.front();
            q.pop();
            while((j >= 0) && (j < G.n))
            {            
                if((G.edge[f][j] != 0) && (!visited[j]))
                {
                    search_tree[j][f] = G.edge[f][j];
                    search_tree[f][j] = G.edge[f][j];
                    cout<<G.vertex[j]<<" ";
                    bfn[j] = count++;
                    visited[j] = true;
                    q.push(j);
                }
                j++;
            }
        }
    }
    cout<<endl;
    cout<<"广度优先编号为： "<<endl;
    show_search(bfn);
    cout<<"广度优先生成树矩阵为："<<endl;
    for(int i = 0; i < G.n; i++)
    {
        for(int j = 0; j < G.n; j++)
            cout<<search_tree[i][j]<<" ";
        cout<<endl;
    }
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