#include <systemc.h>
#include <vector>
#include "memory.hpp"

Memory::Memory(sc_module_name name) : sc_module(name) {
    memory.resize(page_num);
    for(int i = 0; i < page_num; i++){
        memory[i].resize(page_size, 0);
    }

    SC_THREAD(process_memory);
    sensitive << clk.pos();
}

void Memory::process_memory(){
    while(true){
        wait();
        std::cout<<"processing memory"<<endl;
        ready.write(0);

        if(read.read() == true && write.read() == true){
            std::cout<<"reading..."<<endl;
            uint32_t addr = address.read();
            int page = addr / page_num;
            int position = addr % page_size;
            uint32_t data = 0;
            for(int i = 3; i >= 0; i--){
                std::cout<<"page: "<<page<<" position: "<<position<<endl;
                data <<= 8;
                data += memory[page][position + i];
                std::cout<<"data: "<<data<<endl;
            }
            std::cout<<"writing..."<<endl;
            r_data.write(data);
            data = w_data.read();
            uint32_t byte = 255;
            for(int i = 0; i < 4; i++){
                memory[page][position + i] = data & byte;
                data >>= 8;
            }
            ready.write(1);
        } else {
            if(read.read() == true && write.read() == false){
                std::cout<<"reading..."<<endl;
                uint32_t addr = address.read();
                int page = addr / page_num;
                int position = addr % page_size;
                uint32_t data = 0;
                for(int i = 3; i >= 0; i--){
                    std::cout<<"page: "<<page<<" position: "<<position<<endl;
                    data <<= 8;
                    data += memory[page][position + i];
                    std::cout<<"data: "<<data<<endl;
                }
                r_data.write(data);
                ready.write(1);
            } else if(write.read() == true && read.read() == false){
                std::cout<<"writing..."<<endl;
                uint32_t addr = address.read();
                int page = addr / page_num;
                int position = addr % page_size;
                uint32_t data = w_data.read();
                uint32_t byte = 255;
                for(int i = 0; i < 4; i++){
                    memory[page][position + i] = data & byte;
                    data >>= 8;
                }
                ready.write(1);
            }
        }

    }
}
