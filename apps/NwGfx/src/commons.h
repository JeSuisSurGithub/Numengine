#ifndef COMMONS_H
#define COMMONS_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define PI 3.14159f
#define DEBUG_LOG_MAX_LEN 1024u

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int16_t i16;
typedef int32_t i32;

typedef float vec3[3];
typedef float vec4[4];

typedef float mat4x4[4][4];

extern char DEBUG_LOG[DEBUG_LOG_MAX_LEN];
extern u16 DEBUG_LOG_LEN;

void write_log(const char* str);
void reset_log();
const char* read_log();

#endif /* COMMONS_H */