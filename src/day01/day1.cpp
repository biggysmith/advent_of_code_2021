#include <vector>
#include <iostream>
#include <fstream>
#include <string>

std::vector<int> load_test_input(){
    return std::vector<int>{
        199,
        200,
        208,
        210,
        200,
        207,
        240,
        269,
        260,
        263
    };
}

std::vector<int> load_input(const std::string& file){
    std::vector<int> ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        ret.push_back(std::stoi(line));
    }
    return ret;
}

int part1(const std::vector<int>& values){
    int count = 0;
    for (int i = 1; i < values.size(); ++i) {
        if (values[i] > values[i-1]) {
            count++;
        }
    }
    return count;
}

int part2(const std::vector<int>& values){
    int prev_sum = INT_MAX;
    int count = 0;
    for (int i = 2; i < values.size(); ++i) {
        int sum3 = values[i-2] + values[i-1] + values[i];
        if (sum3 > prev_sum) {
            count++;
        }
        prev_sum = sum3;
    }
    return count;
}

void main()
{
    auto test_values = load_test_input();
    auto actual_values = load_input("../src/day01/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}