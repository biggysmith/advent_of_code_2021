#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <set>
#include <list>
#include <map>
#include <queue>

struct pos_t{
    int x,y;
};

struct density_t{
    pos_t pos;
    int risk;
};

bool operator<(const density_t& a, const density_t& b){
    return a.risk > b.risk;
}

pos_t operator+(const pos_t& a, const pos_t& b){
    return { a.x + b.x, a.y + b.y };
}

struct cavern_t{
    std::vector<int> grid;
    int width, height;

    int get(const pos_t& p) const{
        return grid[p.y*width+p.x];
    }

    int& get(const pos_t& p){
        return grid[p.y*width+p.x];
    }

    bool in_grid(const pos_t& p) const{
        return p.x >= 0 && p.x < width && p.y >= 0 && p.y < height;
    }
};

cavern_t load_input(const std::string& file){
    cavern_t ret;
    std::ifstream fs(file);
    std::string line;
    int w = 0;
    int h = 0;
    while (std::getline(fs, line)) {
        h++;
        int w2 = 0;
        for(auto c : line){
            ret.grid.push_back(c-'0');
            w2++;
        }
        w = std::max(w, w2);
    }
    ret.width = w;
    ret.height = h;
    return ret;
}

int dijkstra(const cavern_t& cavern, const pos_t& dst)
{
    std::priority_queue<density_t> q;

    cavern_t visited { std::vector<int>(cavern.width*cavern.height, 0), cavern.width, cavern.height };
    cavern_t cost { std::vector<int>(cavern.width*cavern.height, INT_MAX), cavern.width, cavern.height };

    cost.get({0,0}) = cavern.get({0,0});
    q.push({ 0, 0, cavern.get({0,0}) });

    while (!q.empty()) {
        density_t curr = q.top();
        q.pop();

        if(visited.get(curr.pos)){
            continue;
        }

        visited.get(curr.pos) = 1;

        for(auto& d : std::vector<pos_t>{ {-1,0}, {1,0}, {0,-1}, {0,1} }){
            pos_t new_pos = curr.pos + d;
            if(cavern.in_grid(new_pos) && !visited.get(new_pos)){
                cost.get(new_pos) = std::min(cost.get(new_pos), cost.get(curr.pos) + cavern.get(new_pos));
                q.push({ new_pos, cost.get(new_pos) });
            }
        }

    }

    return cost.get(dst) - cavern.get({0,0});
}

size_t part1(const cavern_t& cavern)
{
    return dijkstra(cavern, { cavern.width-1, cavern.height-1 });
}

int wrap(int num, int start, int limit){
    return start + (num - start) % (limit - start);
};

size_t part2(const cavern_t& cavern)
{
    cavern_t new_cavern { std::vector<int>(cavern.width*5*cavern.height*5, 0), cavern.width*5, cavern.height*5 };

    for(int y=0; y<new_cavern.height; ++y){
        for(int x=0; x<new_cavern.width; ++x){
            new_cavern.get({x,y}) = wrap(cavern.get({x % cavern.width,y % cavern.height}) + x/cavern.width + y/cavern.height, 1, 10);
        }
    }

    return dijkstra(new_cavern, { new_cavern.width-1, new_cavern.height-1 });
}

void main()
{
    auto test_values = load_input("../src/day15/example_input.txt");
    auto actual_values = load_input("../src/day15/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}