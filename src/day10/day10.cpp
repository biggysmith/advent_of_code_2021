#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <stack>
#include <map>

std::vector<std::string> load_input(const std::string& file){
    std::vector<std::string> ret;
    std::ifstream fs(file);
    std::string line;
    while (std::getline(fs, line)) {
        ret.push_back(line);
    }
    return ret;
}

bool left_bracket(char c) {
    return c == '(' || c == '[' || c == '{' || c == '<'; 
}

bool right_bracket(char c) {
    return c == ')' || c == ']' || c == '}' || c == '>'; 
}

bool left_and_right_match(char a, char b) {
    return (a == '(' && b == ')') || (a == '[' && b == ']') || (a == '{' && b == '}') || (a == '<' && b == '>');
}

char reverse_bracket(char c) {
    switch(c){
        case '(':   return ')';     break;
        case ')':   return '(';     break;
        case '[':   return ']';     break;
        case ']':   return '[';     break;
        case '{':   return '}';     break;
        case '}':   return '{';     break;
        case '<':   return '>';     break;
        case '>':   return '<';     break;
        default:    return 0;       break;
    };
}

std::string check(const std::string& line) {
    std::stack<char> stack;

    for (auto c : line) 
    {
        if (left_bracket(c)) 
        { 
            stack.push(c); 
        } 
        else if (right_bracket(c)) 
        {
            if (!stack.empty() && left_and_right_match(stack.top(), c)) 
            { 
                stack.pop(); 
            } 
            else 
            { 
                return std::string(1,c); // we got a mismatched bracket, return it
            } 
        } 
    }

    std::string ret;
    while(!stack.empty()){
        ret += reverse_bracket(stack.top());
        stack.pop();
    }

    return ret; // incomplete line, return completion string
}

int part1(const std::vector<std::string>& lines)
{
    std::map<std::string,int> points = { { ")", 3 }, { "]", 57 }, { "}", 1197 }, { ">", 25137 } };

    int score = 0;
    for(auto& line : lines){
        auto res = check(line);
        if(res.size() == 1){
            score += points[res];
        }
    }

    return score;
}

size_t part2(const std::vector<std::string>& lines)
{
    auto score = [&](const std::string& str)
    {
        std::map<char,int> points = { { ')', 1 }, { ']', 2 }, { '}', 3 }, { '>', 4 } };

        size_t ret = 0;
        for(auto c : str){
            ret = ret*5 + points[c];
        }

        return ret;
    };

    std::vector<size_t> scores;
    for(auto& line : lines){
        auto res = check(line);
        if(res.size() > 1){
            scores.push_back(score(res));
        }
    }

    std::nth_element(scores.begin(), scores.begin()+scores.size()/2, scores.end());

    return scores[scores.size()/2];
}

void main()
{
    auto test_values = load_input("../src/day10/example_input.txt");
    auto actual_values = load_input("../src/day10/input.txt");

    std::cout << "part1: " << part1(test_values) << std::endl;
    std::cout << "part1: " << part1(actual_values) << std::endl;

    std::cout << "part2: " << part2(test_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}