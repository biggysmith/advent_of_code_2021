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

    int get(int x,int y) const{
        if(x < 0 || x > width-1 || y < 0 || y > height-1){
            return INT_MAX;
        }
        return points[y*width+x];
    }

    void set(int x,int y,int val){
        points[y*width+x] = val;
    };
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

int lowest(const grid_t& grid,int x,int y){
    int h = grid.get(x,y);
    return h < grid.get(x+1,y) && h < grid.get(x-1,y) && h < grid.get(x,y+1) && h < grid.get(x,y-1);
};

int part1(const grid_t& grid)
{
    std::vector<int> low_points;
    for(int y=0; y<grid.height; ++y){
        for(int x=0; x<grid.width; ++x){
            if(lowest(grid,x,y)){
                low_points.push_back(grid.get(x,y));
            }
        }
    }

    int sum = 0;
    for(auto h : low_points){
        sum += h+1;
    }

    return sum;
}

struct low_point_t{
    int x,y;
    int height;
};

bool search(const grid_t& grid, grid_t& seen, int& basin, int sx, int sy){
    int h = grid.get(sx, sy);
    if(h == 9 || seen.get(sx, sy)){
        return false;
    }

    basin++;
    seen.set(sx,sy,1);

    for(int y=sy+1; y<grid.height; ++y){
        if(!search(grid, seen, basin, sx, y)){
            break;   
        };
    }

    for(int y=sy-1; y>=0; --y){
        if(!search(grid, seen, basin, sx, y)){
            break;   
        };
    }

    for(int x=sx+1; x<grid.width; ++x){
        if(!search(grid, seen, basin, x, sy)){
            break;   
        };
    }

    for(int x=sx-1; x>=0; --x){
        if(!search(grid, seen, basin, x, sy)){
            break;   
        };
    }

    return true;
}

int part2(const grid_t& grid)
{
    std::vector<low_point_t> low_points;
    for(int y=0; y<grid.height; ++y){
        for(int x=0; x<grid.width; ++x){
            if(lowest(grid,x,y)){
                low_points.push_back({x,y,grid.get(x,y)});
            }
        }
    }

    grid_t seen { grid.width, grid.height, std::vector<int>(grid.width*grid.height, 0) };

    std::vector<int> basins(low_points.size());
    for(int b=0; b<basins.size(); ++b){
        search(grid, seen, basins[b], low_points[b].x, low_points[b].y);
    }

    std::partial_sort(basins.begin(), basins.begin()+3, basins.end(), std::greater<int>());

    return std::accumulate(basins.begin(), basins.begin()+3, 1, std::multiplies<int>());
}

void main()
{
    auto test_values = load_input("../src/day09/example_input.txt");
    auto actual_values = load_input("../src/day09/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}