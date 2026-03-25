//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "../ClangTidy.h"
#include "../ClangTidyModule.h"
#include "ConstCorrectnessCheckC.h"
#include "FunctionPassingTypeCheck.h"
#include "UseFixedWidthTypesVarCheck.h"

namespace clang::tidy {
namespace totto {
namespace {

class TottoModule : public ClangTidyModule {
public:
  void addCheckFactories(ClangTidyCheckFactories &CheckFactories) override {
    CheckFactories.registerCheck<FunctionPassingTypeCheck>(
        "totto-function-passing-type");
    CheckFactories.registerCheck<UseFixedWidthTypesVarCheck>(
        "totto-use-fixed-width-types-var");
    CheckFactories.registerCheck<ConstCorrectnessCheckC>(
        "totto-const-correctness-c");
  }
};

} // namespace
} // namespace totto

// Register the TottoTidyModule using this statically initialized variable.
static ClangTidyModuleRegistry::Add<totto::TottoModule>
    X("totto-module", "Adds totto's lint checks.");

// This anchor is used to force the linker to link in the generated object file
// and thus register the TottoModule.
volatile int TottoModuleAnchorSource = 0; // NOLINT(misc-use-internal-linkage)

} // namespace clang::tidy
