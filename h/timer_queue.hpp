//
// Created by os on 5/15/24.
//

#ifndef TEMPLEOS_TIMER_QUEUE_HPP
#define TEMPLEOS_TIMER_QUEUE_HPP

#include "tcb.hpp"
#include "allocator.hpp"
#include "system.hpp"

class TimerQueue
{
public:
    static int add(TCB* thread, time_t time);
    static bool empty();
    static time_t peek_first();
    static TCB* remove();
    static void update();
    static void remove(TCB* thr);

private:
    struct Elem : public SysStruct
    {
        TCB* thread;
        time_t time;
        Elem* next;

        Elem (TCB* thread, time_t time, Elem* next = nullptr)
            :thread(thread), time(time), next(next) {}

    };

    static Elem* head;
};

#endif //TEMPLEOS_TIMER_QUEUE_HPP

