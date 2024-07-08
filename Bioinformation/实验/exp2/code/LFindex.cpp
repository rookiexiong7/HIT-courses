#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>
#include <cstdlib>
#include <algorithm>
#include <utility>

using namespace std;

bool flag = false;
typedef std::pair<int, int> PII;
extern int mark[5000][4];
extern int *BWT;//the original declaration is in main.cpp(extern int* BWT;)
extern int C[5];  //the original declaration is in main.cpp(extern int C[5] = {0, 0, 0, 0,0};)
int Occ(int c, int r){
    //naive way to implement Occ, need to be improved
    int count = 0;
//    for(int i = 0; i < r - 1; i++){
//        if(BWT[i] == c){
//            count++;
//        }
//    }
//    return count;
    //improve the efficiency by using the checkpoints
    int index = r / 1000;
    count = mark[index][c];
    for(int i = index * 1000 + 1; i < r - 1; i++){
        if(BWT[i] == c){
            count++;
        }
    }
    return count;
}

int LF(int r){
    int c = BWT[r];
    return C[c] + Occ(c, r) + 1;
}

int LFC(int r, int c){
    return C[c] + Occ(c, r) + 1;
}

PII EXACTMATCH(string Q){
    int q = Q.length();
    int* QN = new int [q];
    for(int i = 0; i < q; i++){
        if(Q[i] == 'A') QN[i] = 0;
        else if(Q[i] == 'C') QN[i] = 1;
        else if(Q[i] == 'G') QN[i] = 2;
        else if(Q[i] == 'T') QN[i] = 3;
    }
    int c = QN[q - 1];
    int sp = C[c] + 1;
    int ep = C[c + 1];
    int i = q - 2;
    while(sp <= ep && i >= 0){

        c = QN[i];
        sp = LFC(sp, c);
        ep = LFC(ep, c);
        if(sp == ep && i != 0){
            delete [] QN;
            flag = false;
            return{i, i} ;

        }
        i--;
    }

    delete [] QN;
    flag = true;
    return {sp, ep};
}
const char bases[] = {'A', 'C', 'G', 'T'};

vector<PII> modified_positions;

PII replace_char(string &s, int index, int max_changes, int range_start, int range_end, int changes = 0) {
    if (changes == max_changes) {
        return {-1, -1};
    }
    PII res = EXACTMATCH(s);
    if (res.first != -1 && res.second != -1) {
        return res;
    }
    char bases[] = {'A', 'C', 'G', 'T'};
    for (int i = range_start; i <= range_end; i++) {
        for (int j = 0; j < 4; j++) {
            if (s[i] != bases[j]) {
                char tmp = s[i];
                s[i] = bases[j];
                modified_positions.push_back({i, j});
                PII result = replace_char(s, index + 1, max_changes, range_start, range_end, changes + 1);
                if (result.first != -1 && result.second != -1) {
                    return result;
                }
                s[i] = tmp;
                modified_positions.pop_back();
            }
        }
    }
    return {-1, -1};
}

PII blurred_match1(string Q) {
    int a, b, c = 0;
    int aj, bj, cj = 0;
    char CH[4] = {'A', 'C', 'G', 'T'};
    int n = Q.length();
    vector<PII > wrong_pos;
    PII res = {0, 0};
    res = EXACTMATCH(Q);
    if (flag == true) {
        return res;
    }
    int ed = res.second;
    if (!flag) {
        int i = 0;
//        while(!flag && i >= ed){
//            for(int j = 0; j < 4; j++){
//                if(Q[i] != CH[j]){
//                    Q[i] = CH[j];
//                    res = EXACTMATCH(Q);
//                    if(flag){
//                        wrong_pos.emplace_back(i, j);
//                        for(auto &k : wrong_pos){
//                            cout << k.first << " " << k.second << endl;
//                        }
//                        return res;
//                    }
//                }
//            }
//            i--;
//        }
        int mid = n << 2;
        /*******************************************/
        /*******************************************/
        /*Case 1: the left part has up to 3 errors, while the right part has no errors*/
        /*******************************************/
        /*******************************************/
        replace_char(Q, 0, 3, 0, n - 1);

        /*******************************************/
        /*******************************************/
        /*Case 2: the left part has more than 1 errors and up to 3 errors while the right part has no errors*/
        /*******************************************/
        /*******************************************/
        replace_char(Q, 0, 3, 0, n - 2);

        /*******************************************/
        /*******************************************/
        /*Case 3: the left part has more than 1 errors and up to 2 errors while the right part has exactly one error*/
        /*******************************************/
        /*******************************************/
        replace_char(Q, 0, 2, 0, n - 2);

        /*******************************************/
        /*******************************************/
        /*Case 4: the left part has exactly 2 errors while the right part has exactly one error*/
        /*******************************************/
        /*******************************************/
        replace_char(Q, 0, 2, 0, n - 3);

    }
}
string read_reverse(string read) {
    //reverse the read
    reverse(read.begin(), read.end());
    for(int i = 0; i < read.length(); i++) {
        switch (read[i]) {
            case 'A':
                read[i] = 'T';
                break;
            case 'C':
                read[i] = 'G';
                break;
            case 'G':
                read[i] = 'C';
                break;
            case 'T':
                read[i] = 'A';
                break;
            default:
                cout << "只有AGCT被支持!" << endl;
        }
    }
    return read;
}
