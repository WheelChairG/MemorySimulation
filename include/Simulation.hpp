#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <cstdint>

struct Request {
    uint32_t addr;
    uint32_t data;  // 对于写操作，该值有效；对于读操作可忽略
    uint8_t w;      // 1 表示写操作，0 表示读操作
};

struct Result {
    uint32_t cycles;
    uint32_t misses;
    uint32_t hits;
    uint32_t primitiveGateCount;
};

// 运行仿真函数：根据各参数配置缓存及主存，按顺序处理 requests 数组中的请求，返回统计结果
Result run_simulation(uint32_t cycles,
                        const char* tracefile,
                        uint8_t numCacheLevels,
                        uint32_t cachelineSize,
                        uint32_t numLinesL1,
                        uint32_t numLinesL2,
                        uint32_t numLinesL3,
                        uint32_t latencyCacheL1,
                        uint32_t latencyCacheL2,
                        uint32_t latencyCacheL3,
                        uint8_t mappingStrategy,
                        uint32_t numRequests,
                        Request* requests);

#endif // SIMULATION_HPP