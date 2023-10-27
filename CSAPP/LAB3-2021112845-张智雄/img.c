#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#define WIDTH 1920
#define HEIGHT 1080
#define TESE_NUMBER 100
#define PTHREAD_NUMBER 2
typedef struct bound{
	int l_bound;
	int r_bound;
}pth; 
long img[WIDTH][HEIGHT];
int min(int a, int b)   {return a>b?b:a;}
int max(int a, int b)   {return a>b?a:b;}
void img_smooth1();
void img_smooth2();
void img_smooth3();
void img_smooth4();
void img_smooth5();
void img_smooth6();
void img_smooth7();
void img_smooth8();
void *smooth(void *pt);
void test(void (*func)()) {
	clock_t start = clock();
	int t;
    for(t = 0; t < TESE_NUMBER; t++)
        func();
	clock_t end = clock();
	printf("测试时间: %ld(ms)\n",(end - start) * 1000 / CLOCKS_PER_SEC);
}

int main()
{
	for(int i = 0; i <  WIDTH ; i++)
		for(int j = 0; j < HEIGHT; j++)
			{
				img[i][j] = rand();
			}
    printf("未优化 ");test(img_smooth1);
    printf("一般有用的优化 ");test(img_smooth2);
    printf("循环展开 ");test(img_smooth3);
    printf("循环展开+重组 ");test(img_smooth4);
    printf("循环展开+重组+子表达式 ");test(img_smooth5);
    printf("面向Cache分块 ");test(img_smooth6);
    printf("多进程优化 ");fflush(stdout);test(img_smooth7);
    printf("多线程优化 ");fflush(stdout);test(img_smooth8);
}

void img_smooth1()   //程序未优化版本--未考虑边缘点处理 
{
	int i, j, m, n;
	long temp[2][HEIGHT];               //记录原始信息
	for(n = 0; n < HEIGHT ; n++)
		temp[1][n] = img[0][n];
	for(i = 1; i < WIDTH - 1; i++)
	{
        for(n = 0; n < HEIGHT ; n++)
        {
            temp[0][n] = temp[1][n];
            temp[1][n] = img[i][n];
        }

		for(j = 1; j < HEIGHT - 1; j++)
        {
            //平滑算法
			img[i][j] = (temp[1][j - 1] + img[i][j + 1] + temp[0][j] + img[i + 1][j]) / 4; 
            //printf("%d,%d %d\n ",i+1,j+1,img[i][j]);
        }
	}
}
 
void img_smooth2()   //一般有用的优化 
{ 
	int i, j, m, n;
	long temp[2][HEIGHT];
	for(n = 0; n < HEIGHT ; n++)
		temp[1][n] = img[0][n];
	for(i = 1; i < WIDTH - 1; i++)
	{
        for(n = 0; n < HEIGHT ; n++)
        {
            temp[0][n] = temp[1][n];
            temp[1][n] = img[i][n];
        }

		for(j = 1; j < HEIGHT - 1; j++)
        {
            //用逻辑右移取代除法
			img[i][j] = (temp[1][j - 1] + img[i][j + 1] + temp[0][j] + img[i + 1][j]) >> 2;
            //printf("%d,%d %d\n ",i+1,j+1,img[i][j]);
        }
	}
}

void img_smooth3()   //面向CPU的优化 循环展开
{
	int i, j, k, m, n;
	long temp[2][HEIGHT];
	for(n = 0; n < HEIGHT ; n++)
		temp[1][n] = img[0][n];
	for(i = 1; i < WIDTH - 1; i++)
	{
        for(n = 0; n < HEIGHT ; n++)
        {
            temp[0][n] = temp[1][n];
            temp[1][n] = img[i][n];
        }

		for(j = 1; j < HEIGHT - 1; j += 4)
        {
            //循环展开
			img[i][j]     = (temp[1][j - 1] + img[i][j + 1] + temp[0][j]     + img[i + 1][j])     >> 2;
            img[i][j + 1] = (temp[1][j]     + img[i][j + 2] + temp[0][j + 1] + img[i + 1][j + 1]) >> 2;
            img[i][j + 2] = (temp[1][j + 1] + img[i][j + 3] + temp[0][j + 2] + img[i + 1][j + 2]) >> 2;
            img[i][j + 3] = (temp[1][j + 2] + img[i][j + 4] + temp[0][j + 3] + img[i + 1][j + 3]) >> 2;
            //printf("%d,%d %d\n ",i+1,j+1,img[i][j]);
        }
	}
}

void img_smooth4()   //面向CPU的优化 循环展开+重组
{ 
	int i, j, k, m, n;
	long temp[2][HEIGHT];
	for(n = 0; n < HEIGHT ; n++)
		temp[1][n] = img[0][n];
	for(i = 1; i < WIDTH - 1; i++)
	{
        for(n = 0; n < HEIGHT ; n++)
        {
            temp[0][n] = temp[1][n];
            temp[1][n] = img[i][n];
        }
        //重组
		for(j = 1; j < HEIGHT - 1; j += 8)
        {
            //循环展开
			img[i][j]     = (temp[1][j - 1] + img[i][j + 1] + temp[0][j]     + img[i + 1][j])     >> 2;
            img[i][j + 2] = (temp[1][j + 1] + img[i][j + 3] + temp[0][j + 2] + img[i + 1][j + 2]) >> 2;
            img[i][j + 4] = (temp[1][j + 3] + img[i][j + 5] + temp[0][j + 4] + img[i + 1][j + 4]) >> 2;
            img[i][j + 6] = (temp[1][j + 5] + img[i][j + 7] + temp[0][j + 6] + img[i + 1][j + 6]) >> 2;
            //printf("%d,%d %d\n ",i+1,j+1,img[i][j]);
        }
        for(k = 2; k < HEIGHT - 1; k += 8)
        {
            //循环展开
			img[i][j]     = (temp[1][j - 1] + img[i][j + 1] + temp[0][j]     + img[i + 1][j])     >> 2;
            img[i][j + 2] = (temp[1][j + 1] + img[i][j + 3] + temp[0][j + 2] + img[i + 1][j + 2]) >> 2;
            img[i][j + 4] = (temp[1][j + 3] + img[i][j + 5] + temp[0][j + 4] + img[i + 1][j + 4]) >> 2;
            img[i][j + 6] = (temp[1][j + 5] + img[i][j + 7] + temp[0][j + 6] + img[i + 1][j + 6]) >> 2;
            //printf("%d,%d %d\n ",i+1,j+1,img[i][j]);
        }
	}
}

void img_smooth5()   //面向CPU的优化 循环展开+重组+共享公用子表达式
{ 
	int i, j, k, m, n;
	long temp[2][HEIGHT];
	for(n = 0; n < HEIGHT ; n++)
		temp[1][n] = img[0][n];
	for(i = 1; i < WIDTH - 1; i++)
	{
        for(n = 0; n < HEIGHT ; n++)  {temp[0][n] = temp[1][n];temp[1][n] = img[i][n];}
        int z = i + 1; //公用子表达式
        //重组
		for(j = 1; j < HEIGHT - 1; j += 8)
        {
            //公用子表达式
            int x1 = j + 1, x2 = j + 2, x3 = j + 3, x4 = j + 4, x5 = j + 5, x6 = j + 6; 
            //循环展开
			img[i][j]  = (temp[1][j - 1] + img[i][x1]    + temp[0][j]     + img[z][j])  >> 2;
            img[i][x2] = (temp[1][x1]    + img[i][x3]    + temp[0][x2]    + img[z][x2]) >> 2;
            img[i][x4] = (temp[1][x3]    + img[i][x5]    + temp[0][x4]    + img[z][x4]) >> 2;
            img[i][x6] = (temp[1][x5]    + img[i][j + 7] + temp[0][x6]    + img[z][x6]) >> 2;
        }
        for(k = 2; k < HEIGHT - 1; k += 8)
        {
            //公用子表达式
            int y1 = k + 1, y2 = k + 2, y3 = k + 3, y4 = k + 4, y5 = k + 5, y6 = k + 6; 
            //循环展开
			img[i][k]  = (temp[1][k - 1] + img[i][y1]    + temp[0][k]     + img[z][k])  >> 2;
            img[i][y2] = (temp[1][y1]    + img[i][y3]    + temp[0][y2]    + img[z][y2]) >> 2;
            img[i][y4] = (temp[1][y3]    + img[i][y5]    + temp[0][y4]    + img[z][y4]) >> 2;
            img[i][y6] = (temp[1][y5]    + img[i][k + 7] + temp[0][y6]    + img[z][y6]) >> 2;
        }
	}
}

void img_smooth6()   //面向cache的优化
{
	int i0, j0, i, j, m, n;
    int T_w = 4, T_h = 4;              //分块信息
	long temp[(T_w+1)*(T_h+1)];               //记录原始信息
	for(i0 = 1; i0 < WIDTH - 1; i0 += T_w)
	{
		for(j0 = 1; j0 < HEIGHT - 1; j0 += T_h)
        {
            //以一个5*5的一维数组记录原始信息（左多一列，上多一行，不考虑边缘处理）
            for(i = i0 - 1; i < min(i0 + T_w , WIDTH); i++) 
            {
                int x = i+1-i0;
				for(j = j0 - 1; j < min(j0 + T_h , HEIGHT); j++) 
                {
                    int y = j+1-j0;
                    temp[x*(T_h+1)+y] = img[i][j];
                }
            }
            //4*4分块计算
            for(i = i0; i < min(i0 + T_w, WIDTH - 1); i++) 
            {
                int x = i+1-i0;
				for(j = j0; j < min(j0 + T_h, HEIGHT - 1); j++) 
                {
                    //平滑算法
                    int y = j+1-j0;
			        img[i][j] = (temp[x*(T_h+1)+y-1] + img[i][j + 1] + temp[(x-1)*(T_h+1)+y] + img[i + 1][j]) / 4; 
                    //printf("左%d,右%d,上%d,下%d %d\n ",temp[(i+1-i0)*(T_w+1)+j-j0],img[i][j + 1],temp[(i-i0)*(T_w+1)+j],img[i + 1][j],img[i][j]);
                }
            }
        }
	}
}

void img_smooth7()   //多进程优化 
{
	int i, j, m, n;
	long temp[2][HEIGHT];               //记录原始信息
    int pid = fork();
    if(pid == 0)    //子进程处理矩阵的上半部分
    {
        for(n = 0; n < HEIGHT ; n++)
            temp[1][n] = img[0][n];
        for(i = 1; i < WIDTH / 2; i++)
        {
            for(n = 0; n < HEIGHT ; n++)
            {
                temp[0][n] = temp[1][n];
                temp[1][n] = img[i][n];
            }

            for(j = 1; j < HEIGHT - 1; j++)
            {
                //平滑算法
                img[i][j] = (temp[1][j - 1] + img[i][j + 1] + temp[0][j] + img[i + 1][j]) / 4; 
                //printf("%d,%d %d\n ",i+1,j+1,img[i][j]);
            }
        }
        exit(0);
    }

    else            //父进程处理矩阵的下半部分
    {
        for(n = 0; n < HEIGHT ; n++)
            temp[1][n] = img[WIDTH / 2 - 1][n];
        for(i = WIDTH / 2; i < WIDTH - 1; i++)
        {
            for(n = 0; n < HEIGHT ; n++)
            {
                temp[0][n] = temp[1][n];
                temp[1][n] = img[i][n];
            }

            for(j = 1; j < HEIGHT - 1; j++)
            {
                //平滑算法
                img[i][j] = (temp[1][j - 1] + img[i][j + 1] + temp[0][j] + img[i + 1][j]) / 4; 
                //printf("%d,%d %d\n ",i+1,j+1,img[i][j]);
            }
        }
    }
}

void img_smooth8()   //多线程优化 
{
    int i;
    pthread_t tid[PTHREAD_NUMBER];
    pthread_attr_t attr[PTHREAD_NUMBER];
    struct bound pth[PTHREAD_NUMBER];
    for(i = 0; i < PTHREAD_NUMBER; i++) {
        pth[i].l_bound = max(1, i*(WIDTH/PTHREAD_NUMBER)); 
        pth[i].r_bound = min(WIDTH - 1, (i+1)*(WIDTH/PTHREAD_NUMBER)); 
        pthread_attr_init(&attr[i]);
	pthread_create(&tid[i], &attr[i], smooth, (void*)&pth[i]);
	
    }
    //回收线程
    for(i = 0; i < PTHREAD_NUMBER; i++) {
		pthread_join(tid[i], NULL);
	}
}
void *smooth(void *pt)
{
    struct bound *p = (struct bound*)pt;
    int x = p->l_bound, y = p->r_bound;
	int i, j, m, n;
	long temp[2][HEIGHT];               //记录原始信息
    for(n = 0; n < HEIGHT ; n++)
        temp[1][n] = img[x-1][n];
    for(i = x; i < y; i++){
        for(n = 0; n < HEIGHT ; n++){
            temp[0][n] = temp[1][n];
            temp[1][n] = img[i][n];
        }
        for(j = 1; j < HEIGHT - 1; j++){
            //平滑算法
            img[i][j] = (temp[1][j - 1] + img[i][j + 1] + temp[0][j] + img[i + 1][j]) / 4; 
            //printf("%d,%d %d\n ",i+1,j+1,img[i][j]);
        }
    }
}