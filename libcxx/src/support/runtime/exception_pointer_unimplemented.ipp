// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <__verbose_abort>
#include <exception>

namespace std {

exception_ptr::~exception_ptr() noexcept {
#if defined(__UEFI__)
#  if _LIBCPP_HAS_EXCEPTIONS
#    error "EXCEPTIONS SHOULD BE DISABLED"
#  endif
#else
#  warning exception_ptr not yet implemented
  __libcpp_verbose_abort("exception_ptr not yet implemented\n");
#endif
}

exception_ptr::exception_ptr(const exception_ptr& other) noexcept : __ptr_(other.__ptr_) {
#if defined(__UEFI__)
#  if _LIBCPP_HAS_EXCEPTIONS
#    error "EXCEPTIONS SHOULD BE DISABLED"
#  endif
#else
#  warning exception_ptr not yet implemented
  __libcpp_verbose_abort("exception_ptr not yet implemented\n");
#endif
}

exception_ptr& exception_ptr::operator=(const exception_ptr& other) noexcept {
#if defined(__UEFI__)
#  if _LIBCPP_HAS_EXCEPTIONS
#    error "EXCEPTIONS SHOULD BE DISABLED"
#  endif
#else
#  warning exception_ptr not yet implemented
  __libcpp_verbose_abort("exception_ptr not yet implemented\n");
#endif
}

exception_ptr exception_ptr::__from_native_exception_pointer(void* __e) noexcept {
#if defined(__UEFI__)
#  if _LIBCPP_HAS_EXCEPTIONS
#    error "EXCEPTIONS SHOULD BE DISABLED"
#  endif
#else
#  warning exception_ptr not yet implemented
  __libcpp_verbose_abort("exception_ptr not yet implemented\n");
#endif
}

nested_exception::nested_exception() noexcept : __ptr_(current_exception()) {}

#if !defined(__GLIBCXX__)

nested_exception::~nested_exception() noexcept {}

#endif

[[noreturn]] void nested_exception::rethrow_nested() const {
#if defined(__UEFI__)
#  if _LIBCPP_HAS_EXCEPTIONS
#    error "EXCEPTIONS SHOULD BE DISABLED"
#  endif
  if (__ptr_ == nullptr)
    terminate();
  rethrow_exception(__ptr_);
#else
#  warning exception_ptr not yet implemented
  __libcpp_verbose_abort("exception_ptr not yet implemented\n");
#endif
}

exception_ptr current_exception() noexcept {
#if defined(__UEFI__)
#  if _LIBCPP_HAS_EXCEPTIONS
#    error "EXCEPTIONS SHOULD BE DISABLED"
#  endif
#else
#  warning exception_ptr not yet implemented
  __libcpp_verbose_abort("exception_ptr not yet implemented\n");
#endif
}

[[noreturn]] void rethrow_exception(exception_ptr p) {
#if defined(__UEFI__)
#  if _LIBCPP_HAS_EXCEPTIONS
#    error "EXCEPTIONS SHOULD BE DISABLED"
#  endif
  terminate();
#else
#  warning exception_ptr not yet implemented
  __libcpp_verbose_abort("exception_ptr not yet implemented\n");
#endif
}

} // namespace std
