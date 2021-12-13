#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <queue>
#include <map>
#include <unordered_map>

struct point_t{
    int x,y;
};

struct grid_t{
    std::vector<int> points;
    std::vector<point_t> folds;
    int width;
    int height;
};

grid_t load_input(const std::string& file){
    grid_t ret;
    std::ifstream fs(file);
    int max_x = 0;
    int max_y = 0;
    point_t point;
    std::vector<point_t> points;
    char comma(',');
    std::string line;
    while (std::getline(fs, line)) {
        if(line == ""){
            break;
        }
        std::istringstream ss(line);
        ss >> point.x >> comma >> point.y;
        points.push_back(point);
        max_x = std::max(max_x, point.x);
        max_y = std::max(max_y, point.y);
    }

    ret.width = max_x*2;
    ret.height = max_y*2;

    ret.points.resize(ret.width*ret.height, 0);
    for(auto& p : points){
        ret.points[p.y*ret.width+p.x] = 1;
    }

    char axis('x');
    int fold_point;
    while (std::getline(fs, line)) {
        std::string str = line.substr(line.find_first_of("fold_along")+10);
        std::istringstream ss(str);
        ss >> axis >> comma >> fold_point;
        ret.folds.push_back({ axis, fold_point });
    }
    return ret;
}

int do_folding(grid_t& grid, int num_folds, bool print)
{
    int fold_x = grid.width;
    int fold_y = grid.height;

    for(int f=0; f<num_folds; ++f){
        auto& fold = grid.folds[f];
        if(fold.x == 'x'){
            for(int y=0; y<grid.height; ++y){
                for(int x=0; x<fold.y; ++x){
                    grid.points[y*grid.width+x] |= grid.points[y*grid.width+(fold.y*2-x)];
                    grid.points[y*grid.width+(fold.y*2-x)] = 0;
                }
            }
            fold_x = std::min(fold_x, fold.y);
        }else{
            for(int y=0; y<fold.y; ++y){
                for(int x=0; x<grid.width; ++x){
                    grid.points[y*grid.width+x] |= grid.points[(fold.y*2-y)*grid.width+x];
                    grid.points[(fold.y*2-y)*grid.width+x] = 0;
                }
            }
            fold_y = std::min(fold_y, fold.y);
        }
    }

    int dots = 0;
    if(print) std::cout << std::endl;
    for(int y=0; y<fold_y; ++y){
        for(int x=0; x<fold_x; ++x){
            dots += grid.points[y*grid.width+x];
            if(print) std::cout << (grid.points[y*grid.width+x] ? '#' : ' ');
        }
        if(print) std::cout << std::endl;
    }

    return dots;
}

int part1(const grid_t& grid)
{
    return do_folding(grid_t(grid), 1, false);
}

int part2(const grid_t& grid)
{
    return do_folding(grid_t(grid), (int)grid.folds.size(), true);
}

void main()
{
    auto test_values = load_input("../src/day13/example_input.txt");
    auto actual_values = load_input("../src/day13/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}