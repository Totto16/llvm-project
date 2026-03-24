set(LLVM_LINK_COMPONENTS
  FrontendOpenMP
  Support
  )

include_directories(BEFORE "${CMAKE_CURRENT_SOURCE_DIR}/../../include-cleaner/include")


add_clang_library(clangTidyTottoModule STATIC
  TottoTidyModule.cpp
  UseFixedWidthTypesVarCheck.cpp
  ConstCorrectnessCheckC.cpp

  LINK_LIBS
  clangTidy
  clangTidyUtils

  DEPENDS
  omp_gen
  ClangDriverOptions
  )

clang_target_link_libraries(clangTidyTottoModule
  PRIVATE
  clangAnalysis
  clangAST
  clangASTMatchers
  clangBasic
  clangFormat
  clangLex
  clangSerialization
  clangTooling
  clangToolingInclusions
  clangToolingInclusionsStdlib
  )
target_link_libraries(clangTidyTottoModule
  PRIVATE
  clangIncludeCleaner
  )
