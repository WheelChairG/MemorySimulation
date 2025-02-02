#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <systemc.h>
#include "memory.hpp"
#include "rahmensprogramm.h"
#include "cache.hpp"
#include "cacheline.hpp"
#include "cacheconfig.hpp"

//convert csv file in to requests
std::vector<Request> read_input(const std::string& file_path){

    std::ifstream file(file_path);
    if(!file.is_open()){
        std::cerr<<"Wrong file path"<<std::endl;
    }
    std::cout<<"Input file loaded"<<std::endl;


    std::string type_str;
    std::string addr_str;
    std::string data_str;
    std::vector<Request> requests;

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
            req.w_data = 0;
        } else {
            std::cerr<<"Invalid operation: "<<type_str<<std::endl;
        }
        requests.push_back(req);
    }
    file.close();
    return requests;
}

const char* mstos(MappingStrategy ms){
    switch(ms){
        case DIRECT_MAPPING: return "DIRECT_MAPPING";
        case SET_ASSOCIATIVE: return "SET_ASSOCIATIVE";
        case FULLY_ASSOCIATIVE: return "FULLY_ASSOCIATIVE";
        default: return "UNKNOWN";
    }
}

int sc_main(int argc, char *argv[]){
    Memory memory("Memory");
    CacheConfig cache_config;

    parse_args(argc, argv, &cache_config);
    // setDefault(&cache_config);
    validateCacheConfig(&cache_config);

    std::cout<<"cycles: "<<cache_config.cycles<<std::endl;
    std::cout<<"cache levels: "<<cache_config.numCacheLevels<<std::endl;
    std::cout<<"cache line size: "<<cache_config.cachelineSize<<std::endl;
    std::cout<<"L1 cache lines: "<<cache_config.numLinesL1<<std::endl;
    std::cout<<"L2 cache lines: "<<cache_config.numLinesL2<<std::endl;
    std::cout<<"L3 cache lines: "<<cache_config.numLinesL3<<std::endl;
    std::cout<<"L1 cache latency: "<<cache_config.latencyCacheL1<<std::endl;
    std::cout<<"L2 cache latency: "<<cache_config.latencyCacheL2<<std::endl;
    std::cout<<"L3 cache latency: "<<cache_config.latencyCacheL3<<std::endl;
    std::cout<<"Mapping Strategy: "<<mstos(cache_config.mappingStrategy)<<std::endl;

    
    
    return 0;
}