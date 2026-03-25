//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_TOTTO_FUNCTIONPASSINGTYPECHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_TOTTO_FUNCTIONPASSINGTYPECHECK_H

#include "../ClangTidyCheck.h"

namespace clang::tidy::totto {

namespace custom {
struct RegexOption {
  std::string ValueInput;
  llvm::Regex Value;

  RegexOption(llvm::StringRef Str) : ValueInput{Str}, Value{Str} {}
};
} // namespace custom

/// FIXME: Write a short description.
///
/// For the user-facing documentation see:
/// https://clang.llvm.org/extra/clang-tidy/checks/totto/function-passing-type.html
class FunctionPassingTypeCheck : public ClangTidyCheck {
public:
  FunctionPassingTypeCheck(StringRef Name, ClangTidyContext *Context);

  // only allow C, as in C++ we should use references instead of ptrs, and this
  // works with ptrs!
  bool isLanguageVersionSupported(const LangOptions &LangOpts) const override {
    return LangOpts.C99 || LangOpts.C11 || LangOpts.C17 || LangOpts.C23 ||
           LangOpts.C2y;
  }

  void storeOptions(ClangTidyOptions::OptionMap &Opts) override;
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  custom::RegexOption ByValue;
  custom::RegexOption ByPtr;
};

} // namespace clang::tidy::totto

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_TOTTO_FUNCTIONPASSINGTYPECHECK_H
