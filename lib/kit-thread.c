#include "kit-thread.h"
#include <pthread.h>

KitThread kit_thread(void* (*f)(void*), void* args) {
	KitThread t;
	pthread_create(&t, NULL, f, args);
	return t;
}

inline int kit_thread_detach(KitThread t) {
	return -1;
}
