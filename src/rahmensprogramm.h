#ifndef RAHMENSPROGRAMM_H
#define RAHMENSPROGRAMM_H

#include <stdint.h>

typedef struct CacheConfig {
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
} CacheConfig;

void print_usage(const char *prog_name);

int parse_args(int argc, char *argv[], CacheConfig *config);

#endif
