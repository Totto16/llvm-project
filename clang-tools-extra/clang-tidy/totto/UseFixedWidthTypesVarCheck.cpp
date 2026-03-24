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

    for (size_t i = 0; i < this->depth; ++i) {
      if (i != 0)
        Result += " ";

      Result += "*";
    }

    return Result;
  }
};

static ClassifiedTypeResult classifyQualType(const QualType &Type,
                                             const size_t depth = 0) {
  const auto &UnqualifiedType = Type.getUnqualifiedType();

  if (UnqualifiedType->isAnyPointerType())
    return classifyQualType(UnqualifiedType->getPointeeType(), depth + 1);

  // NOTE: don't use isBuiltinType() as that uses the canonical type, we want
  // the literal type, canonical can be the wrong e.g. on size_t
  if (!(isa<BuiltinType>(UnqualifiedType))) {
    // NOTE: not checking if this is an alias, as we want alias to be
    return ClassifiedTypeResult{ClassifiedType::UserDefined, depth,
                                UnqualifiedType};
  }

  const auto *const BuiltinTypeVal = dyn_cast<BuiltinType>(UnqualifiedType);
  assert(BuiltinTypeVal);

  if (BuiltinTypeVal->getKind() == BuiltinType::Bool)
    return ClassifiedTypeResult{ClassifiedType::BuiltinOther, depth,
                                UnqualifiedType};

  if (BuiltinTypeVal->getKind() == BuiltinType::Void)
    return ClassifiedTypeResult{ClassifiedType::BuiltinOther, depth,
                                UnqualifiedType};

  // char* are an exceptions, as they are strings
  if (depth == 1 && (BuiltinTypeVal->getKind() == BuiltinType::Char_S ||
                     BuiltinTypeVal->getKind() == BuiltinType::Char_U)) {
    return ClassifiedTypeResult{ClassifiedType::BuiltinOther, depth,
                                UnqualifiedType};
  }

  return ClassifiedTypeResult{ClassifiedType::BuiltinInteger, depth,
                              UnqualifiedType};
}

void UseFixedWidthTypesVarCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedDecl = Result.Nodes.getNodeAs<VarDecl>("var");

  const auto &TypeOfDecl = MatchedDecl->getType();

  const auto ClassifiedType = classifyQualType(TypeOfDecl);

  if (ClassifiedType.type != ClassifiedType::BuiltinInteger)
    return;

  diag(MatchedDecl->getLocation(),
       "variable %0 has type '%1', which should be rewritten into using a "
       "fixed type",
       DiagnosticIDs::Warning)
      << MatchedDecl << ClassifiedType.format();
}

} // namespace clang::tidy::totto
