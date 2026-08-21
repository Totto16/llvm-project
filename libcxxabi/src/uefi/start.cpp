

#if !defined(__UEFI__)
#  error "Only availabel in UEFI"
#endif

#include <abort_message.h>

#include <libc/main.h>

#include "./cxa_exit_abi.hpp"

// see https://wiki.osdev.org/C%2B%2B#GCC
// for some reference, and some snippets I used, but modified heavily

atexit_func_entry_t __cxa_atexit_funcs[atexit_max_funcs];
uarch_t __cxa_atexit_func_count = 0;

int __cxa_atexit_cxx_impl(__cxa_at_exit_destructor_function_t destructor, void* arg, void* dso) {
  if (__cxa_atexit_func_count >= atexit_max_funcs) {
    return -1;
  }

  if (dso != __dso_handle) {
    __abort_message("Called '__cxa_atexit' with and invalid '__dso_handle': we only use static executables, so this "
                    "should always be the same: %p != %p",
                    dso, __dso_handle);
  }

  __cxa_atexit_funcs[__cxa_atexit_func_count] = atexit_func_entry_t{destructor, arg};

  ++__cxa_atexit_func_count;

  return 0;
}

void __cxa_finalize_cxx_impl(void* f) {
  // taken from https://wiki.osdev.org/C%2B%2B#GCC
  // but heavily modified

  // see: https://refspecs.linuxbase.org/LSB_5.0.0/LSB-Core-generic/LSB-Core-generic/baselib---cxa-finalize.html
  // for the specs

  if (f == nullptr) {
    /*
     *
     * According to the Itanium C++ ABI, if __cxa_finalize is called without a
     * function ptr, then it means that we should destroy EVERYTHING MUAHAHAHA!!
     *
     */

    for (uarch_t i = __cxa_atexit_func_count; i != 0; --i) {
      atexit_func_entry_t* const entry = &__cxa_atexit_funcs[i - 1];
      if (entry->__destructor != nullptr) {
        (*(entry->__destructor))(entry->__arg);
      }

      *entry = atexit_func_entry_t{nullptr, nullptr};
    }
    __cxa_atexit_func_count = 0;

    return;
  }

  for (uarch_t i = __cxa_atexit_func_count; i != 0; --i) {
    atexit_func_entry_t* const entry = &__cxa_atexit_funcs[i - 1];

    /*
     * The ABI states that multiple calls to the __cxa_finalize(destructor_func_ptr) function
     * should not destroy objects multiple times. Only one call is needed to eliminate multiple
     * entries with the same address.
     *
     **/
    if (entry->__destructor == f) {

      //should be always proven, as we check for f == nullptr previously
      _LIBCXXABI_ASSERT(entry->__destructor != nullptr,
                        "UNREACHABLE: destructor equal to f (!= NULL) should never be NULL");

      (*(entry->__destructor))(entry->__arg);

      *entry = atexit_func_entry_t{nullptr, nullptr};
    }
  }

  // clear the list of called functions, don't leaves holes, which is not even that complicated and can be accomplished in one iteration over the whole list

  uarch_t current_idx = 0;
  for (uarch_t i = 0; i < __cxa_atexit_func_count; ++i) {
    atexit_func_entry_t* const src_entry = &__cxa_atexit_funcs[i];

    if (src_entry->__destructor != nullptr) {

      if (i == current_idx) {
        // no move needed
      } else {
        atexit_func_entry_t* const dest_entry = &__cxa_atexit_funcs[current_idx];

        *dest_entry = *src_entry;
        *src_entry = atexit_func_entry_t{nullptr, nullptr};
      }

      // one slot is filled, so we move the index
      ++current_idx;
    } else {
      // do nothing, we don't need to move it, and we need to leave the current_idx as is
    }
  }
  __cxa_atexit_func_count = current_idx;
}

extern "C" {

/* Define __dso_handle which is needed for C++. But as we only have one static executable, this doesn't have to be unique or a valid address*/
void* __dso_handle = nullptr;

int __cxa_atexit(__cxa_at_exit_destructor_function_t destructor, void* arg, void* dso) {
  return __cxa_atexit_cxx_impl(destructor, arg, dso);
}

void __cxa_finalize(void* f) { __cxa_finalize_cxx_impl(f); }
}

void __cxa_uefi_init_libcxx() {
  //TODO
}

void __cxa_uefi_deinit_libcxx() {
  //TODO
}

int __cxa_uefi_entrypoint(IN int Argc, IN char** Argv) {

  // this is called from the edk2-libc, so the c standard library is already initialized

  //TODO: C constructors (in .init_array) are already initialized, what is with c++ global / static constructors?

  // we do initialization of the standard library, call the c++ main and than deinitialization
  // we need to pay attention to also do the cleanup,w ehn calling exit(), so we need some edk2-libc functionality to accomplish that

  __cxa_uefi_init_libcxx();
  edk2_libcxx_set_destroy(__cxa_uefi_deinit_libcxx);

  int result = EDK2_LIBCXX_ENTRY_NAME(Argc, Argv);

  edk2_libcxx_set_destroy(nullptr);
  __cxa_uefi_deinit_libcxx();

  return result;
}

extern "C" {
int EDK2_LIBC_ENTRY_NAME(IN int Argc, IN char** Argv) { return __cxa_uefi_entrypoint(Argc, Argv); }
}
