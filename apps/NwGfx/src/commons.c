#include "commons.h"

#include <string.h>

#define DEBUG_LOG_MAX_LEN 1024u
char DEBUG_LOG[DEBUG_LOG_MAX_LEN] = {0};
u16 DEBUG_LOG_LEN = 0;

void write_log(const char* str) {
    u16 message_len = strlen(str);
    if (DEBUG_LOG_LEN + message_len + 2 /* null + newline*/ < DEBUG_LOG_MAX_LEN) {
        strcat(DEBUG_LOG, str);
        strcat(DEBUG_LOG, "\n");
        DEBUG_LOG_LEN += (message_len + 1);
    } else {
        reset_log();
    }
}

void reset_log() {
    memset(DEBUG_LOG, 0, sizeof(char) * DEBUG_LOG_MAX_LEN);
    DEBUG_LOG_LEN = 0;
}

const char* read_log() {
    return DEBUG_LOG;
}