//
// Created by dengjun on 2019/3/20.
//

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "cloudvoice_log.h"
#define MAX_PRINT_LEN	2048

static LogCallback logCallbackInternal;
static LogLevel logLevelInternal = LOG_INFO;

void cloudVoiceLogSetCallback(LogCallback logCallback){
    logCallbackInternal = logCallback;
}

void cloudVoiceLog(int level, const char *format, ...){
    if (level<=logLevelInternal){
        va_list args;
        va_start(args, format);
        if (logCallbackInternal){
            logCallbackInternal(level, format, args);
        }
        va_end(args);
    }
}

void cloudVoiceLogV( const char *format, ...){
    if (LOG_VERBOSE<=logLevelInternal){
        va_list args;
        va_start(args, format);
        if (logCallbackInternal){
            logCallbackInternal(LOG_VERBOSE, format, args);
        }
        va_end(args);
    }
}
void cloudVoiceLogD( const char *format, ...){
    if (LOG_DEBUG<=logLevelInternal){
        va_list args;
        va_start(args, format);
        if (logCallbackInternal){
            logCallbackInternal(LOG_DEBUG, format, args);
        }
        va_end(args);
    }
}
void cloudVoiceLogI( const char *format, ...){
    if (LOG_INFO<=logLevelInternal){
        va_list args;
        va_start(args, format);
        if (logCallbackInternal){
            logCallbackInternal(LOG_INFO, format, args);
        }
        va_end(args);
    }
}
void cloudVoiceLogW( const char *format, ...){
    if (LOG_WARNING<=logLevelInternal){
        va_list args;
        va_start(args, format);
        if (logCallbackInternal){
            logCallbackInternal(LOG_WARNING, format, args);
        }
        va_end(args);
    }
}
void cloudVoiceLogE( const char *format, ...){
    if (LOG_ERROR<=logLevelInternal){
        va_list args;
        va_start(args, format);
        if (logCallbackInternal){
            logCallbackInternal(LOG_ERROR, format, args);
        }
        va_end(args);
    }
}

static const char hexdig[] = "0123456789abcdef";

void cloudVoiceLogHex(int level, const uint8_t *data, unsigned long len){
    unsigned long i;
    char line[50], *ptr;

    if ( level > logLevelInternal )
        return;

    ptr = line;

    for(i=0; i<len; i++) {
        *ptr++ = hexdig[0x0f & (data[i] >> 4)];
        *ptr++ = hexdig[0x0f & data[i]];
        if ((i & 0x0f) == 0x0f) {
            *ptr = '\0';
            ptr = line;
            cloudVoiceLog(level, "%s", line);
        } else {
            *ptr++ = ' ';
        }
    }
    if (i & 0x0f) {
        *ptr = '\0';
        cloudVoiceLog(level, "%s", line);
    }
}


void cloudVoiceLogHexString(int level, const uint8_t *data, unsigned long len){
#define BP_OFFSET 9
#define BP_GRAPH 60
#define BP_LEN	80
    char	line[BP_LEN];
    unsigned long i;

    if ( !data || level > logLevelInternal )
        return;

    /* in case len is zero */
    line[0] = '\0';

    for ( i = 0 ; i < len ; i++ ) {
        int n = i % 16;
        unsigned off;

        if( !n ) {
            if( i ) cloudVoiceLog( level, "%s", line );
            memset( line, ' ', sizeof(line)-2 );
            line[sizeof(line)-2] = '\0';

            off = i % 0x0ffffU;

            line[2] = hexdig[0x0f & (off >> 12)];
            line[3] = hexdig[0x0f & (off >>  8)];
            line[4] = hexdig[0x0f & (off >>  4)];
            line[5] = hexdig[0x0f & off];
            line[6] = ':';
        }

        off = BP_OFFSET + n*3 + ((n >= 8)?1:0);
        line[off] = hexdig[0x0f & ( data[i] >> 4 )];
        line[off+1] = hexdig[0x0f & data[i]];

        off = BP_GRAPH + n + ((n >= 8)?1:0);

        if ( isprint( data[i] )) {
            line[BP_GRAPH + n] = data[i];
        } else {
            line[BP_GRAPH + n] = '.';
        }
    }

    cloudVoiceLog( level, "%s", line );
}

void cloudVoiceLogSetLevel(LogLevel logLevel){
    logLevelInternal = logLevel;
}
LogLevel cloudVoiceLogGetLevel(void){
    return logLevelInternal;
}