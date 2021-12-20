#if 0
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

bool operator==(const vec3& a,const vec3& b){
    return a.x==b.x && a.y==b.y && a.z==b.z;
}

int cos2(int x){
	if (x == 90)
		return 0;
	if (x == 180)
		return -1;
	if (x == 270)
		return 0;
	return 1;
}

int sin2(int x){
	if (x == 90)
		return 1;
	if (x == 180)
		return 0;
	if (x == 270)
		return -1;
	return 0;
}

vec3 rotate3d_z(vec3 v, int theta){
	int c = cos2(theta);
    int s = sin2(theta);
    return { v.x*c - v.y*s, v.x*s+v.y*c, v.z };
}

vec3 rotate3d_x(vec3 v, int theta){
	int c = cos2(theta);
    int s = sin2(theta);
    return {v.x, v.y*c - v.z*s, v.y*s + v.z*c};
}

vec3 rotate3d_y(vec3 v, int theta){
	int c = cos2(theta);
    int s = sin2(theta);
    return {v.x*c + v.z*s, v.y, -v.x*s + v.z*c};
}

std::function<vec3(vec3)> apply(int rx, int ry, int rz){
	auto f = [=](vec3 v){
		v = rotate3d_x(v, rx);
		v = rotate3d_y(v, ry);
		return rotate3d_z(v, rz);
    };
	return f;
}

std::vector<std::function<vec3(vec3)>> rotations = {
	apply(  0,   0,   0),
	apply( 90,   0,   0),
	apply(180,   0,   0),
	apply(270,   0,   0),
	apply(  0,  90,   0),
	apply( 90,  90,   0),
	apply(180,  90,   0),
	apply(270,  90,   0),
	apply(  0, 180,   0),
	apply( 90, 180,   0),
	apply(180, 180,   0),
	apply(270, 180,   0),
	apply(  0, 270,   0),
	apply( 90, 270,   0),
	apply(180, 270,   0),
	apply(270, 270,   0),
	apply(  0,   0,  90),
	apply( 90,   0,  90),
	apply(180,   0,  90),
	apply(270,   0,  90),
	apply(  0,   0, 270),
	apply( 90,   0, 270),
	apply(180,   0, 270),
	apply(270,   0, 270),
};




std::vector<std::vector<vec3>> load_input(const std::string& file){
    std::vector<std::vector<vec3>> ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        if(line.find("---") != std::string::npos){
            ret.push_back(std::vector<vec3>());
            continue;
        }
        if(line.empty()){
            continue;
        }
        char comma = ',';
        int x,y,z;
        std::stringstream ss(line);
        ss >> x >> comma >> y >> comma >> z;
        ret.back().push_back({ x, y, z });
    }
    return ret;
}

vec3 basis_transform(const vec3& v, const std::vector<vec3>& basis){
    return { basis[0].x*v.x+basis[0].y*v.y+basis[0].z*v.z, basis[1].x*v.x+basis[1].y*v.y+basis[1].z*v.z, basis[2].x*v.x+basis[2].y*v.y+basis[2].z*v.z };
}

vec3 diff(const vec3& a, const vec3& b){
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

vec3 add(const vec3& a, const vec3& b){
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

std::vector<std::vector<vec3>> bases;

std::optional<std::pair<std::vector<vec3>,vec3>> common_points(const std::vector<vec3>& s1,const std::vector<vec3>& s2)
{
    for(auto& basis : bases){
		//new_s2 = tuple(basis_transform(vec, basis) for vec in s2)
        std::vector<vec3> new_s2(s2.size());
        for(int i=0; i<new_s2.size(); ++i){
            new_s2[i] = basis_transform(s2[i], basis);
        }

        for(int i=0; i<s1.size(); ++i){
            for(int j=0; j<new_s2.size(); ++j){
                vec3 dist = diff(s1[i], new_s2[j]);

                std::vector<vec3> translated_s2(new_s2.size());
                for(int k=0; k<translated_s2.size(); ++k){
                    translated_s2[i] = add(new_s2[k], dist);
                }

                std::vector<vec3> inter;
                for(auto& a : s1){
                    for(auto& b : translated_s2){
                        if(a == b){
                            inter.push_back(a);
                        }
                    }
                }

                if(inter.size() >= 12){
                    return std::make_pair(translated_s2,dist);
                }
            }
        }
    }

    return std::nullopt;
}

int part1(const std::vector<std::vector<vec3>>& scanners)
{
    std::vector<vec3> identity{
        {1,0,0},
        {0,1,0},
        {0,0,1}
    };

    //std::vector<std::vector<vec3>> bases;
    for(auto& f : rotations){
        bases.push_back(std::vector<vec3>{ f(identity[0]), f(identity[1]), f(identity[1]) });
    }

    std::map<int,std::vector<vec3>> matched;
    matched[0] = scanners[0];
    std::vector<vec3> distances = {{0,0,0}};
    std::set<int> done;

    while(true)
    {
	    for(int i=0; i<scanners.size(); ++i){
		    if (done.count(i) || !matched.count(i)){
			    continue;
            }

            for(int j=0; j<scanners.size(); ++j){
		        if (matched.count(j) || i == j){
			        continue;
                }

			    auto cp = common_points(matched[i], scanners[j]);

			    if(cp){
				    matched[j] = cp->first;
				    distances.push_back(cp->second);
                }
            }

		    done.insert(i);
        }

	    if (done.size() == scanners.size()){
		    break;
        }
    };

    return 0;
}

int part2(const std::vector<std::vector<vec3>>& numbers)
{
    return 0;
}

void main()
{
    auto test_values = load_input("../src/day19/example_input.txt");
    auto actual_values = load_input("../src/day19/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    //std::cout << "part1: " << part1(actual_values) << std::endl;

    /*std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;*/
}
#endif



#if 1
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

int cos2(int x){
	if (x == 90)
		return 0;
	if (x == 180)
		return -1;
	if (x == 270)
		return 0;
	return 1;
}

int sin2(int x){
	if (x == 90)
		return 1;
	if (x == 180)
		return 0;
	if (x == 270)
		return -1;
	return 0;
}

template <int axis> 
matrix33 rotation_axis(const int v){
    int s = sin2(v);
    int c = cos2(v);
    if(axis == 0){
        vec3 r0{1,0,0};
        vec3 r1{0,c,-s};
        vec3 r2{0,s,c};
        return matrix33{r0,r1,r2};
    }else if(axis == 1){
        vec3 r0{c,0,s};
        vec3 r1{0,1,0};
        vec3 r2{-s,0,c};
        return matrix33{r0,r1,r2};
    }else{
        vec3 r0{c,-s,0};
        vec3 r1{s,c,0};
        vec3 r2{0,0,1};
        return matrix33{r0,r1,r2};
    }
}

vec3 rotate3d_z(vec3 v, int theta){
	int c = cos2(theta);
    int s = sin2(theta);
    return { v.x*c - v.y*s, v.x*s+v.y*c, v.z };
}

vec3 rotate3d_x(vec3 v, int theta){
	int c = cos2(theta);
    int s = sin2(theta);
    return {v.x, v.y*c - v.z*s, v.y*s + v.z*c};
}

vec3 rotate3d_y(vec3 v, int theta){
	int c = cos2(theta);
    int s = sin2(theta);
    return {v.x*c + v.z*s, v.y, -v.x*s + v.z*c};
}

std::function<vec3(vec3)> apply(int rx, int ry, int rz){
	auto f = [=](vec3 v){
		v = rotate3d_x(v, rx);
		v = rotate3d_y(v, ry);
		return rotate3d_z(v, rz);
    };
	return f;
}

int dot(const vec3& a,const vec3& b){ return a.x*b.x + a.y*b.y + a.z*b.z; }

matrix33 operator*(const matrix33& a,const matrix33& b) {
    return matrix33{
        vec3{ dot(a.r0,{ b.r0.x, b.r1.x, b.r2.x }), dot(a.r0,{ b.r0.y, b.r1.y, b.r2.y }), dot(a.r0,{ b.r0.z, b.r1.z, b.r2.z }) },
        vec3{ dot(a.r1,{ b.r0.x, b.r1.x, b.r2.x }), dot(a.r1,{ b.r0.y, b.r1.y, b.r2.y }), dot(a.r1,{ b.r0.z, b.r1.z, b.r2.z }) },
        vec3{ dot(a.r2,{ b.r0.x, b.r1.x, b.r2.x }), dot(a.r2,{ b.r0.y, b.r1.y, b.r2.y }), dot(a.r2,{ b.r0.z, b.r1.z, b.r2.z }) }
    };
}

vec3 operator*(const matrix33& a,const vec3& b) {
    return { dot(a.r0,b), dot(a.r1,b), dot(a.r2,b) };
}

vec3 operator-(const vec3& a,const vec3& b) {
    return { a.x-b.x, a.y-b.y, a.z-b.z };
}

vec3 operator+(const vec3& a,const vec3& b) {
    return { a.x+b.x, a.y+b.y, a.z+b.z };
}

bool operator==(const vec3& a,const vec3& b) {
    return a.x==b.x && a.y==b.y && a.z==b.z;
}

bool operator==(const matrix33& a,const matrix33& b) {
    return a.r0==b.r0 && a.r1==b.r1 && a.r2==b.r2;
}

bool operator<(const vec3& a,const vec3& b) {
    return std::make_tuple(a.x,a.y,a.z) < std::make_tuple(b.x,b.y,b.z);
}

matrix33 rotation_matrix(int rx,int ry,int rz){
    return rotation_axis<2>(rz) * rotation_axis<1>(ry) * rotation_axis<0>(rx);
}

std::ostream& operator<<(std::ostream& os, const vec3& v){
    os << v.x << ", " << v.y << ", " << v.z;
    return os;
}

std::ostream& operator<<(std::ostream& os, const matrix33& v){
    os << v.r0 << std::endl;
    os << v.r1 << std::endl;
    os << v.r2;
    return os;
}

std::vector<std::function<vec3(vec3)>> rotations = {
	apply(  0,   0,   0),
	apply( 90,   0,   0),
	apply(180,   0,   0),
	apply(270,   0,   0),
	apply(  0,  90,   0),
	apply( 90,  90,   0),
	apply(180,  90,   0),
	apply(270,  90,   0),
	apply(  0, 180,   0),
	apply( 90, 180,   0),
	apply(180, 180,   0),
	apply(270, 180,   0),
	apply(  0, 270,   0),
	apply( 90, 270,   0),
	apply(180, 270,   0),
	apply(270, 270,   0),
	apply(  0,   0,  90),
	apply( 90,   0,  90),
	apply(180,   0,  90),
	apply(270,   0,  90),
	apply(  0,   0, 270),
	apply( 90,   0, 270),
	apply(180,   0, 270),
	apply(270,   0, 270),
};

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

std::vector<matrix33> bases;



std::optional<std::pair<set_t,vec3>> common_points(const set_t& s1,const set_t& s2)
{
    for(auto& rot : bases){
        set_t new_s2;
        for(auto& v : s2){
            new_s2.insert(rot * v);
        }

        for(auto& v0 : s1){
            for(auto& v1 : new_s2){
                vec3 dist = v0 - v1;

                set_t translated_s2;
                for(auto& v : new_s2){
                    translated_s2.insert(v + dist);
                }

                set_t inter;
                std::set_intersection(s1.begin(), s1.end(), translated_s2.begin(), translated_s2.end(), std::inserter(inter,inter.end()));

                if(inter.size() >= 12){
                    return std::make_pair(translated_s2,dist);
                }

            }
        }
    }

    return std::nullopt;
}

int manhatten(const vec3& a,const vec3& b){
    return std::abs(a.x-b.x) + std::abs(a.y-b.y) + std::abs(a.z-b.z);
}

auto process(const std::vector<set_t>& scanners)
{
    matrix33 identity{
        {1,0,0},
        {0,1,0},
        {0,0,1}
    };
    for(auto& f : rotations){
        bases.push_back(matrix33{ f(identity.r0), f(identity.r1), f(identity.r2) });
    }

    std::map<int,set_t> matched;
    matched[0] = scanners[0];

    std::vector<vec3> distances;
    distances.push_back({0,0,0});
    std::set<int> done;

    while(true)
    {
        for(int i=0; i<scanners.size(); ++i){
            if(done.count(i) || !matched.count(i)){
                continue;
            }
        
            for(int j=0; j<scanners.size(); ++j){
                if(matched.count(j) || i == j){
                    continue;
                }

                auto cp = common_points(matched[i], scanners[j]);

                if(cp){
                    matched[j] = cp->first;
                    //matched.insert(matched.end(), cp->first.begin(), cp->first.end());
                    distances.push_back(cp->second);
                }
            }
        
            done.insert(i);
        }

        if(done.size() == scanners.size()){
            break;
        }
    
    }


    set_t points;
    for(auto& match : matched){
        points.insert(match.second.begin(), match.second.end());
    }

    int max_dist = 0;
    for(auto& v0 : distances){
        for(auto& v1 : distances){
            max_dist =  std::max(max_dist, manhatten(v0,v1));
        }
    }

    return std::make_pair(points.size(), max_dist);
}

void main()
{
    auto test_values = load_input("../src/day19/example_input.txt");
    auto actual_values = load_input("../src/day19/input.txt");

    auto test_result = process(test_values); 
    std::cout << "part1: " << test_result.first << std::endl;
    std::cout << "part1: " << test_result.second << std::endl;

    auto result = process(actual_values); 
    std::cout << "part2: " << result.first << std::endl;
    std::cout << "part2: " << result.second << std::endl;
}


#endif
