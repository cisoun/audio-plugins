#ifndef KIT_THREAD_H
#define KIT_THREAD_H

#include <pthread.h>
#define KitThread pthread_t

KitThread kit_thread        (void* (*)(void*), void*);
int       kit_thread_detach (KitThread);

#endif
