#ifndef cpu_6502_hpp
#define cpu_6502_hpp

#include <cstdio>
#include <cstdint>
#include <ostream>
#include <exception>
#include <iostream>
#include <cstring>
#include <iomanip>
#include <string>
#include "utils.hpp"
#include "memory.hpp"



#define ERROR_UNKNOWN_INSTRUCTION -1



namespace nes {
    
    struct instruction_info {
        std::string asm_name;
        uint8_t len;
    };

    struct registers {
        uint8_t A;
        uint8_t X;
        uint8_t Y;
/*
    7 bit 0
    ---- ----
    NVss DIZC
    |||| ||||
    |||| |||+- Carry
    |||| ||+-- Zero
    |||| |+--- Interrupt Disable
    |||| +---- Decimal
    ||++------ No CPU effect, see: the B flag
    |+-------- Overflow
    +--------- Negative
*/
        struct {
            uint8_t carry_flag : 1;
            uint8_t zero_flag : 1;
            uint8_t interrupt_disable : 1;
            uint8_t decimal_mode : 1;
            uint8_t break_command : 1;
            uint8_t no_effect : 1;
            uint8_t overflow_flag : 1;
            uint8_t negative_flag : 1;

            operator uint8_t() const
            {
                return this->negative_flag << 7
                | this->overflow_flag << 6
                | this->no_effect << 5
                | this->break_command << 4
                | this->decimal_mode << 3
                | this->interrupt_disable << 2
                | this->zero_flag << 1
                | this->carry_flag
                ;
            }
            
        } P;
        
        uint8_t SP;
        uint16_t PC;
    };
    
    std::ostream& operator<<(std::ostream& os, const registers& r);

    static const uint16_t g_frame_irq_state_address = 0x4017;
    static const uint16_t g_apu_state_address = 0x4015;
    
    class cpu_6502 {

        registers reg_{0};
        memory& mem_;
        
        
        uint8_t op_val_{0};
        uint16_t op_address_{0};
        
        // addressing modes
        
        void implied_addressing();
        void accumulator_addressing();
        void immediate_addressing();
        void zero_page_addressing();
        void zero_page_x_addressing();
        void zero_page_y_addressing();
        void relative_addressing();
        void absolute_addressing();
        void absolute_x_addressing();
        void absolute_y_addressing();
        void indirect_addressing();
        void indirect_x_addressing();
        void indirect_y_addressing();
       
        // transfer reg

        void TAX();
        void TAY();
        void TXA();
        void TYA();
        void TSX();
        void TXS();

        
        // load && store

        void LDA();
        void LDX();
        void LDY();
        void STA();
        void STX();
        void STY();

        //flag clr && set

        void CLC();
        void CLI();
        void CLD();
        void CLV();
        void SEC();
        void SEI();
        void SED();


        //inc && dec

        void INX();
        void INY();
        void INC();


        void DEX();
        void DEY();
        void DEC();


        void ADC();
        void BRK();
        
        template<typename T>
        void set_nf(T n)
        {
            this->reg_.P.negative_flag = get_bit(n, sizeof(T) * 8 - 1);
        }
        
        template<typename T>
        void set_zf(T n)
        {
            this->reg_.P.zero_flag = n == 0 ? 1 : 0;
        }
        
        template<typename T>
        void set_nzf(T n)
        {
            this->set_nf(n);
            this->set_zf(n);
        }
        
        void reset_reg();
        void reset_mem();

    public:
        cpu_6502() = delete;
        cpu_6502(const cpu_6502&) = delete;
        cpu_6502(cpu_6502&&) = delete;
        cpu_6502& operator=(const cpu_6502&) = delete;
        cpu_6502& operator=(cpu_6502&&) = delete;

        
        cpu_6502(memory& m) noexcept
        :mem_(m)
        {
        }
        

        void load_code_segment(uint16_t segment_base_addr, const uint8_t *buf, size_t size);

        void debug_print_reg() const;
        
        template<typename T>
        void debug_print_asm(uint16_t addr, const instruction_info& info, T t) const
        {
            std::string asm_str = info.asm_name;
            std::cout << std::setw(4) << std::hex << addr << ": " << asm_str << " ";
            std::cout << std::setw(4) << std::hex << t;
            std::cout << std::endl;
        }
        
        void debug_print_asm(uint16_t addr, const instruction_info& info) const
        {
            std::string asm_str = info.asm_name;
            std::cout << std::setw(4) << std::hex << addr << ": " << asm_str << " ";
            std::cout << std::endl;
        }
        
                
        uint8_t eval();
        void run();

        void toggle_frame_irq(uint8_t state = 0x00);
        void toggle_apu(uint8_t state = 0x00);

        template<typename T>
        void push(T v)
        {
            this->mem_.write(v, g_stack_offset.start | this->reg_.SP);
            this->reg_.SP -= sizeof(T);
        }

        template<typename T>
        T pop()
        {
            this->reg_.SP += sizeof(T);
            this->mem_.read<T>(g_stack_offset.start | this->reg_.SP);
        }

        void power_up();
        void reset();
        void interrupt();

        void dissassembly(const uint8_t *buf, size_t size);
        void test();
    };
}





#endif /* cpu_6502_hpp */
