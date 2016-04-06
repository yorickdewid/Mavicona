#ifndef _LOG_HPP_
#define _LOG_HPP_

#include <cstdio>
#include <cstdarg>

#include "thread.h"


/*
	Server log class.
	\author Miguel Boton Campo
*/
class CLog {
	static CMutex Mutex;	//!< Mutex object.

  private:
	/*!
		Print a formatted message to a stream.
		\param stream the target stream.
		\param fmt the message format.
		\param args the list of arguments.
	*/
	static void PrintStream(FILE *stream, const char *fmt, va_list args);

  public:
	/*!
		Print a formatted message.
		\param fmt the message format.
	*/
	static void Print(const char *fmt, ...);

	/*!
		Print a formatted error message.
		\param fmt the message format.
	*/
	static void PrintErr(const char *fmt, ...);

};

#endif	/* _LOG_HPP_ */
