#ifndef _THREAD_HPP_
#define _THREAD_HPP_

#include <pthread.h>


/*!
	Mutex abstraction class.
	\author Miguel Boton Campo
*/
class CMutex {
	pthread_mutex_t mutex;	//!< Mutex object.

  public:
	/*!
		Mutex object constructor.
	*/
	CMutex(void);

	/*!
		Mutex object destructor.
	*/
	~CMutex(void);

	/*!
		Lock the mutex.
		\returns true if success, otherwise false.
	*/
	bool Lock(void);

	/*!
		Unlock the mutex.
		\returns true if success, otherwise false.
	*/
	bool Unlock(void);
};


/*!
	Thread abstraction class.
	\author Miguel Boton Campo
*/
class CThread {
	pthread_t thread;	//!< Thread object.

  public:
	/*!
		Thread object destructor.
	*/
	~CThread(void);

	/*!
		Create a thread.
		\param routine the routine to be executed in the thread.
		\param arg the argument to be passed to the routine.
		\returns true if success, otherwise false.
	*/
	bool Create(void *(*routine)(void *), void *arg);

	/*!
		Destroy the thread.
		\returns true if success, otherwise false.
	*/
	bool Destroy(void);

	/*!
		Join the thread.
		\returns true if success, otherwise false.
	*/
	bool Join(void);

	/*!
		Terminate the thread
		(to be called from the thread routine).
		\param ret the return value pointer.
	*/
	static void Exit(void *ret);
};

#endif	/* _THREAD_HPP_ */
