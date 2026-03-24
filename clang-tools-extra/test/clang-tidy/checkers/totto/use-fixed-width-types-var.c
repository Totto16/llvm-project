// RUN: %check_clang_tidy %s totto-use-fixed-width-types-var %t

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void f(int wrong_parameter);
// CHECK-MESSAGES: :[[@LINE-1]]:6: warning: variable 'wrong_parameter' has type 'int', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]

void f2(size_t fine_paramater);

void f3 (bool fine_too, uint8_t also_fine);

void f4(char wrong1, unsigned char wrong2, unsigned wrong3, short wrong4, unsigned short wrong5);
// CHECK-MESSAGES: :[[@LINE-1]]:14: warning: variable 'wrong1' has type 'char', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]
// CHECK-MESSAGES: [[@LINE-2]]:36: warning: variable 'wrong2' has type 'unsigned char', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]
// CHECK-MESSAGES: [[@LINE-3]]:53: warning: variable 'wrong3' has type 'unsigned int', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]
// CHECK-MESSAGES: [[@LINE-4]]:67: warning: variable 'wrong4' has type 'short', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]
// CHECK-MESSAGES: [[@LINE-5]]:90: warning: variable 'wrong5' has type 'unsigned short', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]

void f4_2(int wrong1, unsigned int wrong2, long wrong3, unsigned long wrong4, long long wrong5, unsigned long long wrong6);
// CHECK-MESSAGES: :[[@LINE-1]]:15: warning: variable 'wrong1' has type 'int', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]
// CHECK-MESSAGES: [[@LINE-2]]:36: warning: variable 'wrong2' has type 'unsigned int', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]
// CHECK-MESSAGES: [[@LINE-3]]:49: warning: variable 'wrong3' has type 'long', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]
// CHECK-MESSAGES: [[@LINE-4]]:71: warning: variable 'wrong4' has type 'unsigned long', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]
// CHECK-MESSAGES: [[@LINE-5]]:89: warning: variable 'wrong5' has type 'long long', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]
// CHECK-MESSAGES: [[@LINE-6]]:116: warning: variable 'wrong6' has type 'unsigned long long', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]

void f5(const char wrong_too);
// CHECK-MESSAGES: :[[@LINE-1]]:6: warning: variable 'wrong_too' has type 'char', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]

typedef int NativeFd;

typedef struct {
    int value;
} StructT;

void f6(const uint8_t fine, NativeFd fine2, StructT fine3);

void f7(void* fine, uint8_t* fine2);

void f8(void****** fine);

void f9(int * not_fine);
// CHECK-MESSAGES: :[[@LINE-1]]:34: warning: variable 'not_fine' has type 'int *', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]

void f10(const int const * const not_fine);
// CHECK-MESSAGES: :[[@LINE-1]]:34: warning: variable 'not_fine' has type 'int *', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]

void f10_exception(const char* exception_should_be_fine);
