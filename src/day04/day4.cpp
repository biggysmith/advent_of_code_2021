#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <algorithm>
#include <numeric>

struct bingo_board_t{
    std::vector<int> numbers;
    std::vector<int> marked;
};

struct bingo_data_t{
    std::vector<int> numbers;
    std::vector<bingo_board_t> boards;
};

bingo_data_t load_input(const std::string& file){
    bingo_data_t ret;
    std::ifstream fs(file);
    std::string line;

    std::getline(fs, line);
    {
        std::istringstream iss(line);
        std::string item;
        while (std::getline(iss, item, ',')) {
            ret.numbers.push_back(std::stoi(item));
        }
    }

    while (std::getline(fs, line)) {
        if(line.empty()){
            ret.boards.push_back(bingo_board_t());
            continue;
        }

        std::istringstream iss(line);
        while(iss.good()){
            int num;
            iss >> num;
            ret.boards.back().numbers.push_back(num);
            ret.boards.back().marked.push_back(0);
        }
    }
    return ret;
}

bool check_for_line(const bingo_board_t& board){
    for(int y=0; y<5; ++y){
        int count = 0;
        for(int x=0; x<5; ++x){
            count += board.marked[y*5+x];
        }
        if(count == 5){
            return true;
        }
    }

    for(int x=0; x<5; ++x){
        int count = 0;
        for(int y=0; y<5; ++y){
            count += board.marked[y*5+x];
        }
        if(count == 5){
            return true;
        }
    }

    return false;
}

void clear_marked(bingo_data_t& data){
    for(auto& board : data.boards){
        std::fill(board.marked.begin(), board.marked.end(), 0);
    }
}

int part1(bingo_data_t& data){
    clear_marked(data);
    for(auto num : data.numbers){
        for(auto& board : data.boards){
            auto it = std::find(board.numbers.begin(), board.numbers.end(), num);
            if(it != board.numbers.end()){
                board.marked[std::distance(board.numbers.begin(),it)] = 1;
            }

            if(check_for_line(board)){
                int count = 0;
                for(int i=0; i<25; ++i){
                    if(!board.marked[i]){
                        count += board.numbers[i];
                    }
                }
                return num * count;
            }
        }
    }

    return 0;
}

int part2(bingo_data_t& data){
   clear_marked(data);
   std::vector<int> winning_boards(data.boards.size(), 0);
   for(auto num : data.numbers){
        int board_num = 0;
        for(auto& board : data.boards){
            auto it = std::find(board.numbers.begin(), board.numbers.end(), num);
            if(it != board.numbers.end()){
                board.marked[std::distance(board.numbers.begin(),it)] = 1;
            }

            if(!winning_boards[board_num] && check_for_line(board)){
                winning_boards[board_num] = 1;
                int num_winning = std::accumulate(winning_boards.begin(), winning_boards.end(), 0);
                if(num_winning == data.boards.size()){
                    int count = 0;
                    for(int i=0; i<25; ++i){
                        if(!board.marked[i]){
                            count += board.numbers[i];
                        }
                    }
                    return num * count;
                }
            }

            board_num++;
        }
    }

    return 0;
}

void main()
{
    auto test_values = load_input("../src/day04/question_input.txt");
    auto actual_values = load_input("../src/day04/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}