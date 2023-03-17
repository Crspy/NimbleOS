#ifndef STD_ABS_H
#define STD_ABS_H


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#define DEF_ABS(type,name) \
static inline type name(type x)  { return x >= 0 ? x : -x; }
DEF_ABS(int,abs)
DEF_ABS(long,labs)
DEF_ABS(long long,llabs)


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _STD_ABS_H */
