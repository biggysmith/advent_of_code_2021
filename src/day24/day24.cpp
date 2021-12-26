#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <map>
#include <functional>
#include <array>
#include <omp.h>
#include <set>

#define X 0
#define Y 1
#define Z 2
#define W 3

struct instruction_t{
    char op;
    int64_t a,b;
    bool is_b_reg = true;
};

struct input_t{
    std::string model_num;
    int pos = 0;

    int next_digit(){
        return model_num[pos++] - '0';
    }
};

struct alu_progam_t{
    std::vector<instruction_t> instructions;
};

using register_t = std::array<int64_t,4>;

alu_progam_t load_input(const std::string& file){
    alu_progam_t ret;
    std::ifstream fs(file);
    std::string line;
    while(std::getline(fs, line)){
        instruction_t ins;
        char s(' ');
        std::string ins_str,ins_str2;
        std::stringstream ss(line);

        ss >> ins_str;
        if(ins_str == "inp"){
            ins.op = 'i';
        }else if(ins_str == "add"){
            ins.op = 'a';
        }else if(ins_str == "mul"){
            ins.op = 'm';
        }else if(ins_str == "div"){
            ins.op = 'd';
        }else if(ins_str == "mod"){
            ins.op = 'o';
        }else if(ins_str == "eql"){
            ins.op = 'e';
        }

        ss >> ins_str;
        if(ins_str == "x"){
            ins.a = X;
        }else if(ins_str == "y"){
            ins.a = Y;
        }else if(ins_str == "z"){
            ins.a = Z;
        }else if(ins_str == "w"){
            ins.a = W;
        }

        if(ins.op != 'i'){
            ss >> ins_str;
            if(ins_str == "x"){
                ins.b = X;
            }else if(ins_str == "y"){
                ins.b = Y;
            }else if(ins_str == "z"){
                ins.b = Z;
            }else if(ins_str == "w"){
                ins.b = W;
            }else{
                ins.b = std::stoi(ins_str);
                ins.is_b_reg = false;
            }
        }else{
            ins.is_b_reg = false;
        }

        ret.instructions.push_back(ins);
    }
    return ret;
}

void execute(const instruction_t& ins, register_t& r, input_t& in)
{
    auto a = [&]() -> auto&{
        return r[ins.a];
    };

    auto b = [&](){
        return (ins.is_b_reg ? r[ins.b] : ins.b);
    };

    if(ins.op == 'i'){
        a() = in.next_digit();
    }else if(ins.op == 'a'){
        a() += b();
    }else if(ins.op == 'm'){
        a() *= b();
    }else if(ins.op == 'd'){
        a() /= b();
    }else if(ins.op == 'o'){
        a() %= b();
    }else if(ins.op == 'e'){
        a() = (a() == b()) ? 1 : 0;
    } 
}

size_t part2(const alu_progam_t& prog, const std::string& order)
{
    int div[14]   = { 1, 1, 1, 1, 26, 1, 26, 26, 1, 26, 1, 26, 26, 26 };
    int plus0[14] = { 10, 15, 14, 15, -8, 10, -16, -4, 11, -3, 12, -7, -15, -7 };
    int plus1[14] = { 2, 16, 9, 0, 1, 12, 6, 6, 3, 5, 9, 3, 2, 3 };


    /*  translating the input to C code reveals the following repeated sections with 3 lines where the values differ.
        the truncated code is something like: */
    auto func = [&](int b,register_t r,int i){
        r[3] = i;
        r[0] *= 0;
        r[0] += r[2];
        r[0] %= 26;
        r[2] /= div[b]; // div varies
        r[0] += plus0[b]; // plus0 varies
        r[0] = r[0] == r[3];
        r[0] = r[0] == 0;
        r[1] *= 0;
        r[1] += 25;
        r[1] *= r[0];
        r[1] += 1;
        r[2] *= r[1];
        r[1] *= 0;
        r[1] += r[3];
        r[1] += plus1[b]; // plus1 varies
        r[1] *= r[0];
        r[2] += r[1];
        return r;
    };

    /*  the above is essentially:

        r0 = (r2 % 26) + plus0;
        r2 = r2 / div;
        if (r0 != r3){
            return (r2 * 26) + r3 + plus1;
        }else{
            return r2;
        }

        -> (new_r2 * 26) + r3 + plus1 = r2
        -> (new_r2 * 26) = r2 - r3 - plus1
        -> new_r2 = (r2 - r3 - plus1) / 26
        -> (new_r2 / div) = (r2 - r3 - plus1) / 26
        -> new_r2 = ((r2 - r3 - plus1) / 26) * div
    */

    std::set<register_t> regs { {0,0,0,0} };

    std::map<int64_t,std::string> strings;

    for(int f=13; f>=0; --f)
    {
        std::set<register_t> regs2;

        for(auto r3_c : order){
            for(auto& [_,__,r2,___] : regs)
            {
                int64_t r3 = r3_c-'0';
                int64_t r0 = r2 - r3 - plus1[f];

                if ((r0 % 26) == 0){ // whole multiple of 26
                    int64_t new_r2 = r0/26 * div[f];
                    regs2.insert({0,0,new_r2,0});
                    strings[new_r2] = r3_c + strings[r2];
                }

                if (r3-plus0[f] >= 0 && r3-plus0[f] < 26){
                    int64_t new_r2 = r3 - plus0[f] + r2 * div[f];
                    regs2.insert({0,0,new_r2,0});
                    strings[new_r2] = r3_c + strings[r2];
                }
            }
        }

        regs = regs2;
    }

    register_t reg {0,0,0,0};
    input_t in { strings[0], 0 };
    for(int f=0; f<14; ++f){
        func(f, reg, strings[0][f]-'0');
    }

    if(reg[Z] != 0){
        throw std::exception("invalid model number!");
    }

    return std::stoull(strings[0]);
}

void main()
{
    auto actual_values = load_input("../src/day24/input.txt");

    std::cout << "part1: " << part2(actual_values, std::string("123456789")) << std::endl;
    std::cout << "part2: " << part2(actual_values, std::string("987654321")) << std::endl;
}
