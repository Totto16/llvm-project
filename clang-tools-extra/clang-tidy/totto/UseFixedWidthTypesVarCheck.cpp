//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "UseFixedWidthTypesVarCheck.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang::tidy::totto {

void UseFixedWidthTypesVarCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(varDecl().bind("var"), this);
}

enum class ClassifiedType : std::uint8_t {
  BuiltinOther,
  BuiltinInteger,
  UserDefined
};

struct ClassifiedTypeResult {
public:
  ClassifiedType type;
  size_t depth;
  QualType val;

private:
  [[nodiscard]] std::string format_type() const {
    return this->val.getAsString();
  }

public:
  [[nodiscard]] std::string format() const {
    if (depth == 0)
      return this->format_type();

    std::string Result = this->format_type();

    for (size_t i = 0; i < this->depth; ++i)
      Result += " *";

    return Result;
  }
};

static ClassifiedTypeResult classifyQualType(const QualType &type_,
                                             const size_t depth = 0) {
  const auto &UnqualifiedType = type_.getUnqualifiedType();

  if (UnqualifiedType->isAnyPointerType())
    return classifyQualType(UnqualifiedType->getPointeeType(), depth + 1);

  if (!UnqualifiedType->isBuiltinType())
    return ClassifiedTypeResult{ClassifiedType::UserDefined, depth,
                                UnqualifiedType};

  if (UnqualifiedType->isBooleanType())
    return ClassifiedTypeResult{ClassifiedType::BuiltinOther, depth,
                                UnqualifiedType};

  const auto *const BuiltinTypeVal = dyn_cast<BuiltinType>(UnqualifiedType);
  assert(builtinType);

  // TODO
  (void)BuiltinTypeVal;

  return ClassifiedTypeResult{ClassifiedType::BuiltinInteger, depth,
                              UnqualifiedType};
}

void UseFixedWidthTypesVarCheck::check(const MatchFinder::MatchResult &Result) {
  // FIXME: Add callback implementation.
  const auto *MatchedDecl = Result.Nodes.getNodeAs<VarDecl>("var");

  const auto &TypeOfDecl = MatchedDecl->getType();

  const auto ClassifiedType = classifyQualType(TypeOfDecl);

  if (ClassifiedType.type == ClassifiedType::BuiltinInteger)
    return;

  diag(MatchedDecl->getLocation(),
       "variable %0 has a non fixed integer type (or is not an alias to that "
       "type):\n%1",
       DiagnosticIDs::Error)
      << MatchedDecl << ClassifiedType.format();
}

} // namespace clang::tidy::totto
