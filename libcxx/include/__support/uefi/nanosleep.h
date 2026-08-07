// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___SUPPORT_UEFI_NANOSLEEP_H
#define _LIBCPP___SUPPORT_UEFI_NANOSLEEP_H

#include <Library/TimerLib.h>
#include <unistd.h>

inline int nanosleep(const struct timespec* __req, struct timespec* __rem) {
  // The nanosleep() function is not available on uefi. Therefore, we will call
  // NanoSecondDelay

  if (__req == nullptr) {
    return -1;
  }

  UINT64 ns = (UINT64)__req->tv_sec * 1000000000ULL + (UINT64)__req->tv_nsec;

  NanoSecondDelay(ns);

  if (__rem != nullptr) {
    __rem->tv_sec  = 0;
    __rem->tv_nsec = 0;
  }

  return 0;
}

#endif // _LIBCPP___SUPPORT_UEFI_NANOSLEEP_H
