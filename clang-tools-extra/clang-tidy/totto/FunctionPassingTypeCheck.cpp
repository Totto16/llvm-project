//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "FunctionPassingTypeCheck.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang::tidy::totto {

const char DefaultByValue[] = "";
const char DefaultByPtr[] = "";

FunctionPassingTypeCheck::FunctionPassingTypeCheck(StringRef Name,
                                                   ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context),
      ByValue(Options.get("ByValue", DefaultByValue)),
      ByPtr(Options.get("ByPtr", DefaultByPtr)) {}

void FunctionPassingTypeCheck::storeOptions(ClangTidyOptions::OptionMap &Opts) {
  Options.store(Opts, "ByValue", ByValue.ValueInput);
  Options.store(Opts, "ByPtr", ByPtr.ValueInput);
}

void FunctionPassingTypeCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(functionDecl().bind("func"), this);
}

enum class ClassifiedParamaterType : std::uint8_t {
  PassedByValue,
  PassedByPtr,
};

struct ClassifiedParamater {
  ClassifiedParamaterType type;
  std::string TypeName;
};

static std::string
get_type_name_without_qualifiers_and_ptr_level(const QualType &Type) {
  const auto &UnqualifiedType = Type.getUnqualifiedType();

  if (UnqualifiedType->isAnyPointerType())
    return get_type_name_without_qualifiers_and_ptr_level(
        UnqualifiedType->getPointeeType());

  if ((isa<BuiltinType>(UnqualifiedType))) {
    const auto *const BuiltinTypeVal = dyn_cast<BuiltinType>(UnqualifiedType);
    assert(BuiltinTypeVal);

    // TODO: better print, use the correct languageoptions!
    return BuiltinTypeVal->getName(PrintingPolicy{LangOptions{}}).str();
  }

  if ((isa<RecordType>(UnqualifiedType))) {
    const auto *const RecordTypeVal = dyn_cast<RecordType>(UnqualifiedType);
    assert(RecordTypeVal);

    const auto RecordTypeDecl = RecordTypeVal->getDecl();

    std::string StructName;

    if (const auto *MaybeName = RecordTypeDecl->getTypedefNameForAnonDecl();
        MaybeName)
      StructName = MaybeName->getNameAsString();
    else if (RecordTypeDecl->isAnonymousStructOrUnion())
      StructName =
          "@anonymous_struct"; // special value, so that you can match this
    else
      StructName = RecordTypeDecl->getNameAsString();

    if (StructName.empty())
      StructName = "@anonymous_struct";

    return StructName;
  }

  if ((isa<EnumType>(UnqualifiedType))) {
    const auto *const EnumTypeVal = dyn_cast<EnumType>(UnqualifiedType);
    assert(EnumTypeVal);

    const auto EnumTypeDecl = EnumTypeVal->getDecl();

    const IdentifierInfo *EnumDeclVal = EnumTypeDecl->getIdentifier();

    std::string EnumName;

    if (EnumDeclVal)
      EnumName = EnumDeclVal->getName().str();
    else if (const auto *MaybeName = EnumTypeDecl->getTypedefNameForAnonDecl();
             MaybeName)
      EnumName = MaybeName->getNameAsString();
    else
      EnumName = "@anonymous_enum"; // special value, so that you can match this

    if (EnumName.empty())
      EnumName = "@anonymous_enum";

    return EnumName;
  }

  return UnqualifiedType.getAsString();
}

static ClassifiedParamater classifyParamater(const VarDecl &decl) {
  const auto &TypeOfDecl = decl.getType();

  const auto &UnqualifiedType = TypeOfDecl.getUnqualifiedType();

  const ClassifiedParamaterType type =
      (UnqualifiedType->isAnyPointerType())
          ? ClassifiedParamaterType::PassedByPtr
          : ClassifiedParamaterType::PassedByValue;

  const std::string TypeName =
      get_type_name_without_qualifiers_and_ptr_level(TypeOfDecl);

  return ClassifiedParamater{type, TypeName};
}

void FunctionPassingTypeCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *FuncDecl = Result.Nodes.getNodeAs<FunctionDecl>("func");

  for (const auto *Param : FuncDecl->parameters()) {
    const auto classification = classifyParamater(*Param);

    if (classification.type == ClassifiedParamaterType::PassedByPtr) {
      if (!classification.TypeName.empty()) {
        if (ByValue.Value.match(classification.TypeName)) {
          diag(Param->getLocation(),
               "paramater %0 with type '%1' matches the types, that need to be "
               "passed by value, so don't pass it by ptr",
               DiagnosticIDs::Error)
              << Param << classification.TypeName;
        }
      }

    } else if (classification.type == ClassifiedParamaterType::PassedByValue) {
      if (!classification.TypeName.empty()) {
        if (ByPtr.Value.match(classification.TypeName)) {
          diag(Param->getLocation(),
               "paramater %0 with type '%1' matches the types, that need to be "
               "passed by ptr, so don't pass it by value",
               DiagnosticIDs::Error)
              << Param << classification.TypeName;
        }
      }
    } else {
      llvm_unreachable("implementation error");
    }
  }
}

} // namespace clang::tidy::totto
