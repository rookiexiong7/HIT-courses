#ifndef YOUR_HEADER_FILE_NAME_H
#define YOUR_HEADER_FILE_NAME_H

#include <vector>
#include <utility>
#include <string>

// 使用 typedef 或 using 来定义类型别名
typedef std::pair<int, int> PII;

// 在头文件中声明你的函数
extern bool flag;
extern int mark[5000][4];
extern int *BWT;
extern int C[5];
extern std::vector<PII> modified_positions;

int Occ(int c, int r);
int LF(int r);
int LFC(int r, int c);
PII EXACTMATCH(std::string Q);
PII replace_char(std::string &s, int index, int max_changes, int range_start, int range_end, int changes = 0);
PII blurred_match1(std::string Q);
std::string read_reverse(std::string read);

#endif // YOUR_HEADER_FILE_NAME_H
