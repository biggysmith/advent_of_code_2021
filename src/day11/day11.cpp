#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>

struct grid_t{
    int width;
    int height;
    std::vector<int> points;
};

grid_t load_input(const std::string& file){
    grid_t ret;
    std::ifstream fs(file);
    std::string line;
    int w = 0;
    int h = 0;
    while (std::getline(fs, line)) {
        w = std::max(w, (int)line.size());
        h++;
        for(auto c : line){
            ret.points.push_back(c-'0');
        }
    }
    ret.width = w;
    ret.height = h;
    return ret;
}

int step(grid_t& grid, bool find_simultaneous_flash)
{
    int num_points = grid.width * grid.height;

    for(int i=0; i<num_points; ++i){
        grid.points[i]++;
    }

    std::vector<int> will_flash(num_points, 0);
    std::vector<int> has_flashed(num_points, 0);

    do
    {
        std::fill(will_flash.begin(), will_flash.end(), 0);

        for(int i=0; i<num_points; ++i){
            if(!has_flashed[i] && grid.points[i] > 9){
                will_flash[i] = 1;
            };
        }

        for(int y=0; y<grid.height; ++y){
            for(int x=0; x<grid.width; ++x){
                if(will_flash[y*grid.width+x]){
                    for(int y2=std::max(0,y-1); y2<=std::min(grid.height-1,y+1); ++y2){
                        for(int x2=std::max(0,x-1); x2<=std::min(grid.width-1,x+1); ++x2){
                            grid.points[y2*grid.width+x2]++;
                        }
                    }
                    has_flashed[y*grid.width+x] = 1;
                };
            }
        }

    } while (std::any_of(will_flash.begin(), will_flash.end(), [](int wf){ return wf == 1; }));

    for(int i=0; i<num_points; ++i){
        if(grid.points[i] > 9){
            grid.points[i] = 0;
        };
    }

    if(find_simultaneous_flash){
        return std::accumulate(grid.points.begin(), grid.points.end(), 0) == 0;
    }else{
        return std::accumulate(has_flashed.begin(), has_flashed.end(), 0);
    }
}


int part1(const grid_t& in_grid)
{
    grid_t grid = in_grid;

    int sum = 0;
    for(int s=0; s<100; ++s)
    {
        sum += step(grid, false);
    }

    return sum;
}

int part2(const grid_t& in_grid)
{
    grid_t grid = in_grid;

    for(int s=0; ; ++s)
    {
        if(step(grid, true)){
            return s+1;   
        };
    }

    return 0;
}

void main()
{
    auto test_values = load_input("../src/day11/example_input.txt");
    auto actual_values = load_input("../src/day11/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}