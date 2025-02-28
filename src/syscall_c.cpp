//
// Created by os on 5/11/24.
//

#include "../h/syscall_c.hpp"
#include "../h/tcb.hpp"
#include "../h/allocator.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_cpp.hpp"

void syscall(uint64 code, uint64 arg1 = 0, uint64 arg2 = 0, uint64 arg3 = 0, uint64 arg4 = 0)
{
    __asm__ volatile("ecall");
}

void* mem_alloc (size_t size)
{
    syscall(MEM_ALLOC, MemoryAllocator::get_num_blocks(size));
    return (void*)Riscv::r_a0();
}

int mem_free (void* mem_ptr)
{
    syscall(MEM_FREE, (uint64)mem_ptr);
    return Riscv::r_a0();
}

int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg)
{
    uint64* stack = new uint64[DEFAULT_STACK_SIZE];
    if (stack == nullptr) return -2;
    syscall(THREAD_CREATE, (uint64)handle, (uint64)start_routine, (uint64) arg, (uint64)stack);
    return Riscv::r_a0();
}

void thread_dispatch()
{
    syscall(THREAD_DISPATCH);
}

int thread_exit()
{
    syscall(THREAD_EXIT);
    return Riscv::r_a0();
}

int time_sleep (time_t time)
{
    syscall(TIME_SLEEP, time);
    return Riscv::r_a0();
}

int sem_open(sem_t* handle, unsigned init)
{
    syscall(SEM_OPEN, (uint64)handle, (uint64)init);
    return 0;
}

int sem_close (sem_t handle)
{
    syscall(SEM_CLOSE, (uint64)handle);
    return 0;
}

int sem_wait (sem_t id)
{
    syscall(SEM_WAIT,(uint64)id);
    return Riscv::r_a0();
}

int sem_signal (sem_t id)
{
    syscall(SEM_SIGNAL,(uint64)id);
    return Riscv::r_a0();
}

int sem_timedwait(sem_t id, time_t timeout)
{
    syscall(TIMED_WAIT, (uint64)id, (uint64)timeout);
    return Riscv::r_a0();
}

int sem_trywait(sem_t id)
{
    syscall(TRY_WAIT, (uint64)id);
    return Riscv::r_a0();
}

char getc ()
{
    syscall(GETC);
    return Riscv::r_a0();
}
void putc (char c)
{
    syscall(PUTC, (uint64)c);
}