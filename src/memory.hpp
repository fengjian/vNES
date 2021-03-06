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
#include <new>
#include "utils.hpp"

#define NES_MAX_RAM 0x10000


namespace nes {
    
    struct address_offset {
        uint16_t start;
        uint16_t end;
    };
    
    static const address_offset g_stack_offset = { 0x1ff, 0x100 };
    
    class memory {
        
        uint8_t *internal_ram_addr_space_{nullptr};
        address_offset code_segment_offset_{0x00, 0x00};

    public:
        memory(const memory&) = delete;
        memory(memory&&) = delete;
        memory& operator=(const memory&) = delete;
        memory& operator=(memory&&) = delete;

        memory() noexcept 
        :internal_ram_addr_space_(new(std::nothrow) uint8_t[NES_MAX_RAM]())
        {

        }

        ~memory()
        {
            if (internal_ram_addr_space_) {
                delete[] internal_ram_addr_space_;
            }
        }

        const address_offset& get_code_segment_offset() const
        {
            return this->code_segment_offset_;
        }
        
        void set_code_segment_offset(uint16_t start, uint16_t end)
        {
            this->code_segment_offset_.start = start;
            this->code_segment_offset_.end = end;
        }

        
        uint8_t* map_offset_addr(uint16_t offset)
        {
            //TODO
            return this->internal_ram_addr_space_ + offset;
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

        void bzero(uint16_t begin, uint16_t end);
        
        void debug_dump_ram(uint8_t row=12) const;

    };

}



#endif /* memory_hpp */
