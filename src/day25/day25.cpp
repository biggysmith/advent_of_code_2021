#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <map>
#include <functional>
#include "timer.hpp"

struct seafloor_t{
    std::vector<char> points;
    int width, height;

    char& get(int x,int y){ return points[(y % height) * width + (x % width)]; }
    const char& get(int x,int y) const{ return points[(y % height) * width + (x % width)]; }

    void move_right(int x,int y){
        get(x,y) = '.';
        get(x+1,y) = '>';
    }

    void move_down(int x,int y){
        get(x,y) = '.';
        get(x,y+1) = 'v';
    }
};

seafloor_t load_input(const std::string& file){
    seafloor_t ret;
    std::ifstream fs(file);
    std::string line;
    ret.width = ret.height = 0;
    while(std::getline(fs, line)){
        ret.points.insert(ret.points.end(),line.begin(),line.end());
        ret.height++;
        ret.width = std::max(ret.width, (int)line.size());
    }
    return ret;
}

size_t part1(const seafloor_t& seafloor)
{
    auto floor0 = seafloor;
    auto floor1 = floor0;

    int steps = 0;
    bool still_moving = true;

    while(still_moving)
    {
        still_moving = false;

        for(int y=0; y<seafloor.height; ++y){
            for(int x=0; x<seafloor.width; ++x){

                if(floor0.get(x,y) == '>'){
                    if(floor0.get(x+1,y) == '.'){
                        floor1.move_right(x,y);
                        still_moving = true;
                    }
                }

            }
        }

        floor0 = floor1;

        for(int y=0; y<seafloor.height; ++y){
            for(int x=0; x<seafloor.width; ++x){

                if(floor0.get(x,y) == 'v'){
                    if(floor0.get(x,y+1) == '.'){
                        floor1.move_down(x,y);
                        still_moving = true;
                    }
                }

            }
        }

        floor0 = floor1;
        steps++;
    }

    return steps;
}

void main()
{
    auto test_values = load_input("../src/day25/example_input.txt");
    auto actual_values = load_input("../src/day25/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;
}
