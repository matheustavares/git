/*
 * Header used to adapt pthread-based POSIX code to Windows API threads.
 *
 * Copyright (C) 2009 Andrzej K. Haczewski <ahaczewski@gmail.com>
 */

#ifndef PTHREAD_H
#define PTHREAD_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

/*
 * Defines that adapt Windows API threads to pthreads API
 */
#define pthread_mutex_t CRITICAL_SECTION

static inline int return_0(int i) {
	return 0;
}
#define pthread_mutex_init(a,b) return_0((InitializeCriticalSection((a)), 0))
#define pthread_mutex_destroy(a) DeleteCriticalSection((a))
#define pthread_mutex_lock EnterCriticalSection
#define pthread_mutex_unlock LeaveCriticalSection

typedef int pthread_mutexattr_t;
#define pthread_mutexattr_init(a) (*(a) = 0)
#define pthread_mutexattr_destroy(a) do {} while (0)
#define pthread_mutexattr_settype(a, t) 0
#define PTHREAD_MUTEX_RECURSIVE 0

#define pthread_cond_t CONDITION_VARIABLE

#define pthread_cond_init(a,b) InitializeConditionVariable((a))
#define pthread_cond_destroy(a) do {} while (0)
#define pthread_cond_wait(a,b) return_0(SleepConditionVariableCS((a), (b), INFINITE))
#define pthread_cond_signal WakeConditionVariable
#define pthread_cond_broadcast WakeAllConditionVariable

#define pthread_once_t INIT_ONCE

#define PTHREAD_ONCE_INIT INIT_ONCE_STATIC_INIT
int pthread_once(pthread_once_t *once_control, void (*init_routine)(void));

/*
 * Simple thread creation implementation using pthread API
 */
typedef struct {
	HANDLE handle;
	void *(*start_routine)(void*);
	void *arg;
	DWORD tid;
} pthread_t;

int pthread_create(pthread_t *thread, const void *unused,
		   void *(*start_routine)(void*), void *arg);

/*
 * To avoid the need of copying a struct, we use small macro wrapper to pass
 * pointer to win32_pthread_join instead.
 */
#define pthread_join(a, b) win32_pthread_join(&(a), (b))

int win32_pthread_join(pthread_t *thread, void **value_ptr);

#define pthread_equal(t1, t2) ((t1).tid == (t2).tid)
pthread_t pthread_self(void);

static inline void NORETURN pthread_exit(void *ret)
{
	ExitThread((DWORD)(intptr_t)ret);
}

typedef struct {
	DWORD index;
	void (*destructor)(void *value);
} pthread_key_t;

int pthread_key_create(pthread_key_t *keyp, void (*destructor)(void *value));
#define pthread_key_delete(key) do_pthread_key_delete(&(key))
#define pthread_setspecific(key, value) do_pthread_setspecific(&(key), value)
void *pthread_getspecific(pthread_key_t key);

int do_pthread_key_delete(pthread_key_t *key);
int do_pthread_setspecific(pthread_key_t *key, const void *value);

#ifndef __MINGW64_VERSION_MAJOR
static inline int pthread_sigmask(int how, const sigset_t *set, sigset_t *oset)
{
	return 0;
}
#endif

#endif /* PTHREAD_H */
