//
// Created by os on 5/15/24.
//

#include "../h/timer_queue.hpp"

TimerQueue::Elem* TimerQueue::head = nullptr;

void TimerQueue::remove(TCB * thr)
{
    for (Elem *prev = nullptr, *curr = head; curr; prev = curr, curr = curr->next)
    {
        if (curr->thread == thr)
        {
            if (prev)
            {
                prev->next = curr->next;
                if (prev->next) prev->next->time += curr->time;
            }
            else
            {
                head = curr->next;
                head->time += curr->time;
            }

            delete curr;
            return;
        }
    }
}

int TimerQueue::add(TCB* thread, time_t time)
{
    if (thread == nullptr or time == 0) return -1;

    Elem *prev, *curr;
    for (prev = nullptr, curr = head; curr; prev = curr, curr = curr->next)
    {
        if (time < curr->time)
        {
            curr->time -= time;
            break;
        }
        time -= curr->time;
    }

    Elem* new_elem = new Elem(thread, time);

    if (prev) prev->next = new_elem;
    else head = new_elem;
    new_elem->next = curr;

    return 0;
}

bool TimerQueue::empty()
{
    return head == nullptr;
}

TCB *TimerQueue::remove()
{
    if (head == nullptr) return nullptr;
    TCB* ret = head->thread;
    Elem* to_delete = head;
    head = head->next;
    delete to_delete;
    return ret;
}

time_t TimerQueue::peek_first()
{
    if (head) return head->time;
    return -1;
}

void TimerQueue::update()
{
    if (head && head->time > 0) head->time--;
}




