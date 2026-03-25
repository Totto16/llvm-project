// RUN: %check_clang_tidy %s totto-function-passing-type %t \
// RUN:   -config="{CheckOptions: \
// RUN:     { \
// RUN: totto-function-passing-type.ByValue: '^(PassByValue)$', \
// RUN: totto-function-passing-type.ByPtr: '^(PassByPtr)$', \
// RUN:  }}"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    char small[8];
}PassByValue;

void f(PassByValue value, int hello);

typedef struct {
    unsigned long long big[128];
}PassByPtr;

void f2(PassByPtr* const value);

void f3(PassByPtr wrong_1);
// CHECK-MESSAGES: :[[@LINE-1]]:19: warning: paramater 'wrong_1' with type 'PassByPtr' matches the types, that need to be passed by ptr, so don't pass it by value [totto-function-passing-type]

void f4(PassByValue* const wrong_2);
// CHECK-MESSAGES: :[[@LINE-1]]:28: warning: paramater 'wrong_2' with type 'PassByValue' matches the types, that need to be passed by value, so don't pass it by ptr [totto-function-passing-type]
