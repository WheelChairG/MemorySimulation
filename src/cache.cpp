#include "Cache.hpp"
#include <iostream>
#include <cassert>

Cache::Cache(sc_module_name name,
             uint8_t numLevels,
             uint32_t cachelineSize,
             uint32_t numLinesL1,
             uint32_t numLinesL2,
             uint32_t numLinesL3,
             uint32_t latencyL1,
             uint32_t latencyL2,
             uint32_t latencyL3,
             uint8_t mappingStrategy)
    : sc_module(name),
      cachelineSize(cachelineSize),
      numLevels(numLevels),
      mappingStrategy(mappingStrategy),
      hitCount(0),
      missCount(0),
      fifoCounterGlobal(0),
      enableLogging(true)  // 默认开启日志
{
    // 根据实际的缓存层数初始化各级参数
    numLines.push_back(numLinesL1);
    latencies.push_back(latencyL1);
    if(numLevels >= 2) {
        numLines.push_back(numLinesL2);
        latencies.push_back(latencyL2);
    }
    if(numLevels >= 3) {
        numLines.push_back(numLinesL3);
        latencies.push_back(latencyL3);
    }
    // 初始化各级缓存行
    levels.resize(numLevels);
    for (uint8_t lvl = 0; lvl < numLevels; lvl++) {
        levels[lvl].resize(numLines[lvl], CacheLine(cachelineSize));
    }
    
    SC_THREAD(process);
    sensitive << clk.pos();
}

uint8_t Cache::getCacheLineContent(uint32_t level, uint32_t lineIndex, uint32_t index) {
    if(level >= levels.size() || lineIndex >= levels[level].size() || index >= cachelineSize)
        return 0;
    return levels[level][lineIndex].data[index];
}

void Cache::process() {
    while(true) {
        wait(); // 每个时钟周期触发一次
        if (r.read() || w.read()) {
            ready.write(false);
            uint32_t currentAddr = addr.read();
            uint32_t blockAddr = currentAddr / cachelineSize;
            uint32_t offset = currentAddr % cachelineSize;
            
            CACHE_LOG("Processing request at address 0x" << std::hex << currentAddr);
            
            // 处理读操作
            if (r.read()) {
                bool hit = false;
                uint32_t dataWord = 0;
                for (uint8_t lvl = 0; lvl < numLevels; lvl++) {
                    int lineIndex = -1;
                    if (searchCacheLine(lvl, blockAddr, lineIndex)) {
                        hit = true;
                        // 模拟该级缓存的延迟
                        for (uint32_t i = 0; i < latencies[lvl]; i++) {
                            wait();
                        }
                        // 读取 4 字节数据（若 offset+b 超出 cachelineSize 则置 0）
                        for (int b = 0; b < 4; b++) {
                            if(offset + b < cachelineSize)
                                dataWord |= (levels[lvl][lineIndex].data[offset + b]) << (8 * b);
                        }
                        CACHE_LOG("L" << (int)lvl+1 << " Hit for address 0x" << std::hex << currentAddr);
                        break;
                    }
                }
                if (hit) {
                    hitCount++;
                    rdata.write(dataWord);
                    ready.write(true);
                } else {
                    missCount++;
                    CACHE_LOG("Miss for address 0x" << std::hex << currentAddr);
                    // Cache Miss：采用多次主存访问完整填充缓存行
                    std::vector<uint8_t> blockData(cachelineSize, 0);
                    for (uint32_t off = 0; off < cachelineSize; off += 4) {
                        // 计算对应地址（这里简单采用对齐方式，实际设计中可能需要更精细的地址映射）
                        uint32_t accessAddr = (currentAddr - offset) + off;
                        mem_r.write(true);
                        mem_addr.write(accessAddr);
                        do {
                            wait();
                        } while (!mem_ready.read());
                        uint32_t partData = mem_rdata.read();
                        mem_r.write(false);
                        for (uint32_t i = 0; i < 4 && (off + i) < cachelineSize; i++) {
                            blockData[off + i] = (partData >> (8 * i)) & 0xFF;
                        }
                    }
                    
                    // 严格包含性更新：从高层到低层更新，仅在更高层尚未包含时更新当前层
                    for (int lvl = numLevels - 1; lvl >= 0; lvl--) {
                        bool existsInUpper = false;
                        for (uint8_t upper = lvl + 1; upper < numLevels; upper++) {
                            int tmpIdx = -1;
                            if(searchCacheLine(upper, blockAddr, tmpIdx)) {
                                existsInUpper = true;
                                break;
                            }
                        }
                        if (!existsInUpper) {
                            int victim = chooseVictim(lvl, blockAddr);
                            levels[lvl][victim].valid = true;
                            levels[lvl][victim].tag = computeTag(lvl, blockAddr);
                            levels[lvl][victim].data = blockData;
                            levels[lvl][victim].fifoCounter = ++fifoCounterGlobal;
                            CACHE_LOG("Updating L" << lvl+1 << " cache, victim line " << victim);
                        } else {
                            CACHE_LOG("Skipping update for L" << lvl+1 << " as higher level already contains block 0x" << std::hex << blockAddr);
                        }
                    }
                    // 读取请求从 L1 返回数据（此处简单返回主存第一次访问返回的4字节数据）
                    rdata.write(*(uint32_t*)(&blockData[0]));
                    ready.write(true);
                }
            }
            // 处理写操作（写透策略）
            else if (w.read()) {
                uint32_t writeData = wdata.read();
                bool hit = false;
                for (uint8_t lvl = 0; lvl < numLevels; lvl++) {
                    int lineIndex = -1;
                    if (searchCacheLine(lvl, blockAddr, lineIndex)) {
                        hit = true;
                        // 更新缓存中对应字节（写入 4 字节数据）
                        for (int b = 0; b < 4; b++) {
                            if(offset + b < cachelineSize)
                                levels[lvl][lineIndex].data[offset + b] = (writeData >> (8 * b)) & 0xFF;
                        }
                        CACHE_LOG("L" << (int)lvl+1 << " Write Hit for address 0x" << std::hex << currentAddr);
                    }
                }
                if (!hit) {
                    missCount++;
                    CACHE_LOG("Write Miss for address 0x" << std::hex << currentAddr);
                    // 未命中时采用多次主存访问填充整个缓存行
                    std::vector<uint8_t> blockData(cachelineSize, 0);
                    for (uint32_t off = 0; off < cachelineSize; off += 4) {
                        uint32_t accessAddr = (currentAddr - offset) + off;
                        mem_r.write(true);
                        mem_addr.write(accessAddr);
                        do {
                            wait();
                        } while (!mem_ready.read());
                        uint32_t partData = mem_rdata.read();
                        mem_r.write(false);
                        for (uint32_t i = 0; i < 4 && (off + i) < cachelineSize; i++) {
                            blockData[off + i] = (partData >> (8 * i)) & 0xFF;
                        }
                    }
                    // 严格包含性更新：只有当前层及更低层缺失时才更新
                    for (int lvl = numLevels - 1; lvl >= 0; lvl--) {
                        bool existsInUpper = false;
                        for (uint8_t upper = lvl + 1; upper < numLevels; upper++) {
                            int tmpIdx = -1;
                            if(searchCacheLine(upper, blockAddr, tmpIdx)) {
                                existsInUpper = true;
                                break;
                            }
                        }
                        if (!existsInUpper) {
                            int victim = chooseVictim(lvl, blockAddr);
                            levels[lvl][victim].valid = true;
                            levels[lvl][victim].tag = computeTag(lvl, blockAddr);
                            levels[lvl][victim].data = blockData;
                            levels[lvl][victim].fifoCounter = ++fifoCounterGlobal;
                            CACHE_LOG("Updating L" << lvl+1 << " cache for write, victim line " << victim);
                        } else {
                            CACHE_LOG("Skipping update for L" << lvl+1 << " during write as higher level contains block 0x" << std::hex << blockAddr);
                        }
                    }
                } else {
                    hitCount++;
                }
                // 写透：同时写入主存
                mem_w.write(true);
                mem_addr.write(currentAddr - offset);
                mem_wdata.write(writeData);
                do {
                    wait();
                } while (!mem_ready.read());
                mem_w.write(false);
                ready.write(true);
            }
        }
    }
}

bool Cache::searchCacheLine(uint8_t level, uint32_t blockAddr, int &lineIndex) {
    if(mappingStrategy == 0) { // direct-mapped：计算 index 和 tag
        int idx = blockAddr % numLines[level];
        uint32_t tag = blockAddr / numLines[level];
        if (levels[level][idx].valid && levels[level][idx].tag == tag) {
            lineIndex = idx;
            return true;
        }
        return false;
    }
    else if(mappingStrategy == 1) { // fully associative：遍历所有行
        for (size_t i = 0; i < levels[level].size(); i++) {
            if(levels[level][i].valid && levels[level][i].tag == blockAddr) {
                lineIndex = i;
                return true;
            }
        }
        return false;
    }
    else if(mappingStrategy == 2) { // set-associative：假设 4 路组相联
        uint32_t ways = 4;
        if(numLines[level] < ways) { // 若行数不足，按全相联处理
            for (size_t i = 0; i < levels[level].size(); i++) {
                if(levels[level][i].valid && levels[level][i].tag == blockAddr) {
                    lineIndex = i;
                    return true;
                }
            }
            return false;
        }
        else {
            uint32_t numSets = numLines[level] / ways;
            uint32_t setIndex = blockAddr % numSets;
            uint32_t tag = blockAddr / numSets;
            for (uint32_t i = setIndex * ways; i < setIndex * ways + ways; i++) {
                if(levels[level][i].valid && levels[level][i].tag == tag) {
                    lineIndex = i;
                    return true;
                }
            }
            return false;
        }
    }
    return false;
}

int Cache::chooseVictim(uint8_t level, uint32_t blockAddr) {
    if(mappingStrategy == 0) { // direct-mapped：直接替换对应 index
        return blockAddr % numLines[level];
    }
    else if(mappingStrategy == 1) { // fully associative：若有空行则使用空行，否则选 FIFO 最小者
        for (size_t i = 0; i < levels[level].size(); i++) {
            if(!levels[level][i].valid)
                return i;
        }
        int victim = 0;
        uint32_t minCounter = levels[level][0].fifoCounter;
        for (size_t i = 1; i < levels[level].size(); i++) {
            if(levels[level][i].fifoCounter < minCounter) {
                minCounter = levels[level][i].fifoCounter;
                victim = i;
            }
        }
        return victim;
    }
    else if(mappingStrategy == 2) { // set-associative
        uint32_t ways = 4;
        if(numLines[level] < ways) { // 全相联处理
            for (size_t i = 0; i < levels[level].size(); i++) {
                if(!levels[level][i].valid)
                    return i;
            }
            int victim = 0;
            uint32_t minCounter = levels[level][0].fifoCounter;
            for (size_t i = 1; i < levels[level].size(); i++) {
                if(levels[level][i].fifoCounter < minCounter) {
                    minCounter = levels[level][i].fifoCounter;
                    victim = i;
                }
            }
            return victim;
        }
        else {
            uint32_t numSets = numLines[level] / ways;
            uint32_t setIndex = blockAddr % numSets;
            int victim = -1;
            uint32_t minCounter = 0xFFFFFFFF;
            for (uint32_t i = setIndex * ways; i < setIndex * ways + ways; i++) {
                if(!levels[level][i].valid)
                    return i;
                if(levels[level][i].fifoCounter < minCounter) {
                    minCounter = levels[level][i].fifoCounter;
                    victim = i;
                }
            }
            return victim;
        }
    }
    return 0;
}

uint32_t Cache::computeTag(uint8_t level, uint32_t blockAddr) {
    if(mappingStrategy == 0) {
        return blockAddr / numLines[level];
    }
    else if(mappingStrategy == 1) {
        return blockAddr;
    }
    else if(mappingStrategy == 2) {
        uint32_t ways = 4;
        if(numLines[level] < ways)
            return blockAddr;
        else
            return blockAddr / (numLines[level] / ways);
    }
    return blockAddr;
}