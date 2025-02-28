//
// Created by marko on 20.4.22..
//

#include "../h/riscv.hpp"
#include "../h/printing.hpp"
#include "../h/tcb.hpp"
#include "../h/syscall_c.hpp"
#include "../h/sem.hpp"

ConsoleBuffer* Riscv::output = nullptr;
ConsoleBuffer* Riscv::input = nullptr;
bool Riscv::user_mode = false;

void Riscv::switch_to_user_mode()
{
    user_mode = true;
}

void Riscv::empty_buffer()
{
    while ( *((char*)CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT )
        if (output->get_cnt() > 0)
            *(char*)CONSOLE_TX_DATA = output->get();
}

void Riscv::pop_spp_spie()
{
    if (user_mode) mc_sstatus(SSTATUS_SPP);

    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

void Riscv::handle_trap()
{
    uint64 code, arg1, arg2, arg3, arg4, scause, ret;
    bool has_ret = true;

    //read code and arguments
    __asm__ volatile("mv %[code], a0" : [code] "=r"(code));
    __asm__ volatile("mv %[arg1], a1" : [arg1] "=r"(arg1));
    __asm__ volatile("mv %[arg2], a2" : [arg2] "=r"(arg2));
    __asm__ volatile("mv %[arg3], a3" : [arg3] "=r"(arg3));
    __asm__ volatile("mv %[arg4], a4" : [arg4] "=r"(arg4));

    //read system registers
    scause = Riscv::r_scause();
    uint64 volatile sepc = r_sepc();
    uint64 volatile sstatus = r_sstatus();

    ret = 0;
    if (scause == 0x8UL || scause == 0x9UL)
    {
        //ecall
        sepc += 4;
        switch (code)
        {
            case MEM_ALLOC:
                ret = (uint64)MemoryAllocator::allocate(arg1);
            break;

            case MEM_FREE:
                ret = (uint64)MemoryAllocator::free((void*)arg1);
            break;

            case THREAD_CREATE:
                ret = TCB::createThread((thread_t *)arg1, (TCB::Body) arg2, (void *) arg3, (uint64 *) arg4);
            break;

            case THREAD_EXIT:
                ret = TCB::exit();
            break;

            case THREAD_DISPATCH:
                has_ret = false;
                TCB::dispatch();
            break;

            case SEM_OPEN:
                *(sem_t*)arg1 = new Sem(arg2);
            break;

            case SEM_CLOSE:
                delete sem_t(arg1);
            break;

            case SEM_WAIT:
                ret = sem_t(arg1)->wait();
            break;

            case SEM_SIGNAL:
                ret = sem_t(arg1)->signal();
            break;

            case TRY_WAIT:
                ret = sem_t(arg1)->trywait();
            break;

            case TIMED_WAIT:
                ret = sem_t(arg1)->timedwait(arg2);
            break;

            case TIME_SLEEP:
                ret = TCB::sleep(arg1);
            break;

            case GETC:
                ret = input->get();
                ///ret = __getc();
            break;

            case PUTC:
                has_ret = false;
                output->put(arg1);
                //__putc(char(arg1));
            break;

        }

        if (has_ret)
        {
            //puts return value in a0 register on stack
            //TODO: napisi ovo lepse
            uint64 volatile temp;
            __asm__ volatile("mv %0, a1" : "=r"(temp));
            __asm__ volatile("mv a1, %0": :"r"(ret));
            __asm__ volatile("sd a1, 80(s0)" : :"r"(ret));
            __asm__ volatile("mv a1, %0": :"r"(temp));
        }

        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    else if (scause == (1UL << 63 | 1UL))
    {
        //timer interrupt
        mc_sip(SIP_SSIP);

        TimerQueue::update();
        while (not TimerQueue::empty() && TimerQueue::peek_first() == 0)
            TCB::wake(TimerQueue::remove());

        TCB::time_counter++;

        if (TCB::time_counter >= TCB::running->getTimeSlice())
        {
            TCB::time_counter = 0;
            TCB::dispatch();
            w_sstatus(sstatus);
            w_sepc(sepc);
        }

    }
    else if (scause == (1UL << 63 | 9UL))
    {
        //external hardware interrupt
        if (plic_claim() == CONSOLE_IRQ) {
            //console interrupt

            while (  *(char *)(CONSOLE_STATUS) & CONSOLE_RX_STATUS_BIT )
            {
                char c = *(char*)CONSOLE_RX_DATA;
                if (c == '\r') input->put('\n');
                else input->put(c);
            }

            w_sstatus(sstatus);
            w_sepc(sepc);
            plic_complete(CONSOLE_IRQ);
        }
    }
    else
    {
        exit_emulator();
    }
}
