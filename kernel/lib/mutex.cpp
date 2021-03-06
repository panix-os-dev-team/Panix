/**
 * @file mutex.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2020-08-30
 *
 * @copyright Copyright the Panix Contributors (c) 2020
 *
 */

// All of the GCC builtin functions used here are documented at the link provided
// below. These builtins effectly generate assembly that will atomically perform
// the function described by the function name. Normally there would be a wrapper
// around these provided by <atomic.h> but we can't use that since we don't have
// a standard library for either C or C++;
//
// Reference:
// https://gcc.gnu.org/onlinedocs/gcc-8.3.0/gcc/_005f_005fatomic-Builtins.html

#include <lib/mutex.hpp>
#include <lib/errno.h>
#include <mem/heap.hpp>
#include <stddef.h>

#define ACQUIRE_MUTEX_LOCK(mutex) __atomic_test_and_set(&mutex->locked, __ATOMIC_RELEASE)

#define IS_MUTEX_VALID(mutex) { \
    if (mutex == NULL)          \
    {                           \
        errno = EINVAL;         \
        return -1;              \
    }                           \
}

mutex::mutex(const char *name)
    : locked(false)
{
    task_sync.dbg_name = name;
};

int mutex_init(mutex_t *mutex) {
    IS_MUTEX_VALID(mutex);
    // Initialize the value to false
    mutex->locked = false;
    tasks_sync_init(&mutex->task_sync);
    // Success, return 0
    return 0;
}

int mutex_destroy(mutex_t *mutex) {
    IS_MUTEX_VALID(mutex);
    free(mutex);
    // Success, return 0
    return 0;
}

int mutex_lock(mutex_t *mutex) {
    IS_MUTEX_VALID(mutex);
    // Check if the mutex is unlocked
    while (ACQUIRE_MUTEX_LOCK(mutex))
    {
        // Block the current kernel task
        TASK_ONLY tasks_sync_block(&mutex->task_sync);
    }
    // Success, return 0
    return 0;
}

int mutex_trylock(mutex_t *mutex) {
    IS_MUTEX_VALID(mutex);
    // If we cannot immediately acquire the lock then just return an error
    if (ACQUIRE_MUTEX_LOCK(mutex))
    {
        errno = EINVAL;
        return -1;
    }
    // Success, return 0
    return 0;
}

int mutex_unlock(mutex_t *mutex) {
    IS_MUTEX_VALID(mutex);
    // Clear the lock
    __atomic_clear(&mutex->locked, __ATOMIC_RELEASE);
    TASK_ONLY tasks_sync_unblock(&mutex->task_sync);
    // Success, return 0
    return 0;
}
