//
// Created by os on 5/11/24.
//

#include "../h/tcb.hpp"
#include "../h/scheduler.hpp"
#include "../h/riscv.hpp"

TCB* TCB::running = nullptr;
uint64 TCB::time_counter = 0;


TCB::TCB(TCB::Body body, void *arg, uint64* stack)
    :body(body), stack(stack),
     context({
                body != nullptr ? (uint64) &threadWrapper : 0,
                body != nullptr ? (uint64) &stack[DEFAULT_STACK_SIZE] : 0
            }),
     arg(arg), status(ACTIVE) { if (body != nullptr) Scheduler::put(this); }

int TCB::createThread(thread_t* handle, TCB::Body body, void *arg, uint64 *stack)
{
    if (handle == nullptr) return -1;
    *handle = new TCB(body, arg, stack);
    if (*handle == nullptr) return -2;
    return 0;
}

void TCB::dispatch()
{
    TCB *old = running;
    if (old->get_status() == ACTIVE)
    {
        Scheduler::put(old);
    }

    running = Scheduler::get();
    TCB::time_counter = 0;
    TCB::context_switch(&old->context, &running->context);
}

int TCB::exit()
{
    running->status = FINISHED;
    dispatch();
    return 0;
}

void TCB::threadWrapper()
{
    Riscv::pop_spp_spie();
    running->body(running->arg);
    thread_exit();
}

int TCB::sleep(time_t time)
{
    if (time == 0UL) return 0;

    int ret = TimerQueue::add(running, time);
    if (ret < 0) return ret;

    running->status = SLEEPING;
    dispatch();
    return 0;
}

void TCB::wake(TCB *thread)
{
    if (thread->status == SLEEPING) thread->status = ACTIVE;
    Scheduler::put(thread);
}

TCB::~TCB()
{
    delete[] stack;
    stack = nullptr;
    status = FINISHED;
}

int TCB::sem_sleep(time_t time)
{
    if (TimerQueue::add(running, time) < 0) return -1;

    running->status = SEM_SLEEPING;
    dispatch();
    return 0;
}

