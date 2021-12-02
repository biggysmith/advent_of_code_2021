#include <vector>
#include <iostream>
#include <fstream>
#include <string>

struct command_t{
    std::string dir;
    int units;
};

std::vector<command_t> load_test_input(){
    return std::vector<command_t>{
        { "forward", 5 },
        { "down", 5 },
        { "forward", 8 },
        { "up", 3 },
        { "down", 8 },
        { "forward", 2 }
    };
}

std::vector<command_t> load_input(const std::string& file){
    std::vector<command_t> ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        size_t space = line.find_first_of(' ');
        ret.push_back({ line.substr(0,space), std::stoi(line.substr(space+1,1)) } );
    }
    return ret;
}

int part1(const std::vector<command_t>& commands){
    int x = 0;
    int y = 0;
    for(auto& command : commands){
        if(command.dir == "forward"){
            x += command.units;
        }else if(command.dir == "down"){
            y += command.units;
        }else if(command.dir == "up"){
            y -= command.units;
        }
    }
    return x*y;
}


int part2(const std::vector<command_t>& commands){
    int x = 0;
    int y = 0;
    int aim = 0;
    for(auto& command : commands){
        if(command.dir == "forward"){
            x += command.units;
            y += aim * command.units;
        }else if(command.dir == "down"){
            aim += command.units;
        }else if(command.dir == "up"){
            aim -= command.units;
        }
    }
    return x*y;
}

void main()
{
    auto test_values = load_test_input();
    auto actual_values = load_input("../src/day02/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}