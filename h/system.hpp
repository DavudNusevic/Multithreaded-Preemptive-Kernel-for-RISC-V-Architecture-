//
// Created by os on 5/17/24.
//

#ifndef TEMPLEOS_SYSTEM_HPP
#define TEMPLEOS_SYSTEM_HPP

#include "../lib/hw.h"
#include "allocator.hpp"

struct SysStruct
{
    void *operator new(size_t size) { return MemoryAllocator::allocate(MemoryAllocator::get_num_blocks(size)); }
    void operator delete(void *mem_ptr) { MemoryAllocator::free(mem_ptr); }
    void *operator new[](size_t size) { return MemoryAllocator::allocate(MemoryAllocator::get_num_blocks(size)); }
    void operator delete[](void *mem_ptr) { MemoryAllocator::free(mem_ptr); }
};

#endif //TEMPLEOS_SYSTEM_HPP
