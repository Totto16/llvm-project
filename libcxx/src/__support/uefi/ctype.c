
#include "__support/uefi/ctype.h"

#include <ctype.h>

int isalpha_l(int c, locale_t loc) {
  (void)loc;
  return isalpha(c);
}

int isdigit_l(int c, locale_t loc) {
  (void)loc;
  return isdigit(c);
}

int toupper_l(int c, locale_t loc) {
  (void)loc;
  return toupper(c);
}

int tolower_l(int c, locale_t loc) {
  (void)loc;
  return tolower(c);
}
