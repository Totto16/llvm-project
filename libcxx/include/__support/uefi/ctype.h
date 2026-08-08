// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___SUPPORT_UEFI_CTYPE_H
#define _LIBCPP___SUPPORT_UEFI_CTYPE_H

#ifndef __UEFI__
#  error "Only supported on UEFI"
#endif

#include <__support/uefi/locale_t.h>

#ifdef __cplusplus
extern "C" {
#endif

int isalpha_l(int c, locale_t loc);

int isdigit_l(int c, locale_t loc);

int toupper_l(int c, locale_t loc);

int tolower_l(int c, locale_t loc);

#ifdef __cplusplus
}
#endif

#endif // _LIBCPP___SUPPORT_UEFI_CTYPE_H
