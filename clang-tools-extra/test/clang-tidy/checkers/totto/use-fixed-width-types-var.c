// RUN: %check_clang_tidy %s totto-use-fixed-width-types-var %t

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void f(int wrong_parameter);
// CHECK-MESSAGES: :[[@LINE-1]]:12: error: variable 'wrong_parameter' has type 'int', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]

void f2(size_t fine_paramater);

void f3 (bool fine_too, uint8_t also_fine);

void f4(char wrong1, unsigned char wrong2, unsigned wrong3, short wrong4, unsigned short wrong5);
// CHECK-MESSAGES: :[[@LINE-1]]:14: error: variable 'wrong1' has type 'char', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]
// CHECK-MESSAGES: [[@LINE-2]]:36: error: variable 'wrong2' has type 'unsigned char', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]
// CHECK-MESSAGES: [[@LINE-3]]:53: error: variable 'wrong3' has type 'unsigned int', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]
// CHECK-MESSAGES: [[@LINE-4]]:67: error: variable 'wrong4' has type 'short', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]
// CHECK-MESSAGES: [[@LINE-5]]:90: error: variable 'wrong5' has type 'unsigned short', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]

void f4_2(int wrong1, unsigned int wrong2, long wrong3, unsigned long wrong4, long long wrong5, unsigned long long wrong6);
// CHECK-MESSAGES: :[[@LINE-1]]:15: error: variable 'wrong1' has type 'int', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]
// CHECK-MESSAGES: [[@LINE-2]]:36: error: variable 'wrong2' has type 'unsigned int', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]
// CHECK-MESSAGES: [[@LINE-3]]:49: error: variable 'wrong3' has type 'long', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]
// CHECK-MESSAGES: [[@LINE-4]]:71: error: variable 'wrong4' has type 'unsigned long', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]
// CHECK-MESSAGES: [[@LINE-5]]:89: error: variable 'wrong5' has type 'long long', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]
// CHECK-MESSAGES: [[@LINE-6]]:116: error: variable 'wrong6' has type 'unsigned long long', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]

void f5(const char wrong_too);
// CHECK-MESSAGES: :[[@LINE-1]]:20: error: variable 'wrong_too' has type 'char', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]

typedef int NativeFd;

typedef struct {
    size_t value;
} StructOk;

void f6(const uint8_t fine, NativeFd fine2, StructOk fine3);

void f7(void* fine, uint8_t* fine2);

void f8(void****** fine);

void f9(int * not_fine);
// CHECK-MESSAGES: :[[@LINE-1]]:15: error: variable 'not_fine' has type 'int*', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]

void f10(const int const * const not_fine);
// CHECK-MESSAGES: :[[@LINE-1]]:34: error: variable 'not_fine' has type 'int*', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]

void f10_exception(const char* exception_should_be_fine);

int f11(size_t fine){
    // CHECK-MESSAGES: :[[@LINE-1]]:1: error: return type for function 'f11' has type 'int', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]

    size_t valid = (size_t)1L;

    //TODO
    return (int)1L + valid;
    // CHECK-MESSAGES: :[[@LINE-1]]:12: error: cast to type 'int', should be rewritten into using a fixed type [totto-use-fixed-width-types-var]
}


size_t f12(size_t ok_fn);

typedef enum {
    EnumOk1Val1,
    EnumOk1Val2
} EnumOk1;

typedef enum : uint8_t {
    EnumOk2Val1,
    EnumOk2Val2
} EnumOk2;

EnumOk1 f13(size_t ok_fn, EnumOk2 ok_2);

// CHECK-MESSAGES: :[[@LINE+1]]:9: error: enum 'EnumWrong1' has the underlying type 'int', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]
typedef enum : int {
    EnumWrong1Val1,
    EnumWrong1Val2
} EnumWrong1;

// CHECK-MESSAGES: :[[@LINE+1]]:6: error: enum 'EnumWrong2' has the underlying type 'int', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]
enum EnumWrong2 : int {
    EnumWrong2Val1,
    EnumWrong2Val2
};

typedef struct {
    int wrong1;
    char wrong2;
} StructWrong1;
// CHECK-MESSAGES: :[[@LINE-3]]:9: error: member 'wrong1' of the struct 'StructWrong1' has type 'int', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]
// CHECK-MESSAGES: :[[@LINE-3]]:10: error: member 'wrong2' of the struct 'StructWrong1' has type 'char', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]

// CHECK-MESSAGES: :[[@LINE+2]]:9: error: member 'wrong3' of the struct 'StructWrong2' has type 'int', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]
struct StructWrong2 {
    int wrong3;
};

EnumWrong1 f14(StructWrong1 s, enum EnumWrong2 e, struct StructWrong2 s2);

int32_t f15(){
    struct { int a; } result = { .a = 1 };
// CHECK-MESSAGES: :[[@LINE-1]]:18: error: member 'a' of the struct '(anonymous struct)' has type 'int', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]

    return (int32_t)result.a;
}

int32_t f16(){
    enum : int { EnumUnamed1} result = EnumUnamed1;
// CHECK-MESSAGES: :[[@LINE-1]]:5: error: enum '(anonymous enum)' has the underlying type 'int', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]

    return (int32_t)result;
}


float f17(double should_be_ok){
    size_t valid_type[2] = {};

    //TODO: make this work
    char invalid_type[8] = {};
// CHECK-MESSAGES: :[[@LINE-1]]:10: error: variable 'invalid_type' has type '(char)[]', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]

    int invalid_type2[2] = {};
// CHECK-MESSAGES: :[[@LINE-1]]:9: error: variable 'invalid_type2' has type '(int)[]', which should be rewritten into using a fixed type [totto-use-fixed-width-types-var]

    return (invalid_type[0] + valid_type[0] + invalid_type2[0]) == 0 ? 0.0F : 1.0F;
}


