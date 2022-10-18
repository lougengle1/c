#ifndef SLK_5_0_0_X_H
#define SLK_5_0_0_X_H


#define sl_get_kernel_time(SL_TIME_IN) { \
    struct timespec64 t; \
    ktime_get_real_ts64(&t);  \
    (SL_TIME_IN)->tv_sec = t.tv_sec; \
  }

#endif
