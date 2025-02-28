//
// Created by os on 5/11/24.
//

#include "../h/syscall_cpp.hpp"
#include "../h/tcb.hpp"
#include "../h/syscall_c.hpp"

void* operator new(size_t n)
{
    return mem_alloc(n);
}

void* operator new[](size_t n)
{
    return mem_alloc(n);
}

void operator delete(void *p)
{
    mem_free(p);
}

void operator delete[](void *p)
{
    mem_free(p);
}

Thread::Thread(void (*body)(void *), void *arg)
    :body(body), arg(arg) { }

Thread::Thread()
    :body(nullptr), arg(nullptr) { }

void Thread::wrapper(void *arg)
{
    ((Thread*)arg)->run();
}

int Thread::start()
{
    if (body) return thread_create(&myHandle, body, arg);
    return thread_create(&myHandle, wrapper, this);
}

void Thread::dispatch()
{
    thread_dispatch();
}

int Thread::sleep(time_t time)
{
    return time_sleep(time);
}

Thread::~Thread() { delete myHandle; }

Semaphore::Semaphore(unsigned int init) { sem_open(&myHandle, init); }

Semaphore::~Semaphore() { sem_close(myHandle); }

int Semaphore::wait()
{
    if (myHandle == nullptr) return -5;
    return sem_wait(myHandle);
}

int Semaphore::signal()
{
    if (myHandle == nullptr) return -5;
    return sem_signal(myHandle);
}

int Semaphore::timedWait(time_t time)
{
    if (myHandle == nullptr) return -5;
    return sem_timedwait(myHandle, time);
}

int Semaphore::tryWait()
{
    if (myHandle == nullptr) return -5;
    return sem_trywait(myHandle);
}

PeriodicThread::PeriodicThread(time_t period)
    :period(period) { }

void PeriodicThread::run()
{
    while (period != -1UL)
    {
        periodicActivation();
        if (period != -1UL) sleep(period);
        else break;
    }
}

void PeriodicThread::terminate()
{
    period = -1UL;
}

char Console::getc()
{
    return ::getc();
}

void Console::putc(char c)
{
    ::putc(c);
}
