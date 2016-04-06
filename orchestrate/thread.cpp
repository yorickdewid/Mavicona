#include "thread.h"


CMutex::CMutex(void) {
	/* Initialize the mutex */
	pthread_mutex_init(&mutex, NULL);
}

CMutex::~CMutex(void) {
	/* Destroy the mutex */
	pthread_mutex_destroy(&mutex);
}

bool CMutex::Lock(void) {
	int res;

	/* Lock the mutex */
	res = pthread_mutex_lock(&mutex);
	if (res)
		return false;

	return true;
}

bool CMutex::Unlock(void) {
	int res;

	/* Unlock the mutex */
	res = pthread_mutex_unlock(&mutex);
	if (res)
		return false;

	return true;
}


CThread::~CThread(void) {
	/* Destroy thread */
	Destroy();
}

bool CThread::Create(void *(*routine)(void *), void *arg) {
	int res;

	/* Create thread */
	res = pthread_create(&thread, NULL, routine, arg);
	if (res)
		return false;

	return true;
}

bool CThread::Destroy(void) {
	int res;

	/* Destroy thread */
	res = pthread_cancel(thread);
	if (res)
		return false;

	return true;
}

bool CThread::Join(void) {
	int res;

	/* Join thread */
	res = pthread_join(thread, NULL);
	if (res)
		return false;

	return true;
}

void CThread::Exit(void *ret) {
	/* Terminate thread */
	pthread_exit(ret);
}

