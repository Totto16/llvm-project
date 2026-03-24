//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_TOTTO_CONSTCORRECTNESSCHECK_C_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_TOTTO_CONSTCORRECTNESSCHECK_C_H

#include "../ClangTidyCheck.h"
#include "clang/Analysis/Analyses/ExprMutationAnalyzer.h"
#include "llvm/ADT/DenseSet.h"

namespace clang::tidy::totto {

/// This check warns on variables which could be declared const but are not.
///
// copy of clang::tidy::misc:ConstCorrectnessCheck
//  but supports c
class ConstCorrectnessCheckC : public ClangTidyCheck {
public:
  ConstCorrectnessCheckC(StringRef Name, ClangTidyContext *Context);

  // only allow C, even if the original says, it doesn't work on C
  bool isLanguageVersionSupported(const LangOptions &LangOpts) const override {
    return LangOpts.C99 || LangOpts.C11 || LangOpts.C17 || LangOpts.C23 ||
           LangOpts.C2y;
  }
  void storeOptions(ClangTidyOptions::OptionMap &Opts) override;
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
  void registerScope(const Stmt *LocalScope, ASTContext *Context);

  bool isMutated(const VarDecl *Variable, const Stmt *Scope,
                 const FunctionDecl *Func, ASTContext *Context);
  using MutationAnalyzer = std::unique_ptr<ExprMutationAnalyzer>;
  llvm::DenseMap<const Stmt *, MutationAnalyzer> ScopesCache;
  llvm::DenseSet<SourceLocation> TemplateDiagnosticsCache;
  ExprMutationAnalyzer::Memoized ParamMutationAnalyzerMemoized;

  const bool AnalyzePointers;
  const bool AnalyzeReferences;
  const bool AnalyzeValues;
  const bool AnalyzeParameters;

  const bool WarnPointersAsPointers;
  const bool WarnPointersAsValues;

  const bool TransformPointersAsPointers;
  const bool TransformPointersAsValues;
  const bool TransformReferences;
  const bool TransformValues;

  const std::vector<StringRef> AllowedTypes;
};

} // namespace clang::tidy::totto

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_TOTTO_CONSTCORRECTNESSCHECK_C_H
