//
// Created by os on 5/5/24.
//

#include "../h/riscv.hpp"
#include "../h/printing.hpp"
#include "../h/syscall_c.hpp"
#include "../h/scheduler.hpp"
#include "../h/timer_queue.hpp"
#include "../h/syscall_cpp.hpp"

extern void userMain();

void user_wrapper(void*)
{
    userMain();
}

thread_t user_thread;

void konzola(void*)
{
    while(true)
    {
        while((*((char*)(CONSOLE_STATUS)) & CONSOLE_TX_STATUS_BIT))
        {
            char c = Riscv::output->get();
            *((char*)CONSOLE_TX_DATA) = c;
        }
    }
}

void test()
{

    thread_t thr;
    thread_create(&thr, konzola, nullptr);

    thread_dispatch();
    Riscv::switch_to_user_mode();

    thread_create(&user_thread, user_wrapper, nullptr);
    while (not user_thread->is_finished())
    {
        thread_dispatch();
    }
    delete user_thread;
    delete thr;
}


void main()
{
    Riscv::w_stvec((uint64) &Riscv::trap);
    MemoryAllocator::init();
    Riscv::init();

    thread_t main_thread;
    thread_create(&main_thread, nullptr, nullptr);
    TCB::running = main_thread;

    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    test();

    //Free allocated memory for main_thread and empty Scheduler
    while (Scheduler::get());
    delete main_thread;

    printString("\nKernel finished\n\n");

    //MemoryAllocator::print();
    Riscv::empty_buffer();
}