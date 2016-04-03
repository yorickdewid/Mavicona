#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

void start_log();
void lprintf(const char *format, ...);
void lprint(const char *str);
void stop_log();

#ifdef __cplusplus
}
#endif

#endif // LOG_H_INCLUDED
