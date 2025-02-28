//
// Created by os on 5/16/24.
//

#ifndef TEMPLEOS_SEM_HPP
#define TEMPLEOS_SEM_HPP

#include "syscall_c.hpp"
#include "tcb.hpp"

class Sem : public SysStruct
{
public:
    explicit Sem (unsigned val = 1);

    int wait();
    int signal();
    int trywait();
    int timedwait(time_t time);

    ~Sem();

protected:
    int block();
    int unblock();

private:
    struct SemBlocked : public SysStruct
    {
        thread_t thr;
        time_t time;
        bool waiting;
        SemBlocked* next;

        SemBlocked(TCB *thr, time_t time, bool waiting, SemBlocked *next)
            :thr(thr), time(time), waiting(waiting), next(next) {}
    };

    void put();
    SemBlocked* get();
    void remove(TCB* thr);

    int val;
    SemBlocked *head, *tail;

};

#endif //TEMPLEOS_SEM_HPP
