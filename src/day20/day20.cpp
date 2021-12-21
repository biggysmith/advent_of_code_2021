#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <set>
#include <map>
#include <functional>
#include <iterator>
#include "timer.hpp"

struct image_t{
    std::vector<char> data;
    int width, height;

    char& get(int x,int y){ return data[y*width+x]; }
    const char get(int x,int y) const{ return data[y*width+x]; }
};

struct algo_t{
    image_t img;
    image_t enhancement;
};

algo_t load_input(const std::string& file){
    algo_t ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        if(line.empty()){
            break;
        }
        ret.enhancement.data.insert(ret.enhancement.data.end(), line.begin(), line.end());
    }
    ret.enhancement.width = (int)ret.enhancement.data.size();
    ret.enhancement.height = 1;

    ret.img.width = 0;
    ret.img.height = 0;
    while (std::getline(fs, line)) {
        ret.img.data.insert(ret.img.data.end(), line.begin(), line.end());
        ret.img.height++;
        ret.img.width = std::max(ret.img.width,(int)line.size());
    }
    return ret;
}

size_t process(const algo_t& algo, int steps)
{
    scoped_timer timer;

    int buffer = steps+3;

    int new_width = algo.img.width + buffer*2;
    int new_height = algo.img.height + buffer*2;

    image_t dst0 { std::vector<char>(new_width*new_width, '.'), new_width, new_height };
    image_t dst1 { std::vector<char>(new_width*new_width, '.'), new_width, new_height };

    for(int y=buffer; y<algo.img.height+buffer; ++y){
        for(int x=buffer; x<algo.img.width+buffer; ++x){
            dst0.get(x,y) = algo.img.get(x-buffer,y-buffer);
        }
    }

    for(int i=0; i<steps; ++i){

        std::fill(dst1.data.begin(), dst1.data.end(), 'p'); // padding

        #pragma omp parallel for schedule(dynamic,1)
        for(int y=2; y<dst0.height-2; ++y){
            for(int x=2; x<dst0.width-2; ++x){

                std::string binary;
                for(int j=-1; j<2; ++j){
                    for(int i=-1; i<2; ++i){
                        char c = dst0.get(x+i,y+j);
                        if(c == 'p'){
                            continue;
                        }
                        binary += c == '#' ? "1" : "0";
                    }
                }

                int dec = std::stoi(binary, 0, 2);
                dst1.get(x,y) = algo.enhancement.data[dec];

            }
        }

        std::swap(dst0, dst1);
    }

    int sum = 0;
    for(int y=2; y<dst0.height-2; ++y){
        for(int x=2; x<dst0.width-2; ++x){
            sum += dst0.get(x,y) == '#';
        }
    }

    return sum;
}

void main()
{
    auto test_values = load_input("../src/day20/example_input.txt");
    auto actual_values = load_input("../src/day20/input.txt");

    std::cout << "part1: " << process(test_values, 2) << std::endl;
    std::cout << "part1: " << process(actual_values, 2) << std::endl;

    std::cout << "part2: " << process(test_values, 50) << std::endl;
    std::cout << "part2: " << process(actual_values, 50) << std::endl;
}
