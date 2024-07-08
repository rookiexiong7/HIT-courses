#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>
#include <cstdlib>

#include "BWT_easy_vers.h"
char extract_char(uint64_t* addr, int idx){
    int i = idx / 32;
    int j = idx % 32;
    uint64_t code = addr[i];
    code = (code << j * 2) >> 62;//get the bits we want: shift left j*2 bits, then shift right 62 bits
    switch (code) {
        case(0) : return 'A';
        case(1) : return 'C';
        case(2) : return 'G';
        case(3) : return 'T';
        default : return 'N';
    }
}
void print_BWT(int* SA, uint64_t* addr, int len){
    for(int i = 0; i < len; i++){
        if(SA[i] == 0)
            std::cout << '$';
        else
            std::cout << extract_char(addr, SA[i] - 1) << ' ';
    }
    std::cout << std::endl;
}
