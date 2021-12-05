#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

struct line_t{
    int x1,y1,x2,y2;
};

struct grid_t{
    grid_t(int w,int h) : points(w*h,0), width(w), height(h) {}
    std::vector<int> points;
    int width, height;
};

std::vector<line_t> load_input(const std::string& file){
    std::vector<line_t> ret;
    std::ifstream fs(file);
    std::string line;
    char comma = ',';
    std::string arrow(" -> ");
    while (std::getline(fs, line)) {
        std::istringstream iss(line);
        line_t line_data;
        iss >> line_data.x1 >> comma >> line_data.y1 >> arrow >> line_data.x2 >> comma >> line_data.y2;
        ret.push_back(line_data);
    }
    return ret;
}

int solve(const std::vector<line_t>& lines, bool use_diag){
    int max_x = 0;
    int max_y = 0;
    for(auto& line : lines){
        max_x = std::max(line.x1, std::max(line.x2, max_x));
        max_y = std::max(line.y1, std::max(line.y2, max_y));
    }

    grid_t grid(max_x+1,max_y+1);

    for(auto& line : lines){
        int xdiff = line.x2 - line.x1;
        int ydiff = line.y2 - line.y1;

        if(xdiff && ydiff){ // diagonal
            if(use_diag){
                int xdir = xdiff > 0 ? 1 : -1;
                int ydir = ydiff > 0 ? 1 : -1;
                for(int x=line.x1, y=line.y1; x!=(line.x2+xdir); x+=xdir, y+=ydir){
                    grid.points[y*grid.width+x] += 1;
                }
            }else{
                continue;
            }
        }else if(xdiff){ // horizontal
            int dir = xdiff > 0 ? 1 : -1;
            for(int x=line.x1; x!=(line.x2+dir); x+=dir){
                grid.points[line.y1*grid.width+x] += 1;
            }
        }else{ // vertical
            int dir = ydiff > 0 ? 1 : -1;
            for(int y=line.y1; y!=(line.y2+dir); y+=dir){
                grid.points[y*grid.width+line.x1] += 1;
            }
        }
    }

    return (int)std::count_if(grid.points.begin(), grid.points.end(), [](auto c){
        return c > 1;  
    });
}

void main()
{
    auto test_values = load_input("../src/day05/example_input.txt");
    auto actual_values = load_input("../src/day05/input.txt");

    std::cout << "part1: " << solve(test_values, false) << std::endl;
    std::cout << "part1: " << solve(actual_values, false) << std::endl;

    std::cout << "part2: " << solve(test_values, true) << std::endl;
    std::cout << "part2: " << solve(actual_values, true) << std::endl;
}