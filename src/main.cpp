#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <systemc.h>
#include "../include/memory.hpp"
#include "../include/rahmensprogramm.h"
#include "../include/cache.hpp"
#include "../include/cacheline.hpp"
#include "../include/cacheconfig.hpp"

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

void write_requests_to_csv(const std::string& filename, std::vector<Request> requests, size_t num_requests) {
    std::ofstream outfile(filename);
    for (size_t i = 0; i < num_requests; ++i) {
        outfile << (requests[i].wr ? "W," : "R,")
                << requests[i].addr << ","
                << requests[i].w_data << std::endl;
    }
    outfile.close();
}

struct Result{
    uint32_t cycles;
    uint32_t misses;
    uint32_t hits;
    uint32_t primitiveGateCount;
};

Result run_simulation(
    uint32_t cycles,
    const std::vector<Request>& requests,
    CacheConfig& cache_config,
    Cache& cache
) {
    // Initialisiere Cache und Speicher

    uint32_t misses = 0;
    uint32_t hits = 0;

    // Starte Simulation für gegebene Anzahl an Zyklen oder bis Requests abgearbeitet sind
    for (uint32_t i = 0; i < cycles && i < requests.size(); i++) {
        const Request& req = requests[i];

        bool hit;
        uint32_t result = 0; // Initialize result

        if (req.wr == 1) {
            // Schreibzugriff
            cache.writeCache(req.addr, req.w_data);
        } else {
            // Lesezugriff
            hit = cache.readCache(req.addr, result);
        }

        if (hit) {
            hits++;
        } else {
            misses++;
        }
    }

    // Geschätzte Anzahl an primitiven Gattern (kann angepasst werden)
    uint32_t primitiveGateCount = cache.estimateGateCount(cache_config);

    // Ergebnisse zurückgeben
    Result sim_result = {cycles, misses, hits, primitiveGateCount};
    return sim_result;
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

    std::vector<Request> requests = read_input(argv[1]);
    
    Cache cache("Cache", cache_config, memory);
    Result result = run_simulation(cache_config.cycles, requests, cache_config, cache);

    write_requests_to_csv(argv[1], requests, requests.size());
    return 0;
}