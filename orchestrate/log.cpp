#include <ctime>

#include "log.h"

/* Mutex object */
CMutex CLog::Mutex;


inline void CLog::PrintStream(FILE *stream, const char *fmt, va_list args) {
	time_t     rawtime;
	struct tm *timeinfo;

	char datetime[32];
	char message[256];

	/* Get date */
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	/* Generate message */
	strftime(datetime, sizeof(datetime), "[%x - %X]", timeinfo);
	vsnprintf(message,  sizeof(message),  fmt, args);

	/* Lock mutex */
	Mutex.Lock();

	/* Print message */
	fprintf(stream, "%s %s\n", datetime, message);

	/* Unlock mutex */
	Mutex.Unlock();
}

void CLog::Print(const char *fmt, ...) {
	va_list args;

	/* Initialize list */
	va_start(args, fmt);

	/* Print message (standard output) */
	PrintStream(stdout, fmt, args);

	/* Free list */
	va_end(args);
}

void CLog::PrintErr(const char *fmt, ...) {
	va_list args;

	/* Initialize list */
	va_start(args, fmt);

	/* Print message (standard error) */
	PrintStream(stderr, fmt, args);

	/* Free list */
	va_end(args);
}
