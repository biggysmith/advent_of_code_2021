#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>

std::vector<int> load_input(const std::string& file){
    std::vector<int> ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line, ',')) {
        ret.push_back(std::stoi(line));
    }
    return ret;
}

int part1(const std::vector<int>& crabs)
{
    int min_sum = INT_MAX;
    for(int i=0; i<crabs.size(); ++i)
    {
        int sum = 0;
        for(int j=0; j<crabs.size(); ++j){
            sum += std::abs(crabs[j]-i);
        }

        min_sum = std::min(min_sum, sum);
    }

    return min_sum;
}

int part2(const std::vector<int>& crabs)
{
    auto series = [](int n){
        return (n*(n+1))/2;
    };

    int min_sum = INT_MAX;
    for(int i=0; i<crabs.size(); ++i)
    {
        int sum = 0;
        for(int j=0; j<crabs.size(); ++j){
            sum += series(std::abs(crabs[j]-i));
        }

        min_sum = std::min(min_sum, sum);
    }

    return min_sum;
}

void main()
{
    auto test_values = load_input("../src/day07/example_input.txt");
    auto actual_values = load_input("../src/day07/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}