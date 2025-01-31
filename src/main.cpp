#include <iostream>
#include <vector>
#include <systemc.h>
#include "memory.hpp"
#include "rahmensprogramm.h"
#include "cache.hpp"
#include "cacheline.hpp"


int sc_main(int argc, char *argv[]){
    print_usage(argv[0]);
    CacheConfig cacheConfig;

    if(parse_args(argc, argv, &cacheConfig) == -1){
        return EXIT_FAILURE;
    }

    std::cout<<"simulation starts"<<endl;
    sc_signal<bool> w_signal, r_signal, ready_signal;
    sc_signal<uint32_t> wdata;
    sc_signal<uint32_t> addr;
    sc_signal<uint32_t> rdata;
    sc_signal<uint32_t> mLatency;
    sc_clock clk_signal("clk_signal", 10, SC_NS);
    
    Memory memory("Memory");
    
    memory.clk(clk_signal);
    memory.write(w_signal);
    memory.read(r_signal);
    memory.w_data(wdata);
    memory.r_data(rdata);
    memory.address(addr);
    memory.ready(ready_signal);
    memory.m_latency(mLatency);

    //0001 0010 0100 1000 0001 0010 0100 1000
    //0x12481248
    //4096 512 64 8
    uint32_t x = 0x12481248;
    mLatency.write(10);
    wdata.write(x);
    addr.write(0);
    
    w_signal.write(true);
    r_signal.write(false);

    sc_start(10, SC_NS);

    w_signal.write(false);
    r_signal.write(true);

    sc_start(10, SC_NS);
    
    std::cout<<"r_data: "<<rdata<<std::endl;

    
    return 0;
}