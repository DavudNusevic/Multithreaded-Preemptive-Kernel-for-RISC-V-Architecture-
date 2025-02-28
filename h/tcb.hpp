//
// Created by os on 5/11/24.
//

#ifndef TEMPLEOS_TCB_HPP
#define TEMPLEOS_TCB_HPP

#include "syscall_c.hpp"
#include "timer_queue.hpp"
#include "system.hpp"
#include "allocator.hpp"

//Tartalja me je slago
class TCB : public SysStruct
{
public:
    enum Status : uint64
    {
        ACTIVE,
        SLEEPING,
        BLOCKED,
        SEM_SLEEPING,
        FORCED,
        FINISHED
    };

    using Body = void(*)(void*);

    ~TCB();
    uint64 getTimeSlice() const { return time_slice; }
    Status get_status() const {return status;}
    bool is_finished() const {return status == FINISHED;}

    static int createThread(thread_t* handle, Body body, void* arg, uint64* stack);

    static TCB* running;

private:
    TCB(Body body, void* arg, uint64* stack);

    struct Context
            {
                uint64 ra;
                uint64 sp;
            };

    static void threadWrapper();
    static void context_switch(Context *oldContext, Context *runningContext);
    static void dispatch();
    static int exit();
    static int sleep(time_t time);
    static int sem_sleep(time_t time);
    static void wake(TCB* thread);

    static uint64 time_counter;
    static bool initialized;

    Body body;
    uint64* stack;
    Context context;
    void* arg;
    uint64 time_slice = DEFAULT_TIME_SLICE;
    Status status;
public:
    static bool system_mode;

    friend class Riscv;
    friend class Sem;
};

#endif //TEMPLEOS_TCB_HPP