//
// Created by os on 5/30/24.
//

#include "../h/console_buffer.hpp"
#include "../h/allocator.hpp"
#include "../h/sem.hpp"

ConsoleBuffer::ConsoleBuffer(int _cap) : cap(_cap + 1), head(0), tail(0)
{
    buffer = (int *)MemoryAllocator::allocate(MemoryAllocator::get_num_blocks( sizeof(int) * cap));
    itemAvailable = new Sem(0);
    spaceAvailable = new Sem(_cap);
    mutexHead = new Sem(1);
    mutexTail = new Sem(1);
}

ConsoleBuffer::~ConsoleBuffer() {}

void ConsoleBuffer::put(int val)
{
    spaceAvailable->wait();
    mutexTail->wait();

    buffer[tail] = val;
    tail = (tail + 1) % cap;
    mutexTail->signal();

    itemAvailable->signal();
}

int ConsoleBuffer::get() {
    itemAvailable->wait();

    mutexHead->wait();

    int ret = buffer[head];
    head = (head + 1) % cap;
    mutexHead->signal();

    spaceAvailable->signal();

    return ret;
}

int ConsoleBuffer::get_cnt()
{
    int ret;

//    mutexHead->wait();
//    mutexTail->wait();

    if (tail >= head) {
        ret = tail - head;
    } else {
        ret = cap - head + tail;
    }

//    mutexTail->signal();
//    mutexHead->signal();

    return ret;
}

