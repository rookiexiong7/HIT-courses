
#ifndef EXP1_SORT_H
#define EXP1_SORT_H
#include "general_heading.h"
char extract_char(uint64_t* addr, int idx);
uint64_t extract_val(uint64_t* addr, int idx);
int compare(uint64_t* addr, int i, int j);
void my_qsort(int* SA, int i, int j );
void my_quicksort(int SA[], int left, int right, uint64_t* addr);
#endif //EXP1_SORT_H
