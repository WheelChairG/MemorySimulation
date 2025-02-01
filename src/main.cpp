#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <systemc.h>
#include "memory.hpp"
#include "rahmensprogramm.h"
#include "cache.hpp"
#include "cacheline.hpp"

//get line number of csv file for read_input function
size_t get_file_line_num(const std::string& file_path){
    std::ifstream file(file_path);
    if(!file.is_open()){
        std::cerr<<"Wrong file path"<<std::endl;
    }
    size_t line_count = 0;
    std::string line;
    while(std::getline(file, line)){
        line_count++;
    }
    file.close();
    return line_count;
}

//convert csv file in to requests
std::vector<Request> read_input(const std::string& file_path, size_t request_num){

    std::ifstream file(file_path);
    if(!file.is_open()){
        std::cerr<<"Wrong file path"<<std::endl;
    }
    std::cout<<"Input file loaded"<<std::endl;

    std::string type_str;
    std::string addr_str;
    std::string data_str;
    std::vector<Request> requests;
    requests.resize(request_num);

    std::string line;
    while(std::getline(file, line)){
        std::istringstream s(line);
        
        if(!std::getline(s, type_str, ',') || !std::getline(s, addr_str, ',')){
            std::cerr<<"Invalid format: "<<line<<std::endl;
            exit(EXIT_FAILURE);
        }

        Request req;
        try {
            req.addr = static_cast<uint32_t>(std::stoul(addr_str, nullptr, 16));
        } catch (const std::invalid_argument& e){
            std::cerr<<"Invalid address: "<<addr_str<<std::endl;
            exit(EXIT_FAILURE);
        } catch (const std::out_of_range& e){
            std::cerr<<"Adddress out of range: "<<addr_str<<std::endl;
            exit(EXIT_FAILURE);
        }

        if(type_str == "W"){
            if(!std::getline(s, data_str, ',')){
                std::cerr<<"Invalid foramt: "<<line<<std::endl;
                exit(EXIT_FAILURE);
            }
            try{
                req.w_data = static_cast<uint32_t>(std::stoul(data_str, nullptr, 16));
            } catch (const std::invalid_argument& e){
                std::cerr<<"Invalid address: "<<addr_str<<std::endl;
                exit(EXIT_FAILURE);
            } catch (const std::out_of_range& e){
                std::cerr<<"Adddress out of range: "<<addr_str<<std::endl;
                exit(EXIT_FAILURE);
            }
            req.wr = 1;
        } else if(type_str == "R"){
            if(std::getline(s, data_str, ',')){
                std::cerr<<"Invalid format: "<<line<<std::endl;
                exit(EXIT_FAILURE);
            }
            req.wr = 0;
        } else {
            std::cerr<<"Invalid operation: "<<type_str<<std::endl;
        }
        requests.push_back(req);
    }
    file.close();
    return requests;
}



int sc_main(int argc, char *argv[]){
    print_usage(argv[0]);
    CacheConfig cacheConfig;

    if(parse_args(argc, argv, &cacheConfig) == -1){
        return EXIT_FAILURE;
    }

    std::cout<<"simulation starts"<<endl;
    sc_signal<bool> w_signal, r_signal, ready_signal;
    sc_signal<uint32_t> wdata;
    sc_signal<uint32_t> addr;
    sc_signal<uint32_t> rdata;
    sc_signal<uint32_t> mLatency;
    sc_clock clk_signal("clk_signal", 10, SC_NS);
    
    Memory memory("Memory");
    
    memory.clk(clk_signal);
    memory.write(w_signal);
    memory.read(r_signal);
    memory.w_data(wdata);
    memory.r_data(rdata);
    memory.address(addr);
    memory.ready(ready_signal);
    memory.m_latency(mLatency);

    //0001 0010 0100 1000 0001 0010 0100 1000
    //0x12481248
    //4096 512 64 8
    uint32_t x = 0x12481248;
    mLatency.write(10);
    wdata.write(x);
    addr.write(0);
    
    w_signal.write(true);
    r_signal.write(false);

    sc_start(10, SC_NS);

    w_signal.write(false);
    r_signal.write(true);

    sc_start(10, SC_NS);
    
    std::cout<<"r_data: "<<rdata<<std::endl;

    
    return 0;
}