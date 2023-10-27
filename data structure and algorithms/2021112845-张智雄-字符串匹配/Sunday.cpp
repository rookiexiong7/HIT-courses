#include <iostream>
#include <string>

using namespace std;

const int maxNum = 1005;

//储存移动步数
int shift[maxNum];

int Sunday(const string T, const string P) {
    int n = T.length();
    int m = P.length();

    // 模式串长度+1
    for(int i = 0; i < maxNum; i++) 
        shift[i] = m + 1;
    // 模式串长度 - 该字符最右出现的位置(以0开始) = 模式串中该字符最右出现的位置到尾部的距离 + 1
    for(int i = 0; i < m; i++) 
        shift[P[i]] = m - i;
    // 模式串窗口在主串的起始位置
    int s = 0;
    // 模式串已经匹配到的位置
    int j;
    while(s <= n - m) {
        j = 0;
        while(T[s + j] == P[j]) {
            j++;
            // 字符串全部匹配，返回当前模式串的起始位置
            if(j >= m) 
                return s;
        }
        //移动窗口位置
        s += shift[T[s + m]];
    }
    return -1;
}

int main() {
    string T, P;
    cout << "文本串为：" << endl;
    getline(cin, T);
    cout << "模式串为：" << endl;
    getline(cin, P);
    int res = Sunday(T, P);
    if(res == -1) 
        cout << "文本串和模式串不匹配。" << endl;
    else 
        cout << "模式串在文本串的起始位置为：" << res + 1 << endl;
    return 0;
}