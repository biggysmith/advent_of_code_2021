#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>

std::string load_input(const std::string& file){
    std::ifstream fs(file);
    std::string line;
    std::getline(fs, line);
    return line;
}

std::string hex_to_bin(char c)
{
    switch(c){
        case '0':   return "0000";    break;
        case '1':   return "0001";    break;
        case '2':   return "0010";    break;
        case '3':   return "0011";    break;
        case '4':   return "0100";    break;
        case '5':   return "0101";    break;
        case '6':   return "0110";    break;
        case '7':   return "0111";    break;
        case '8':   return "1000";    break;
        case '9':   return "1001";    break;
        case 'A':   return "1010";    break;
        case 'B':   return "1011";    break;
        case 'C':   return "1100";    break;
        case 'D':   return "1101";    break;
        case 'E':   return "1110";    break;
        case 'F':   return "1111";    break;
        default:    throw std::exception("unrecognized hex");
    }
}

std::string hex_to_bin(const std::string& hex){
    std::string binary;
    for(auto c : hex){
        binary += hex_to_bin(c);
    }
    return binary;
}

template<class T = void>
struct minimum_op {	
    T operator()(const T& a, const T& b) const {
	    return std::min(a, b);
	}
};

template<class T = void>
struct maximum_op {	
    T operator()(const T& a, const T& b) const {
	    return std::max(a, b);
	}
};

int version(const std::string& s, int& pos){
    int ver = std::stoi(s.substr(pos, 3), 0, 2); pos+=3;
    return ver;
}

int type_id(const std::string& s, int& pos){
    int id = std::stoi(s.substr(pos, 3), 0, 2); pos+=3;
    return id;
}

size_t literal_value(const std::string& s, int& pos){
    bool valid = true;
    std::string literal;
    while(valid){
        auto ss = s.substr(pos, 5); pos+=5;
        valid = ss[0] == '1';
        literal += ss.substr(1,4);
    }; 
    return std::stoull(literal, 0, 2);
}

int length_type_id(const std::string& s, int& pos){
    int id = s[pos]-'0'; pos+=1;
    return id;
}

int sub_packet_length(const std::string& s, int& pos){
    int ret = std::stoi(s.substr(pos, 15), 0, 2); pos+=15;
    return ret;
}

int num_sub_packets(const std::string& s, int& pos){
    int ret = std::stoi(s.substr(pos, 11), 0, 2); pos+=11;
    return ret;
}

int process1(const std::string& s, int& pos, int& version_sum){
    int ver = version(s,pos);
    int id = type_id(s,pos);
    version_sum += ver;
    if(id == 4){
        literal_value(s,pos);
    }else{
        int lid = length_type_id(s,pos);
        if(lid == 0){
            int len = sub_packet_length(s,pos) + pos;
            while(pos < len){
                process1(s,pos,version_sum);
            }
        }else{
            int n = num_sub_packets(s,pos);
            for(int i=0; i<n; ++i){
                process1(s,pos,version_sum);
            }
        }
    }

    return version_sum;
}

size_t op(int id, const std::vector<size_t>& args){
    if(id == 0){
        return std::accumulate(args.begin()+1, args.end(), args[0], std::plus<size_t>());
    }else if(id == 1){
        return std::accumulate(args.begin()+1, args.end(), args[0], std::multiplies<size_t>());
    }else if(id == 2){
        return std::accumulate(args.begin()+1, args.end(), args[0], minimum_op<size_t>());
    }else if(id == 3){
        return std::accumulate(args.begin()+1, args.end(), args[0], maximum_op<size_t>());
    }else if(id == 5){
        return std::accumulate(args.begin()+1, args.end(), args[0], std::greater<size_t>());
    }else if(id == 6){
        return std::accumulate(args.begin()+1, args.end(), args[0], std::less<size_t>());
    }else /*if(id == 7)*/{
        return std::accumulate(args.begin()+1, args.end(), args[0], std::equal_to<size_t>());
    }
}

size_t process2(const std::string& s, int& pos){
    int ver = version(s,pos);
    int id = type_id(s,pos);

    if(id == 4){
        return literal_value(s,pos);
    }

    std::vector<size_t> args;

    int len_id = length_type_id(s,pos);
    if(len_id == 0){
        int len = sub_packet_length(s,pos) + pos;
        while(pos < len){
            args.push_back(process2(s,pos));
        }
    }else{
        int n = num_sub_packets(s,pos);
        for(int i=0; i<n; ++i){
            args.push_back(process2(s,pos));
        }
    }

    return op(id, args);
}

size_t part1(const std::string& data)
{
    int pos = 0;
    int version_sum = 0;
    return process1(hex_to_bin(data), pos, version_sum);
}

size_t part2(const std::string& data)
{
    int pos = 0;
    return process2(hex_to_bin(data), pos);
}

void main()
{
    auto actual_values = load_input("../src/day16/input.txt");

    std::cout << "part1: " << part1(actual_values) << std::endl;
    std::cout << "part2: " << part2(actual_values) << std::endl;
}