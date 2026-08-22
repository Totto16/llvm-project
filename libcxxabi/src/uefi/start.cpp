#if !defined(__UEFI__)
#  error "Only availabel in UEFI"
#endif

#include <abort_message.h>

#include <libc/main.h>

#include "./cxa_exit_abi.hpp"

#ifdef LLIBCXXABI_USE___CXA_ATEXIT

// see https://wiki.osdev.org/C%2B%2B#GCC
// for some reference, and some snippets I used, but modified heavily

#  include <cstdlib>
#  include <type_traits>
#  include <utility>

namespace CXA::helper {

struct atexit_func_entry_t {
  __cxa_at_exit_destructor_function_t __destructor;
  void* __arg;
  //NOTE: don't use the handle, as we don't need it
  // void* dso_handle;
};

static_assert(std::is_trivially_destructible_v<atexit_func_entry_t>);

struct CXAAtexitFunctions {
private:
  constexpr static size_t block_size = 32;
  constexpr static size_t start_cap_mul = 8;
  constexpr static size_t increase_multiplier = 4;

public:
  atexit_func_entry_t* m_funcs;
  size_t m_size;
  size_t m_capacity;

  consteval CXAAtexitFunctions() : m_funcs{nullptr}, m_size{0}, m_capacity{0} {}

  CXAAtexitFunctions(const CXAAtexitFunctions&) = delete;
  CXAAtexitFunctions& operator=(const CXAAtexitFunctions&) = delete;

  CXAAtexitFunctions(CXAAtexitFunctions&&) = delete;
  CXAAtexitFunctions& operator=(CXAAtexitFunctions&&) = delete;

  //TODO: make sure this is never called
  constexpr ~CXAAtexitFunctions() = default;

  [[nodiscard]] bool init() {
    const size_t capacity = CXAAtexitFunctions::block_size * CXAAtexitFunctions::start_cap_mul;
    atexit_func_entry_t* funcs = (atexit_func_entry_t*)malloc(sizeof(*funcs) * capacity);

    if (funcs == nullptr) {
      return false;
    }

    this->m_funcs = funcs;
    this->m_size = 0;
    this->m_capacity = capacity;
    return true;
  }

  [[nodiscard]] bool __partial_capacity_resize() {
    const size_t old_block_size =
        (this->m_size / (CXAAtexitFunctions::block_size * CXAAtexitFunctions::increase_multiplier));
    const size_t new_capacity =
        (old_block_size + 1) * (CXAAtexitFunctions::block_size * CXAAtexitFunctions::increase_multiplier);
    _LIBCXXABI_ASSERT(new_capacity > this->m_capacity, "New capacity has to be greater than the old one");

    atexit_func_entry_t* new_funcs = (atexit_func_entry_t*)realloc(this->m_funcs, sizeof(*new_funcs) * new_capacity);

    if (new_funcs == nullptr) {
      return false;
    }

    this->m_funcs = new_funcs;
    this->m_capacity = new_capacity;
    return true;
  }

  [[nodiscard]] bool append(atexit_func_entry_t&& entry) {

    if (this->m_size >= this->m_capacity) {

      const size_t new_capacity =
          this->m_capacity + (CXAAtexitFunctions::block_size * CXAAtexitFunctions::increase_multiplier);
      _LIBCXXABI_ASSERT(new_capacity > this->m_capacity, "New capacity has to be greater than the old one");

      atexit_func_entry_t* new_funcs = (atexit_func_entry_t*)realloc(this->m_funcs, sizeof(*new_funcs) * new_capacity);

      if (new_funcs == nullptr) {
        return false;
      }

      this->m_funcs = new_funcs;
      this->m_capacity = new_capacity;
    }

    this->m_funcs[this->m_size] = std::move(entry);
    ++(this->m_size);

    return true;
  }

  void clear_all() {
    for (size_t i = this->m_size; i != 0; --i) {
      atexit_func_entry_t* const entry = &((this->m_funcs)[i - 1]);
      if (entry->__destructor != nullptr) {
        (*(entry->__destructor))(entry->__arg);
      }

      *entry = atexit_func_entry_t{nullptr, nullptr};
    }
    this->m_size = 0;

    // reset the capacity, if possible
    atexit_func_entry_t* old_funcs = this->m_funcs;
    const size_t old_size = this->m_size;
    const size_t old_capacity = this->m_capacity;
    bool ok = this->init();

    if (!ok) {
      this->m_funcs = old_funcs;
      this->m_size = old_size;
      this->m_capacity = old_capacity;
    } else {
      free(old_funcs);
    }
  }

  void clear_by(void* f) {
    for (size_t i = this->m_size; i != 0; --i) {
      atexit_func_entry_t* const entry = &((this->m_funcs)[i - 1]);

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

    size_t current_idx = 0;
    for (size_t i = 0; i < this->m_size; ++i) {
      atexit_func_entry_t* const src_entry = &((this->m_funcs)[i]);

      if (src_entry->__destructor != nullptr) {

        if (i == current_idx) {
          // no move needed
        } else {
          atexit_func_entry_t* const dest_entry = &((this->m_funcs)[current_idx]);

          *dest_entry = *src_entry;
          *src_entry = atexit_func_entry_t{nullptr, nullptr};
        }

        // one slot is filled, so we move the index
        ++current_idx;
      } else {
        // do nothing, we don't need to move it, and we need to leave the current_idx as is
      }
    }
    this->m_size = current_idx;

    // reset the capacity, if possible, not reseting everything, as we have still some things left
    atexit_func_entry_t* old_funcs = this->m_funcs;
    const size_t old_size = this->m_size;
    const size_t old_capacity = this->m_capacity;
    bool ok = this->__partial_capacity_resize();

    if (!ok) {
      this->m_funcs = old_funcs;
      this->m_size = old_size;
      this->m_capacity = old_capacity;
    } else {
      free(old_funcs);
    }
  }

  void deinit() {

    free(this->m_funcs);

    this->m_funcs = nullptr;
    this->m_size = 0;
    this->m_capacity = 0;
  }
};

static_assert(std::is_trivially_destructible_v<CXAAtexitFunctions>);

} // namespace CXA::helper

constinit CXA::helper::CXAAtexitFunctions __cxa_atexit_funcs{};

int __cxa_atexit_cxx_impl(__cxa_at_exit_destructor_function_t destructor, void* arg, void* dso) {

  // see: https://refspecs.linuxbase.org/LSB_5.0.0/LSB-Core-generic/LSB-Core-generic/baselib---cxa-atexit.html
  // for the specs

  if (__cxa_atexit_funcs.m_funcs == nullptr) {
    __abort_message("Called '__cxa_atexit' before having initialized the state required by '__cxa_atexit'");
  }

  if (dso != &__dso_handle) {
    __abort_message("Called '__cxa_atexit' with and invalid '__dso_handle': we only use static executables, so this "
                    "should always be the same: %p != %p",
                    dso, &__dso_handle);
  }

  if (!__cxa_atexit_funcs.append(CXA::helper::atexit_func_entry_t{destructor, arg})) {
    return -1;
  }

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

    __cxa_atexit_funcs.clear_all();
    return;
  }

  __cxa_atexit_funcs.clear_by(f);
}

extern "C" {

/* Define __dso_handle which is needed for C++. But as we only have one static executable, this doesn't have to be unique or a valid address*/
void* __dso_handle = nullptr;

int __cxa_atexit(__cxa_at_exit_destructor_function_t destructor, void* arg, void* dso) {
  return __cxa_atexit_cxx_impl(destructor, arg, dso);
}

void __cxa_finalize(void* f) { __cxa_finalize_cxx_impl(f); }
}

#endif

// see: https://gcc.gnu.org/onlinedocs/gccint/Initialization.html
// for more information on how gcc handles initialization

void __cxa_uefi_init_libcxx() {
  // Not really used, as we use the C way of using .init_array and not .ctors

#ifdef LLIBCXXABI_USE___CXA_ATEXIT
  // but we need to setup the __cxa_atexit state, if we use it

  _LIBCXXABI_ASSERT(__cxa_atexit_funcs.init(), "Initializing of the state required by '__cxa_atexit' failed");
#endif
}

void __cxa_uefi_deinit_libcxx() {
#ifndef LLIBCXXABI_USE___CXA_ATEXIT

  // Not really used when:
  // -fno-use-cxa-atexit is used:
  // -> the C way of using .fini_array and not .dtors
  // already clean this up
#else

  // we need to call the remaining destructors when:
  // -fuse-cxa-atexit is used:
  // -> using the __cxa_atexit methods defined above
  // calls all the remaining destructors

  __cxa_finalize(nullptr);
  // deinitialize the __cxa_atexit state
  __cxa_atexit_funcs.deinit();

#endif
}

int __cxa_uefi_entrypoint(IN int Argc, IN char** Argv) {

  // this is called from the edk2-libc, so the c standard library is already initialized

  //TODO: C constructors (in .init_array) are already initialized, what is with c++ global / static constructors?

  // we do initialization of the standard library, call the c++ main and than deinitialization
  // we need to pay attention to also do the cleanup,w ehn calling exit(), so we need some edk2-libc functionality to accomplish that

  //TODO: this needs to be called before init_array, as that might use cxa_atexit
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
