#pragma once

#define unlikely(t)  __builtin_expect((t), 0)
#define likely(t)  __builtin_expect((t), 1)

#define wmb()       __asm__ __volatile__("": : :"memory")
#define wmb_wc()    __builtin_ia32_sfence()
#define mmiowb()    do{}while(0)
#define smp_rmb()   __asm__ __volatile__("lfence": : :"memory")

#define force_inline __inline__ __attribute__((always_inline))