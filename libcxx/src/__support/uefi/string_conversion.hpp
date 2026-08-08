#pragma once

#include <codecvt>
#include <locale>
#include <string>

using namespace std;

inline wstring stringFromUTF8(const string& text) {
  wstring result;
  result.resize(text.length());
  mbstowcs(&result[0], &text[0], text.length());
  return result;
}

inline string stringToUTF8(const wstring& text) {
  string result;
  result.resize(text.length());
  wcstombs(&result[0], &text[0], text.length());
  return result;
}
