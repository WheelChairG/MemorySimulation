#ifndef CACHE_HPP
#define CACHE_HPP

#include <vector>
#include <systemc.h>
#include "cacheconfig.hpp"
#include <stdint.h>
#include "memory.hpp"

class Cache : sc_module {
    public:
    sc_in<bool> clk;
    sc_in<uint32_t> addr;
    sc_in<uint32_t> wdata; 
    sc_in<bool> read;
    sc_in<bool> write;
    sc_out<uint32_t> rdata;
    sc_out<bool> ready;
    sc_in<bool> mem_ready;
    sc_in<uint32_t> mem_rdata;

    SC_CTOR(Cache);

    private:
    typedef struct{
        bool valid;
        uint32_t tag;
        std::vector<uint8_t> data;
    }CacheLine;

    CacheConfig cache_config;

    std::vector<CacheLine> cache;
    std::vector<uint32_t> cache_sizes;
    std::vector<uint32_t> cache_lines;
    std::vector<uint32_t> latencies;

    uint8_t cache_levels = 2;

    uint32_t *tag = new uint32_t[9];

    void process_cache();

};

#endif