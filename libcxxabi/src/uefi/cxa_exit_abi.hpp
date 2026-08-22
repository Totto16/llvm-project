

#pragma once

#ifdef LLIBCXXABI_USE___CXA_ATEXIT

#  include <cstdint>

extern "C" {

extern void* __dso_handle;

typedef void (*__cxa_at_exit_destructor_function_t)(void*);

extern int __cxa_atexit(__cxa_at_exit_destructor_function_t destructor, void* arg, void* dso);

extern void __cxa_finalize(void* f);
}

using uarch_t = uint8_t;

constexpr uarch_t atexit_max_funcs = 128;

struct atexit_func_entry_t {
  __cxa_at_exit_destructor_function_t __destructor;
  void* __arg;
  //NOTE: don't use the handle, as we don't need it
  // void* dso_handle;
};

#endif
