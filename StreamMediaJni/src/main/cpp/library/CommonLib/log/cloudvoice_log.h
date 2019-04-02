//
// Created by dengjun on 2019/3/20.
//

#ifndef VOICELIBDEMO_CLOUDVOICE_LOG_H
#define VOICELIBDEMO_CLOUDVOICE_LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

#ifdef  __cplusplus
extern "C"{
#endif

typedef enum
{  LOG_ERROR = 0, LOG_WARNING, LOG_INFO,
    LOG_DEBUG, LOG_VERBOSE
} LogLevel;


typedef void (LogCallback)(int level, const char *fmt, va_list);
void cloudVoiceLogSetCallback(LogCallback logCallback);

#ifdef __GNUC__
void cloudVoiceLog(int level, const char *format, ...) __attribute__ ((__format__ (__printf__, 2, 3)));
void cloudVoiceLogV(const char *format, ...) __attribute__ ((__format__ (__printf__, 2, 3)));
void cloudVoiceLogD( const char *format, ...) __attribute__ ((__format__ (__printf__, 2, 3)));
void cloudVoiceLogI(const char *format, ...) __attribute__ ((__format__ (__printf__, 2, 3)));
void cloudVoiceLogW(const char *format, ...) __attribute__ ((__format__ (__printf__, 2, 3)));
void cloudVoiceLogE(const char *format, ...) __attribute__ ((__format__ (__printf__, 2, 3)));
#else
void cloudVoiceLog(int level, const char *format, ...);
void cloudVoiceLogV( const char *format, ...);
void cloudVoiceLogD( const char *format, ...);
void cloudVoiceLogI( const char *format, ...);
void cloudVoiceLogW( const char *format, ...);
void cloudVoiceLogE( const char *format, ...);
#endif

void cloudVoiceLogHex(int level, const uint8_t *data, unsigned long len);
void cloudVoiceLogHexString(int level, const uint8_t *data, unsigned long len);
void cloudVoiceLogSetLevel(LogLevel logLevel);
LogLevel cloudVoiceLogGetLevel(void);
    
#ifdef __cplusplus
}
#endif



#endif //VOICELIBDEMO_CLOUDVOICE_LOG_H
