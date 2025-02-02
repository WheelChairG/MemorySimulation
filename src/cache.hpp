#ifndef CACHE_HPP
#define CACHE_HPP

#include <vector>
#include <systemc.h>
#include "cacheconfig.hpp"
#include "cacheline.hpp"
#include <stdint.h>
#include "memory.hpp"

// Write-Through
struct Request {
    uint32_t addr;
    uint32_t w_data;
    uint32_t wr; // 0 = read, 1 = write
};

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

    sc_out<uint32_t> mem_addr;
    sc_out<uint32_t> mem_wdata;
    sc_out<bool> mem_r;
    sc_out<bool> mem_w;

    SC_CTOR(Cache);
    Cache(sc_module_name name, const CacheConfig &config, Memory &memory);

    bool readCache(uint32_t addr, uint32_t &data);
    void writeCache(uint32_t addr, uint32_t data);
    void handleMiss(uint32_t addr);
    void process_cache();
    uint8_t getCacheLineContent(uint32_t level, uint32_t lineIndex, uint32_t index);

private:
    CacheConfig cache_config;
    std::vector<std::vector<CacheLine*> > cache_levels;//use Type CacheLine* instead of type CacheLine(no standard constructor for intialization in vector)
    std::vector<uint32_t> cache_sizes;
    std::vector<uint32_t> cache_lines;
    std::vector<uint32_t> latencies;
    uint8_t num_cache_levels;

    int getCacheIndex(uint32_t addr, uint8_t level);
    Memory &memory;

    uint32_t latencyL1;
    uint32_t latencyL2;
    uint32_t latencyL3;

    // 在 Cache 内部使用 sc_signal
    sc_signal<uint32_t> cache_tag, cache_offset, cache_w_data, cache_r_data;
    sc_signal<bool> cache_ready;
};

Cache::Cache(sc_module_name name, const CacheConfig& config, Memory &memory) : sc_module(name), cache_config(config), memory(memory){
    SC_METHOD(process_cache);
    sensitive << clk.pos();
    num_cache_levels = config.numCacheLevels;

    cache_sizes = std::vector<uint32_t>();
    cache_sizes.push_back(config.numLinesL1 * config.cachelineSize);
    cache_sizes.push_back(config.numLinesL2 * config.cachelineSize);
    cache_sizes.push_back(config.numLinesL3 * config.cachelineSize);

    latencies.push_back(config.latencyCacheL1);
    latencies.push_back(config.latencyCacheL2);
    latencies.push_back(config.latencyCacheL3);

    cache_levels.resize(num_cache_levels);
    for(int i = 0; i < num_cache_levels; i++){
        cache_levels[i].resize(cache_sizes[i] / config.cachelineSize);
        for(int j = 0; j < cache_levels[i].size(); j++){
            cache_levels[i][j]->latency = latencies[i];
        }
    }
}

// 获取缓存索引，Direct-Mapped 和 Set-Associative
int Cache::getCacheIndex(uint32_t addr, uint8_t level) {
    MappingStrategy strategy = static_cast<MappingStrategy>(cache_config.mappingStrategy);

    switch (strategy) {
        case DIRECT_MAPPING:
            return (addr / cache_config.cachelineSize) % cache_levels[level].size();
        case SET_ASSOCIATIVE:
            return (addr / cache_config.cachelineSize) % (cache_levels[level].size() / 4);
        case FULLY_ASSOCIATIVE:
            return -1;
        default:
            throw std::invalid_argument("Invalid mapping strategy.");
    }
}

bool Cache::readCache(uint32_t addr, uint32_t &data) {
    uint32_t tag = addr / cache_config.cachelineSize;
    uint32_t index = getCacheIndex(addr, 0);
    uint32_t offset = addr % cache_config.cachelineSize;

    // for(int i = 0; i < num_cache_levels; i++){
    //     CacheLine &line = *cache_levels[i][index];
    //     if(line.valid && line.tag.read() == tag){
    //         line.offset.write(offset);
    //         line.read();
    //         data = line.r_data.read();
    //         return true;
    //     }
    // }
    // return false;
    
    for (int i = 0; i < num_cache_levels; i++) {
        if (cache_config.mappingStrategy == FULLY_ASSOCIATIVE) {
            for (size_t j = 0; j < cache_levels[i].size(); ++j) {
                CacheLine *line = cache_levels[i][j];
                if (line && line->valid && cache_tag.read() == tag) {
                    data = cache_r_data.read();
                    return true;
                }
            }
        } else {
            CacheLine *line = cache_levels[i][index];
            if (line && line->valid && cache_tag.read() == tag) {
                cache_offset.write(offset);
                data = cache_r_data.read();
                return true;
            }
        }
    }
    return false;
}

void Cache::writeCache(uint32_t addr, uint32_t data) {
    uint32_t tag = addr / cache_config.cachelineSize;
    uint32_t index = getCacheIndex(addr, 0);
    uint32_t offset = addr % cache_config.cachelineSize;

   // for(int i = 0; i < num_cache_levels; i++){
    //     CacheLine &line = *cache_levels[i][index];
    //     line.valid = true;
    //     line.tag.write(tag);
    //     line.offset.write(offset);
    //     line.w_data.write(data);
    //     line.write();
    // }
    for (int i = 0; i < num_cache_levels; i++) {
        if (cache_config.mappingStrategy == FULLY_ASSOCIATIVE) {
            for (size_t j = 0; j < cache_levels[i].size(); ++j) {
                CacheLine *line = cache_levels[i][j];
                if (!line || !line->valid) {
                    line = new CacheLine("CacheLine");
                    line->valid = true;
                    cache_tag.write(tag);
                    cache_offset.write(offset);
                    cache_w_data.write(data);
                    line->write();
                    return;
                }
            }
        } else {
            CacheLine *line = cache_levels[i][index];
            if (!line) {
                line = new CacheLine("CacheLine");
                cache_levels[i][index] = line;
            }
            line->valid = true;
            cache_tag.write(tag);
            cache_offset.write(offset);
            cache_w_data.write(data);
            line->write();
        }
    }

    // Write-Through: 直接写入 Memory
    sc_signal<uint32_t> a;
    sc_signal<uint32_t> d;
    sc_signal<bool> r;
    sc_signal<bool> w;
    a.write(addr);
    d.write(data);
    r.write(false);
    w.write(true);

    memory.address(a);
    memory.w_data(d);
    memory.read(r);
    memory.write(w);
}

void Cache::handleMiss(uint32_t addr) {
    if (mem_ready.read()) {
        uint32_t mem_data = mem_rdata.read();
        writeCache(addr, mem_data);
    }
}

void Cache::process_cache() {
    if (read.read()) {
        uint32_t data;
        if (readCache(addr.read(), data)) {
            rdata.write(data);
            ready.write(true);
        } else {
            handleMiss(addr.read());
            ready.write(false);
        }
    }
    if (write.read()) {
        writeCache(addr.read(), wdata.read());
    }
}

uint8_t Cache::getCacheLineContent(uint32_t level, uint32_t lineIndex, uint32_t index) {
    if (level < cache_levels.size() && lineIndex < cache_levels[level].size()) {
        return cache_levels[level][lineIndex]->line[index];
    }
    return 0;
}

#endif
