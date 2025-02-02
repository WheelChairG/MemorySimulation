#ifndef CACHECONFIG_HPP
#define CACHECONFIG_HPP

#include <stdint.h>
#include <string>
#include <fstream>
#include <iostream>

typedef enum {
    DIRECT_MAPPING,
    SET_ASSOCIATIVE,
    FULLY_ASSOCIATIVE
}MappingStrategy;

class CacheConfig{
    public:
        static uint32_t cycles;
        static uint8_t numCacheLevels;//1-3
        static uint32_t cachelineSize;//32 64 128 Bytes min 16
        static uint32_t numLinesL1;//512 - 64K min 16
        static uint32_t numLinesL2;//2K - 512K min 256
        static uint32_t numLinesL3;//8K - 4M min 1K
        static uint32_t latencyCacheL1;//2 - 4Takt min 1Takt
        static uint32_t latencyCacheL2;//10 - 20Takt min 5Takt
        static uint32_t latencyCacheL3;//30 - 100Takt min 15Takt
        static uint8_t mappingStrategy;
        static std::string tracefile;
        static std::string input_file;

    static MappingStrategy intToMappingStrategy(uint8_t value);

    static void validateCacheConfig();

    static bool loadFile(const std::string &file_path);

    static void setDefault();

    CacheConfig();

    CacheConfig(const std::string cache_config);
};
#endif