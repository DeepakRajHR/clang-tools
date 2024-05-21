#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace llvm;

// Define command line options
static llvm::cl::OptionCategory ToolCategory("Tool options");
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// Define a callback handler that will be called when a match is found
class MyMatchHandler : public MatchFinder::MatchCallback {
public:
  virtual void run(const MatchFinder::MatchResult &Result) {
    if (const FunctionDecl *FD = Result.Nodes.getNodeAs<FunctionDecl>("funcDecl")) {
      FD->dump(); // Replace this with your own logic
    }
  }
};

// Define a FrontendAction to register the matchers and callback handler
class MyFrontendAction : public ASTFrontendAction {
public:
  void EndSourceFileAction() override {
    Finder.matchAST(*Context);
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &Compiler,
                                                 StringRef File) override {
    Context = &Compiler.getASTContext();
    Finder.addMatcher(functionDecl().bind("funcDecl"), &Handler);
    return std::make_unique<ASTConsumer>();
  }

private:
  ASTContext *Context;
  MatchFinder Finder;
  MyMatchHandler Handler;
};

int main(int argc, const char **argv) {
  auto options = CommonOptionsParser::create(argc, argv, ToolCategory);
  ClangTool Tool(options->getCompilations(), options->getSourcePathList());

  return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
