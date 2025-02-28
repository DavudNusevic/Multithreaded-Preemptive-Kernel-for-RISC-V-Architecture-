//
// Created by os on 5/11/24.
//

#ifndef TEMPLEOS_SYSCALL_CPP_HPP
#define TEMPLEOS_SYSCALL_CPP_HPP

#include "syscall_c.hpp"

void* operator new (size_t);
void operator delete (void*);

class Thread {
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread ();
    int start ();
    static void dispatch ();
    static int sleep (time_t time);
protected:
    Thread ();
    virtual void run () {}
private:
    thread_t myHandle;
    void (*body)(void*);
    void* arg;

    static void wrapper(void* arg);
};

class Semaphore
{
public:
    explicit Semaphore (unsigned init = 1);
    virtual ~Semaphore();

    int wait();
    int signal();
    int timedWait(time_t time);
    int tryWait();

private:
    sem_t myHandle = nullptr;
};

class PeriodicThread : public Thread {
public:
    void terminate ();
protected:
    explicit PeriodicThread (time_t period);
    virtual void periodicActivation () {}
private:
    time_t period;

    void run() final override;
};

class Console {
public:
    static char getc ();
    static void putc (char c);
};

#endif //TEMPLEOS_SYSCALL_CPP_HPP