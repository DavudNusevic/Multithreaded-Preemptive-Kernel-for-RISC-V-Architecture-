//
// Created by os on 5/17/24.
//

#include "../h/sem.hpp"
#include "../h/scheduler.hpp"

Sem::Sem(unsigned val)
    :val(val), head(nullptr), tail(nullptr) { }

void Sem::put()
{
    SemBlocked* blck = new SemBlocked(TCB::running, 0, false, nullptr);
    if (tail) tail->next = blck;
    else head = blck;
    tail = blck;
}

Sem::SemBlocked* Sem::get()
{
    SemBlocked* ret = head;
    if (head) head = head->next;
    if (not head) tail = nullptr;

    return ret;
}

int Sem::block()
{
    put();
    TCB::running->status = TCB::BLOCKED;
    TCB::dispatch();

    return 0;
}

int Sem::unblock()
{
    SemBlocked* unblocked = get();
    if (unblocked == nullptr) return -1;
    unblocked->thr->status = TCB::ACTIVE;
    Scheduler::put(unblocked->thr);

    delete unblocked;
    return 0;
}

int Sem::wait()
{
    if (--val < 0)
    {
        block();
        if (TCB::running->status == TCB::FORCED)
        {
            TCB::running->status = TCB::ACTIVE;
            return -1;
        }

    }
    return 0;
}

int Sem::timedwait(time_t time)
{
    if (val <= 0)
    {
        if (time == 0) return -2;
        put();
        TCB::sem_sleep(time);

        if (TCB::running->status == TCB::FORCED)
        {
            //remove thread from both blocked and timer queue
            remove(TCB::running);
            TimerQueue::remove(TCB::running);
            //semaphore closed so no need to update semaphore value
            TCB::running->status = TCB::ACTIVE;
            return -1;
        }
        else if (TCB::running->status == TCB::SEM_SLEEPING)
        {
            //remove thread from blocked queue
            remove(TCB::running);
            //update semaphore value

            TCB::running->status = TCB::ACTIVE;
            return -2;
        }
        else if (TCB::running->status == TCB::ACTIVE)
        {
            //remove thread from timer queue
            TimerQueue::remove(TCB::running);
            //thread woken up by signal so no need to update semaphore value
            TCB::running->status = TCB::ACTIVE;

            return 0;
        }
    }
    else val--;

    return 0;
}

int Sem::signal()
{
    if (++val <= 0) return unblock();
    return 0;
}

Sem::~Sem()
{
    SemBlocked* unblocked;
    while ((unblocked = get()) != nullptr)
    {
        unblocked->thr->status = TCB::FORCED;
        Scheduler::put(unblocked->thr);
        delete unblocked;
    }
}

int Sem::trywait()
{
    if (val > 0)
    {
        val--;
        return 0;
    }
    return 1;
}

void Sem::remove(TCB* thr)
{
    for (SemBlocked *prev = nullptr, *curr = head; curr; prev = curr, curr = curr->next)
    {
        if (curr->thr == thr)
        {
            if (prev) prev->next = curr->next;
            else head = curr->next;

            if(curr == tail) tail = prev;
            delete curr;

            return;
        }
    }
}

