#ifndef EXP1_BWT_EASY_VERS_H
#define EXP1_BWT_EASY_VERS_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>
#include <cstdlib>

char extract_char(uint64_t* addr, int idx);
void print_BWT(int* SA, uint64_t* addr, int len);
#endif //EXP1_BWT_EASY_VERS_H
