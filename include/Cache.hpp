#ifndef CACHE_HPP
#define CACHE_HPP

#include <systemc>
#include <vector>
#include <cstdint>
#include <iostream>
using namespace sc_core;

// 定义日志宏，若 enableLogging 为 true 则输出日志信息
#define CACHE_LOG(msg) if(enableLogging) std::cout << "[Cache] " << msg << std::endl

SC_MODULE(Cache) {
public:
    // CPU 侧信号
    sc_in<bool> clk;
    sc_in<uint32_t> addr;
    sc_in<uint32_t> wdata;
    sc_in<bool> r;
    sc_in<bool> w;
    
    // 与主存交互的信号
    sc_in<bool> mem_ready;
    sc_in<uint32_t> mem_rdata;
    
    sc_out<uint32_t> rdata;
    sc_out<bool> ready;
    
    sc_out<uint32_t> mem_addr;
    sc_out<uint32_t> mem_wdata;
    sc_out<bool> mem_r;
    sc_out<bool> mem_w;
    
    // 构造函数：参数包括缓存层数、cacheline 大小、各级缓存行数、各级延迟以及映射策略
    SC_HAS_PROCESS(Cache);
    Cache(sc_module_name name,
          uint8_t numLevels,
          uint32_t cachelineSize,
          uint32_t numLinesL1,
          uint32_t numLinesL2,
          uint32_t numLinesL3,
          uint32_t latencyL1,
          uint32_t latencyL2,
          uint32_t latencyL3,
          uint8_t mappingStrategy);
    
    // 用于测试的接口：返回指定级别、行、字节索引处的内容
    uint8_t getCacheLineContent(uint32_t level, uint32_t lineIndex, uint32_t index);
    
    // 统计数据：命中次数与未命中次数
    uint32_t hitCount;
    uint32_t missCount;
    
    // 日志开关
    bool enableLogging;
    
private:
    // 内部缓存行结构
    struct CacheLine {
        bool valid;
        uint32_t tag;
        std::vector<uint8_t> data;  // 长度为 cachelineSize 的字节数组
        uint32_t fifoCounter;       // 用于 FIFO 替换策略
        CacheLine(uint32_t lineSize) : valid(false), tag(0), data(lineSize, 0), fifoCounter(0) {}
    };
    
    // 每一级缓存：levels[0] 表示 L1，levels[1] 表示 L2，levels[2] 表示 L3（如果有）
    std::vector< std::vector<CacheLine> > levels;
    std::vector<uint32_t> latencies; // 每一级的访问延迟
    std::vector<uint32_t> numLines;  // 每一级的缓存行数
    
    uint32_t cachelineSize;
    uint8_t numLevels;
    uint8_t mappingStrategy; // 0: direct-mapped, 1: fully associative, 2: set-associative
    
    uint32_t fifoCounterGlobal; // 全局 FIFO 计数器，用于替换策略
    
    // 主线程：根据 CPU 的读写请求，执行缓存操作（命中／未命中、写透等）
    void process();
    
    // 辅助函数：
    // 在指定 level 中查找 blockAddr 对应的缓存行，若命中则返回 true 并在 lineIndex 中写入命中行索引
    bool searchCacheLine(uint8_t level, uint32_t blockAddr, int &lineIndex);
    // 根据替换策略选择 victim 行索引（若有空行则返回空行索引，否则返回 FIFO 最小的行）
    int chooseVictim(uint8_t level, uint32_t blockAddr);
    // 根据映射策略计算 tag 值
    uint32_t computeTag(uint8_t level, uint32_t blockAddr);
};

#endif // CACHE_HPP