#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <map>
#include <functional>
#include <queue>
#include <unordered_set>

struct move_t{
    int room, hallway;
};

bool operator<(const move_t& a,const move_t& b){
    return std::make_tuple(a.room,a.hallway) < std::make_tuple(b.room,b.hallway);
}

struct burrow_t{
    burrow_t(int r) : num_rooms(r), spaces(hall_width+r,'.') {}

    const char& room(int r) const{ return spaces[hall_width+r]; }
    char& room(int r){ return spaces[hall_width+r]; }

    const char& hallway(int h) const{ return spaces[h]; }
    char& hallway(int h){ return spaces[h]; }

    bool hallway_empty(int h) const{ return hallway(h) == '.';  }
    bool room_empty(int r) const{ return room(r) == '.'; }

    int room_depth() const{ return num_rooms / 4; }
    int room_col(int r) const{ return r / room_depth(); }

    void swap(int r, int h){ std::swap(room(r), hallway(h)); }

    bool can_move_to_hallway(int r,int h) const{
        if(room_empty(r)){ // cant move something that isn't there
            return false;
        }

        int col = room_col(r);
        int cs = col*room_depth();
        for(int i=r; i>cs; --i){ // path out of room clear?
            if(!room_empty(i-1)){
                return false;
            }
        }

        int c1 = col+1;
        if(h <= c1){
            for(int i=c1; i>=h; --i){ // left path to hallway clear?
                if(!hallway_empty(i)){
                    return false;
                }
            }
        }else{
            for(int i=c1+1; i<=h; ++i){ // right path to hallway clear?
                if(!hallway_empty(i)){
                    return false;
                }
            }
        }

        return true;
    }

    bool can_move_to_room(int h,int r) const{
        if(!room_empty(r)){ // can only move to empty room
            return false;
        }

        char hallway_type = hallway(h);

        // can only enter destination room e.,g. A can only go in A column
        int col = room_col(r);
        if(col+65 != hallway_type){
            return false;
        }

        // room cant have other types in there already
        int cs = col * room_depth();
        for(int i=cs; i<cs+room_depth(); ++i){ 
            if(!(room_empty(i) || room(i) == hallway_type)){
                return false;
            }
        }

        // can only move to lowest space room
        for(int i=r+1; i<cs+room_depth(); ++i){ 
            if(room_empty(i)){
                return false;
            }
        }

        int c1 = col+1; 
        if(h <= c1){
            for(int i=h+1; i<std::min(5,c1+1); ++i){ // right path to room clear?
                if(!hallway_empty(i)){
                    return false;
                }
            }
        }else{
            for(int i=h-1; i>=std::max(2,c1+1); --i){ // left path to room clear?
                if(!hallway_empty(i)){
                    return false;
                }
            }
        }

        return true;
    }

    std::vector<char> spaces;
    int num_rooms;
    int energy = 0;
    static const int hall_width = 7;
};

bool operator==(const burrow_t& a,const burrow_t& b){
    return a.spaces == b.spaces;
}

bool operator<(const burrow_t& a,const burrow_t& b){
    return a.energy > b.energy;
}

struct burrow_hash_t {
    size_t operator()(const burrow_t& b) const{                                               
        return std::hash<std::string_view>()(std::string_view(b.spaces.data(), b.spaces.size()));
    }                                              
};

burrow_t load_input(const std::string& file,int num_rooms){
    burrow_t ret(num_rooms);
    std::ifstream fs(file);
    std::string line;
    std::getline(fs, line);
    std::getline(fs, line);
    for(int i=0; i<num_rooms/4; ++i){
        std::getline(fs, line);
        for(int j=0; j<4; ++j){
            ret.room(i+j*num_rooms/4) = line[3+j*2];         
        }
    }
    return ret;
}

burrow_t target_burrow(int num_rooms){
    burrow_t burrow(num_rooms);
    for(int i=0, c='A'; i<num_rooms; i+=num_rooms/4, c++){
        for(int j=0; j<num_rooms/4; ++j){
            burrow.room(i+j) = c;
        }
    }
    return burrow;
}

std::map<move_t,int> generate_move_map(int num_rooms)
{
    std::vector<int> dists = { 3,2,2,4,6,8,9,  5,4,2,2,4,6,7,  7,6,4,2,2,4,5,  9,8,6,4,2,2,3 };

    std::map<move_t,int> move_map;
    int room_depth = num_rooms / 4;
    for(int r=0; r<num_rooms; ++r){
        for(int h=0; h<burrow_t::hall_width; ++h){
            move_map[{r,h}] = dists[h+(r/room_depth)*burrow_t::hall_width] + (r%room_depth);
        }
    }

    return move_map;
}

template<typename Con,typename T>
bool contains(const Con& c,const T& t){
    return c.find(t) != c.end();
}

int dijkstra(const burrow_t& src, const burrow_t& dst)
{
    std::map<char,int> energy_cost = {{'A', 1}, {'B', 10}, {'C', 100}, {'D', 1000}};

    std::priority_queue<burrow_t> q;

    std::unordered_set<burrow_t,burrow_hash_t> visited;
    std::unordered_map<burrow_t,int,burrow_hash_t> cost;

    auto move_map = generate_move_map(src.num_rooms);

    cost[src] = 0;
    q.push(src);

    std::vector<burrow_t> path;
    path.push_back(src);

    while (!q.empty()) {
        burrow_t curr = q.top();
        q.pop();

        if(curr == dst){
            return cost[dst];
        }

        if(contains(visited,curr)){
            continue;
        }

        visited.insert(curr);

        for(auto& [move,dist] : move_map)
        {
            auto add = [&](burrow_t& new_burrow,int move_cost){
                if(!contains(visited,new_burrow)){
                    cost.try_emplace(new_burrow,INT_MAX);
                    if(cost[curr] + move_cost < cost[new_burrow]){
                        cost[new_burrow] = std::min(cost[new_burrow], cost[curr] + move_cost);
                        new_burrow.energy = cost[new_burrow];
                        q.push(new_burrow);
                    }
                }
            };

            if(curr.can_move_to_hallway(move.room,move.hallway)){
                burrow_t new_burrow = curr;
                new_burrow.swap(move.room,move.hallway);
                add(new_burrow, dist * energy_cost[new_burrow.hallway(move.hallway)]);
            }
            else if(curr.can_move_to_room(move.hallway,move.room)){
                burrow_t new_burrow = curr;
                new_burrow.swap(move.room,move.hallway);
                add(new_burrow, dist * energy_cost[new_burrow.room(move.room)]);
            }
        }

    }

    return cost[dst];
}

size_t part1(const burrow_t& burrow)
{
    return dijkstra(burrow, target_burrow(8));
}

size_t part2(const burrow_t& burrow)
{
    return dijkstra(burrow, target_burrow(16));
}

void main()
{
    auto test_values1 = load_input("../src/day23/example_input.txt", 8);
    auto actual_values1 = load_input("../src/day23/input.txt", 8);

    std::cout << "part1: " << part1(test_values1) << std::endl;
    std::cout << "part1: " << part1(actual_values1) << std::endl;

    auto test_values2 = load_input("../src/day23/example_input0.txt", 16);
    auto actual_values2 = load_input("../src/day23/input0.txt", 16);

    std::cout << "part2: " << part2(test_values2) << std::endl;
    std::cout << "part2: " << part2(actual_values2) << std::endl;
}
