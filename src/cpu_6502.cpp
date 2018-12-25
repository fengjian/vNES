#include "cpu_6502.hpp"
#include <cassert>

namespace nes {
    
    
    std::ostream& operator<<(std::ostream& os, const registers& r)
    {
        return os <<  "A: " << std::hex << static_cast<unsigned int>(r.A) << std::endl
        << "X: " << std::hex << static_cast<unsigned int>(r.X) << std::endl
        << "Y: " << std::hex << static_cast<unsigned int>(r.Y) << std::endl
        << "SP: " << std::hex << static_cast<unsigned int>(r.SP) << std::endl
        << "PC: " << std::hex << static_cast<unsigned int>(r.PC) << std::endl
        << "P: " << to_binary_string(r.P) << std::endl
        << "cf: " << static_cast<unsigned int>(r.P.carry_flag)
        << " zf: " << static_cast<unsigned int>(r.P.zero_flag)
        << " id: " << static_cast<unsigned int>(r.P.interrupt_disable)
        << " dm: " << static_cast<unsigned int>(r.P.decimal_mode)
        << " bc: " << static_cast<unsigned int>(r.P.break_command)
        << " of: " << static_cast<unsigned int>(r.P.overflow_flag)
        << " nf: " << static_cast<unsigned int>(r.P.negative_flag);
    }
    
    
    //TODO
    void cpu_emulator::implied_addressing()
    {
        //pass
    }
    
    void cpu_emulator::accumulator_addressing()
    {
        //pass
    }
    
    void cpu_emulator::immediate_addressing()
    {
        this->op_val_ = this->mem_.read<uint8_t>(this->reg_.PC + 1);
    }
    
    void cpu_emulator::zero_page_addressing()
    {
        this->op_address_ = this->mem_.read<uint8_t>(this->reg_.PC + 1);
        this->op_val_ = this->mem_.read<uint8_t>(op_address_);
    }
    
    void cpu_emulator::zero_page_x_addressing()
    {
        uint16_t addr = this->mem_.read<uint8_t>(this->reg_.PC + 1) + this->reg_.X;
        this->op_address_ = addr & 0xff;
        this->op_val_ = this->mem_.read<uint8_t>(op_address_);
    }
    
    void cpu_emulator::zero_page_y_addressing()
    {
        uint16_t addr = this->mem_.read<uint8_t>(this->reg_.PC + 1) + this->reg_.Y;
        this->op_address_ = addr & 0xff;
        this->op_val_ = this->mem_.read<uint8_t>(op_address_);
    }
    
    void cpu_emulator::relative_addressing()
    {
        //pass;
    }
    
    void cpu_emulator::absolute_addressing()
    {
        this->op_address_ = this->mem_.read<uint16_t>(this->reg_.PC + 1);
        this->op_val_ = this->mem_.read<uint8_t>(op_address_);
    }
    
    void cpu_emulator::absolute_x_addressing()
    {
        this->op_address_ = this->mem_.read<uint16_t>(this->reg_.PC + 1) + this->reg_.X;
        this->op_val_ = this->mem_.read<uint8_t>(op_address_);
    }
    
    void cpu_emulator::absolute_y_addressing()
    {
        this->op_address_ = this->mem_.read<uint16_t>(this->reg_.PC + 1) + this->reg_.Y;
        this->op_val_ = this->mem_.read<uint8_t>(op_address_);
    }
    
    void cpu_emulator::indirect_addressing()
    {
        this->op_address_ = this->mem_.read<uint16_t>(this->mem_.read<uint16_t>(this->reg_.PC + 1));
    }
    
    void cpu_emulator::indirect_x_addressing()
    {
        uint8_t t = this->mem_.read<uint8_t>(this->reg_.PC + 1) + this->reg_.X;
        uint16_t addr = this->mem_.read<uint16_t>(t);
        this->op_val_ = this->mem_.read<uint8_t>(addr);
    }
    
    void cpu_emulator::indirect_y_addressing()
    {
        uint8_t t = this->mem_.read<uint8_t>(this->reg_.PC + 1) + this->reg_.Y;
        uint16_t addr = this->mem_.read<uint16_t>(t);
        this->op_val_ = this->mem_.read<uint8_t>(addr);
    }
    
    void cpu_emulator::BRK()
    {
        
    }

    void cpu_emulator::TAX()
    {
        this->reg_.X = this->reg_.A;
        this->set_nzf(this->reg_.X);
    }
    
    void cpu_emulator::TAY()
    {
        this->reg_.Y = this->reg_.A;
        this->set_nzf(this->reg_.Y);
    }
    
    void cpu_emulator::TXA()
    {
        this->reg_.A = this->reg_.X;
        this->set_nzf(this->reg_.A);
    }
    
    void cpu_emulator::TYA()
    {
        this->reg_.A = this->reg_.Y;
        this->set_nzf(this->reg_.A);
    }
    
    void cpu_emulator::TSX()
    {
        this->reg_.X = this->reg_.SP;
        this->set_nzf(this->reg_.X);
    }
    
    void cpu_emulator::TXS()
    {
        this->reg_.SP = this->reg_.X;
        this->set_nzf(this->reg_.SP);
    }
    
    void cpu_emulator::INX()
    {
        this->reg_.X += 1;
        this->set_nzf(this->reg_.X);
    }
    
    void cpu_emulator::ADC()
    {
        this->reg_.A += this->op_val_;
        this->set_nzf(this->reg_.A);
    }

    void cpu_emulator::LDA()
    {
        this->reg_.A = this->op_val_;
        this->set_nzf(this->reg_.A);
    }
    
    void cpu_emulator::STA()
    {
        this->mem_.write(this->reg_.A, this->op_address_);
    }
    
    void cpu_emulator::load_code_segment(uint16_t segment_base_addr, const uint8_t *buf, size_t size)
    {
        uint8_t *ptr = this->mem_.map_offset_addr(segment_base_addr);
        memcpy(ptr, buf, size);
        this->mem_.set_code_segment_offset(segment_base_addr, segment_base_addr + size);
    }
    

    void cpu_emulator::debug_print_reg() const
    {
        std::cout << "------------------------" << std::endl
                  << "[DEBUG reg]" << std::endl
                  << "------------------------" << std::endl
                  << this->reg_ << std::endl;
    }
    
    
    uint8_t cpu_emulator::eval()
    {

        uint8_t opsize = 0;
        uint8_t opcode = this->mem_.read<uint8_t>(this->reg_.PC);
        
        switch (opcode) {
            case 0xa9:
                this->immediate_addressing();
                this->LDA();
                opsize = 2;
                break;
            case 0x8d:
                this->absolute_addressing();
                this->STA();
                opsize = 3;
                break;
            case 0xaa:
                this->implied_addressing();
                this->TAX();
                opsize = 1;
                break;
                
            case 0xe8:
                this->accumulator_addressing();
                this->INX();
                opsize = 1;
                break;
            
            case 0x69:
                this->immediate_addressing();
                this->ADC();
                opsize = 2;
                break;
                
            case 0x00:
                this->implied_addressing();
                this->BRK();
                opsize = 0;
                break;
                
            default:
                std::cout << "error instruction: " << opcode << std::endl;
                return ERROR_UNKNOWN_INSTRUCTION;
                break;
        }
        
        return opsize;
    }
    
    void cpu_emulator::run()
    {
        this->reset_reg();
    
        uint8_t opsize = 1;

        while (this->reg_.PC < this->mem_.get_code_segment_offset().end && opsize > 0) {
            opsize = this->eval();
            this->reg_.PC += opsize;
        }
    }


    
    void cpu_emulator::reset_reg()
    {
        this->reg_.PC = this->mem_.get_code_segment_offset().start;
        this->reg_.SP = this->mem_.get_stack_offset().start & 0xff; //low addr

        memset(&this->reg_.P, 0, sizeof(this->reg_.P));
        this->reg_.A = 0;
        this->reg_.X = 0;
        this->reg_.Y = 0;
        this->reg_.P.break_command = 1;
    }
    
    void cpu_emulator::reset_mem()
    {
        this->mem_.bzero();
    }

    
    void cpu_emulator::test()
    {
        
        uint8_t code1[] = {
            0xa9, 0x01,
            0x8d, 0x00, 0x02,
            0xa9, 0x05,
            0x8d, 0x01, 0x02,
            0xa9, 0x08,
            0x8d, 0x02, 0x02
        };
        
        uint8_t code2[] = {
            0xa9, 0xc0,
            0xaa,
            0xe8,
            0x69, 0xc4,
            0x00
        };
        
        
        this->load_code_segment(0, code1, sizeof(code1));
        this->run();
        assert(this->reg_.A == 0x08);
        assert(this->reg_.X == 0x00);
        assert(this->reg_.Y == 0x00);
        assert(this->reg_.SP == 0xff);
        assert(this->reg_.PC == 0x0f);
        
        
        this->debug_print_reg();
        this->mem_.debug_dump_ram();
        
        this->reset_mem();
        this->reset_reg();
        
        this->load_code_segment(0, code2, sizeof(code2));
        this->run();
        
        this->debug_print_reg();
        this->mem_.debug_dump_ram();
        
        
    }
    
}

