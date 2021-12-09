#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <map>

struct signal_t{
    std::vector<std::string> input;
    std::vector<std::string> output;
};

std::vector<signal_t> load_input(const std::string& file){
    std::vector<signal_t> ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        ret.push_back(signal_t());
        bool input = true;
        std::string line2;
        std::istringstream ss(line);
        while (std::getline(ss, line2, ' ')) {
            if(line2 == "|"){
                input = false;
                continue;
            }

            if(input){
                ret.back().input.push_back(line2);
            }else{
                ret.back().output.push_back(line2);
            }
        }
    }
    return ret;
}


int part1(const std::vector<signal_t>& signals)
{
    std::vector<int> signal_count(9, 0);
    for(auto& signal : signals){
        for(auto& output_signal : signal.output){
            signal_count[output_signal.size()]++;
        }
    }

    return signal_count[2] + signal_count[4] + signal_count[3] + signal_count[7];
}

int part2(const std::vector<signal_t>& signals)
{
    std::vector<std::vector<int>> number_displays = {
       // a  b  c  d  e  f  g  
        { 0, 1, 2   , 4, 5, 6 }, // 0
        {       2,       5    }, // 1
        { 0,    2, 3, 4,    6 }, // 2
        { 0,    2, 3,    5, 6 }, // 3
        {    1, 2, 3,    5    }, // 4
        { 0, 1,    3,    5, 6 }, // 5
        { 0, 1,    3, 4, 5, 6 }, // 6
        { 0,    2,       5    }, // 7
        { 0, 1, 2, 3, 4, 5, 6 }, // 8
        { 0, 1, 2, 3,    5, 6 }, // 9
    };

    std::vector<int> segment_hist(7,0);
    for(auto& number_display : number_displays){
        for(auto& number : number_display){
            segment_hist[number]++;
        }
    }

    std::vector<int> number_displays_sum(10,0);
    for(int i=0; i<number_displays_sum.size(); ++i){
        for(auto& number_display : number_displays[i]){
            number_displays_sum[i] += segment_hist[number_display];
        }
    }

    int ret = 0;
    for(auto& signal : signals){
        std::vector<int> signal_displays_sum(7,0);
        for(auto& input_signal : signal.input){
            for(auto c : input_signal){
                signal_displays_sum[c-97]++;
            }
        }

        std::string output_str;
        std::map<int,int> lookup;
        for(auto& input_signal : signal.input){
            int sum = 0;
            for(auto c : input_signal){
                sum += signal_displays_sum[c-97];
            }
            auto it = std::find(number_displays_sum.begin(), number_displays_sum.end(), sum);
            lookup[sum] = (int)std::distance(number_displays_sum.begin(), it);
        }

        for(auto& output_signal : signal.output){
            int sum = 0;
            for(auto c : output_signal){
                sum += signal_displays_sum[c-97];
            }
            output_str += std::to_string(lookup[sum]);
        }

        ret += std::stoi(output_str);
    }

    return ret;
}

void main()
{
    auto test_values = load_input("../src/day08/example_input.txt");
    auto actual_values = load_input("../src/day08/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    auto test_values0 = load_input("../src/day08/example_input0.txt");

    std::cout << "part2: " << part2(test_values0) << std::endl;
    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}