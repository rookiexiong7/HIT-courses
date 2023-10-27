#include<iostream>
#include<stdio.h>
#define N 150
using namespace std;

int main()
{
    int n , max = 2;

    for(int i = 2; i <= 100; i++)
    {
        n = i;
        int j = 0;
        int x[N]={0};//初始化数组待存入序列值；
        x[0] = i;
        if (n == 2)
        {
            printf("When n = 2 , the last answer is: 1\n");
            printf("2->1\n");
            printf("序列长度为2\n");
        }    

        while(n > 2)
        {
            j++;
            //若n为偶数
            if(n % 2 == 0)  n /= 2;

            //若n为奇数
            else  n = 3 * n + 1;

            //输出序列以及序列长度
            x[j] = n;
            if(n == 2){
                cout<<"\nWhen n = "<<i<<", ""the last answer is:"<<n/2<<endl;
                x[j+1] = n / 2;
                int k = 0;
                printf("%d",x[k]);
                for(k = 1; k <= N; k++)
                {
                    printf("->%d",x[k]);
                    if(x[k] == 1)
                        break;
                }
                cout<<"\n序列长度为"<<k+1<<endl;
                if(max < k+1)
                    max = k+1;
                break;
            }
        }

    }


cout<<"最长序列长度为"<<max<<endl;
}