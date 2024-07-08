#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>
#include <cstdlib>
#include "LFindex.h"
int* BWT = NULL;
int C[5] = {0, 0, 0, 0, 0};
using namespace std;
int num[4] = {0, 0, 0, 0};
int mark[5000][4];
/* i: || 0 -- A||1 -- C||2 -- G|| 3--T || */
/* C[i]: the index just 1 less than the start position, that is to say C[0] = 0, C[1] = 1 + num of A and so forth */
int main() {
    ifstream BWT_file;
    char cc;
    BWT_file.open("F:\\bioinfo\\exp2\\BWT.txt");
    int cnt = 0;
    //count the number of each character
    while(BWT_file >> cc){
        cnt++;
        if(cc == '$'){
            continue;
        }else{
            if(cc == 'A') num[0]++;
            else if(cc == 'C') num[1]++;
            else if(cc == 'G') num[2]++;
            else if(cc == 'T') num[3]++;
        }
        if(cnt % 1000 == 0){
            mark[cnt / 1000][0] = num[0];
            mark[cnt / 1000][1] = num[1];
            mark[cnt / 1000][2] = num[2];
            mark[cnt / 1000][3] = num[3];
        }

    }
    BWT_file.close();
    //calculate the start position of each character
    for(int i = 1; i <= 4; i++){
        C[i] = C[i - 1] + num[i - 1];
    }
    //read the BWT file again and store the BWT in the array BWT
    BWT_file.open("./NC_008253.se.5end.forwatd.fq");
    if(BWT_file.is_open())
        cout << "BWT file is open" << endl;
    else{
        cout << "BWT file is not open" << endl;
        return 1;
    }

    BWT = new int[cnt];
    int i = 0;
    while(BWT_file >> cc){
        if(cc == 'A') BWT[i] = 0;
        else if(cc == 'C') BWT[i] = 1;
        else if(cc == 'G') BWT[i] = 2;
        else if(cc == 'T') BWT[i] = 3;
        else BWT[i] = 4;
        i++;
    }
    BWT_file.close();
    //degub:print 1000 characters of BWT
//    for(int i = 0; i < 1000; i++){
//        cout << BWT[i] << " ";
//    }
    //test the correctness of exact match
    ifstream Q_file;
    Q_file.open("./NC_008253.se.5end.forwatd.fq");
    PII res = {0, 0};
    string Q ;
    // test();
//    while(getline(Q_file, Q)){
//        if(Q[0] == 'A'||Q[0] == 'C'||Q[0] == 'G'||Q[0] == 'T'){
            res = EXACTMATCH("AGCTTTTCATTCTGACTGCAACGGGCAATATGTCTCTGTGTGGATTAAAAAAAGAGTGTCTGATAGCAGC");
            cout << res.first << " " << res.second << endl;
//        }
//    }

    return 0;
}
