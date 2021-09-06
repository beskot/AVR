#ifndef _MACROS_H_
#define _MACROS_H_

#include <stdint.h>
#include <stdbool.h>

#define BM_JOIN_3(N0, N1, N2) N0##N1##N2
#define BM_JOIN_2(N0, N1) N0##N1

#define BM_ByteHi(Int) (uint8_t)(Int>>8)
#define BM_ByteLow(Int) (uint8_t)(Int & 0x00FF)
#define BM_WORD(Hi, Low) (uint16_t)(Hi<<8 | Low)

#define BM_Bit(bit)                       (1<<(bit))
#define BM_SetBit(var, bit)               do{(var) |= (1<<(bit));}while(0)
#define BM_ClearBit(var, bit)             do{(var) &= ~(1<<(bit));}while(0)
#define BM_InvBit(var, bit)               do{(var) ^= (1<<(bit));}while(0)
#define BM_TestBit(var, bit)              ((var) & (1<<(bit)))
#define BM_BitIsClear(var, bit)           (((var) & (1<<(bit))) == 0)
#define BM_BitIsSet(var, bit)             (((var) & (1<<(bit))) != 0)
#define BM_SetBitVal(var, bit, val)       do{{if (((val)&1)==0) {(var) &= ~(1<<(bit))};\
else {(var) |= (1<<(bit));}}while(0)

#endif