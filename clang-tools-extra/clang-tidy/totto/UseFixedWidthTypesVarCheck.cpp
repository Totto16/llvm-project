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
  Finder->addMatcher(functionDecl().bind("func"), this);
  Finder->addMatcher(fieldDecl().bind("field"), this);
  Finder->addMatcher(enumDecl().bind("enum"), this);
  Finder->addMatcher(castExpr().bind("cast"), this);
}

enum class ClassifiedType : std::uint8_t {
  BuiltinOther,
  BuiltinInteger,
  UserDefined,
  Error,
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

  // NOTE: don't use isBuiltinType() as that uses the canonical type, we want
  // the literal type, canonical can be the wrong e.g. on size_t
  if ((isa<BuiltinType>(UnqualifiedType))) {
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

    if (BuiltinTypeVal->isFloatingPoint())
      return ClassifiedTypeResult{ClassifiedType::BuiltinOther, depth,
                                  UnqualifiedType};

    return ClassifiedTypeResult{ClassifiedType::BuiltinInteger, depth,
                                UnqualifiedType};
  }

  if (UnqualifiedType->isIncompleteType())
    return ClassifiedTypeResult{ClassifiedType::Error, depth, UnqualifiedType};

  if (UnqualifiedType->isStructureOrClassType()) {
    return ClassifiedTypeResult{ClassifiedType::UserDefined, depth,
                                UnqualifiedType};
  }

  if (isa<EnumType>(UnqualifiedType)) {
    return ClassifiedTypeResult{ClassifiedType::UserDefined, depth,
                                UnqualifiedType};
  }

  if (UnqualifiedType->isAnyPointerType())
    return classifyQualType(UnqualifiedType->getPointeeType(), depth + 1);

  if (isa<ArrayType>(UnqualifiedType)) {
    const auto *const ArrayTypeVal = dyn_cast<ArrayType>(UnqualifiedType);
    assert(ArrayTypeVal);

    return classifyQualType(ArrayTypeVal->getElementType(), depth + 1);
  }

  if (UnqualifiedType->isTypedefNameType()) {
    // NOTE: not checking if this is an alias, as we want an alias to be not
    // resolved
    return ClassifiedTypeResult{ClassifiedType::UserDefined, depth,
                                UnqualifiedType};
  }

  if (UnqualifiedType->isCompoundType())
    return ClassifiedTypeResult{ClassifiedType::UserDefined, depth,
                                UnqualifiedType};

  return ClassifiedTypeResult{ClassifiedType::Error, depth, UnqualifiedType};
}

void UseFixedWidthTypesVarCheck::processVarDecl(const VarDecl &decl) {
  const auto &TypeOfDecl = decl.getType();

  const auto ClassifiedType = classifyQualType(TypeOfDecl);

  if (ClassifiedType.type == ClassifiedType::Error) {
    diag(decl.getLocation(),
         "var decl '%0' - type '%1' is not recognized by our type processor",
         DiagnosticIDs::Fatal)
        << decl.getName() << TypeOfDecl;
  }

  if (ClassifiedType.type != ClassifiedType::BuiltinInteger)
    return;

  diag(decl.getLocation(),
       "variable '%0' has type '%1', which should be rewritten into using a "
       "fixed type",
       DiagnosticIDs::Error)
      << decl.getName() << ClassifiedType.format();
}

void UseFixedWidthTypesVarCheck::processCastExpr(const CastExpr &decl) {
  if (decl.getCastKind() == CastKind::CK_LValueToRValue)
    return;

  const auto &TypeOfDecl = decl.getType();

  const auto ClassifiedType = classifyQualType(TypeOfDecl);

  if (ClassifiedType.type == ClassifiedType::Error) {
    diag(decl.getExprLoc(),
         "cast expr '%0' - type '%1' is not recognized by our type processor",
         DiagnosticIDs::Fatal)
        << decl.getCastKindName() << TypeOfDecl;
  }

  if (ClassifiedType.type != ClassifiedType::BuiltinInteger)
    return;

  diag(decl.getExprLoc(),
       "cast to type '%0', should be rewritten into using a "
       "fixed type",
       DiagnosticIDs::Error)
      << ClassifiedType.format();
}

void UseFixedWidthTypesVarCheck::processFunctionDecl(const FunctionDecl &decl) {
  const auto &ReturnTypeOfDecl = decl.getReturnType();

  const auto ClassifiedType = classifyQualType(ReturnTypeOfDecl);

  if (ClassifiedType.type == ClassifiedType::Error) {
    diag(decl.getLocation(),
         "function decl '%0' - type '%1' is not recognized by our type "
         "processor",
         DiagnosticIDs::Error)
        << decl.getName() << ReturnTypeOfDecl.getAsString();
  }

  if (ClassifiedType.type != ClassifiedType::BuiltinInteger)
    return;

  diag(decl.getReturnTypeSourceRange().getBegin(),
       "return type for function '%0' has type '%1', which should be rewritten "
       "into using a "
       "fixed type",
       DiagnosticIDs::Error)
      << decl.getName() << ClassifiedType.format();
}

void UseFixedWidthTypesVarCheck::processFieldDecl(const FieldDecl &decl) {
  const auto &TypeOfDecl = decl.getType();

  const auto ClassifiedType = classifyQualType(TypeOfDecl);

  if (ClassifiedType.type == ClassifiedType::Error) {
    diag(decl.getLocation(),
         "field decl '%0' - type '%1' is not recognized by our type processor",
         DiagnosticIDs::Fatal)
        << decl.getName() << TypeOfDecl;
  }

  if (ClassifiedType.type != ClassifiedType::BuiltinInteger)
    return;

  const RecordDecl *StructDecl = decl.getParent();

  std::string StructName;

  if (const auto *MaybeName = StructDecl->getTypedefNameForAnonDecl();
      MaybeName)
    StructName = MaybeName->getNameAsString();
  else if (StructDecl->isAnonymousStructOrUnion())
    StructName = "(anonymous struct)";
  else
    StructName = StructDecl->getNameAsString();

  if (StructName.empty())
    StructName = "(anonymous struct)";

  diag(
      decl.getLocation(),
      "member '%0' of the struct '%1' has type '%2', which should be rewritten "
      "into using a "
      "fixed type",
      DiagnosticIDs::Error)
      << decl.getName() << StructName << ClassifiedType.format();
}

void UseFixedWidthTypesVarCheck::processEnumDecl(const EnumDecl &decl) {
  // no underlying type
  if (!decl.getIntegerTypeSourceInfo())
    return;

  const auto &FixedTypeOfDecl = decl.getIntegerType();

  const auto ClassifiedType = classifyQualType(FixedTypeOfDecl);

  if (ClassifiedType.type == ClassifiedType::Error) {
    diag(decl.getLocation(),
         "enum decl '%0' - type '%1' is not recognized by our type processor",
         DiagnosticIDs::Fatal)
        << decl.getName() << FixedTypeOfDecl.getAsString();
  }

  if (ClassifiedType.type != ClassifiedType::BuiltinInteger)
    return;

  const IdentifierInfo *EnumDeclVal = decl.getIdentifier();

  std::string EnumName;

  if (EnumDeclVal)
    EnumName = EnumDeclVal->getName().str();
  else if (const auto *MaybeName = decl.getTypedefNameForAnonDecl(); MaybeName)
    EnumName = MaybeName->getNameAsString();
  else
    EnumName = "(anonymous enum)";

  if (EnumName.empty())
    EnumName = "(anonymous enum)";

  diag(decl.getLocation(),
       "enum '%0' has the underlying type '%1', which should be rewritten into "
       "using a "
       "fixed type",
       DiagnosticIDs::Error)
      << EnumName << ClassifiedType.format();
}

void UseFixedWidthTypesVarCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *VarDeclVal = Result.Nodes.getNodeAs<VarDecl>("var");

  if (VarDeclVal) {
    processVarDecl(*VarDeclVal);
    return;
  }

  const auto *FuncDeclVal = Result.Nodes.getNodeAs<FunctionDecl>("func");

  if (FuncDeclVal) {
    processFunctionDecl(*FuncDeclVal);
    return;
  }

  const auto *FieldDeclVal = Result.Nodes.getNodeAs<FieldDecl>("field");

  if (FieldDeclVal) {
    processFieldDecl(*FieldDeclVal);
    return;
  }

  const auto *EnumDeclVal = Result.Nodes.getNodeAs<EnumDecl>("enum");

  if (EnumDeclVal) {
    processEnumDecl(*EnumDeclVal);
    return;
  }

  const auto *CastExprVal = Result.Nodes.getNodeAs<CastExpr>("cast");

  if (CastExprVal) {
    processCastExpr(*CastExprVal);
    return;
  }

  llvm_unreachable("implementation error");
}

} // namespace clang::tidy::totto
