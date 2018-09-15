
#ifndef BRKS_COMMON_MUTEX_H_
#define BRKS_COMMON_MUTEX_H_

#include "Logger.h"

#include <execinfo.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define likely(x) __builtin_expect ((x), 1)
#define unlikely(x) __builtin_expect ((x), 0)

void print_backtrace (void)
{
    void *array[100];
    size_t size;
    char **strings;
    size_t i;

    size = backtrace (array, 100);
    strings = (char **)backtrace_symbols (array, size);

    LOG_ERROR("Stack trace:\n");
    for (i = 0; i < size; i++)
    {
        LOG_ERROR("%d %s \n",i, strings[i]);
    }

    free (strings);
}

void posix_abort (const char *errmsg_)
{
    (void) errmsg_;
    print_backtrace();
    abort ();
}

#define posix_assert(x)                                                        \
    do {                                                                       \
        if (unlikely (x)) {                                                    \
            const char *errstr = strerror (x);                                 \
            fprintf (stderr, "%s (%s:%d)\n", errstr, __FILE__, __LINE__);      \
            fflush (stderr);                                                   \
            posix_abort (errstr);                                           \
        }                                                                      \
    } while (false)

class mutex_t
{
public:
    inline mutex_t ()
    {
        int rc = pthread_mutexattr_init (&attr);
        posix_assert (rc);

        rc = pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE);
        posix_assert (rc);

        rc = pthread_mutex_init (&mutex, &attr);
        posix_assert (rc);
    }

    inline ~mutex_t ()
    {
        int rc = pthread_mutex_destroy (&mutex);
        posix_assert (rc);

        rc = pthread_mutexattr_destroy (&attr);
        posix_assert (rc);
    }

    inline void lock ()
    {
        int rc = pthread_mutex_lock (&mutex);
        posix_assert (rc);
    }

    inline bool try_lock ()
    {
        int rc = pthread_mutex_trylock (&mutex);
        if (rc == EBUSY)
            return false;

        posix_assert (rc);
        return true;
    }

    inline void unlock ()
    {
        int rc = pthread_mutex_unlock (&mutex);
        posix_assert (rc);
    }

    inline pthread_mutex_t *get_mutex () { return &mutex; }

private:
    pthread_mutex_t mutex;
    pthread_mutexattr_t attr;

    // Disable copy construction and assignment.
    mutex_t (const mutex_t &);
    const mutex_t &operator= (const mutex_t &);
};

#endif