#include <stdint.h>
#include <vector>
#include <systemc.h>

class CacheLine : sc_module{
    public:
    bool valid;
    uint32_t tag;
    std::vector<std::vector<uint8_t>> data;

    CacheLine(uint32_t line_size){
        valid = false;
        tag = 0;
        data.resize(line_size);
    };

    uint32_t read(uint32_t tag);
    bool write(uint32_t tag);
};