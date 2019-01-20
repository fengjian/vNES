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
    
    
    void cpu_6502::cross_page_cycles()
    {
        if ((this->op_address_ >> 8) != (this->reg_.PC >> 8)) {
            this->add_cycles_ = 1;
        }
        else {
            this->add_cycles_ = 0;
        }
    }

    void cpu_6502::implied_addressing()
    {
        this->add_cycles_ = 0;
    }
    
    void cpu_6502::accumulator_addressing()
    {
        this->add_cycles_ = 0;
    }
    
    void cpu_6502::immediate_addressing()
    {
        this->op_val_ = this->mem_.read<uint8_t>(this->reg_.PC);
        this->reg_.PC++;
        this->add_cycles_ = 0;
    }
    
    void cpu_6502::zero_page_addressing()
    {
        this->op_address_ = this->mem_.read<uint8_t>(this->reg_.PC);
        this->op_val_ = this->mem_.read<uint8_t>(op_address_);
        this->reg_.PC++;
        this->add_cycles_ = 0;
    }
    
    void cpu_6502::zero_page_x_addressing()
    {
        uint16_t addr = this->mem_.read<uint8_t>(this->reg_.PC) + this->reg_.X;
        this->op_address_ = addr & 0xff;
        this->op_val_ = this->mem_.read<uint8_t>(op_address_);
        this->reg_.PC++;
        this->add_cycles_ = 0;
    }
    
    void cpu_6502::zero_page_y_addressing()
    {
        uint16_t addr = this->mem_.read<uint8_t>(this->reg_.PC) + this->reg_.Y;
        this->op_address_ = addr & 0xff;
        this->op_val_ = this->mem_.read<uint8_t>(op_address_);
        this->reg_.PC++;
        this->add_cycles_ = 0;
    }
    
    void cpu_6502::relative_addressing()
    {
        this->op_address_ = this->mem_.read<uint8_t>(this->reg_.PC);
        this->reg_.PC++;
        if (this->op_address_ & 0x80) { 
            this->op_address_ -= 0x100; 
        }
        this->op_address_ += this->reg_.PC++;
        this->cross_page_cycles();
    }
    
    void cpu_6502::absolute_addressing()
    {
        this->op_address_ = this->mem_.read<uint16_t>(this->reg_.PC);
        this->op_val_ = this->mem_.read<uint8_t>(op_address_);
        this->reg_.PC += 2;
        this->add_cycles_ = 0;
    }
    
    void cpu_6502::absolute_x_addressing()
    {
        this->op_address_ = this->mem_.read<uint16_t>(this->reg_.PC) + this->reg_.X;
        this->op_val_ = this->mem_.read<uint8_t>(op_address_);
        this->reg_.PC += 2;
        this->cross_page_cycles();
    }
    
    void cpu_6502::absolute_y_addressing()
    {
        this->op_address_ = this->mem_.read<uint16_t>(this->reg_.PC) + this->reg_.Y;
        this->op_val_ = this->mem_.read<uint8_t>(op_address_);
        this->reg_.PC += 2;
        this->cross_page_cycles();
    }
    
    void cpu_6502::indirect_addressing()
    {
        uint16_t addr = this->mem_.read<uint16_t>(this->reg_.PC);
        if ((addr & 0xff) == 0xff) {
            this->op_address_ = (this->mem_.read<uint8_t>(addr & 0xff00) << 8) + this->mem_.read<uint8_t>(addr);
        } else {
            this->op_address_ = this->mem_.read<uint16_t>(addr);
        }

        this->reg_.PC += 2;
        this->add_cycles_ = 0;
    }
    
    void cpu_6502::indirect_x_addressing()
    {
        uint8_t addr = this->mem_.read<uint8_t>(this->reg_.PC);
        this->op_address_ = (this->mem_.read<uint8_t>((addr + this->reg_.X + 1) & 0xff) << 8) | this->mem_.read<uint8_t>((addr + this->reg_.X) & 0xff);
        this->op_val_ = this->mem_.read<uint8_t>(this->op_address_);
        this->reg_.PC++;
        this->add_cycles_ = 0;
    }
    
    void cpu_6502::indirect_y_addressing()
    {
        uint8_t addr = this->mem_.read<uint8_t>(this->reg_.PC);
        this->op_address_ = (((this->mem_.read<uint8_t>((addr + 1) & 0xff) << 8) | this->mem_.read<uint8_t>(addr)) + this->reg_.Y) & 0xffff;
        this->op_val_ = this->mem_.read<uint8_t>(this->op_address_);
        this->reg_.PC++;
        this->cross_page_cycles();
    }
    
    void cpu_6502::NOP()
    {
    }

    void cpu_6502::BRK()
    {
        
    }

    void cpu_6502::TAX()
    {
        this->reg_.X = this->reg_.A;
        this->set_nzf(this->reg_.X);
    }
    
    void cpu_6502::TAY()
    {
        this->reg_.Y = this->reg_.A;
        this->set_nzf(this->reg_.Y);
    }
    
    void cpu_6502::TXA()
    {
        this->reg_.A = this->reg_.X;
        this->set_nzf(this->reg_.A);
    }
    
    void cpu_6502::TYA()
    {
        this->reg_.A = this->reg_.Y;
        this->set_nzf(this->reg_.A);
    }
    
    void cpu_6502::TSX()
    {
        this->reg_.X = this->reg_.SP;
        this->set_nzf(this->reg_.X);
    }
    
    void cpu_6502::TXS()
    {
        this->reg_.SP = this->reg_.X;
    }
    
    void cpu_6502::INX()
    {
        this->reg_.X++;
        this->set_nzf(this->reg_.X);
    }

    void cpu_6502::INY()
    {
        this->reg_.Y++;
        this->set_nzf(this->reg_.Y);
    }

    void cpu_6502::INC()
    {
        uint8_t t = this->op_val_ + 1;
        this->mem_.write(t, this->op_address_);
        this->set_nzf(t);
    }

    void cpu_6502::DEX()
    {
        this->reg_.X--;
        this->set_nzf(this->reg_.X);
    }

    void cpu_6502::DEY()
    {
        this->reg_.Y--;
        this->set_nzf(this->reg_.Y);
    }

    void cpu_6502::DEC()
    {
        uint8_t t = this->op_val_ - 1;
        this->mem_.write(t, this->op_address_);
        this->set_nzf(t);
    }

    void cpu_6502::ORA()
    {
        this->reg_.A |= this->op_val_;
        this->set_nzf(this->reg_.A);
    }

    void cpu_6502::AND()
    {
        this->reg_.A &= this->op_val_;
        this->set_nzf(this->reg_.A);
    }

    void cpu_6502::EOR()
    {
        this->reg_.A ^= this->op_val_;
        this->set_nzf(this->reg_.A);
    }

    void cpu_6502::ASL()
    {
        this->reg_.P.carry_flag = this->op_val_ & 0x80 ? 1 : 0;
        this->op_val_ <<= 1;
        this->mem_.write(this->op_val_, this->op_address_);
        this->set_nzf(this->op_val_);
    }

    void cpu_6502::ASLA()
    {
        this->reg_.P.carry_flag = this->reg_.A & 0x80 ? 1 : 0;
        this->reg_.A <<= 1;
        this->set_nzf(this->reg_.A);
    }

    void cpu_6502::ROL()
    {
        uint8_t old_carry = this->reg_.P.carry_flag;
        this->reg_.P.carry_flag = this->op_val_ & 0x80 ? 1 : 0;
        this->op_val_ <<= 1;
        this->op_val_ |= old_carry ? 1 : 0;
        this->mem_.write(this->op_val_, this->op_address_);
        this->set_nzf(this->op_val_);
    }

    void cpu_6502::ROLA()
    {
        uint8_t old_carry = this->reg_.P.carry_flag;
        this->reg_.P.carry_flag = this->reg_.A & 0x80 ? 1 : 0;
        this->reg_.A <<= 1;
        this->reg_.A |= old_carry ? 1 : 0;
        this->set_nzf(this->reg_.A);
    }

    void cpu_6502::ROR()
    {
        uint8_t old_carry = this->reg_.P.carry_flag;
        this->reg_.P.carry_flag = this->op_val_ & 0x80 ? 1 : 0;
        this->op_val_ <<= 1;
        this->op_val_ |= (old_carry ? 1 : 0) << 7;
        this->mem_.write(this->op_val_, this->op_address_);
        this->set_nzf(this->op_val_);
    }

    void cpu_6502::RORA()
    {
        uint8_t old_carry = this->reg_.P.carry_flag;
        this->reg_.P.carry_flag = this->reg_.A & 0x80 ? 1 : 0;
        this->reg_.A <<= 1;
        this->reg_.A |= (old_carry ? 1 : 0) << 7;
        this->set_nzf(this->reg_.A);
    }

    void cpu_6502::LSR()
    {
        this->reg_.P.carry_flag = this->op_val_ & 0x01;
        this->op_val_ >>= 1;
        this->mem_.write(this->op_val_, this->op_address_);
        this->set_nzf(this->op_val_);
    }

    void cpu_6502::LSRA()
    {
        this->reg_.P.carry_flag = this->reg_.A & 0x01;
        this->reg_.A >>= 1;
        this->set_nzf(this->reg_.A);
    }
    
    void cpu_6502::ADC()
    {
        uint16_t tmp = this->op_val_ + this->reg_.A + this->reg_.P.carry_flag;
        this->reg_.P.carry_flag = tmp & 0xff00 ? 1 : 0;
        this->reg_.P.overflow_flag = ((this->op_val_ ^ tmp) & (this->reg_.A ^ tmp)) & 0x80 ? 1 : 0;
        this->reg_.A = (uint8_t)(tmp & 0xff);
        this->set_nzf(this->reg_.A);
    }

    void cpu_6502::SBC()
    {
        uint16_t tmp = this->reg_.A - this->op_val_ - (1 - this->reg_.P.carry_flag);
        this->reg_.P.carry_flag = (tmp & 0xff00) == 0;
        this->reg_.P.overflow_flag = ((this->reg_.A ^ this->op_val_) & (this->reg_.A ^ tmp)) & 0x80 ? 1 : 0;
        this->reg_.A = (uint8_t)(tmp & 0xff);
        this->set_nzf(this->reg_.A);
    }

    void cpu_6502::BMI()
    {
        if (this->reg_.P.negative_flag == 1) {
            this->reg_.PC = this->op_address_;
        }
    }

    void cpu_6502::BCS()
    {
        if (this->reg_.P.carry_flag == 1) {
            this->reg_.PC = this->op_address_;
        }
    }

    void cpu_6502::BEQ()
    {
        if (this->reg_.P.zero_flag == 1) {
            this->reg_.PC = this->op_address_;
        }
    }

    void cpu_6502::BVS()
    {
        if (this->reg_.P.overflow_flag == 1) {
            this->reg_.PC = this->op_address_;
        }
    }

    void cpu_6502::BPL()
    {
        if (this->reg_.P.negative_flag == 0) {
            this->reg_.PC = this->op_address_;
        }
    }

    void cpu_6502::BCC()
    {
        if (this->reg_.P.carry_flag == 0) {
            this->reg_.PC = this->op_address_;
        }
    }

    void cpu_6502::BNE()
    {
        if (this->reg_.P.zero_flag == 0) {
            this->reg_.PC = this->op_address_;
        }
    }

    void cpu_6502::BVC()
    {
        if (this->reg_.P.overflow_flag == 0) {
            this->reg_.PC = this->op_address_;
        }
    }

    void cpu_6502::BIT()
    {
        this->reg_.P.overflow_flag = this->op_val_ & 0x40 ? 1 : 0;
        this->reg_.P.negative_flag = this->op_val_ & 0x80 ? 1 : 0;
        this->reg_.P.zero_flag = this->op_val_ & this->reg_.A ? 0 : 1;
    }

    void cpu_6502::CMP()
    {
        int tmp = this->reg_.A - this->op_val_;
        this->reg_.P.carry_flag = tmp >= 0 ? 1 : 0;
        this->set_nzf((uint8_t)tmp);
    }

    void cpu_6502::CPX()
    {
        int tmp = this->reg_.X - this->op_val_;
        this->reg_.P.carry_flag = tmp >= 0 ? 1 : 0;
        this->set_nzf((uint8_t)tmp);
    }

    void cpu_6502::CPY()
    {
        int tmp = this->reg_.Y - this->op_val_;
        this->reg_.P.carry_flag = tmp >= 0 ? 1 : 0;
        this->set_nzf((uint8_t)tmp);
    }

    void cpu_6502::PHA()
    {
        this->push(this->reg_.A);
    }

    void cpu_6502::PHP()
    {
        this->push((uint8_t)this->reg_.P | 0x30);
    }

    void cpu_6502::PLA()
    {
        this->reg_.A = this->pop<uint8_t>();
        this->set_nzf(this->reg_.A);
    }

    void cpu_6502::PLP()
    {
        this->reg_.P.set_flag((this->pop<uint8_t>() & 0xEF) | 0x20);
    }

    void cpu_6502::RTS()
    {
        this->reg_.PC = this->pop<uint16_t>() + 1;
    }

    void cpu_6502::RTI()
    {
        this->reg_.P.set_flag(this->pop<uint8_t>() | FLAG_EFFECT);
        this->reg_.PC = this->pop<uint16_t>();
    }

    void cpu_6502::JMP()
    {
        this->reg_.PC = this->op_address_;
    }

    void cpu_6502::JSR()
    {
        this->push(this->reg_.PC - 1);
        this->reg_.PC = this->op_address_;
    }

    // load && store
    void cpu_6502::LDA()
    {
        this->reg_.A = this->op_val_;
        this->set_nzf(this->reg_.A);
    }

    void cpu_6502::LDX()
    {
        this->reg_.X = this->op_val_;
        this->set_nzf(this->reg_.X);
    }

    void cpu_6502::LDY()
    {
        this->reg_.Y = this->op_val_;
        this->set_nzf(this->reg_.Y);
    }
    
    void cpu_6502::STA()
    {
        this->mem_.write(this->reg_.A, this->op_address_);
    }

    void cpu_6502::STX()
    {
        this->mem_.write(this->reg_.X, this->op_address_);
    }

    void cpu_6502::STY()
    {
        this->mem_.write(this->reg_.Y, this->op_address_);
    }

    void cpu_6502::CLC()
    {
        this->reg_.P.carry_flag = 0;
    }

    void cpu_6502::CLI()
    {
        this->reg_.P.interrupt_disable = 0;
    }

    void cpu_6502::CLD()
    {
        this->reg_.P.decimal_mode = 0;
    }

    void cpu_6502::CLV()
    {
        this->reg_.P.overflow_flag = 0;
    }

    void cpu_6502::SEC()
    {
        this->reg_.P.carry_flag = 1;
    }

    void cpu_6502::SEI()
    {
        this->reg_.P.interrupt_disable = 1;
    }

    void cpu_6502::SED()
    {
        this->reg_.P.decimal_mode = 1;
    }
    
    void cpu_6502::load_code_segment(uint16_t segment_base_addr, const uint8_t *buf, size_t size)
    {
        uint8_t *ptr = this->mem_.map_offset_addr(segment_base_addr);
        memcpy(ptr, buf, size);
        this->mem_.set_code_segment_offset(segment_base_addr, segment_base_addr + (uint16_t)size);
    }
    

    void cpu_6502::debug_print_reg() const
    {
        std::cout << "------------------------" << std::endl
                  << "[DEBUG reg]" << std::endl
                  << "------------------------" << std::endl
                  << this->reg_ << std::endl;
    }
    
#define ERROR_UNKNOWN_INSTRUCTION -1
#define BRK_INSTRUCTION -2

    uint8_t cpu_6502::eval(int& cycles)
    {
        uint8_t opcode = this->mem_.read<uint8_t>(this->reg_.PC);
        this->reg_.PC++;
 
        switch (opcode) {
            case 0x00: this->implied_addressing();     this->BRK();  cycles -= 7; break;
            case 0x01: this->indirect_x_addressing();  this->ORA();  cycles -= 6; break;
            case 0x04: this->zero_page_addressing();    this->NOP();  cycles -= 1; break;
            case 0x05: this->zero_page_addressing();    this->ORA();  cycles -= 3; break;
            case 0x06: this->zero_page_addressing();    this->ASL();  cycles -= 5; break;
            case 0x08: this->implied_addressing();     this->PHP();  cycles -= 3; break;
            case 0x09: this->immediate_addressing();   this->ORA();  cycles -= 2; break;
            case 0x0A: this->accumulator_addressing(); this->ASLA(); cycles -= 2; break;
            case 0x0C: this->absolute_addressing();    this->NOP();  cycles -= 1; break;
            case 0x0D: this->absolute_addressing();    this->ORA();  cycles -= 4; break;
            case 0x0E: this->absolute_addressing();    this->ASL();  cycles -= 6; break;
            case 0x10: this->relative_addressing();    this->BPL();  cycles -= 2; break;
            case 0x11: this->indirect_y_addressing();  this->ORA();  cycles -= 5; break;
            case 0x14: this->zero_page_x_addressing();  this->NOP();  cycles -= 1; break;
            case 0x15: this->zero_page_x_addressing();  this->ORA();  cycles -= 4; break;
            case 0x16: this->zero_page_x_addressing();  this->ASL();  cycles -= 6; break;
            case 0x18: this->implied_addressing();     this->CLC();  cycles -= 2; break;
            case 0x19: this->absolute_y_addressing();  this->ORA();  cycles -= 4; break;
            case 0x1A: this->accumulator_addressing(); this->NOP();  cycles -= 1; break;
            case 0x1C: this->absolute_x_addressing();  this->NOP();  cycles -= 1; break;
            case 0x1D: this->absolute_x_addressing();  this->ORA();  cycles -= 4; break;
            case 0x1E: this->absolute_x_addressing();  this->ASL();  cycles -= 7; break;
            case 0x20: this->absolute_addressing();    this->JSR();  cycles -= 6; break;
            case 0x21: this->indirect_x_addressing();  this->AND();  cycles -= 6; break;
            case 0x24: this->zero_page_addressing();    this->BIT();  cycles -= 3; break;
            case 0x25: this->zero_page_addressing();    this->AND();  cycles -= 3; break;
            case 0x26: this->zero_page_addressing();    this->ROL();  cycles -= 5; break;
            case 0x28: this->implied_addressing();     this->PLP();  cycles -= 3; break;
            case 0x29: this->immediate_addressing();   this->AND();  cycles -= 2; break;
            case 0x2A: this->accumulator_addressing(); this->ROLA(); cycles -= 2; break;
            case 0x2C: this->absolute_addressing();    this->BIT();  cycles -= 4; break;
            case 0x2D: this->absolute_addressing();    this->AND();  cycles -= 2; break;
            case 0x2E: this->absolute_addressing();    this->ROL();  cycles -= 6; break;
            case 0x30: this->relative_addressing();    this->BMI();  cycles -= 2; break;
            case 0x31: this->indirect_y_addressing();  this->AND();  cycles -= 5; break;
            case 0x34: this->zero_page_x_addressing();  this->NOP();  cycles -= 1; break;
            case 0x35: this->zero_page_x_addressing();  this->AND();  cycles -= 4; break;
            case 0x36: this->zero_page_x_addressing();  this->ROL();  cycles -= 6; break;
            case 0x38: this->implied_addressing();     this->SEC();  cycles -= 2; break;
            case 0x39: this->absolute_y_addressing();  this->AND();  cycles -= 4; break;
            case 0x3A: this->accumulator_addressing(); this->NOP();  cycles -= 1; break;
            case 0x3C: this->absolute_x_addressing();  this->NOP();  cycles -= 1; break;
            case 0x3D: this->absolute_x_addressing();  this->AND();  cycles -= 4; break;
            case 0x3E: this->absolute_x_addressing();  this->ROL();  cycles -= 7; break;
            case 0x40: this->implied_addressing();     this->RTI();  cycles -= 6; break;
            case 0x41: this->indirect_x_addressing();  this->EOR();  cycles -= 6; break;
            case 0x44: this->zero_page_addressing();    this->NOP();  cycles -= 1; break;
            case 0x45: this->zero_page_addressing();    this->EOR();  cycles -= 3; break;
            case 0x46: this->zero_page_addressing();    this->LSR();  cycles -= 5; break;
            case 0x48: this->implied_addressing();     this->PHA();  cycles -= 3; break;
            case 0x49: this->immediate_addressing();   this->EOR();  cycles -= 2; break;
            case 0x4A: this->accumulator_addressing(); this->LSRA(); cycles -= 2; break;
            case 0x4C: this->absolute_addressing();    this->JMP();  cycles -= 3; break;
            case 0x4D: this->absolute_addressing();    this->EOR();  cycles -= 4; break;
            case 0x4E: this->absolute_addressing();    this->LSR();  cycles -= 6; break;
            case 0x50: this->relative_addressing();    this->BVC();  cycles -= 2; break;
            case 0x51: this->indirect_y_addressing();  this->EOR();  cycles -= 5; break;
            case 0x54: this->zero_page_x_addressing();  this->NOP();  cycles -= 1; break;
            case 0x55: this->zero_page_x_addressing();  this->EOR();  cycles -= 4; break;
            case 0x56: this->zero_page_x_addressing();  this->LSR();  cycles -= 6; break;
            case 0x59: this->absolute_y_addressing();  this->EOR();  cycles -= 4; break;
            case 0x5A: this->accumulator_addressing(); this->NOP();  cycles -= 1; break;
            case 0x5C: this->absolute_x_addressing();  this->NOP();  cycles -= 1; break;
            case 0x5D: this->absolute_x_addressing();  this->EOR();  cycles -= 4; break;
            case 0x5E: this->absolute_x_addressing();  this->LSR();  cycles -= 7; break;
            case 0x60: this->implied_addressing();     this->RTS();  cycles -= 6; break;
            case 0x61: this->indirect_x_addressing();  this->ADC();  cycles -= 6; break;
            case 0x64: this->zero_page_addressing();    this->NOP();  cycles -= 1; break;
            case 0x65: this->zero_page_addressing();    this->ADC();  cycles -= 3; break;
            case 0x66: this->zero_page_addressing();    this->ROR();  cycles -= 5; break;
            case 0x68: this->implied_addressing();     this->PLA();  cycles -= 4; break;
            case 0x69: this->immediate_addressing();   this->ADC();  cycles -= 2; break;
            case 0x6A: this->accumulator_addressing(); this->RORA(); cycles -= 2; break;
            case 0x6C: this->indirect_addressing();    this->JMP();  cycles -= 5; break;
            case 0x6D: this->absolute_addressing();    this->ADC();  cycles -= 4; break;
            case 0x6E: this->absolute_addressing();    this->ROR();  cycles -= 6; break;
            case 0x70: this->relative_addressing();    this->BVS();  cycles -= 2; break;
            case 0x71: this->indirect_y_addressing();  this->ADC();  cycles -= 5; break;
            case 0x74: this->zero_page_addressing();    this->NOP();  cycles -= 1; break;
            case 0x75: this->zero_page_x_addressing();  this->ADC();  cycles -= 4; break;
            case 0x76: this->zero_page_x_addressing();  this->ROR();  cycles -= 6; break;
            case 0x78: this->implied_addressing();     this->SEI();  cycles -= 2; break;
            case 0x79: this->absolute_y_addressing();  this->ADC();  cycles -= 4; break;
            case 0x7A: this->accumulator_addressing(); this->NOP();  cycles -= 1; break;
            case 0x7C: this->absolute_x_addressing();  this->NOP();  cycles -= 1; break;
            case 0x7D: this->absolute_x_addressing();  this->ADC();  cycles -= 4; break;
            case 0x7E: this->absolute_x_addressing();  this->ROR();  cycles -= 7; break;
            case 0x80: this->immediate_addressing();   this->NOP();  cycles -= 1; break;
            case 0x81: this->indirect_x_addressing();  this->STA();  cycles -= 6; break;
            case 0x84: this->zero_page_addressing();    this->STY();  cycles -= 3; break;
            case 0x85: this->zero_page_addressing();    this->STA();  cycles -= 3; break;
            case 0x86: this->zero_page_addressing();    this->STX();  cycles -= 3; break;
            case 0x88: this->implied_addressing();     this->DEY();  cycles -= 2; break;
            case 0x8A: this->implied_addressing();     this->TXA();  cycles -= 2; break;
            case 0x8C: this->absolute_addressing();    this->STY();  cycles -= 4; break;
            case 0x8D: this->absolute_addressing();    this->STA();  cycles -= 4; break;
            case 0x8E: this->absolute_addressing();    this->STX();  cycles -= 4; break;
            case 0x90: this->relative_addressing();    this->BCC();  cycles -= 2; break;
            case 0x91: this->indirect_y_addressing();  this->STA();  cycles -= 6; break;
            case 0x94: this->zero_page_x_addressing();  this->STY();  cycles -= 4; break;
            case 0x95: this->zero_page_x_addressing();  this->STA();  cycles -= 4; break;
            case 0x96: this->zero_page_y_addressing();  this->STX();  cycles -= 4; break;
            case 0x98: this->implied_addressing();     this->TYA();  cycles -= 2; break;
            case 0x99: this->absolute_y_addressing();  this->STA();  cycles -= 5; break;
            case 0x9A: this->implied_addressing();     this->TXS();  cycles -= 2; break;
            case 0x9D: this->absolute_x_addressing();  this->STA();  cycles -= 5; break;
            case 0xA0: this->immediate_addressing();   this->LDY();  cycles -= 2; break;
            case 0xA1: this->indirect_x_addressing();  this->LDA();  cycles -= 6; break;
            case 0xA2: this->immediate_addressing();   this->LDX();  cycles -= 2; break;
            case 0xA4: this->zero_page_addressing();    this->LDY();  cycles -= 3; break;
            case 0xA5: this->zero_page_addressing();    this->LDA();  cycles -= 3; break;
            case 0xA6: this->zero_page_addressing();    this->LDX();  cycles -= 3; break;
            case 0xA8: this->implied_addressing();     this->TAY();  cycles -= 3; break;
            case 0xA9: this->immediate_addressing();   this->LDA();  cycles -= 2; break;
            case 0xAA: this->implied_addressing();     this->TAX();  cycles -= 2; break;
            case 0xAC: this->absolute_addressing();    this->LDY();  cycles -= 4; break;
            case 0xAD: this->absolute_addressing();    this->LDA();  cycles -= 4; break;
            case 0xAE: this->absolute_addressing();    this->LDX();  cycles -= 4; break;
            case 0xB0: this->relative_addressing();    this->BCS();  cycles -= 2; break;
            case 0xB1: this->indirect_y_addressing();  this->LDA();  cycles -= 5; break;
            case 0xB4: this->zero_page_x_addressing();  this->LDY();  cycles -= 4; break;
            case 0xB5: this->zero_page_x_addressing();  this->LDA();  cycles -= 4; break;
            case 0xB6: this->zero_page_y_addressing();  this->LDX();  cycles -= 4; break;
            case 0xB8: this->implied_addressing();     this->CLV();  cycles -= 2; break;
            case 0xB9: this->absolute_y_addressing();  this->LDA();  cycles -= 4; break;
            case 0xBA: this->implied_addressing();     this->TSX();  cycles -= 2; break;
            case 0xBC: this->absolute_x_addressing();  this->LDY();  cycles -= 4; break;
            case 0xBD: this->absolute_x_addressing();  this->LDA();  cycles -= 4; break;
            case 0xBE: this->absolute_y_addressing();  this->LDX();  cycles -= 4; break;
            case 0xC0: this->immediate_addressing();   this->CPY();  cycles -= 2; break;
            case 0xC1: this->indirect_x_addressing();  this->CMP();  cycles -= 6; break;
            case 0xC4: this->zero_page_addressing();    this->CPY();  cycles -= 3; break;
            case 0xC5: this->zero_page_addressing();    this->CMP();  cycles -= 3; break;
            case 0xC6: this->zero_page_addressing();    this->DEC();  cycles -= 5; break;
            case 0xC8: this->implied_addressing();     this->INY();  cycles -= 2; break;
            case 0xC9: this->immediate_addressing();   this->CMP();  cycles -= 2; break;
            case 0xCA: this->implied_addressing();     this->DEX();  cycles -= 2; break;
            case 0xCC: this->absolute_addressing();    this->CPY();  cycles -= 4; break;
            case 0xCD: this->absolute_addressing();    this->CMP();  cycles -= 4; break;
            case 0xCE: this->absolute_addressing();    this->DEC();  cycles -= 6; break;
            case 0xD0: this->relative_addressing();    this->BNE();  cycles -= 2; break;
            case 0xD1: this->indirect_y_addressing();  this->CMP();  cycles -= 5; break;
            case 0xD4: this->zero_page_x_addressing();  this->NOP();  cycles -= 1; break;
            case 0xD5: this->zero_page_x_addressing();  this->CMP();  cycles -= 5; break;
            case 0xD6: this->zero_page_x_addressing();  this->DEC();  cycles -= 6; break;
            case 0xD8: this->implied_addressing();     this->CLD();  cycles -= 2; break;
            case 0xD9: this->absolute_y_addressing();  this->CMP();  cycles -= 4; break;
            case 0xDA: this->accumulator_addressing(); this->NOP();  cycles -= 1; break;
            case 0xDC: this->absolute_x_addressing();  this->NOP();  cycles -= 1; break;
            case 0xDD: this->absolute_x_addressing();  this->CMP();  cycles -= 4; break;
            case 0xDE: this->absolute_x_addressing();  this->DEC();  cycles -= 7; break;
            case 0xE0: this->immediate_addressing();   this->CPX();  cycles -= 2; break;
            case 0xE1: this->indirect_x_addressing();  this->SBC();  cycles -= 6; break;
            case 0xE4: this->zero_page_addressing();    this->CPX();  cycles -= 3; break;
            case 0xE5: this->zero_page_addressing();    this->SBC();  cycles -= 3; break;
            case 0xE6: this->zero_page_addressing();    this->INC();  cycles -= 5; break;
            case 0xE8: this->implied_addressing();     this->INX();  cycles -= 2; break;
            case 0xE9: this->immediate_addressing();   this->SBC();  cycles -= 2; break;
            case 0xEA: this->accumulator_addressing(); this->NOP();  cycles -= 2; break;
            case 0xEC: this->absolute_addressing();    this->CPX();  cycles -= 4; break;
            case 0xED: this->absolute_addressing();    this->SBC();  cycles -= 4; break;
            case 0xEE: this->absolute_addressing();    this->INC();  cycles -= 6; break;
            case 0xF0: this->relative_addressing();    this->BEQ();  cycles -= 2; break;
            case 0xF1: this->indirect_y_addressing();  this->SBC();  cycles -= 5; break;
            case 0xF4: this->zero_page_x_addressing();  this->NOP();  cycles -= 1; break;
            case 0xF5: this->zero_page_x_addressing();  this->SBC();  cycles -= 4; break;
            case 0xF6: this->zero_page_x_addressing();  this->INC();  cycles -= 6; break;
            case 0xF8: this->implied_addressing();     this->SED();  cycles -= 2; break;
            case 0xF9: this->absolute_y_addressing();  this->SBC();  cycles -= 4; break;
            case 0xFA: this->accumulator_addressing(); this->NOP();  cycles -= 1; break;
            case 0xFC: this->absolute_x_addressing();  this->NOP();  cycles -= 1; break;
            case 0xFD: this->absolute_x_addressing();  this->SBC();  cycles -= 4; break;
            case 0xFE: this->absolute_x_addressing();  this->INC();  cycles -= 7; break;
                
            default:
                std::cout << "error instruction: " << opcode << std::endl;
                return ERROR_UNKNOWN_INSTRUCTION;
                break;
        }
        
        return 0;
    }


    void cpu_6502::toggle_frame_irq(uint8_t state)
    {
        this->mem_.write(state, g_frame_irq_state_address);
    }

    void cpu_6502::toggle_apu(uint8_t state)
    {
        this->mem_.write(state, g_apu_state_address);
    }

    /*
        At power-up
            http://wiki.nesdev.com/w/index.php/CPU_power_up_state
            CPU power up state
            The following results are from a US (NTSC) NES, original front-loading design, RP2A03G CPU chip, NES-CPU-07 main board revision,
            manufactured in 1988. The memory values are probably slightly different for each individual NES console.
            Please note that you should NOT rely on the state of any registers after Power-UP and especially not the stack register and RAM ($0000-$07FF).
            P = $34[1] (IRQ disabled)[2]
            A, X, Y = 0
            S = $FD
            $4017 = $00 (frame irq enabled)
            $4015 = $00 (all channels disabled)
            $4000-$400F = $00 (not sure about $4010-$4013)
            All 15 bits of noise channel LFSR = $0000[3]. The first time the LFSR is clocked from the all-0s state, it will shift in a 1.
            Internal memory ($0000-$07FF) has unreliable startup state. Some machines may have consistent RAM contents at power-on, but others do not.
    */

    void cpu_6502::power_up()
    {
        //	P = 00110100
        this->reg_.P.negative_flag = 0;
        this->reg_.P.overflow_flag = 0;
        this->reg_.P.no_effect = 1;
        this->reg_.P.break_command = 1;
        this->reg_.P.decimal_mode = 0;
        this->reg_.P.interrupt_disable = 1;
        this->reg_.P.zero_flag = 0;
        this->reg_.P.carry_flag = 0;

        this->reg_.A = 0;
        this->reg_.X = 0;
        this->reg_.Y = 0;
        this->reg_.SP = 0xfd;


        this->toggle_frame_irq(0x00);
        this->toggle_apu(0x00);

        this->mem_.bzero(0x4000, 0x400f + 1);

        //TODO init LSFR

        this->reg_.PC = this->mem_.read<uint16_t>(0xfffc);
    }

/*
    After reset

        A, X, Y were not affected
        S was decremented by 3 (but nothing was written to the stack)
        The I (IRQ disable) flag was set to true (status ORed with $04)
        The internal memory was unchanged
        APU mode in $4017 was unchanged
        APU was silenced ($4015 = 0)
*/
    void cpu_6502::reset()
    {
        this->reg_.SP -= 3;
        this->reg_.P.interrupt_disable = 1;
        this->toggle_apu();

        this->reg_.PC = this->mem_.read<uint16_t>(0xfffc);
    }

    void cpu_6502::interrupt()
    {
    }
    
    void cpu_6502::run()
    {
        this->reset_reg();

        int cpu_cycles = 1000, cycles = 1000;
        int tmp = cycles;

        while (this->reg_.PC < this->mem_.get_code_segment_offset().end) {
            uint8_t status = this->eval(cycles);
            cycles -= this->add_cycles_;

            if (status != 0) {
                break;
            }
        }
        cpu_cycles += tmp - cycles;

    }

    void cpu_6502::reset_reg()
    {
        this->reg_.PC = this->mem_.get_code_segment_offset().start;
        this->reg_.SP = g_stack_offset.start & 0xff; //low addr

        memset(&this->reg_.P, 0, sizeof(this->reg_.P));
        this->reg_.A = 0;
        this->reg_.X = 0;
        this->reg_.Y = 0;
        this->reg_.P.break_command = 1;
    }
    
    void cpu_6502::reset_mem()
    {
        this->mem_.bzero();
    }

    
    void cpu_6502::test()
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
        //this->mem_.debug_dump_ram();
        
        this->reset_mem();
        this->reset_reg();
        
        this->load_code_segment(0, code2, sizeof(code2));
        this->run();
        
        this->debug_print_reg();
        //this->mem_.debug_dump_ram();
        
        
    }
    
}


