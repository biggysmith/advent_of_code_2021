#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <set>
#include <list>
#include <map>

struct instructions_t{
    std::string polymer_template;
    std::map<std::string,char> instructions;
};

instructions_t load_input(const std::string& file){
    instructions_t ret;
    std::ifstream fs(file);
    std::string line;

    std::getline(fs, line);
    ret.polymer_template = line;

    std::getline(fs, line);
    while (std::getline(fs, line)) {
        ret.instructions[line.substr(0,2)] = line[6];
    }
    return ret;
}

size_t process(const instructions_t& data, int steps)
{
    std::map<std::string,size_t> combos;

    for(int i=0; i<data.polymer_template.size()-1; ++i){
        combos[data.polymer_template.substr(i,2)]++;
    }

    for(int step=0; step<steps; ++step){
        std::map<std::string,size_t> new_combos;
        for(auto& [pair, count] : combos){
            if(count){
                new_combos[pair.substr(0,1) + data.instructions.at(pair)] += count;
                new_combos[data.instructions.at(pair) + pair.substr(1,2)] += count;
            } 
        }
        combos = new_combos;
    }

    std::map<char,size_t> hist;
    for(auto& [pair, count] : combos){
        hist[pair[0]] += count;
        hist[pair[1]] += count;
    }

    auto round_up = [](size_t a,size_t b){
        return (a+b-1)/b;
    };

    auto [mn, mx] = std::minmax_element(hist.begin(), hist.end(), [] (auto& p1, auto& p2) {
        return p1.second < p2.second;
    });

    return round_up(mx->second,2) - round_up(mn->second,2);
}

size_t part1(const instructions_t& data)
{
    return process(data, 10);
}

size_t part2(const instructions_t& data)
{
    return process(data, 40);
}

void main()
{
    auto test_values = load_input("../src/day14/example_input.txt");
    auto actual_values = load_input("../src/day14/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}