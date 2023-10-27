#include<iostream>
#include<math.h>
#include<algorithm>
#include<stack>
#include<fstream>
#define N 12
#define inf 1000
using namespace std;

//Dijkstra算法
void Dijkstra(int i,int Distance[N], int Path[N], int matrix[][N]);
//Floyd算法
void Floyd(int A[][N], int P[][N], int matrix[][N]);
//单目标最短路径输出
void Dijkstra_show_path_1(int u, int v, int Distance[], int Path[]);
//单源最短路径输出
void Dijkstra_show_path(int u, int v, int Distance[], int Path[]);
//Floyd最短路径输出
void Floyd_show_path(int i, int c, int A[][N], int P[][N]);

int main()
{
    int Distance[2][N], Path[2][N];
    int C[N][N],A[N][N],P[N][N];
    int m,n;
    //初始化
    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++)
            C[i][j] = inf;

    //文件导入边信息
    ifstream infile;
    infile.open("data.txt");
    while(!infile.eof())
        infile>>m>>n>>C[m][n];
    cout<<"图存入成功！！"<<endl;

    Floyd(A, P, C);
    //Dijkstra输出源点及其到其他顶点的最短路径长度和最短路径
    int a;
    cout<<"================================="<<endl;
    cout<<"计算单源最短路径，指定源点为：";
    cin>>a;
    Dijkstra(a, Distance[0], Path[0], C);
    for(int i = 0; i < N; i++)
        if(a != i)  Dijkstra_show_path(a,i,Distance[0],Path[0]);
    
    //Floyd计算任意两个顶点间的最短距离矩阵和最短路径矩阵，并输出任意两个顶点间的最短路径长度和最短路径
    cout<<"================================="<<endl;
    cout<<"最短距离矩阵如下："<<endl;
    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++)
            {
                if(i != j && A[i][j] < inf)  cout<<A[i][j]<<"\t";
                else if(A[i][j] >= inf)  cout<<"inf"<<"\t";
                else cout<<0<<"\t";
            }
        cout<<endl;
    }
    int p,q;
    cout<<"选择任意两顶点p,q:";
    cin>>p>>q;
    Floyd_show_path(p,q,A,P);

    //Dijkstra计算单目标最短路径
    cout<<"================================="<<endl;
    int c;
    cout<<"计算单目标最短路径，指定顶点c为：";
    cin>>c;
    //矩阵转置
    int C1[N][N];
    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++)
            C1[j][i] = C[i][j];
    Dijkstra(c, Distance[0], Path[0], C1);
    for(int i = 0; i < N; i++)
        if(c != i)  Dijkstra_show_path_1(c,i,Distance[0],Path[0]);

    //Dijkstra计算单顶点对间最短路径问题
    cout<<"================================="<<endl;
    int u,v;
    cout<<"计算单顶点对间最短路径，顶点对u，v依次为：";    
    cin>>u>>v;
    Dijkstra(u, Distance[0], Path[0], C);
    Dijkstra_show_path(u,v,Distance[0],Path[0]);
    Dijkstra(v, Distance[1], Path[1], C);
    Dijkstra_show_path(v,u,Distance[1],Path[1]);
}

void Dijkstra(int i,int Distance[N], int Path[N], int matrix[][N])
{
    //数据结构初始化
    //Distance[]-源点到顶点的最短路径长度，Path[]为最短路径上最后经过的顶点，S[]存放源点和已生成的终点
    bool flag[N];
    for(int j = 0; j < N; j++){
        Distance[j] = matrix[i][j];
        Path[j] = i;
        flag[j] = false;
    }
    
    flag[i] = true;

    for(int k = 0; k < N - 1; k++){
        int min = inf;
        int w = i;
        for(int j = 0; j < N; j++){
            if(flag[j] == true) continue;
            if(Distance[j] < min)  {w = j; min = Distance[j];}
        }

        flag[w] = true;

        for(int j = 0; j < N; j++){
            if(flag[j] == true) continue;
            else{
                if(Distance[j] >= Distance[w] + matrix[w][j])
                    {Distance[j] = Distance[w] + matrix[w][j]; Path[j] = w;}
            }
        }
    }
}

void Floyd(int A[][N], int P[][N], int matrix[][N])
{
    //初始化
    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++) {
            A[i][j] = matrix[i][j];
            P[i][j] = -1;
        }
    //迭代
    for(int k = 0; k < N; k++)
        for(int i = 0; i < N; i++)
            for(int j = 0; j < N; j++) {
                if(A[i][j] > (A[i][k] + A[k][j])){
                    A[i][j] = A[i][k] + A[k][j];
                    P[i][j] = k;
                }
            }
}
void find(int i, int c,int P[][N])
{
    if(P[i][c] == -1) cout<<"->"<<c;
    else{
        find(i, P[i][c], P);
        find(P[i][c], c, P);
    }
}
void Floyd_show_path(int i, int c, int A[][N], int P[][N])
{
    if(A[i][c] < inf){
        cout<<"从第"<<i<<"个顶点到顶点"<<c<<"的最短路径为：";
        cout<<i;
        find(i,c,P);
        cout<<",最短距离为"<<A[i][c]<<endl;
    }
    else cout<<"从第"<<i<<"个顶点到顶点"<<c<<"不连通！"<<endl;
}

void Dijkstra_show_path(int u, int v, int Distance[], int Path[])
{
    if(Distance[v] < inf){
        cout<<"从第"<<u<<"个顶点到顶点"<<v<<"的最短路径为：";
        cout<<u<<"->";
        int y = v; 
        stack<int> p;
        while(Path[y] != u){
            y = Path[y];
            p.push(y);
        }
        while(!p.empty())
            {cout<<p.top()<<"->";p.pop();}
        cout<<v<<",最短距离为"<<Distance[v]<<endl;
    }
    else cout<<"从第"<<u<<"个顶点到顶点"<<v<<"不连通！"<<endl;
}

void Dijkstra_show_path_1(int u, int v, int Distance[], int Path[])
{
    if(Distance[v] < inf){
        cout<<"从第"<<v<<"个顶点到顶点"<<u<<"的最短路径为：";
        cout<<v<<"->";
        int y = v; 
        stack<int> p;
        while(Path[y] != u){
            y = Path[y];
            cout<<y<<"->";
        }
        cout<<u<<",最短距离为"<<Distance[v]<<endl;
    }
    else cout<<"从第"<<v<<"个顶点到顶点"<<u<<"不连通！"<<endl;
}