#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <map>
#include <functional>
#include <iterator>

struct vec3_t{
    int64_t x,y,z;
};

struct cuboid_t{
    vec3_t mn;
    vec3_t mx;
    bool on;
    int sign = 1;
};

vec3_t operator-(const vec3_t& a,const vec3_t& b){ return { a.x - b.x, a.y - b.y, a.z - b.z }; }
vec3_t operator+(const vec3_t& a,const vec3_t& b){ return { a.x + b.x, a.y + b.y, a.z + b.z }; }

int64_t dot(const vec3_t& v){ return v.x * v.y * v.z; }
vec3_t minimum(const vec3_t& a,const vec3_t& b){ return { std::min(a.x,b.x), std::min(a.y,b.y), std::min(a.z,b.z) }; }
vec3_t maximum(const vec3_t& a,const vec3_t& b){ return { std::max(a.x,b.x), std::max(a.y,b.y), std::max(a.z,b.z) }; }

int64_t volume(const cuboid_t& c){
    return c.sign * dot(c.mx - c.mn + vec3_t{ 1, 1, 1 });
}

bool inside(const cuboid_t& a,const cuboid_t& b){
    if (a.mn.x < b.mn.x || a.mx.x > b.mx.x) return false;
    if (a.mn.y < b.mn.y || a.mx.y > b.mx.y) return false;
    if (a.mn.z < b.mn.z || a.mx.z > b.mx.z) return false;
    return true;
}

std::vector<cuboid_t> load_input(const std::string& file){
    std::vector<cuboid_t> ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        cuboid_t cuboid;
        std::string on;
        char s;
        std::stringstream ss(line);
        ss >> on >> s >> s >> cuboid.mn.x >> s >> s >> cuboid.mx.x >> s >> s >> s >> cuboid.mn.y >> s >> s >> cuboid.mx.y >> s >> s >> s >> cuboid.mn.z >> s >> s >> cuboid.mx.z;
        cuboid.on = on == "on";
        ret.push_back(cuboid);

    }
    return ret;
}

cuboid_t intersect(const cuboid_t& a, const cuboid_t& b) {
    vec3_t mn = maximum(a.mn, b.mn);
    vec3_t mx = maximum(minimum(a.mx, b.mx), mn-vec3_t{1,1,1});
    return { mn, mx, true, -1 * a.sign * b.sign };
}

int64_t process(const std::vector<cuboid_t>& cubes, bool limit)
{
    int64_t on_count = 0;

    std::vector<cuboid_t> intersections;
    for(auto& cube : cubes){

        if(limit && !inside(cube, {{-50,-50,-50},{50,50,50}})){
            continue;
        }

        int size = (int)intersections.size();
        for(int i=0; i<size; ++i){
            cuboid_t inter = intersect(intersections[i], cube);
            auto v = volume(inter);
            if(v != 0){
                on_count += v;
                intersections.push_back(inter);
            }
        }

        if(cube.on){
            auto v = volume(cube);
            on_count += v;
            intersections.push_back(cube);
        }
    }

    return on_count;
}


void main()
{
    auto test_values = load_input("../src/day22/example_input.txt");
    auto test_values1 = load_input("../src/day22/example_input1.txt");
    auto actual_values = load_input("../src/day22/input.txt");

    auto p1 = process(test_values, true);
    auto p2 = process(actual_values, true);
    auto p3 = process(test_values1, false);
    auto p4 = process(actual_values, false);

    if(p1 != 590784) std::cout << "error" << std::endl;
    if(p2 != 580098) std::cout << "error" << std::endl;
    if(p3 != 2758514936282235) std::cout << "error" << std::endl;
    if(p4 != 1134725012490723) std::cout << "error" << std::endl;

    std::cout << "part1: " << p1 << std::endl;
    std::cout << "part1: " << p2 << std::endl;

    std::cout << "part2: " << p3 << std::endl;
    std::cout << "part2: " << p4 << std::endl;
}
