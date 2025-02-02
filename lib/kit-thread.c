#include "kit-thread.h"

KitThread kit_thread(void *(*f)(void*), void* args) {
	KitThread t;
#ifdef __APPLE__
	pthread_create(&t, NULL, f, args);
#else
	thrd_create(&t, f, &args);
#endif
	return t;
}

inline int kit_thread_detach(KitThread t) {
#ifdef __APPLE__
	return -1;
#else
	return thrd_detach(t);
#endif
}
