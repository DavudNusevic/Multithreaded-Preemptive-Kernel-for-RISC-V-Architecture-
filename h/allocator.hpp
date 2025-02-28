//
// Created by os on 5/5/24.
//

#ifndef TEMPLEOS_ALLOCATOR_HPP
#define TEMPLEOS_ALLOCATOR_HPP

#include "../lib/hw.h"
#include "printing.hpp"

class MemoryAllocator
{
public:
    /**
    * Allocates memory for a specified number of blocks.
    * Blocks are the size of MEM_BLOCK_SIZE (defined in hw.h)
    * Beware allocated memory includes list header needed for allocator
    *
    * @param num_blocks The number of memory blocks to allocate.
    * @return A pointer to the allocated memory, or nullptr if allocation fails.
    */
    static void* allocate(size_t num_blocks);

    /**
    * Deallocates memory given as argument, that was previously allocated with method allocate.
    * If given pointer to memory wasn't returned from allocate method behaviour is undefined.
    *
    * @param mem_ptr Pointer to memory that will be deallocated
    * @return Status code: 0 for success, negative value otherwise.
    *
    */
    static int free(void* mem_ptr);

    /**
    * Initializes structures for allocation and deallocation.
    * Needs to be called before use of MemoryAllocator methods.
    * Only first call has effect.
    */
    static void init();

    /**
     *
     * @param num_bytes
     * @return
     */
    static size_t get_num_blocks (size_t num_bytes);

    static void print()
    {
        for (AllocMem* curr = mem_start; curr; curr = curr->next)
        {
            printUInt64(curr->num_blocks);
            printString("-----------------------------------------------------\n");
        }
    }

private:
    struct AllocMem
    {
        size_t num_blocks;
        AllocMem* next;
    };

    static AllocMem *mem_start, *mem_end;
    static bool initialized;
};


#endif //TEMPLEOS_ALLOCATOR_HPP
