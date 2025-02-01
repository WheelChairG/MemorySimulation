#ifndef CACHELINE_HPP
#define CACHELINE_HPP

#include <stdint.h>
#include <vector>
#include <systemc.h>

SC_MODULE(CacheLine){
public:
    sc_in<bool> clk;
    sc_signal<uint32_t> line_size;
    sc_signal<uint32_t> tag;//which line
    sc_signal<uint32_t> offset;//where in line
    sc_signal<uint32_t> w_data;
    sc_signal<uint32_t> r_data;
    sc_signal<bool> ready;

    bool valid;
    std::vector<uint8_t> line;

    SC_CTOR(CacheLine){
        valid = false;
        line.resize(line_size, 0);
    }

    void read(){
        uint32_t pos = offset.read();
        if(pos % 4 == 0){
            uint32_t res = 0;
            for(int i = 3; i > 0; i++){
                res <<= 8;
                res += line[pos + i];
            }
            r_data.write(res);
            ready.write(true);
        } else {
            std::cerr<<"Error: tag must be divisable by 4."<<std::endl;
        }
    }

    void write(){
        uint32_t pos = offset.read();
        if(pos % 4 == 0){
            uint32_t temp;
            uint32_t byte = 255;
            uint32_t to_write = w_data.read();
            for(int i = 0; i < 4; i++){
                temp = to_write & byte;
                line[pos + i] = temp;
                to_write >>= 8;
            }
            ready.write(true);
        } else {
            std::cerr<<"Error: tag must be divisable by 4."<<std::endl;
        }
    };

};

#endif