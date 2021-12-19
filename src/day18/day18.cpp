#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include "timer.hpp"
#include <omp.h>

using list_t = std::vector<int>;

list_t string_to_int(const std::string& line){
    list_t ret;
    for(auto c : line){
        if(c == ','){
            ret.push_back(-3);
        }else if(c == '['){
            ret.push_back(-2);
        }else if(c == ']'){
            ret.push_back(-1);
        }else{
            ret.push_back(c-'0');
        }
    }
    return ret;
}

std::vector<list_t> load_input(const std::string& file){
    std::vector<list_t> ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        ret.push_back(string_to_int(line));
    }
    return ret;
}

list_t add(const list_t& a,const list_t& b){
    list_t ret(a.size()+b.size()+3);
    ret[0] = -2;
    ret[1+a.size()] = -3;
    ret[1+a.size()+1+b.size()] = -1;
    std::copy(a.begin(), a.end(), ret.begin()+1);
    std::copy(b.begin(), b.end(), ret.begin()+1+a.size()+1);
    return ret;
}

void print(const list_t& a){
    for(auto i : a){
        if(i == -3){
            std::cout << ',';
        }else if(i == -2){
            std::cout << '[';
        }else if(i == -1){
            std::cout << ']';
        }else{
            std::cout << i;
        }
    }
    std::cout << std::endl;
}

bool explode(list_t& str)
{
    int depth = 1;
    for(int i=1; i<str.size()-1; ++i){
        if(str[i] == -2){
            depth++;
        }

        if(str[i] == -1){
            depth--;
        }

        if(str[i-1] >= 0 && str[i]==-3 && str[i+1] >= 0 && depth >= 5){
            int lv = str[i-1];
            int rv = str[i+1];

            for(int j=i-2; j>=0; --j){
                if(str[j] >= 0){
                    str[j] += lv;
                    break;
                }
            }
            for(int j=i+2; j<str.size(); ++j){
                if(str[j] >= 0){
                    str[j] += rv;
                    break;
                }
            }

            str.erase(str.begin()+i-2, str.begin()+i+3);
            str.insert(str.begin()+i-2, { 0 });

            return true;
        }
    }
    return false;
}

bool split(list_t& str)
{
    for(int i=0; i<str.size(); ++i){
        if(str[i] >= 10){
            str.insert(str.begin()+i+1, { -2, (int)floor(str[i]*0.5f), -3, (int)ceil(str[i]*0.5f), -1 });
            str.erase(str.begin()+i, str.begin()+i+1);
            return true;
        }
    }
    return false;
}

int magnitude(const list_t& str_in, int& pos)
{
    auto str = str_in;

    while(true){
        for(int i=0; i<str.size()-2; ++i){
            if(str[i] >=0 && str[i+1] == -3 && str[i+2] >= 0){
                int mag = 3*str[i] + 2*str[i+2];
                str.erase(str.begin()+i-1, str.begin()+i+3+1);
                if(str.empty()){
                    return mag;
                }
                str.insert(str.begin()+i-1, { mag });
            }
        }
    }

    return -1;
}

int magnitude(const list_t& str){
    int pos = 0;
    return magnitude(str, pos);
}

void reduce(list_t& str){
    while(true){
        if(!explode(str)){
            if(!split(str)){
                break;
            }
        }
    };
}

size_t part1(const std::vector<list_t>& numbers)
{
    std::vector<int> a = numbers[0];
    for(int i=1; i<numbers.size(); ++i){
        a = add(a,numbers[i]);
        reduce(a);
    }

    return magnitude(a);
}

size_t part2(const std::vector<list_t>& numbers)
{
    std::vector<int> hishests(omp_get_max_threads(), 0);

    #pragma omp parallel for
    for(int y=0; y<numbers.size(); ++y){
        for(int x=y+1; x<numbers.size(); ++x){
            auto a = add(numbers[x],numbers[y]);
            reduce(a);
            hishests[omp_get_thread_num()] = std::max(hishests[omp_get_thread_num()], magnitude(a));

            auto b = add(numbers[y],numbers[x]);
            reduce(b);
            hishests[omp_get_thread_num()] = std::max(hishests[omp_get_thread_num()], magnitude(b));
        }
    }

    return std::accumulate(hishests.begin(), hishests.end(), 0, [](auto& a,auto& b){
        return std::max(a,b);  
    });
}

void main()
{
    auto test_values = load_input("../src/day18/example_input.txt");
    auto actual_values = load_input("../src/day18/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}


