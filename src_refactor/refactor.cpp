#include <iostream>
#include <string>
#include <vector>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Decl.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

namespace {

llvm::cl::OptionCategory refactorOptCategory("refactor options");

class Refactorer : public clang::RecursiveASTVisitor<Refactorer> {
public:
  explicit Refactorer(clang::Rewriter &writer) : writer(writer) {}

  bool VisitVarDecl(clang::VarDecl *decl) {
    // canonical type remove all the elaborated types
    clang::QualType qualified_type = decl->getType().getCanonicalType();

    // get the underlying type is the decl type is a reference
    std::string refLex;
    if (llvm::isa<clang::ReferenceType>(qualified_type)) {
      qualified_type =
          llvm::cast<clang::ReferenceType>(qualified_type)->getPointeeType();
      refLex = "& ";
    }

    // move on if not struct of our interest
    if (qualified_type.getAsString() != "struct ST")
      return true;

    clang::SourceLocation type_start = decl->getTypeSpecStartLoc();
    // type_start.dump(writer.getSourceMgr());
    clang::SourceLocation type_end = decl->getTypeSpecEndLoc();
    // type_end.dump(writer.getSourceMgr());
    clang::SourceRange range_to_replace =
        clang::SourceRange(type_start, type_end);

    std::string replacement = "ST_T";
    if (!refLex.empty())
      replacement += refLex;

    writer.ReplaceText(range_to_replace, replacement);

    /*
    // if not a class/struct/union then move on
    if (!llvm::isa<clang::RecordType>(qualified_type))
      return true;

    const clang::RecordDecl *record_type =
        llvm::cast<clang::RecordType>(qualified_type)->getDecl();

    // move on if not struct of our interest
    if (!record_type->isStruct() && record_type->getNameAsString() != "ST")
      return true; 
    */

    return true;
  }

  bool VisitCXXRecordDecl(clang::CXXRecordDecl *R) {
    llvm::outs() << "Struct name: " << R->getQualifiedNameAsString() << "\n";
    return true;
  }

private:
  clang::Rewriter &writer;
};

class Consumer : public clang::ASTConsumer {
public:
  explicit Consumer(clang::Rewriter &writer) : refactorer(writer) {}
  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    refactorer.TraverseAST(Context);
  }

private:
  Refactorer refactorer;
};

class Action : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler,
                    llvm::StringRef InFile) override {
    writer.setSourceMgr(Compiler.getSourceManager(), Compiler.getLangOpts());
    return std::make_unique<Consumer>(writer);
  }

  void EndSourceFileAction() override {
    std::cout << "Refactored content\n";
    writer.getEditBuffer(writer.getSourceMgr().getMainFileID())
        .write(llvm::outs());

    llvm::outs() << "\n";

  }

private:
  clang::Rewriter writer;
};

class Refactoror {
public:
  Refactoror(clang::tooling::CompilationDatabase &compilation_db,
             const std::vector<std::string> source_files)
      : tool(clang::tooling::ClangTool(compilation_db, source_files)) {}
  void refactor_struct_decl(std::string, std::string);

private:
  clang::tooling::ClangTool tool;
};

void Refactoror::refactor_struct_decl(std::string filename = "",
                                      std::string function_name = "") {
  tool.run(clang::tooling::newFrontendActionFactory<Action>().get());
}

} // end of namespace

int main(int argc, const char **argv) {
  auto options = clang::tooling::CommonOptionsParser::create(
      argc, argv, refactorOptCategory);

  if (!options) {
    std::cout << "Failed to parse given options\n";
    return 1;
  }

  const std::vector<std::string> &source_files = options->getSourcePathList();
  clang::tooling::CompilationDatabase &compilation_db =
      options->getCompilations();

  for (auto &src : source_files)
    std::cout << src << "\n";

  Refactoror refactor_instance(compilation_db, source_files);
  refactor_instance.refactor_struct_decl();

  return 0;
}
