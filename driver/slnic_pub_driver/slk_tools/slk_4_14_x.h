#ifndef SLK_4_14_X_H
#define SLK_4_14_X_H

#define copy_from_user(to,from,len) raw_copy_from_user((to),(from),(len))
#define copy_to_user(to,from,len) raw_copy_to_user((to),(from),(len))

#endif