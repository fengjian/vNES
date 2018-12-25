

#include "memory.hpp"


namespace nes {

    void memory::debug_dump_ram(uint8_t row) const
    {
        uint8_t bw = 4;
        std::string line(row*bw + 8, '-');
        size_t len = arr_len(this->internal_ram_addr_space_);

        std::cout << line << std::endl
                  << "[DEBUG dump ram]" << std::endl
                  << line << std::endl;
        
        for (int i = 0; i < len/row; ++i) {
            size_t off = i * row;
            
            std::cout << '|' << std::setw(bw) << std::hex << off << ": " ;
            for (int j = 0; j < row; ++j) {
                std::cout << std::setw(bw) << std::hex << this->internal_ram_addr_space_[off + j];
            }
            
            std::cout << '|' <<  std::endl;
        }
        
        std::cout << line << std::endl;
    }
    
    void memory::bzero()
    {
        memset(this->internal_ram_addr_space_, 0, arr_len(this->internal_ram_addr_space_));
    }


}