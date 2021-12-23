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
    bool add_vol = true;
};

vec3_t operator-(const vec3_t& a,const vec3_t& b){ return { a.x - b.x, a.y - b.y, a.z - b.z }; }
vec3_t operator+(const vec3_t& a,const vec3_t& b){ return { a.x + b.x, a.y + b.y, a.z + b.z }; }

int64_t dot(const vec3_t& v){ return v.x * v.y * v.z; }
vec3_t minimum(const vec3_t& a,const vec3_t& b){ return { std::min(a.x,b.x), std::min(a.y,b.y), std::min(a.z,b.z) }; }
vec3_t maximum(const vec3_t& a,const vec3_t& b){ return { std::max(a.x,b.x), std::max(a.y,b.y), std::max(a.z,b.z) }; }

int64_t volume(const cuboid_t& c){
    return dot(c.mx - c.mn + vec3_t{ 1, 1, 1 });
}

bool a_inside_b(const cuboid_t& a,const cuboid_t& b){
    if (a.mn.x < b.mn.x || a.mx.x > b.mx.x) return false;
    if (a.mn.y < b.mn.y || a.mx.y > b.mx.y) return false;
    if (a.mn.z < b.mn.z || a.mx.z > b.mx.z) return false;
    return true;
}

bool a_intersects_b(const cuboid_t& a,const cuboid_t& b){
    if (a.mx.x < b.mn.x || a.mn.x > b.mx.x) return false;
    if (a.mx.y < b.mn.y || a.mn.y > b.mx.y) return false;
    if (a.mx.z < b.mn.z || a.mn.z > b.mx.z) return false;
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
    vec3_t mx = minimum(a.mx + vec3_t{1,1,1}, b.mx + vec3_t{1,1,1}) - vec3_t{1,1,1};
    bool add_vol = a.add_vol ^ b.add_vol;
    return { mn, mx, true, add_vol };
}

int64_t process(const std::vector<cuboid_t>& cubes, bool part1)
{
    std::vector<cuboid_t> intersections;
    for(auto& cube : cubes){

        if(part1 && !a_inside_b(cube, {{-50,-50,-50},{50,50,50}})){
            continue;
        }

        std::vector<cuboid_t> new_intersections;
        if(cube.on){
            new_intersections.push_back(cube);
        }

        for(auto& intersection : intersections){
            if(a_intersects_b(intersection, cube)){
                new_intersections.push_back(intersect(intersection, cube));
            }
        }

        intersections.insert(intersections.end(), new_intersections.begin(), new_intersections.end());
    }

    return std::accumulate(intersections.begin(), intersections.end(), (int64_t)0, [&](auto& a,auto& b){
        return a + (b.add_vol ? volume(b) : -volume(b)); // vol(A) + vol(B) - vol(A n B)
    });
}

void main()
{
    auto test_values = load_input("../src/day22/example_input.txt");
    auto test_values1 = load_input("../src/day22/example_input1.txt");
    auto actual_values = load_input("../src/day22/input.txt");

    std::cout << "part1: " << process(test_values, true) << std::endl;
    std::cout << "part1: " << process(actual_values, true) << std::endl;

    std::cout << "part2: " << process(test_values1, false) << std::endl;
    std::cout << "part2: " << process(actual_values, false) << std::endl;
}
