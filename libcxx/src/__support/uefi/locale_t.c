
#include "__support/uefi/locale_t.h"

static struct __uefi_locale __uefi_C_locale_storage;
locale_t __uefi_C_locale = &__uefi_C_locale_storage;

locale_t newlocale(int mask, const char* locale, locale_t base) {
  (void)mask;
  (void)locale;
  (void)base;

  return __uefi_C_locale;
}

locale_t duplocale(locale_t loc) {
  (void)loc;

  return __uefi_C_locale;
}

void freelocale(locale_t loc) { (void)loc; }

static locale_t __uefi_current_locale = &__uefi_C_locale_storage;

locale_t uselocale(locale_t loc) {
  locale_t old = __uefi_current_locale;

  if (loc != (locale_t)0) {
    __uefi_current_locale = loc;
  }

  return old;
}
