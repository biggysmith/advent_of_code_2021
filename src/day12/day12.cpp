#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <queue>
#include <map>
#include <unordered_map>

struct connection_t{
    std::string src;
    std::string dst;
};

std::vector<connection_t> load_input(const std::string& file){
    std::vector<connection_t> ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        auto dash = line.find_first_of('-');
        ret.push_back({line.substr(0, dash), line.substr(dash+1)});
    }
    return ret;
}

using graph_t = std::map<std::string,std::vector<std::string>>;

bool is_big_cave(const std::string& cave){
    return cave[0] >= 'A' && cave[0] <= 'Z';
};

bool valid_next_step1(const std::string& cave, const std::vector<std::string>& path) {
    return is_big_cave(cave) || std::find(path.begin(), path.end(), cave) == path.end();
}

std::unordered_map<std::string,int> hist;

bool valid_next_step2(const std::string& cave, const std::vector<std::string>& path)
{
    if(is_big_cave(cave)){
        return true;
    }

    if(cave == "start"){
        return false;
    }

    for(auto& h : hist){
        h.second = 0;
    }

    bool has_two_small_caves = false;
    for(auto& c : path){
        if(!is_big_cave(c)){
            has_two_small_caves |= ++hist[c] > 1;
        }
    }

    return hist[cave] < (has_two_small_caves ? 1 : 2);
}

int bfs(graph_t& g, const std::string& src, const std::string& dst, bool (*valid_next_step)(const std::string&, const std::vector<std::string>&))
{
    int num_paths = 0;
    std::queue<std::vector<std::string>> q;

    std::vector<std::string> path;
    path.push_back(src);
    q.push(path);
    while (!q.empty()) {
        path = q.front();
        q.pop();
        std::string& last = path.back();

        if (last == dst){     
            num_paths++;
            continue;
        }

        for (auto& step : g[last]) {
            if (valid_next_step(step, path)) {
                std::vector<std::string> next_path(path);
                next_path.push_back(step);
                q.push(next_path);
            }
        }
    }

    return num_paths;
}

void dfs(graph_t& g, std::vector<std::string> path, int& num_paths, bool (*valid_next_step)(const std::string&, const std::vector<std::string>&))
{
    std::string last = path.back();
    if(last == "end")
    {
        num_paths++;
    }
    else
    {
        for (auto& step : g[last]){
            if (valid_next_step(step, path)) {
                path.push_back(step);
                dfs(g, path, num_paths, valid_next_step);
                path.pop_back();
            }
        }
    }
}


graph_t build_adjacency_graph(const std::vector<connection_t>& connections)
{
    graph_t g;

    for(auto& connection : connections)
    {
        if(g.find(connection.src) == g.end()){
            g[connection.src] = std::vector<std::string>();
        }
        g[connection.src].push_back(connection.dst);

        if(g.find(connection.dst) == g.end()){
            g[connection.dst] = std::vector<std::string>();
        }
        g[connection.dst].push_back(connection.src);
    }

    return g;
}

int part1(const std::vector<connection_t>& connections)
{
    //return bfs(build_adjacency_graph(connections), "start", "end", valid_next_step1);

    int num_paths = 0;
    dfs(build_adjacency_graph(connections), { "start" }, num_paths, valid_next_step1);
    return num_paths;
}

int part2(const std::vector<connection_t>& connections)
{
    //return bfs(build_adjacency_graph(connections), "start", "end", valid_next_step2);

    int num_paths = 0;
    dfs(build_adjacency_graph(connections), { "start" }, num_paths, valid_next_step2);
    return num_paths;
}

void main()
{
    auto test_values0 = load_input("../src/day12/example_input0.txt");
    auto test_values1 = load_input("../src/day12/example_input1.txt");
    auto test_values2 = load_input("../src/day12/example_input2.txt");
    auto actual_values = load_input("../src/day12/input.txt");

    std::cout << "part1: " << part1(test_values0) << std::endl;
    std::cout << "part1: " << part1(test_values1) << std::endl;
    std::cout << "part1: " << part1(test_values2) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values0) << std::endl;
    std::cout << "part2: " << part2(test_values1) << std::endl;
    std::cout << "part2: " << part2(test_values2) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}