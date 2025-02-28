//
// Created by os on 5/5/24.
//

#include "../h/allocator.hpp"

MemoryAllocator::AllocMem* MemoryAllocator::mem_start = nullptr;
MemoryAllocator::AllocMem* MemoryAllocator::mem_end = nullptr;
bool MemoryAllocator::initialized = false;

void MemoryAllocator::init()
{
    if (initialized) return;
    initialized = true;

    mem_start = (AllocMem*)HEAP_START_ADDR;
    mem_end = (AllocMem*)((uint64)HEAP_END_ADDR - MEM_BLOCK_SIZE);

    *mem_start = {1, mem_end};
    *mem_end = {1, nullptr};
}

void* MemoryAllocator::allocate(size_t num_blocks)
{
    if (not initialized)  return nullptr;

    AllocMem *prev, *curr;
    for (prev = mem_start, curr = mem_start->next; curr; prev = curr, curr = curr->next)
    {
        uint64 free_mem_start = (uint64)prev  + prev->num_blocks * MEM_BLOCK_SIZE;
        uint64 free_mem_size = (uint64)curr - free_mem_start;
        if (free_mem_size >= num_blocks * MEM_BLOCK_SIZE)
        {
            AllocMem* new_block = (AllocMem*)(free_mem_start);
            *new_block = {num_blocks, curr};
            prev->next = new_block;
            return (void *)(free_mem_start + sizeof(AllocMem));
        }
    }

    return nullptr;
}

int MemoryAllocator::free(void *mem_ptr)
{
    if (not initialized) return -2;

    AllocMem *prev, *curr;
    for (prev = mem_start, curr = mem_start->next; curr != mem_end ;prev = curr, curr = curr->next)
        if ((uint64)curr == (uint64) mem_ptr - sizeof(AllocMem))
        {
            prev->next = curr->next;
            return 0;
        }

    return -1;
}

size_t MemoryAllocator::get_num_blocks(size_t num_bytes)
{
    num_bytes += sizeof(AllocMem);
    return (num_bytes + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
}




