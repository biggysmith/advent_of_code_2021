#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <set>

using set_t = std::set<std::pair<int,int>>;

struct grid_t{
    set_t points;
    std::vector<std::pair<char,int>> folds;
    int width;
    int height;
};

grid_t load_input(const std::string& file){
    grid_t ret;
    std::ifstream fs(file);
    int max_x = 0;
    int max_y = 0;
    int x, y;
    char comma(',');
    std::string line;
    while (std::getline(fs, line)) {
        if(line == ""){
            break;
        }
        std::istringstream ss(line);
        ss >> x >> comma >> y;
        ret.points.insert({ x, y });
        max_x = std::max(max_x,x);
        max_y = std::max(max_y,y);
    }
    ret.width = max_x*2;
    ret.height = max_y*2;

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
        if(fold.first == 'x'){
            for(auto point = grid.points.begin(); point != grid.points.end(); ) { // funky set erasure; erasing whilst iterating requires care
                if(point->first > fold.second){
                    grid.points.insert({ (fold.second*2 - point->first), point->second });
                    point = grid.points.erase(point);
                }else{
                    point++;
                }
            }
            fold_x = std::min(fold_x, fold.second);
        }else{
            for(auto point = grid.points.begin(); point != grid.points.end(); ) {
                if(point->second > fold.second){
                    grid.points.insert({ point->first, (fold.second*2 - point->second) });
                    point = grid.points.erase(point);
                }else{
                    point++;
                }
            }
            fold_y = std::min(fold_y, fold.second);
        }
    }

    if(print) {
        std::vector<char> dots(fold_x*fold_y, ' ');
        for(auto& point : grid.points){
            dots[point.second * fold_x + point.first] = '#';
        }

        std::cout << std::endl;
        for(int y=0; y<fold_y; ++y){
            for(int x=0; x<fold_x; ++x){
                std::cout << dots[y*fold_x+x];
            }
            std::cout << std::endl;
        }
    }

    return (int)grid.points.size();
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