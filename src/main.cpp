#include <clang/Tooling/Tooling.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Tooling/CompilationDatabase.h>

#include <iostream>
#include "../include/RuleLongFunction.hpp"

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

int main(int argc, const char **argv) {
    if (argc < 2) {
        llvm::errs() << "Usage: " << argv[0] << " <source-file> [<more-files>...]\n";
        return 1;
    }

    std::vector<std::string> sourcePaths;
    for (int i = 1; i < argc; ++i) {
        sourcePaths.emplace_back(argv[i]);
    }

    // Basic flags — you can expand this with "-Iinclude" etc. if needed
    std::vector<std::string> compilationFlags = {
        "-std=c++17", "-Iinclude", "-I/usr/include", "-I/usr/local/include"
    };

    FixedCompilationDatabase compilations(".", compilationFlags);
    ClangTool Tool(compilations, sourcePaths);

    LongFunctionCallback longFunctionCallback;
    MatchFinder Finder;
    Finder.addMatcher(functionDecl(isDefinition()).bind("longFunc"), &longFunctionCallback);

    return Tool.run(newFrontendActionFactory(&Finder).get());
}
