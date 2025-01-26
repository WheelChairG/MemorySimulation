#ifndef CACHECONFIG_HPP
#define CACHECONFIG_HPP

#include <stdint.h>

typedef enum{
    DIRECT_MAPPING,
    SET_ASSOCIATIVE,
    FULLY_ASSOCIATIVE
}MappingStrategy;

typedef struct{
    uint32_t cycles;
    uint8_t numCacheLevels;
    uint32_t cachelineSize;
    uint32_t numLinesL1;
    uint32_t numLinesL2;
    uint32_t numLinesL3;
    uint32_t latencyCacheL1;
    uint32_t latencyCacheL2;
    uint32_t latencyCacheL3;
    uint8_t mappingStrategy;
    const char *tracefile;
    char *input_file;
}CacheConfig;

#endif