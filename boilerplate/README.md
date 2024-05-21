- based_on_rec_ast_visitor.cpp: Boilerplate code for a simple clang libtooling based tool based on RecursiveASTVisitor

- based_on_rec_ast_visitor.cpp: Boilerplate code for a simple clang libtooling based tool based on ASTMatcher

# Requirement

Building these examples require clang libtooling tool set. To get the tool set install llvm using package manager like brew or consider build llvm from source with project clang enabled.

# Build

Update `LLVM_INSTALL_DIR` in build.sh to build to llvm installation location. This folder must contain following elements

* bin/llvm-config
* lib/libclang-cpp.so

```
build.sh <src file name>
```

This builds `build/tool`

# Run

Make sure the tool is built. See section Build above. Update the `LLVM_INSTALL_DIR` in  `test/run.sh`.

```
./run.sh
```
