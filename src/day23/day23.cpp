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
#include <array>

struct move_t{
    int room, hallway;
};

bool operator<(const move_t& a,const move_t& b){
    return std::make_tuple(a.room,a.hallway) < std::make_tuple(b.room,b.hallway);
}

struct burrow_t{
    burrow_t() {}

    burrow_t(int r) : num_rooms(r), spaces(7+r,'.') {}

    const char& room(int r) const{
        return spaces[7+r];
    }

    char& room(int r){
        return spaces[7+r];
    }

    const char& hallway(int h) const{
        return spaces[h];
    }

    char& hallway(int h){
        return spaces[h];
    }

    bool hallway_empty(int h) const{ 
        return hallway(h) == '.'; 
    }

    bool room_empty(int r) const{ 
        return room(r) == '.'; 
    }

    void swap(int r, int h){
        std::swap(spaces[7+r], spaces[h]);
    }

    bool can_move_to_hallway(int r,int h) const{
        int room_depth = num_rooms/4;
        int col = r / room_depth;
        int col2 = col*room_depth;
        for(int i=r; i>col2; --i){ // path out of room clear?
            if(!room_empty(i-1)){
                return false;
            }
        }

        int c2 = col+1;
        if(h <= c2){
            for(int i=c2; i>=h; --i){ // left path to hallway clear?
                if(!hallway_empty(i)){
                    return false;
                }
            }
        }else{
            for(int i=c2+1; i<=h; ++i){ // right path to hallway clear?
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

        int room_depth = num_rooms/4;
        int col = r/room_depth;

        // must go to correct room
        int room_type;
        switch(col){
            case 0:   room_type = 'A';    break;
            case 1:   room_type = 'B';    break;
            case 2:   room_type = 'C';    break;
            case 3:   room_type = 'D';    break;
            default:  room_type = 'Z';    break;
        };

        if(room_type != hallway(h)){
            return false;
        }

        // room cant have other types in there already
        int cs = col * room_depth;
        for(int i=cs; i<cs+room_depth; ++i){ 
            if(!(room_empty(i) || room(i) == hallway_type)){
                return false;
            }
        }

        // can only move to bottom room
        //int rd = r % room_depth;
        for(int i=r+1; i<cs+room_depth; ++i){ 
            if(room_empty(i)){
                return false;
            }
        }

        int c2 = col+1; // 1
        if(h <= c2){
            //for(int i=h+1; i<std::min(5,c2+1); ++i){ // right path to room clear?
            for(int i=h+1; i<std::min(5,c2+1); ++i){ // right path to room clear?
            //for(int i=h+1; i<5; ++i){ // right path to room clear?
                if(!hallway_empty(i)){
                    return false;
                }
            }
        }else{
            for(int i=h-1; i>=std::max(2,c2+1); --i){ // left path to room clear?
            //for(int i=h-1; i>=2; --i){ // left path to room clear?
            //for(int i=h-1; i>1; --i){ // left path to room clear?
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
};

bool operator==(const burrow_t& a,const burrow_t& b){
    return a.spaces == b.spaces;
}

bool operator<(const burrow_t& a,const burrow_t& b){
    return a.energy > b.energy;
}

struct burrow_hash_t {
    size_t operator()(const burrow_t& b) const{ // wee bit wasteful                                                 
        std::size_t seed = b.spaces.size();
        for(char c : b.spaces) { seed ^= c + 0x9e3779b9 + (seed << 6) + (seed >> 2); }
        return seed;
        //return std::hash<std::string_view>()(std::string_view(b.spaces.data(), b.spaces.size()));
    }                                              
};

burrow_t load_input(const std::string& file,int num_rooms){
    burrow_t ret(num_rooms);
    std::ifstream fs(file);
    std::string line;
    std::getline(fs, line);
    std::getline(fs, line);
    ret.hallway(0) = line[1]; 
    ret.hallway(1) = line[2];
    ret.hallway(2) = line[4]; 
    ret.hallway(3) = line[6];
    ret.hallway(4) = line[8];
    ret.hallway(5) = line[10];
    ret.hallway(6) = line[11];
    if(num_rooms == 8){
        std::getline(fs, line);
        ret.room(0) = line[3]; 
        ret.room(2) = line[5];
        ret.room(4) = line[7]; 
        ret.room(6) = line[9];
        std::getline(fs, line);
        ret.room(1) = line[3]; 
        ret.room(3) = line[5];
        ret.room(5) = line[7]; 
        ret.room(7) = line[9];
    }else{
        std::getline(fs, line);
        ret.room(0) = line[3]; 
        ret.room(4) = line[5];
        ret.room(8) = line[7]; 
        ret.room(12) = line[9];
        std::getline(fs, line);
        ret.room(1) = line[3]; 
        ret.room(5) = line[5];
        ret.room(9) = line[7]; 
        ret.room(13) = line[9];
        std::getline(fs, line);
        ret.room(2) = line[3]; 
        ret.room(6) = line[5];
        ret.room(10) = line[7]; 
        ret.room(14) = line[9];
        std::getline(fs, line);
        ret.room(3) = line[3]; 
        ret.room(7) = line[5];
        ret.room(11) = line[7]; 
        ret.room(15) = line[9];
    }
    return ret;
}

void print(const burrow_t& burrow){
    std::cout << std::endl;
    std::cout << "#############" << std::endl;
    std::cout << '#' << burrow.hallway(0) << burrow.hallway(1) << '.' << burrow.hallway(2) << '.' << burrow.hallway(3) << '.' << burrow.hallway(4) << '.' << burrow.hallway(5) << burrow.hallway(6) << '#' << std::endl;
    if(burrow.num_rooms==8){
        std::cout << "###" << burrow.room(0) << '#' << burrow.room(2) << '#' <<  burrow.room(4) << '#' << burrow.room(6) << "###" << std::endl;
        std::cout << "  #" << burrow.room(1) << '#' << burrow.room(3) << '#' <<  burrow.room(5) << '#' << burrow.room(7) << "#  " << std::endl;
    }else{
        std::cout << "###" << burrow.room(0) << '#' << burrow.room(4) << '#' <<  burrow.room(8 ) << '#' << burrow.room(12) << "###" << std::endl;
        std::cout << "  #" << burrow.room(1) << '#' << burrow.room(5) << '#' <<  burrow.room(9 ) << '#' << burrow.room(13) << "#  " << std::endl;
        std::cout << "  #" << burrow.room(2) << '#' << burrow.room(6) << '#' <<  burrow.room(10) << '#' << burrow.room(14) << "#  " << std::endl;
        std::cout << "  #" << burrow.room(3) << '#' << burrow.room(7) << '#' <<  burrow.room(11) << '#' << burrow.room(15) << "#  " << std::endl;
    }
    std::cout << "  #########  " << std::endl;
    std::cout << std::endl;
}

burrow_t target_burrow(int num_rooms){
    burrow_t burrow(num_rooms);
    if(num_rooms == 8){ 
        burrow.room(0) = burrow.room(1) = 'A';
        burrow.room(2) = burrow.room(3) = 'B';
        burrow.room(4) = burrow.room(5) = 'C';
        burrow.room(6) = burrow.room(7) = 'D';
    }else{
        burrow.room(0 ) = burrow.room(1 ) = burrow.room(2 ) = burrow.room(3 ) = 'A';
        burrow.room(4 ) = burrow.room(5 ) = burrow.room(6 ) = burrow.room(7 ) = 'B';
        burrow.room(8 ) = burrow.room(9 ) = burrow.room(10) = burrow.room(11) = 'C';
        burrow.room(12) = burrow.room(13) = burrow.room(14) = burrow.room(15) = 'D';
    }
    return burrow;
}

int nmod(int a,int b){
    return (b + (a%b)) % b;
}

struct path_t{
    int dist;
    std::vector<int> path;
};

std::map<move_t,int> generate_move_map(int num_rooms)
{
    std::vector<int> hallway_numbers = { 0,1,3,5,7,9,10 };
    std::vector<int> dists = { 3,2,2,4,6,8,9, 
                               5,4,2,2,4,6,7, 
                               7,6,4,2,2,4,5, 
                               9,8,6,4,2,2,3 };

    std::map<move_t,int> move_map;
    int room_depth = num_rooms / 4;
    int num_hallways = 7;
    for(int r=0; r<num_rooms; ++r){
        for(int h=0; h<num_hallways; ++h){
            //std::cout << r << " to " << hallway_numbers[h] << " - " << dists[h+(r/room_depth)*7]+(r%room_depth) << std::endl;
            path_t path;
            path.path.push_back(1);
            move_map[{r,h}] = dists[h+(r/room_depth)*7]+(r%room_depth);
        }
        //std::cout << std::endl;
    }

    return move_map;
}

int dijkstra(const burrow_t& burrow, const burrow_t& dst)
{
    std::map<char,int> energy_cost = {{'A', 1}, {'B', 10}, {'C', 100}, {'D', 1000}};

    std::priority_queue<burrow_t> q;

    //cavern_t visited { std::vector<int>(cavern.width*cavern.height, 0), cavern.width, cavern.height };
    //cavern_t cost { std::vector<int>(cavern.width*cavern.height, INT_MAX), cavern.width, cavern.height };

    std::unordered_map<burrow_t,int,burrow_hash_t> visited;
    std::unordered_map<burrow_t,int,burrow_hash_t> cost;

    std::unordered_map<burrow_t,burrow_t,burrow_hash_t> parent;

    //cost.get({0,0}) = cavern.get({0,0});
    //q.push({ 0, 0, cavern.get({0,0}) });

    auto move_map = generate_move_map(burrow.num_rooms);

    cost[burrow] = 0;
    q.push(burrow);

    std::vector<burrow_t> path;
    path.push_back(burrow);

    while (!q.empty()) {
        burrow_t curr = q.top();
        q.pop();

        //std::cout << curr.energy << std::endl;

        if(curr == dst){
            //print(curr);
            //std::cout << curr.energy << std::endl;
        }

        if(visited.find(curr) != visited.end()){
            continue;
        }

        //visited.get(curr.pos) = 1;
        visited[curr] = 1;
        //print(curr);
        for(auto& [move,dist] : move_map){
            if(curr.can_move_to_hallway(move.room,move.hallway)){
                burrow_t new_burrow = curr;
                new_burrow.swap(move.room,move.hallway);
                if(visited.find(new_burrow) == visited.end()){
                    int move_cost = dist * energy_cost[new_burrow.hallway(move.hallway)];
                    if(curr.room(move.room) == '.'){
                        int jj = 0;
                    }
                    if(cost.find(new_burrow) == cost.end()){
                        cost[new_burrow] = INT_MAX;
                    }

                    if(cost[curr] + move_cost < cost[new_burrow]){
                    //if(cost[new_burrow] > cost[curr] + move_cost){
                        cost[new_burrow] = std::min(cost[new_burrow], cost[curr] + move_cost);
                        new_burrow.energy = cost[new_burrow];

                        parent[new_burrow] = curr;

                        //print(new_burrow);
                        q.push(new_burrow);
                        //path.push_back(new_burrow);
                    }
                }
            }

            if(curr.can_move_to_room(move.hallway,move.room)){
                burrow_t new_burrow = curr;
                new_burrow.swap(move.room,move.hallway);
                if(visited.find(new_burrow) == visited.end()){
                    int move_cost = dist * energy_cost[new_burrow.room(move.room)];
                    if(curr.hallway(move.hallway) == '.'){
                        //print(curr);
                        int jj = 0;
                    }
                    if(cost.find(new_burrow) == cost.end()){
                        cost[new_burrow] = INT_MAX;
                    }
                    if(cost[curr] + move_cost < cost[new_burrow]){
                    //if(cost[new_burrow] > cost[curr] + move_cost){
                        cost[new_burrow] = std::min(cost[new_burrow], cost[curr] + move_cost);
                        new_burrow.energy = cost[new_burrow];

                        parent[new_burrow] = curr;

                        /*if(move.room == 4){
                            print(curr);
                            print(new_burrow);
                            int iii = 0;
                        }*/
                        q.push(new_burrow);
                        //path.push_back(new_burrow);
                    }
                }
            }
        }

        int round = 0;

        /*for(auto& d : std::vector<pos_t>{ {-1,0}, {1,0}, {0,-1}, {0,1} }){
            pos_t new_pos = curr.pos + d;
            if(cavern.in_grid(new_pos) && !visited.get(new_pos)){
                cost.get(new_pos) = std::min(cost.get(new_pos), cost.get(curr.pos) + cavern.get(new_pos));
                q.push({ new_pos, cost.get(new_pos) });
            }
        }*/

        /*for(auto& [move,dist] : move_map){
            if(!curr.room_empty(move.room) && curr.hallway_empty(move.hallway)){
                
            }
        }*/

        // try stepping out of room A, 7 possible moves
        /*for(int m=0; m<7; ++m){
            if(!burrow.room_empty(0)){
                if(burrow.hallway_empty(1)){
                    burrow_t new_burrow = burrow;
                    new_burrow.swap(0,1);
                    q.push(new_burrow);
                }
            }
        }*/

        // try stepping out of room B, 7 possible moves

        // try stepping out of room C, 7 possible moves

        // try stepping out of room D, 7 possible moves


        // try stepping into room A, 4 possible moves

        // try stepping into room B, 4 possible moves

        // try stepping into room C, 4 possible moves

        // try stepping into room D, 4 possible moves

    }


    /*for(auto& p : path){
        print(p);
    }*/

   /* std::vector<burrow_t> solution;
    solution.push_back(dst);

    auto target = parent[dst];
    while(!(target == burrow)){
        solution.push_back(target);
        //print(target);
        target = parent[target];
    }
    solution.push_back(target);

    for(auto it=solution.rbegin(); it!=solution.rend(); ++it){
        print(*it);
    }*/


    return cost[dst];
}

size_t part1(const burrow_t& burrow)
{

    //print(burrow);
    //burrow.can_move_to_room(3,7);

    #if 0
auto move_map = generate_move_map(burrow.num_rooms);
    std::map<char,int> energy_cost = {{'A', 1}, {'B', 10}, {'C', 100}, {'D', 1000}};

    // 0 5

    /*print(burrow);
    burrow.can_move_to_room(5,2);*/
    auto burrow2 = burrow;


    auto to_room = [&](int h,int r){
        burrow2.swap(r,h);
        print(burrow2);  
        return move_map[{r,h}] * energy_cost[burrow2.room(r)];
    };

    auto to_hallway = [&](int r,int h){
        burrow2.swap(r,h);
        print(burrow2);  
        return move_map[{r,h}] * energy_cost[burrow2.hallway(h)];
    };

    int energy = 0;

    print(burrow2);
    energy += to_hallway(4,2); 
    energy += to_hallway(2,3);
    energy += to_room(3,4);
    energy += to_hallway(3,3);
    energy += to_room(2,3);
    energy += to_hallway(0,2);
    energy += to_room(2,2);
    energy += to_hallway(6,4);
    energy += to_hallway(7,5);
    energy += to_room(4,7);
    energy += to_room(3,6);
    energy += to_room(5,0);

    std::cout << energy << std::endl;
#endif

    /*burrow2.swap(4,2); print(burrow2);  
    burrow2.swap(2,3); print(burrow2);
    burrow2.swap(4,3); print(burrow2);
    burrow2.swap(3,3); print(burrow2);
    burrow2.swap(3,2); print(burrow2);
    burrow2.swap(0,2); print(burrow2);
    burrow2.swap(2,2); print(burrow2);
    burrow2.swap(6,4); print(burrow2);
    burrow2.swap(7,5); print(burrow2);
    burrow2.swap(7,4); print(burrow2);
    burrow2.swap(6,3); print(burrow2);
    burrow2.swap(0,5); print(burrow2);*/

    //burrow2.swap(0,5); print(burrow2);


    //return 0;

    /*print(burrow);

    print(target_burrow(8));

    auto map = generate_move_map(burrow.num_rooms);*/

    /*for(auto& [move,dist] : map){
        auto new_burrow = burrow;
        std::cout << dist;
        new_burrow.swap(move.room, move.hallway);
        print(new_burrow);

        new_burrow.swap(move.room, move.hallway);
        print(new_burrow);
    }*/

    return dijkstra(burrow, target_burrow(8));
}

size_t part2(const burrow_t& burrow)
{
    return dijkstra(burrow, target_burrow(16));
}

void main()
{
    auto test_values = load_input("../src/day23/example_input.txt", 8);
    auto actual_values = load_input("../src/day23/input.txt", 8);

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    auto test_values0 = load_input("../src/day23/example_input0.txt", 16);
    auto actual_values0 = load_input("../src/day23/input0.txt", 16);

    std::cout << "part2: " << part2(test_values0) << std::endl;
    std::cout << "part2: " << part2(actual_values0) << std::endl;
}
