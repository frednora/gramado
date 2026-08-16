
// syscalls.hpp
// libcpp syscalls for Gramado OS.
// 2024 - Created by Fred Nora.

#pragma once 

namespace LibCPP {

class Syscalls {
public:

    void *sc80 ( 
        unsigned long a, 
        unsigned long b, 
        unsigned long c, 
        unsigned long d );

    void *sc81 ( 
        unsigned long a, 
        unsigned long b, 
        unsigned long c, 
        unsigned long d );

    void *sc82 ( 
        unsigned long a, 
        unsigned long b, 
        unsigned long c, 
        unsigned long d );

    void *sc83 ( 
        unsigned long a, 
        unsigned long b, 
        unsigned long c, 
        unsigned long d );
};

}

