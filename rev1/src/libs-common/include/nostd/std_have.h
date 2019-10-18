/**
 * @file std_have.h
 */

#ifndef NOSTD_STD_HAVE_H_
#define NOSTD_STD_HAVE_H_

/*
gcc -std=c89 -E -dM -x c /dev/null
gcc -std=c90 -E -dM -x c /dev/null
gcc -std=c99 -E -dM -x c /dev/null
gcc -std=c11 -E -dM -x c /dev/null

gcc -std=c++98 -E -dM -x c++ /dev/null
gcc -std=c++03 -E -dM -x c++ /dev/null
gcc -std=c++0x -E -dM -x c++ /dev/null
gcc -std=c++11 -E -dM -x c++ /dev/null
gcc -std=c++1y -E -dM -x c++ /dev/null
gcc -std=c++14 -E -dM -x c++ /dev/null
gcc -std=c++17 -E -dM -x c++ /dev/null
*/

/*
NOSTD_STD_HAVE_INLINE
NOSTD_STD_HAVE_AUTOTYPE
NOSTD_STD_HAVE_DEFAULT_AND_DELETE
NOSTD_STD_HAVE_RVALUE - move semantic
NOSTD_STD_HAVE_NOEXEPT
NOSTD_STD_HAVE_ENUM_CLASS
NOSTD_STD_HAVE_LAMBDA
NOSTD_STD_HAVE_NULLPTR
NOSTD_STD_HAVE_DELEGATING_CONSTRUCTORS
*/

#if !defined(__cplusplus)

#   if !defined(__STDC_VERSION__)
#       define STD_C90
#   elif __STDC_VERSION__ == 199901L
#       define STD_C99
#       define NOSTD_STD_HAVE_INLINE
#   elif __STDC_VERSION__ == 201112L
#       define STD_C11
#       define NOSTD_STD_HAVE_INLINE
#   endif

#else

#   if   __cplusplus == 199711L
#       define STD_CPP98
#       define NOSTD_STD_HAVE_INLINE
#   elif __cplusplus == 201103L
#       define STD_CPP11
#       define NOSTD_STD_HAVE_INLINE
#       define NOSTD_STD_HAVE_AUTOTYPE
#       define NOSTD_STD_HAVE_DEFAULT_AND_DELETE
#       define NOSTD_STD_HAVE_RVALUE
#       define NOSTD_STD_HAVE_NOEXEPT
#       define NOSTD_STD_HAVE_ENUM_CLASS
#       define NOSTD_STD_HAVE_LAMBDA
#       define NOSTD_STD_HAVE_NULLPTR
#       define NOSTD_STD_HAVE_DELEGATING_CONSTRUCTORS
#   elif __cplusplus == 201300L
#       define STD_СPP14
#       define NOSTD_STD_HAVE_INLINE
#       define NOSTD_STD_HAVE_AUTOTYPE
#       define NOSTD_STD_HAVE_DEFAULT_AND_DELETE
#       define NOSTD_STD_HAVE_RVALUE
#       define NOSTD_STD_HAVE_NOEXEPT
#       define NOSTD_STD_HAVE_ENUM_CLASS
#       define NOSTD_STD_HAVE_LAMBDA
#       define NOSTD_STD_HAVE_NULLPTR
#       define NOSTD_STD_HAVE_DELEGATING_CONSTRUCTORS
#   elif __cplusplus == 201402L
#       define STD_CPP14
#       define NOSTD_STD_HAVE_INLINE
#       define NOSTD_STD_HAVE_AUTOTYPE
#       define NOSTD_STD_HAVE_DEFAULT_AND_DELETE
#       define NOSTD_STD_HAVE_RVALUE
#       define NOSTD_STD_HAVE_NOEXEPT
#       define NOSTD_STD_HAVE_ENUM_CLASS
#       define NOSTD_STD_HAVE_LAMBDA
#       define NOSTD_STD_HAVE_NULLPTR
#       define NOSTD_STD_HAVE_DELEGATING_CONSTRUCTORS
#   elif __cplusplus == 201703L
#       define STD_CPP17
#       define NOSTD_STD_HAVE_INLINE
#       define NOSTD_STD_HAVE_AUTOTYPE
#       define NOSTD_STD_HAVE_DEFAULT_AND_DELETE
#       define NOSTD_STD_HAVE_RVALUE
#       define NOSTD_STD_HAVE_NOEXEPT
#       define NOSTD_STD_HAVE_ENUM_CLASS
#       define NOSTD_STD_HAVE_LAMBDA
#       define NOSTD_STD_HAVE_NULLPTR
#       define NOSTD_STD_HAVE_DELEGATING_CONSTRUCTORS
#   endif

#endif

#endif /* NOSTD_STD_HAVE_H_ */
