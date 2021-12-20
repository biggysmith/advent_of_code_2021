#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <set>
#include <map>
#include <optional>
#include <functional>
#include <iterator>

struct vec3{
    int x,y,z;
};

struct matrix33{
    vec3 r0,r1,r2;
};

int sin_dir[] = { 0, 1, 0, -1 };
int cos_dir[] = { 1, 0, -1, 0 };

template <int axis> 
matrix33 rotation_axis(const int v){
    int s = sin_dir[v];
    int c = cos_dir[v];
    if constexpr(axis == 0){
        return {{1,0,0}, {0,c,-s}, {0,s,c}};
    }else if constexpr(axis == 1){
        return {{c,0,s}, {0,1,0}, {-s,0,c}};
    }else{
        return {{c,-s,0}, {s,c,0}, {0,0,1}};
    }
}

int dot(const vec3& a,const vec3& b){ return a.x*b.x + a.y*b.y + a.z*b.z; }

vec3 operator*(const matrix33& a,const vec3& b) { return { dot(a.r0,b), dot(a.r1,b), dot(a.r2,b) }; }

vec3 operator-(const vec3& a,const vec3& b) { return { a.x-b.x, a.y-b.y, a.z-b.z }; }

vec3 operator+(const vec3& a,const vec3& b) { return { a.x+b.x, a.y+b.y, a.z+b.z }; }

bool operator==(const vec3& a,const vec3& b) { return a.x==b.x && a.y==b.y && a.z==b.z; }

bool operator==(const matrix33& a,const matrix33& b) { return a.r0==b.r0 && a.r1==b.r1 && a.r2==b.r2; }

bool operator<(const vec3& a,const vec3& b) { return std::make_tuple(a.x,a.y,a.z) < std::make_tuple(b.x,b.y,b.z); }

matrix33 operator*(const matrix33& a,const matrix33& b) {
    return {
        { dot(a.r0,{ b.r0.x, b.r1.x, b.r2.x }), dot(a.r0,{ b.r0.y, b.r1.y, b.r2.y }), dot(a.r0,{ b.r0.z, b.r1.z, b.r2.z }) },
        { dot(a.r1,{ b.r0.x, b.r1.x, b.r2.x }), dot(a.r1,{ b.r0.y, b.r1.y, b.r2.y }), dot(a.r1,{ b.r0.z, b.r1.z, b.r2.z }) },
        { dot(a.r2,{ b.r0.x, b.r1.x, b.r2.x }), dot(a.r2,{ b.r0.y, b.r1.y, b.r2.y }), dot(a.r2,{ b.r0.z, b.r1.z, b.r2.z }) }
    };
}

matrix33 rotation_matrix(int rx,int ry,int rz){
    return rotation_axis<2>(rz) * rotation_axis<1>(ry) * rotation_axis<0>(rx);
}

using set_t = std::set<vec3>;

std::vector<set_t> load_input(const std::string& file){
    std::vector<set_t> ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        if(line.find("---") != std::string::npos){
            ret.push_back(set_t());
            continue;
        }
        if(line.empty()){
            continue;
        }
        char comma = ',';
        int x,y,z;
        std::stringstream ss(line);
        ss >> x >> comma >> y >> comma >> z;
        ret.back().insert({ x, y, z });
    }
    return ret;
}

auto overlapping_points(const std::vector<matrix33>& rotations, const set_t& beacons1, const set_t& beacons2)
{
    for(auto& rot : rotations)
    {
        set_t rotated_beacons2;
        for(auto& beacon : beacons2){
            rotated_beacons2.insert(rot * beacon);
        }

        for(auto& b0 : beacons1){
            for(auto& b1 : rotated_beacons2){
                vec3 dist = b0 - b1;

                set_t shifted_beacons2;
                for(auto& b2 : rotated_beacons2){
                    shifted_beacons2.insert(b2 + dist);
                }

                set_t inter;
                std::set_intersection(beacons1.begin(), beacons1.end(), shifted_beacons2.begin(), shifted_beacons2.end(), std::inserter(inter,inter.end()));

                if(inter.size() >= 12){
                    return std::make_pair(shifted_beacons2, dist);
                }

            }
        }
    }

    return std::make_pair(set_t(),vec3());
}

int manhatten(const vec3& a,const vec3& b){
    return std::abs(a.x-b.x) + std::abs(a.y-b.y) + std::abs(a.z-b.z);
}

auto process(const std::vector<set_t>& scanners)
{
    std::vector<matrix33> rotations = { 
        rotation_matrix(0,0,0), rotation_matrix(1,0,0), rotation_matrix(2,0,0), rotation_matrix(3,0,0), 
        rotation_matrix(0,1,0), rotation_matrix(1,1,0), rotation_matrix(2,1,0), rotation_matrix(3,1,0),
        rotation_matrix(0,2,0), rotation_matrix(1,2,0), rotation_matrix(2,2,0), rotation_matrix(3,2,0),
        rotation_matrix(0,3,0), rotation_matrix(1,3,0), rotation_matrix(2,3,0), rotation_matrix(3,3,0),
        rotation_matrix(0,0,1), rotation_matrix(1,0,1), rotation_matrix(2,0,1), rotation_matrix(3,0,1),
        rotation_matrix(0,0,3), rotation_matrix(1,0,3), rotation_matrix(2,0,3), rotation_matrix(3,0,3),
    };

    std::map<int,set_t> found;
    found[0] = scanners[0];

    std::vector<vec3> dists;
    dists.push_back({0,0,0});

    uint64_t done = 0;
    uint64_t found_mask = 1;

    while(dists.size() != scanners.size())
    {
        for(int i=0; i<scanners.size(); ++i){
            if((done & (1ull << i)) || !(found_mask & (1ull << i))){
                continue;
            }
        
            for(int j=0; j<scanners.size(); ++j){
                if((found_mask & (1ull << j)) || i == j){
                    continue;
                }

                auto points = overlapping_points(rotations, found[i], scanners[j]);

                if(!points.first.empty()){
                    found_mask |= 1ull << j;
                    found[j] = points.first;
                    dists.push_back(points.second);
                }
            }

            done |= 1ull << i;
        }
    }

    set_t points;
    for(auto& f : found){
        points.insert(f.second.begin(), f.second.end());
    }

    int max_dist = 0;
    for(auto& d0 : dists){
        for(auto& d1 : dists){
            max_dist = std::max(max_dist, manhatten(d0,d1));
        }
    }

    return std::make_pair(points.size(), max_dist);
}

void main()
{
    auto test_values = load_input("../src/day19/example_input.txt");
    auto actual_values = load_input("../src/day19/input.txt");

    auto test_result = process(test_values); 
    auto result = process(actual_values); 

    std::cout << "part1: " << test_result.first << std::endl;
    std::cout << "part1: " << result.first << std::endl;

    std::cout << "part2: " << test_result.second << std::endl;
    std::cout << "part2: " << result.second << std::endl;
}
