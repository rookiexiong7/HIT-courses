#include <iostream> 
#include <string>
#include <math.h>
using namespace std; 
  
// d是输入字母表中的字符数  
#define d 256  

int search(string txt, string pat, int q)  
{  
    int M = pat.length();  
    int N = txt.length();  
    int i, j;  
    int p = 0; // pattern哈希值  
    int t = 0; // txt哈希值  
    int h = ((int)pow(d, M-1)) % q;  
  
    // 计算模式串和文本串的第一个窗口的哈希值  
    for (i = 0; i < M; i++)  
    {  
        p = (d * p + pat[i]) % q;  
        t = (d * t + txt[i]) % q;  
    }  
  
    for (i = 0; i <= N - M; i++)  
    {  
        // 如果哈希值相等，则逐个检查每个字符是否匹配
        if (p == t)  
        {  
            for (j = 0; j < M; j++)  
            {  
                if (txt[i+j] != pat[j])  
                    break;  
            }  
            // 字符串全部匹配，返回当前模式串的起始位置
            if (j == M)  
                return i;
        }  
        // 计算文本串下一个窗口的哈希值
        if (i < N-M)  
        {  
            t = (d*(t - txt[i]*h) + txt[i+M])%q;  
            //负值处理
            if (t < 0)  
                t = (t + q);  
        }  
    } 
    return -1;
}  
  
int main()  
{  
    string T, P;
    cout << "文本串为：" << endl;
    getline(cin, T);
    cout << "模式串为：" << endl;
    getline(cin, P);
    int q = 101;  
    int res = search(T, P, q);  
    if(res == -1) 
        cout << "文本串和模式串不匹配。" << endl;
    else 
        cout << "模式串在文本串的起始位置为：" << res + 1 << endl;
    return 0;
}  