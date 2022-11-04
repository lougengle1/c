#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef int spin_lock_t;

static inline void spin_lock(spin_lock_t *p_c)
{
	while (__sync_val_compare_and_swap(p_c, 0, 1) != 0);
}

static inline void  spin_unlock(spin_lock_t *p_c)
{
	__sync_lock_release(p_c);
}

#ifdef __cplusplus
}
#endif