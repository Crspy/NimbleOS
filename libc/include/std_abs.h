#ifndef _STD_ABS_H
#define _STD_ABS_H 1

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    static inline int abs(int x)
    {
        return x >= 0 ? x : -x;
    }
    static inline long labs(long x)
    {
        return x >= 0 ? x : -x;
    }
    static inline long long llabs(long long x)
    {
        return x >= 0 ? x : -x;
    }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _STD_ABS_H */
