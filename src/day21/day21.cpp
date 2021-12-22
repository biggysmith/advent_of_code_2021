#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <set>
#include <map>
#include <functional>
#include <iterator>
#include <queue>
#include <timer.hpp>

int wrap(int num){
    return 1 + (num - 1) % (11 - 1);
};

int part1(int p1_pos, int p2_pos, int win){
    int turn = 0;
    int p1_score = 0;
    int p2_score = 0;
    int die = 1;
    while(p1_score < win && p2_score < win){
        int roll = die + die+1 + die+2;
        die += 3;
        if(turn & 1){
            p2_pos = wrap(p2_pos+roll);
            p2_score += p2_pos;
        }else{
            p1_pos = wrap(p1_pos+roll);
            p1_score += p1_pos;
        }
        turn++;
    }

    return ((turn & 1) ? p2_score : p1_score) * turn*3;
}

struct game_t{
    int p1_pos;
    int p2_pos;  
    int die = 0;
    int p1_score = 0;
    int p2_score = 0;
    int turn = 0;
};

bool operator<(const game_t& g1,const game_t& g2){
    return std::make_tuple(g1.p1_pos, g1.p2_pos, g1.die, g1.p1_score, g1.p2_score, g1.turn) < std::make_tuple(g2.p1_pos, g2.p2_pos, g2.die, g2.p1_score, g2.p2_score, g2.turn);
}

struct wins_t{
    size_t p1_wins;
    size_t p2_wins;
};

std::map<game_t,wins_t> seen;

wins_t play(game_t& g)
{
    wins_t wins = { 0, 0 };

    if(g.p1_score >= 21){
        return wins_t { 1, 0 };
    }

    if(g.p2_score >= 21){
        return wins_t { 0, 1 };
    }

    if(seen.count(g)){
        return seen.at(g);
    }

    for(int i=1; i<4; ++i)
    {
        for(int j=1; j<4; ++j)
        {
            for(int k=1; k<4; ++k)
            {
                game_t new_game = g;

                int roll = i + j + k;
                new_game.turn++;

                if(new_game.turn & 1){
                    new_game.p1_pos = wrap(new_game.p1_pos+roll);
                    new_game.p1_score += new_game.p1_pos;
                }else{
                    new_game.p2_pos = wrap(new_game.p2_pos+roll);
                    new_game.p2_score += new_game.p2_pos;
                }

                auto next = play(new_game);
                wins.p1_wins += next.p1_wins;
                wins.p2_wins += next.p2_wins;
            } 
        }
    }

    seen[g] = wins;
    return wins;
}

size_t part2(int p1_pos, int p2_pos){
    scoped_timer t;

    game_t game;
    game.p1_pos = p1_pos;
    game.p2_pos = p2_pos;
    auto wins = play(game);
    return std::max(wins.p1_wins, wins.p2_wins);
}

void main()
{
    std::cout << "part1: " << part1(4,8, 1000) << std::endl;
    std::cout << "part1: " << part1(10,6, 1000) << std::endl;

    std::cout << "part2: " << part2(4,8) << std::endl;
    std::cout << "part2: " << part2(10,6) << std::endl;
}
