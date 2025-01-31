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
        uint32_t cycles;
        uint8_t numCacheLevels;//1-3
        uint32_t cachelineSize;//32 64 128 Bytes min 16
        uint32_t numLinesL1;//512 - 64K min 16
        uint32_t numLinesL2;//2K - 512K min 256
        uint32_t numLinesL3;//8K - 4M min 1K
        uint32_t latencyCacheL1;//2 - 4Takt min 1Takt
        uint32_t latencyCacheL2;//10 - 20Takt min 5Takt
        uint32_t latencyCacheL3;//30 - 100Takt min 15Takt
        uint8_t mappingStrategy;
        std::string tracefile;
        std::string input_file;

    MappingStrategy intToMappingStrategy(uint8_t value){
        switch(value){
            case 0: return DIRECT_MAPPING;
            case 1: return SET_ASSOCIATIVE;
            case 2: return FULLY_ASSOCIATIVE;
            default: throw std::invalid_argument("invalid mapping strategy.");
        }
    }


    void validateCacheConfig(){
        if(numCacheLevels < 1 || numCacheLevels > 3){
            numCacheLevels = 2;
        }
        if(cachelineSize < 16 || cachelineSize > 256){
            cachelineSize = 32;
        }

        switch(numCacheLevels){
            case 1:
                if(numLinesL1 < 16 || numLinesL1 > 64000){
                    numLinesL1 = 16;
                }
                if(latencyCacheL1 < 1 || latencyCacheL1 > 4){
                    latencyCacheL1 = 1;
                }
                break;
            case 2:
                if(numLinesL1 < 16 || numLinesL1 > 64000){
                    numLinesL1 = 16;
                }
                if(latencyCacheL1 < 1 || latencyCacheL1 > 4){
                    latencyCacheL1 = 1;
                }
                if(numLinesL2 < 256 || numLinesL2 > 512000){
                    numLinesL2 = 256;
                }
                if(latencyCacheL2 < 5 || latencyCacheL2 > 20){
                    latencyCacheL2 = 5;
                }
            case 3:
                if(numLinesL1 < 16 || numLinesL1 > 64000){
                    numLinesL1 = 16;
                }
                if(latencyCacheL1 < 1 || latencyCacheL1 > 4){
                    latencyCacheL1 = 1;
                }
                if(numLinesL2 < 256 || numLinesL2 > 512000){
                    numLinesL2 = 256;
                }
                if(latencyCacheL2 < 5 || latencyCacheL2 > 20){
                    latencyCacheL2 = 5;
                }
                if(numLinesL3 < 1000 || numLinesL3 > 4000000){
                    numLinesL3 = 1000;
                }
                if(latencyCacheL3 < 15 || latencyCacheL3 > 100){
                    latencyCacheL3 = 15;
                }
        }
    }

    bool loadFile(const std::string &file_path){
        std::ifstream file(file_path);
        if(!file.is_open()){
            std::cerr<<"Error: Path "<<file_path<<" does not contain configuration file or not in valid format."<<std::endl;
            return false;
        }
        
        std::string key;
        while(file >> key){
            if(key == "cycles"){
                file >> cycles;
            } else if(key == "numCacheLevels"){
                file >> numCacheLevels;
            } else if(key == "cachelineSize"){
                file >> cachelineSize;
            } else if(key == "numLinesL1"){
                file >> numLinesL1;
            } else if(key == "numLinesL2"){
                file >> numLinesL2;
            } else if(key == "numLinesL3"){
                file >> numLinesL3;
            } else if(key == "latencyCacheL1"){
                file >> latencyCacheL1;
            } else if(key == "latencyCacheL2"){
                file >> latencyCacheL2;
            } else if(key == "latencyCacheL3"){
                file >> latencyCacheL3;
            } else if(key == "mappingStrategy"){
                int value;
                file >> value;
                mappingStrategy = intToMappingStrategy(value);
            }
        }

        file.close();
        validateCacheConfig();
// uint32_t cycles;
//         uint8_t numCacheLevels;//1-3
//         uint32_t cachelineSize;//32 64 128 Bytes min 16
//         uint32_t numLinesL1;//512 - 64K min 16
//         uint32_t numLinesL2;//2K - 512K min 256
//         uint32_t numLinesL3;//8K - 4M min 1K
//         uint32_t latencyCacheL1;//2 - 4Takt min 1Takt
//         uint32_t latencyCacheL2;//10 - 20Takt min 5Takt
//         uint32_t latencyCacheL3;//30 - 100Takt min 15Takt
//         uint8_t mappingStrategy;
//         std::string tracefile;
//         std::string input_file;

        return false;
    }
};
#endif