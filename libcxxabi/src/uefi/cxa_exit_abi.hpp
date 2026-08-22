

#pragma once

#ifdef LLIBCXXABI_USE___CXA_ATEXIT

#  include <cstdint>

extern "C" {

extern void* __dso_handle;

typedef void (*__cxa_at_exit_destructor_function_t)(void*);

extern int __cxa_atexit(__cxa_at_exit_destructor_function_t destructor, void* arg, void* dso);

extern void __cxa_finalize(void* f);
}

#endif
