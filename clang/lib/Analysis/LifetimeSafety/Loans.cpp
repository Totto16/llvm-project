//===- Loans.cpp - Loan Implementation --------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "clang/Analysis/Analyses/LifetimeSafety/Loans.h"

namespace clang::lifetimes::internal {

void AccessPath::dump(llvm::raw_ostream &OS) const {
  if (const clang::ValueDecl *VD = getAsValueDecl())
    OS << VD->getNameAsString();
  else if (const clang::MaterializeTemporaryExpr *MTE =
               getAsMaterializeTemporaryExpr())
    OS << "MaterializeTemporaryExpr at " << MTE;
  else if (const PlaceholderRoot *PB = getAsPlaceholderRoot()) {
    if (const auto *PVD = PB->getParmVarDecl())
      OS << "$" << PVD->getNameAsString();
    else if (PB->getMethodDecl())
      OS << "$this";
  } else
    llvm_unreachable("access path base invalid");
}

void Loan::dump(llvm::raw_ostream &OS) const {
  OS << getID() << " (Path: ";
  Path.dump(OS);
  OS << ")";
}

const PlaceholderRoot *
LoanManager::getOrCreatePlaceholderRoot(const ParmVarDecl *PVD) {
  if (auto It = PlaceholderRoots.find(PVD); It != PlaceholderRoots.end())
    return It->second;
  void *Mem = LoanAllocator.Allocate<PlaceholderRoot>();
  PlaceholderRoot *NewPR = new (Mem) PlaceholderRoot(PVD);
  PlaceholderRoots.insert({PVD, NewPR});
  return NewPR;
}

const PlaceholderRoot *
LoanManager::getOrCreatePlaceholderRoot(const CXXMethodDecl *MD) {
  if (auto It = PlaceholderRoots.find(MD); It != PlaceholderRoots.end())
    return It->second;
  void *Mem = LoanAllocator.Allocate<PlaceholderRoot>();
  PlaceholderRoot *NewPR = new (Mem) PlaceholderRoot(MD);
  PlaceholderRoots.insert({MD, NewPR});
  return NewPR;
}
} // namespace clang::lifetimes::internal
