#ifndef utils_hpp
#define utils_hpp

#include <cstdio>
#include <cstdint>
#include <ostream>
#include <sstream>
#include <string>
#include <bitset>


template<typename T, typename T2>
static inline T get_bit(T bit, T2 n)
{
    return bit >> n & 0x01;
}


template<typename T, typename T2>
static inline void set_bit(T& bit, T2 n)
{
    T i = 0x1 << n;
    bit |= i;
}

template<typename T, typename T2>
static inline void clr_bit(T& bit, T2 n)
{
    T i = 0x1 << n;
    bit &= ~i;
}



static inline std::ostream& operator<<(std::ostream& os, unsigned char c)
{
    return os << static_cast<unsigned int>(c);
}



template <typename T>
static inline std::string to_binary_string(const T& x)
{
    std::stringstream ss;
    ss << std::bitset<sizeof(T) * 8>(x);
    return ss.str();
}


#endif /* utils_hpp */
