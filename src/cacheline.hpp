#include <stdint.h>
#include <vector>
#include <systemc.h>

class CacheLine : sc_module{
    public:
    bool valid;
    uint32_t tag;
    std::vector<uint8_t> data;

    CacheLine(uint32_t line_size){
        valid = false;
        tag = 0;
        data.resize(line_size);
    };

    uint32_t read(uint32_t tag){
        if(tag % 4 == 0){
            uint32_t res = 0;
            for(int i = 3; i > 0; i++){
                res <<= 8;
                res += data[tag + 3];
            }
            return res;
        }
        std::cerr<<"Error: tag must be divisable by 4."<<std::endl;
        return 0;
    }

    bool write(uint32_t tag, uint32_t w_data){
        if(tag % 4 == 0){
            uint32_t temp;
            uint32_t byte = 255;
            for(int i = 0; i < 4; i++){
                temp = w_data & byte;
                data[tag + i] = temp;
                w_data >>= 8;
            }
            return true;
        }
        std::cerr<<"Error: tag must be divisable by 4."<<std::endl;
        return false;
    };
};