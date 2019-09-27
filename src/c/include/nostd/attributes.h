/**
 * @file attributes.h
 */

#ifndef NOSTD_ATTRIBUTES_H_
#define NOSTD_ATTRIBUTES_H_

#if defined(__GNUC__)

#   define NOSTD_ATTR_VISIBILITY_DEFAULT   __attribute__((visibility ("default")))
#   define NOSTD_ATTR_VISIBILITY_HIDDEN    __attribute__((visibility ("hidden")))
#   define NOSTD_ATTR_VISIBILITY_INTERNAL  __attribute__((visibility ("internal")))
#   define NOSTD_ATTR_VISIBILITY_PROTECTED __attribute__((visibility ("protected")))

#   define NOSTD_ATTR_CONSTRUCTOR __attribute__((constructor))
#   define NOSTD_ATTR_DESTRUCTOR __attribute__((destructor))

#else

#error Non-gcc compiler, defines can be incorrect

#   define NOSTD_ATTR_VISIBILITY_DEFAULT
#   define NOSTD_ATTR_VISIBILITY_HIDDEN
#   define NOSTD_ATTR_VISIBILITY_INTERNAL
#   define NOSTD_ATTR_VISIBILITY_PROTECTED

#   define NOSTD_ATTR_CONSTRUCTOR
#   define NOSTD_ATTR_DESTRUCTOR

#endif

#define ATTR_PUBLIC NOSTD_ATTR_VISIBILITY_DEFAULT
#define ATTR_PRIVATE NOSTD_ATTR_VISIBILITY_HIDDEN
/* #define ATTR_PRIVATE ATTR_VISIBILITY_INTERNAL */

#endif /* NOSTD_ATTRIBUTES_H_ */
