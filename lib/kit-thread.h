#ifndef KIT_THREAD_H
#define KIT_THREAD_H

#ifdef __APPLE__
	#include <pthread.h>
	#define KitThread pthread_t
#else
	#include <threads.h>
	#define KitThread thrd_t
#endif

KitThread    kit_thread        (void *(*)(void*), void*);
int          kit_thread_detach (KitThread);

#endif
