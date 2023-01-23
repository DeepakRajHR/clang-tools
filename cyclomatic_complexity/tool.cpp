#include <iostream>
#include <string>
#include <vector>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

namespace {

llvm::cl::OptionCategory fComplexCategory("fcomplex options");
using StringList = std::vector<std::string>;

/// RecursiveAST visitor provides an interface to extract the information from
/// the AST. An AST node could be visited using a method VisitNodeName. For
/// example to visit all class or struct defination overload
/// VisitCXXRecordDecl.
class ComplexityComputer
: public clang::RecursiveASTVisitor<ComplexityComputer> {
 public:
  explicit ComplexityComputer(clang::ASTContext* context) : context(context){};
  bool VisitFunctionDecl(clang::FunctionDecl* fDecl) {
    const auto cfg = clang::CFG::buildCFG(fDecl,
                                          fDecl->getBody(),
                                          context,
                                          clang::CFG::BuildOptions());

    cfg->dump(context->getLangOpts(), true);

    const int v = cfg->size();
    int e = 0;
    for (const auto* block : *cfg) {
      e += block->succ_size();
    }

    /// In a structured programming language, like C++, the cyclomatic
    /// complexity 'm' is defined as
    ///   m = e - n + 2p
    ///   where
    ///     e are the number of edges
    ///     n are the number of node
    ///     p are the number of connected component, which is always 1 in a
    ///     function
    const unsigned complexity = e - v + (2 * 1);
    std::cout << fDecl->getNameAsString() << ": " << complexity << "\n";

    return true;
  }

 private:
  clang::ASTContext* context;
};

class Consumer : public clang::ASTConsumer {
 public:
  explicit Consumer(clang::ASTContext* Context, clang::SourceManager& SrcMgr)
  : computer(Context) {
  }
  virtual void HandleTranslationUnit(clang::ASTContext& Context) {
    computer.TraverseAST(Context);
  }

 private:
  ComplexityComputer computer;
};


class Action : public clang::ASTFrontendAction {
 public:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance& Compiler, llvm::StringRef InFile) {
    return std::make_unique<Consumer>(&Compiler.getASTContext(),
                                      Compiler.getSourceManager());
  }
};


class CCComputer {
 public:
  CCComputer(clang::tooling::CompilationDatabase& compilation_db,
             const StringList source_files)
  : tool(clang::tooling::ClangTool(compilation_db, source_files)) {
  }
  void compute(std::string, std::string);

 private:
  clang::tooling::ClangTool tool;
};

void CCComputer::compute(std::string filename = "",
                         std::string function_name = "") {
  tool.run(clang::tooling::newFrontendActionFactory<Action>().get());
}

}  // end of namespace

int main(int argc, const char** argv) {
  auto options =
      clang::tooling::CommonOptionsParser::create(argc, argv, fComplexCategory);

  if (!options) {
    std::cout << "Failed to parse given options\n";
    return 1;
  }

  const StringList& source_files = options->getSourcePathList();
  clang::tooling::CompilationDatabase& compilation_db =
      options->getCompilations();

  for (auto& src : source_files) std::cout << src << "\n";

  CCComputer computer(compilation_db, source_files);
  computer.compute();

  std::cout << std::endl;

  return 0;
}

// EOF
