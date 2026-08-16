// rtl.hpp

#pragma once

namespace rtl {

extern "C" {
    void* sc80(unsigned long a, unsigned long b, unsigned long c, unsigned long d);
    void* sc81(unsigned long a, unsigned long b, unsigned long c, unsigned long d);
    void* sc82(unsigned long a, unsigned long b, unsigned long c, unsigned long d);
    void* sc83(unsigned long a, unsigned long b, unsigned long c, unsigned long d);
}

class Syscall {
public:
    static void* cpp_sc80(unsigned long a, unsigned long b, unsigned long c, unsigned long d);
    static void* cpp_sc81(unsigned long a, unsigned long b, unsigned long c, unsigned long d);
    static void* cpp_sc82(unsigned long a, unsigned long b, unsigned long c, unsigned long d);
    static void* cpp_sc83(unsigned long a, unsigned long b, unsigned long c, unsigned long d);
};

} // namespace rtl

