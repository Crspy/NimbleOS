#ifndef ASSERT_H
#define ASSERT_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef NDEBUG
#define STRINGIFY(x) #x
    __attribute__((noreturn)) void __assertion_failed(char const *msg);
#define assert(expr)                                                                                 \
    do                                                                                               \
    {                                                                                                \
        if (!(expr))                                                                                 \
        {                                                                                            \
            printf("\x1B[31m%s:%d: %s: Assertion `%s` failed.\n",__FILE__,__LINE__,__PRETTY_FUNCTION__, STRINGIFY(expr)); \
            abort();                                                                                 \
        }                                                                                            \
    } while (0)

#else
#define assert(expr) ((void)(0))
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ASSERT_H */