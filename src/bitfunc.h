/*
 * bitfunc.h
 *
 *  Created on: Aug 19, 2015
 *      Author: max
 */

#ifndef SRC_BITFUNC_H_
#define SRC_BITFUNC_H_

#include <assert.h>

#include "types.h"


static inline int popCount (bitboard bb) {
   int count = 0;
   while (bb) {
       count++;
       bb &= bb - 1; // reset LS1B
   }
   return count;
}

static inline unsigned char _BitScanForward64(unsigned long* Index, bitboard Mask)
{
	bitboard Ret;
    __asm__
    (
        "bsfq %[Mask], %[Ret]"
        :[Ret] "=r" (Ret)
        :[Mask] "mr" (Mask)
    );
    *Index = (unsigned long)Ret;
    return Mask?1:0;
}

static inline int bitScanForward(bitboard bb) {
   unsigned long index;
   assert (bb != 0);
   _BitScanForward64(&index, bb);
   return (int) index;
}

static inline int pop_lsb(bitboard &bb) {
	assert (bb != 0);
    int idx = bitScanForward(bb);
    bb &= bb - 1; // reset bit outside
    return idx;
}

#endif /* SRC_BITFUNC_H_ */
