#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <systemc.h>
#include <vector>

class Memory : public sc_module{
public:
    sc_in<bool> clk;
    sc_in<bool> read;
    sc_in<bool> write;
    sc_in<uint32_t> address;
    sc_in<uint32_t> w_data;
    sc_out<uint32_t> r_data;
    sc_out<bool> ready;

    SC_CTOR(Memory);

private:
    std::vector<std::vector<uint8_t>> memory;
    int page_size = 4 * 1024; //4KiB page size
    int page_num = 1024 * 1024; //2^20 pages

    void process_memory();
};

#endif