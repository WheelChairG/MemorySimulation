#ifndef CACHECONFIG_HPP
#define CACHECONFIG_HPP

#include <stdint.h>
#include <stdbool.h>

// Enum for Mapping Strategy
typedef enum {
    DIRECT_MAPPING = 0,
    SET_ASSOCIATIVE = 1,
    FULLY_ASSOCIATIVE = 2
} MappingStrategy;

// Struct for CacheConfig
typedef struct {
    uint32_t cycles;
    uint8_t numCacheLevels;  // 1-3
    uint32_t cachelineSize;  // 16-256 Bytes
    uint32_t numLinesL1;     // 16 - 64K
    uint32_t numLinesL2;     // 256 - 512K
    uint32_t numLinesL3;     // 1K - 4M
    uint32_t latencyCacheL1; // 1 - 4 cycles
    uint32_t latencyCacheL2; // 5 - 20 cycles
    uint32_t latencyCacheL3; // 15 - 100 cycles
    MappingStrategy mappingStrategy;
    const char *tracefile;
    char *inputfile;
} CacheConfig;

#endif
