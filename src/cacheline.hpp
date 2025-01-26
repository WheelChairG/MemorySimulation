#include <stdint.h>
#include <vector>
#include <systemc.h>

class CacheLine : sc_module{
    public:
    bool valid;
    uint32_t tag;
    std::vector<std::vector<uint8_t>> data;
};