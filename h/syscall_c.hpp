//
// Created by os on 5/6/24.
//


#ifndef TEMPLEOS_SYSCALL_C_HPP
#define TEMPLEOS_SYSCALL_C_HPP

#include "../lib/hw.h"

class TCB;
typedef TCB*  thread_t;

typedef unsigned long time_t;

class Sem;
typedef Sem* sem_t;


enum InterruptCode {
    MEM_ALLOC = 0x1,
    MEM_FREE = 0x2,
    THREAD_CREATE = 0x11,
    THREAD_EXIT = 0x12,
    THREAD_DISPATCH = 0x13,
    SEM_OPEN = 0x21,
    SEM_CLOSE = 0x22,
    SEM_WAIT = 0x23,
    SEM_SIGNAL = 0x24,
    TIMED_WAIT = 0x25,
    TRY_WAIT = 0x26,
    TIME_SLEEP = 0x31,
    GETC = 0x41,
    PUTC = 0x42
};

/**
* Allocates memory for a specified number of bytes,
* rounded and aligned to blocks of size MEM_BLOCK_SIZE (defined in hw.h)
*
*
* @param num_blocks The number of bytes to allocate.
* @return A pointer to the allocated memory, or nullptr if allocation fails.
*/
void* mem_alloc (size_t size);

/**
* Deallocates memory given as argument, that was previously allocated with mem_alloc function.
* If given pointer to memory wasn't returned from mem_alloc method behaviour is undefined.
*
* @param mem_ptr Pointer to memory that will be deallocated
* @return Status code: 0 for success, negative value otherwise.
*/
int mem_free (void*);

/**
 *
 * @param handle
 * @param start_routine
 * @param arg
 * @return
 */
int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg);

/**
 *
 */
void thread_dispatch();

/**
 *
 * @return
 */
int thread_exit();

/**
 *
 * @return
 */
int time_sleep (time_t time);

/**
 *
 * @param handle
 * @param init
 * @return
 */
int sem_open(sem_t* handle, unsigned init);

/**
 *
 * @param handle
 * @return
 */
int sem_close (sem_t handle);

/**
 *
 * @param id
 * @return
 */
int sem_wait (sem_t id);

/**
 *
 * @param id
 * @return
 */
int sem_signal (sem_t id);


int sem_timedwait(sem_t id, time_t timeout);

/**
 *
 * @param id
 * @return
 */
int sem_trywait(sem_t id);

const int EOF = -1;
/**
 *
 * @return
 */
char getc ();

/**
 *
 */
void putc (char c);
#endif //TEMPLEOS_SYSCALL_C_HPP