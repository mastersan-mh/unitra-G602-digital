/**
 * @file common.h
 */

#ifndef NOSTD_FIXED_PRIVATE_COMMON_H_
#define NOSTD_FIXED_PRIVATE_COMMON_H_

#if !defined(__cplusplus)

#   if defined(__STDC_VERSION__)
#       define FIXEDPOINT_STD_HAVE_INLINE
#   endif

#define FIXEDPOINT_C_NOEXCEPT

#else

#   define FIXEDPOINT_STD_HAVE_INLINE

#define FIXEDPOINT_C_NOEXCEPT noexcept

#endif


#ifndef FIXEDPOINT_STD_HAVE_INLINE
#   define FIXEDPOINT_STD_INLINE
#else
#   define FIXEDPOINT_STD_INLINE inline
#endif

/* #define FIXEDPOINT_DENY_INLINE // */
#define FIXEDPOINT_ALLOW_INLINE // */
/* #define FIXEDPOINT_ALLOW_STRICT_INLINE // */

#if defined(FIXEDPOINT_DENY_INLINE)
#   define FIXEDPOINT_INLINE
#elif defined(FIXEDPOINT_ALLOW_INLINE)
#   define FIXEDPOINT_INLINE FIXEDPOINT_STD_INLINE
#elif defined(FIXEDPOINT_ALLOW_STRICT_INLINE)
#   define FIXEDPOINT_INLINE __attribute__((always_inline)) FIXEDPOINT_STD_INLINE
#endif


#endif /* NOSTD_FIXED_PRIVATE_COMMON_H_ */
