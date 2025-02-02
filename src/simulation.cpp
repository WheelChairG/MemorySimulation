#include "Simulation.hpp"
#include "Cache.hpp"
#include "MainMemory.hpp"
#include <systemc>
#include <vector>
#include <iostream>
#include <cassert>

using namespace sc_core;
using namespace std;

// Testbench 模块：驱动请求并连接各模块
SC_MODULE(Testbench) {
    // 时钟信号
    sc_clock clk;
    sc_in<bool> clk_in;
    // CPU 到 Cache 信号
    sc_signal<uint32_t> addr;
    sc_signal<uint32_t> wdata;
    sc_signal<bool> r;
    sc_signal<bool> w;
    // Cache 与 MainMemory 之间信号
    sc_signal<bool> mem_ready;
    sc_signal<uint32_t> mem_rdata;
    
    sc_signal<uint32_t> rdata;
    sc_signal<bool> ready;
    
    sc_signal<uint32_t> mem_addr;
    sc_signal<uint32_t> mem_wdata;
    sc_signal<bool> mem_r;
    sc_signal<bool> mem_w;
    
    // 模块实例指针
    Cache* cache;
    MAIN_MEMORY* mainMemory;
    
    // 请求列表
    std::vector<Request> reqs;
    uint32_t currentReq;
    
    SC_CTOR(Testbench)
    : clk("clk", 10, SC_NS) // 时钟周期 10 ns
    {
        clk_in.bind(clk);
        currentReq = 0;
        SC_THREAD(run);
        sensitive << clk_in.pos();
    }
    
    void setRequests(const std::vector<Request>& requests) {
        reqs = requests;
    }
    
    void run() {
        // 初始化所有信号
        addr.write(0);
        wdata.write(0);
        r.write(false);
        w.write(false);
        
        // 按顺序处理所有请求
        while (currentReq < reqs.size()) {
            Request req = reqs[currentReq];
            addr.write(req.addr);
            if (req.w == 1) {
                w.write(true);
                wdata.write(req.data);
                r.write(false);
            } else {
                r.write(true);
                w.write(false);
            }
            // 等待 cache 模块完成当前请求（直到 ready 信号置 1）
            do {
                wait();
            } while (!ready.read());
            // 清除请求信号，准备下一个请求
            r.write(false);
            w.write(false);
            currentReq++;
        }
        // 所有请求处理完毕后，再等待几周期后停止仿真
        for (int i = 0; i < 10; i++) wait();
        sc_stop();
    }
};

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
                        Request* requests)
{
    Result result;
    result.cycles = 0;
    result.hits = 0;
    result.misses = 0;
    result.primitiveGateCount = 0;
    
    // 创建 Testbench 实例
    Testbench* tb = new Testbench("tb");
    
    // 创建 Cache 实例
    uint32_t numLines2 = (numCacheLevels >= 2) ? numLinesL2 : 0;
    uint32_t numLines3 = (numCacheLevels >= 3) ? numLinesL3 : 0;
    tb->cache = new Cache("cache",
                          numCacheLevels,
                          cachelineSize,
                          numLinesL1,
                          numLines2,
                          numLines3,
                          latencyCacheL1,
                          latencyCacheL2,
                          latencyCacheL3,
                          mappingStrategy);
    
    // 创建 MainMemory 实例
    tb->mainMemory = new MAIN_MEMORY("mainMemory");
    
    // 连接 Cache 与 Testbench 信号
    tb->cache->clk(tb->clk);
    tb->cache->addr(tb->addr);
    tb->cache->wdata(tb->wdata);
    tb->cache->r(tb->r);
    tb->cache->w(tb->w);
    tb->cache->mem_ready(tb->mem_ready);
    tb->cache->mem_rdata(tb->mem_rdata);
    tb->cache->rdata(tb->rdata);
    tb->cache->ready(tb->ready);
    tb->cache->mem_addr(tb->mem_addr);
    tb->cache->mem_wdata(tb->mem_wdata);
    tb->cache->mem_r(tb->mem_r);
    tb->cache->mem_w(tb->mem_w);
    
    // 连接 MainMemory 与信号
    tb->mainMemory->clk(tb->clk);
    tb->mainMemory->addr(tb->mem_addr);
    tb->mainMemory->wdata(tb->mem_wdata);
    tb->mainMemory->r(tb->mem_r);
    tb->mainMemory->w(tb->mem_w);
    tb->mainMemory->rdata(tb->mem_rdata);
    tb->mainMemory->ready(tb->mem_ready);
    
    // 设置请求
    std::vector<Request> reqVec(requests, requests + numRequests);
    tb->setRequests(reqVec);
    
    // 开始仿真
    sc_start();
    
    // 收集统计数据
    result.hits = tb->cache->hitCount;
    result.misses = tb->cache->missCount;
    // 估计总周期数（此处用 sc_time_stamp 与时钟周期 10 ns 近似计算）
    result.cycles = sc_time_stamp().value() / 10;
    // 粗略估计原语门数（仅为示例，可根据实际设计调整）
    result.primitiveGateCount = (numCacheLevels * 1000) + (numLinesL1 + numLines2 + numLines3) * 10;
    
    // 清理动态分配内存
    delete tb->cache;
    delete tb->mainMemory;
    delete tb;
    
    return result;
}