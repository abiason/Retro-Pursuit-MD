#ifndef RP_TYPES_H
#define RP_TYPES_H
#ifdef SGDK_GCC
#include <types.h>
typedef s8 int8_t;
typedef s16 int16_t;
typedef s32 int32_t;
typedef u16 uint16_t;
typedef u32 uint32_t;
#else
#include <stdint.h>
#endif
#endif
