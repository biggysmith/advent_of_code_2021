#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

std::vector<std::string> load_test_input(){
    return std::vector<std::string>{
        "00100",
        "11110",
        "10110",
        "10111",
        "10101",
        "01111",
        "00111",
        "11100",
        "10000",
        "11001",
        "00010",
        "01010"
    };
}

std::vector<std::string> load_input(const std::string& file){
    std::vector<std::string> ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        ret.push_back(line);
    }
    return ret;
}

int part1(const std::vector<std::string>& readings, int n){
    std::string gamma;
    std::string epsilon;
    for(int i=0; i<n; ++i){
        int ones = 0;
        int zeros = 0;
        for(auto reading : readings){
            if(reading[i] == '0'){
                zeros++;
            }else{
                ones++;
            }
        }
        if(ones > zeros){
            gamma.append("1");
            epsilon.append("0");
        }else{
            gamma.append("0");
            epsilon.append("1");
        }
    }
    return std::stoi(gamma, nullptr, 2) * std::stoi(epsilon, nullptr, 2);
}

std::string filter(const std::vector<std::string>& src, int bitpos, bool is_oxy){
    if(src.size() == 1){
        return src.front();
    }

    std::vector<std::string> zeros, ones;
    for(auto& reading : src){
        if(reading[bitpos] == '1'){
            ones.push_back(reading);
        }else{
            zeros.push_back(reading);
        }
    }

    return filter(is_oxy ^ (ones.size() >= zeros.size()) ? ones : zeros, bitpos+1, is_oxy);
};

int part2(const std::vector<std::string>& readings){
   auto oxy = filter(readings, 0, true);
   auto c02 = filter(readings, 0, false);
   return std::stoi(oxy, nullptr, 2) * std::stoi(c02, nullptr, 2);
}

void main()
{
    auto test_values = load_test_input();
    auto actual_values = load_input("../src/day03/input.txt");

    std::cout << "part1: " << part1(test_values, 5) << std::endl;
    std::cout << "part1: " << part1(actual_values, 12) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}