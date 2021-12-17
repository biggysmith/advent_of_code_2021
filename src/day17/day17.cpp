#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>

std::pair<bool,int> hits_target(int vx,int vy,int x1, int x2, int y1, int y2){
    int maxy = 0;
    int x = 0;
    int y = 0;
    while(x < x2 && y > y1){
        x += vx;
        y += vy;
        vx = std::max(0, vx-1);
        vy = vy-1;

        maxy = std::max(maxy, y);

        if(x >= x1 && x <= x2 && y <= y2 && y >= y1){
            return std::make_pair(true, maxy);  // hit!
        }
    }

    return std::make_pair(false, maxy); // miss!
}

size_t part1(int x1, int x2, int y1, int y2)
{
    int maxy = 0;
    for(int y=1; y<=x2; ++y){
        for(int x=1; x<=x2; ++x){
            auto res = hits_target(x, y, x1, x2, y1, y2);
            if(res.first){
                maxy = std::max(res.second, maxy);
            }
        }
    }

    return maxy;
}

size_t part2(int x1, int x2, int y1, int y2)
{
    int hits = 0;
    for(int y=y1; y<=x2; ++y){
        for(int x=1; x<=x2; ++x){
            auto res = hits_target(x, y, x1, x2, y1, y2);
            if(res.first){
                hits++;
            }
        }
    }

    return hits;
}

void main()
{
    std::cout << "part1: " << part1(20,30,-10,-5) << std::endl;
    std::cout << "part1: " << part1(287,309,-76,-48) << std::endl;

    std::cout << "part2: " << part2(20,30,-10,-5) << std::endl;
    std::cout << "part2: " << part2(287,309,-76,-48) << std::endl;
}