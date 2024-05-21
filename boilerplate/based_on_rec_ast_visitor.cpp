#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

using namespace clang;
using namespace clang::tooling;
using namespace llvm;

// Define command line options
static llvm::cl::OptionCategory ToolCategory("Tool options");
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// Define a class to visit AST nodes
class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
  explicit MyASTVisitor(ASTContext *Context) : Context(Context) {}

  bool VisitFunctionDecl(FunctionDecl *FD) {
    // Add your logic here to process each function declaration
    FD->dumpColor();
    return true;
  }

private:
  ASTContext *Context;
};

// Define an ASTConsumer to process the AST
class MyASTConsumer : public ASTConsumer {
public:
  explicit MyASTConsumer(ASTContext *Context) : Visitor(Context) {}

  void HandleTranslationUnit(ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  MyASTVisitor Visitor;
};

// Define a FrontendAction to create the ASTConsumer
class MyFrontendAction : public ASTFrontendAction {
public:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &Compiler,
                                                 StringRef File) override {
    return std::make_unique<MyASTConsumer>(&Compiler.getASTContext());
  }
};

int main(int argc, const char **argv) {
  auto option = CommonOptionsParser::create(argc, argv, ToolCategory);
  ClangTool Tool(option->getCompilations(), option->getSourcePathList());

  return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
