#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>

std::vector<size_t> load_input(const std::string& file){
    std::vector<size_t> ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line, ',')) {
        ret.push_back(std::stoi(line));
    }
    return ret;
}

size_t solve(const std::vector<size_t>& ages, int days){
    std::vector<size_t> hist(9, 0);
    for(auto age : ages){
        hist[age]++;
    }

    for(int d=0; d<days; ++d){
        std::rotate(hist.begin(), hist.begin()+1, hist.end()); // moves the fishes down 1 day
        hist[6] += hist[8]; // 0s rotate around and add new 8s, and reset to 6s themselves so increment 6s by 8s too
    }

    return std::accumulate(hist.begin(), hist.end(), 0ULL);
}

void main()
{
    auto test_values = load_input("../src/day06/example_input.txt");
    auto actual_values = load_input("../src/day06/input.txt");

    std::cout << "part1: " << solve(test_values, 18) << std::endl;
    std::cout << "part1: " << solve(test_values, 80) << std::endl;

    std::cout << "part2: " << solve(test_values, 256) << std::endl;
    std::cout << "part2: " << solve(actual_values, 256) << std::endl;
}