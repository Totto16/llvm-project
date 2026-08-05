//===---------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===---------------------------------------------------------------------===//

#ifndef _LIBCPP___CSTDDEF_MAX_ALIGN_T_H
#define _LIBCPP___CSTDDEF_MAX_ALIGN_T_H

#if defined(__UEFI__)
//from: https://github.com/gcc-mirror/gcc/blob/master/gcc/ginclude/stddef.h
#if (defined (__STDC_VERSION__) && __STDC_VERSION__ >= 201112L) \
  || (defined(__cplusplus) && __cplusplus >= 201103L)
#ifndef _GCC_MAX_ALIGN_T
#define _GCC_MAX_ALIGN_T
/* Type whose alignment is supported in every context and is at least
   as great as that of any standard type not using alignment
   specifiers.  */
typedef struct {
  long long __max_align_ll __attribute__((__aligned__(__alignof__(long long))));
  long double __max_align_ld __attribute__((__aligned__(__alignof__(long double))));
  /* _Float128 is defined as a basic type, so max_align_t must be
     sufficiently aligned for it.  This code must work in C++, so we
     use __float128 here; that is only available on some
     architectures, but only on i386 is extra alignment needed for
     __float128.  */
#ifdef __i386__
  __float128 __max_align_f128 __attribute__((__aligned__(__alignof(__float128))));
#endif
} max_align_t;
#endif
#endif /* C11 or C++11.  */

#endif

#include <__config>
#include <stddef.h>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

#if !defined(_LIBCPP_CXX03_LANG)
using ::max_align_t _LIBCPP_USING_IF_EXISTS;
#endif

_LIBCPP_END_NAMESPACE_STD

#endif // _LIBCPP___CSTDDEF_MAX_ALIGN_T_H
