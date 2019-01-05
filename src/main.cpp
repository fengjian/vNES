#include <iostream>
#include <vector>
#include <unordered_map>
#include "memory.hpp"
#include "cpu_6502.hpp"




int main(int argc, const char * argv[])
{
    nes::memory mem;
    nes::cpu_6502 cpu(mem);
    cpu.test();
    
    return 0;
}
