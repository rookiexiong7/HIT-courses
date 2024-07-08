#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <omp.h>
#include <bitset>
#include <unordered_map>
#include <sstream>
const int N = 11260625;

std::unordered_map<uint64_t, int> kmer_counts;//kmer, count
//graph
std::unordered_map<uint64_t, std::vector<uint64_t>> graph;//kmer, neighbors
void printcode(uint64_t seq){
    std::bitset<64> x(seq);
    std::cout << x << std::endl;
}//print binary code, for debug
uint64_t C2B(std::string seq){//convert char to binary,to save space and compute time
    uint64_t Bseq = 0;
    uint64_t cur_c = 0;
    for(int i = 0; i < 32; i++) {
        if(seq[i] == 'A'){
            cur_c = 0;
        }
        else if(seq[i] == 'C'){
            cur_c = 1;
        }
        else if(seq[i] == 'G'){
            cur_c = 2;
        }
        else if(seq[i] == 'T'){
            cur_c = 3;
        }
        Bseq = Bseq | cur_c;
//        printcode(Bseq);
//        std::cout<<std::endl;
        if(i != 31) Bseq = Bseq << 2;
    }
//    printcode(Bseq);
    return Bseq;
}
uint64_t prefix(uint64_t kmer)//get prefix
{
    return kmer >> 2;
}

uint64_t suffix(uint64_t kmer)//get suffix
{
    return kmer<< 2 >> 2;
}
std::string get_string(uint64_t kmer){//get string from binary
    std::string seq;
    for(int i = 0; i < 32; i++){
        uint64_t cur_c = kmer >> (62 - 2*i);
        cur_c = cur_c & 3;
        if(cur_c == 0){
            seq += 'A';
        }
        else if(cur_c == 1){
            seq += 'C';
        }
        else if(cur_c == 2){
            seq += 'G';
        }
        else if(cur_c == 3){
            seq += 'T';
        }
    }
    return seq;
}

char last_ch(uint64_t kmer){//get last char
    uint64_t cur_c = kmer & 3;
    if(cur_c == 0){
        return 'A';
    }
    else if(cur_c == 1){
        return 'C';
    }
    else if(cur_c == 2){
        return 'G';
    }
    else if(cur_c == 3){
        return 'T';
    }
    return 'N';
}
std::vector<uint64_t> get_neighbors(uint64_t kmer) {
    //get neighbors
    std::vector<uint64_t> neighbors;
    uint64_t suffix_kmer = suffix(kmer);
    for (int i = 0; i < 4; ++i) {
        uint64_t neighbor = (suffix_kmer << 2) | i;
        if (kmer_counts.count(neighbor)) {
            neighbors.push_back(neighbor);
        }
    }
    return neighbors;
}

void build_de_bruijn_graph() {
    for (const auto& kmer_count : kmer_counts) {
        //kmer_count is a pair, first is kmer, second is count,this for loop is to build graph
        uint64_t kmer = kmer_count.first;
        std::vector<uint64_t> neighbors = get_neighbors(kmer);
        if (!neighbors.empty()) {
            graph[kmer] = neighbors;
        }
    }
}

void hierholzer(uint64_t current_kmer, std::unordered_map<uint64_t, std::vector<uint64_t>>& graph, std::vector<uint64_t>& path) {//hierholzer algorithm
    //this function is to find a eulerian path
    //how can find: https://www.geeksforgeeks.org/hierholzers-algorithm-directed-graph/
    //thanks a lot to the author
    while (!graph[current_kmer].empty()) {
        uint64_t next_kmer = graph[current_kmer].back();
        graph[current_kmer].pop_back();
        hierholzer(next_kmer, graph, path);
    }
    path.push_back(current_kmer);
}

std::vector<uint64_t> find_start_kmer(const std::unordered_map<uint64_t, std::vector<uint64_t>>& graph) {
    //find start kmer, which has out degree but no in degree
    std::vector<uint64_t> start_kmers;
    for (const auto& entry : graph) {
        if (entry.second.size() > 0) {
            start_kmers.push_back(entry.first);
        }
    }
    return start_kmers;
}

std::string path_to_contig(const std::vector<uint64_t>& path);
std::vector<std::vector<uint64_t>> eulerian_paths(std::unordered_map<uint64_t, std::vector<uint64_t>>& graph) {
    //find eulerian path

    std::vector<std::vector<uint64_t>> paths;
    std::vector<uint64_t> path;
    std::vector<uint64_t> start_kmers = find_start_kmer(graph);
    for (const uint64_t& start_kmer : start_kmers) {
        hierholzer(start_kmer, graph, path);
        std::reverse(path.begin(), path.end());
        if(path.size() > 1000){
            std::cout << path_to_contig(path) << std::endl;
        }
    }
    return paths;
}

std::string path_to_contig(const std::vector<uint64_t>& path) {
    std::string contig = get_string(path.front()).substr(0, 31);
    for (const uint64_t kmer : path) {
        contig += last_ch(kmer);
    }
    return contig;
}


int main() {
    std::ifstream fin("./NC_008253.se.5end.forward.27mers.txt");
    std::string line;
    int cnt_tmp = 0;
    int i = 0;
    // delim within '\t'
    while (std::getline(fin, line)) {
        std::istringstream iss(line);
        std::string part1, part2;
        std::getline(iss, part1, '\t');
        std::getline(iss, part2);
        cnt_tmp = std::stoi(part2);
        uint64_t kmer = C2B(part1);
        kmer_counts[kmer] = cnt_tmp;
        i++;
    }
    std::cout << "finish reading" << std::endl;
    std::cout << "the size is " << i << std::endl;
    std::cout << "now begin to build the graph" << std::endl;
    build_de_bruijn_graph();
//    C2B("GCCTGGGCGAATTATTTCTCCGAACACACAAC");
    std::cout << "finish building the graph" << std::endl;
    std::cout << "now begin to find the eulerian path" << std::endl;
    std::vector<std::vector<uint64_t>> paths = eulerian_paths(graph);
    std::cout << "finish finding the eulerian path" << std::endl;
    //print the contigs
    for(const auto& path : paths){
        std::cout << path_to_contig(path) << std::endl;
    }

    return 0;
}

