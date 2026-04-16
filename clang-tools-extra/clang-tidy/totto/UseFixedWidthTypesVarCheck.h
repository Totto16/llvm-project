//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_TOTTO_USEFIXEDWIDTHTYPESVARCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_TOTTO_USEFIXEDWIDTHTYPESVARCHECK_H

#include "../ClangTidyCheck.h"

namespace clang::tidy::totto {

/// FIXME: Write a short description.
///
/// For the user-facing documentation see:
/// https://clang.llvm.org/extra/clang-tidy/checks/totto/use-fixed-width-types-var.html
class UseFixedWidthTypesVarCheck : public ClangTidyCheck {
public:
  UseFixedWidthTypesVarCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

  // only allow C, as we don't have to handle complex c++ types
  bool isLanguageVersionSupported(const LangOptions &LangOpts) const override {
    return LangOpts.C99 || LangOpts.C11 || LangOpts.C17 || LangOpts.C23 ||
           LangOpts.C2y;
  }

private:
  void processVarDecl(const VarDecl &decl);
  void processCastExpr(const CastExpr &decl);
  void processFunctionDecl(const FunctionDecl &decl);
  void processFieldDecl(const FieldDecl &decl);
  void processEnumDecl(const EnumDecl &decl);
};

} // namespace clang::tidy::totto

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_TOTTO_USEFIXEDWIDTHTYPESVARCHECK_H
