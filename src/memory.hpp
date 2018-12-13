#ifndef memory_hpp
#define memory_hpp

#include <cstdio>
#include <cstdint>
#include <ostream>
#include <exception>
#include <iostream>
#include <cstring>
#include <iomanip>
#include <string>
#include "nes.hpp"
#include "utils.hpp"


namespace nes {
    
    class memory {
        
        uint8_t internal_ram_addr_space[NES_MAX_RAM] = {0};
        
    public:
        struct {
            uint16_t start = 0x1FF;
            uint16_t end = 0x100;
        } stack_offset;
        
        uint8_t* map_offset_addr(uint16_t offset)
        {
            //TODO
            return this->internal_ram_addr_space + offset;
        }
    
        template<typename T, typename T2>
        T read(T2 offset)
        {
            T b = *((T *)this->map_offset_addr(offset));
            return b;
        }
        
        template<typename T, typename T2>
        void write(T v, T2 offset)
        {
            *((T *)this->map_offset_addr(offset)) = v;
        }
        
        void bzero();
        
        void debug_dump_ram(uint8_t row=12) const;

    };

}



#endif /* memory_hpp */
