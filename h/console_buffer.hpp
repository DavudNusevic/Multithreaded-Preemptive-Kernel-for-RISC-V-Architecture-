//
// Created by os on 5/30/24.
//

#ifndef TEMPLEOS_CONSOLE_BUFFER_HPP
#define TEMPLEOS_CONSOLE_BUFFER_HPP

#include "../h/syscall_c.hpp"

class ConsoleBuffer
{
private:
    int cap;
    int *buffer;
    int head, tail;

    sem_t spaceAvailable;
    sem_t itemAvailable;
    sem_t mutexHead;
    sem_t mutexTail;

public:
    ConsoleBuffer(int _cap);
    ~ConsoleBuffer();

    void put(int val);
    int get();
    int get_cnt();
};

#endif //TEMPLEOS_CONSOLE_BUFFER_HPP
