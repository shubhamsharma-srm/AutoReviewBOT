#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/ASTContext.h>
#include <llvm/Support/CommandLine.h>
#include <clang/Frontend/CompilerInstance.h>
#include <iostream>
#include "../include/RuleLongFunction.hpp"
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>

using namespace clang::ast_matchers;
using namespace clang;
using namespace clang::tooling;
using namespace llvm;

static cl::OptionCategory ToolCategory("AutoCheckCpp tool options");

// AST Visitor to walk the syntax tree
class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
    explicit MyASTVisitor(ASTContext *Context) : Context(Context) {}

    bool VisitFunctionDecl(FunctionDecl *func) {
        if (func->getNameAsString() == "main") {
            FullSourceLoc fullLoc(func->getBeginLoc(), Context->getSourceManager());
            if (fullLoc.isValid()) {
                llvm::outs() << "⚠️  Warning: Function named 'main' found at "
                             << fullLoc.getSpellingLineNumber() << ":"
                             << fullLoc.getSpellingColumnNumber() << "\n";
            }
        }
        return true;
    }

private:
    ASTContext *Context;
};

// AST Consumer
class MyASTConsumer : public ASTConsumer {
public:
    explicit MyASTConsumer(ASTContext *Context) : Visitor(Context) {}

    void HandleTranslationUnit(ASTContext &Context) override {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
    MyASTVisitor Visitor;
};

// FrontendAction
class MyFrontendAction : public ASTFrontendAction {
public:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                   StringRef file) override {
        return std::make_unique<MyASTConsumer>(&CI.getASTContext());
    }
};

// Main entry point
int main(int argc, const char **argv) {
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, ToolCategory);
    if (!ExpectedParser) {
        llvm::errs() << "Failed to parse options: "
                     << llvm::toString(ExpectedParser.takeError()) << "\n";
        return 1;
    }
    CommonOptionsParser &OptionsParser = ExpectedParser.get();

    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

    LongFunctionCallback longFunctionCallback;
    MatchFinder Finder;
    Finder.addMatcher(functionDecl(isDefinition()).bind("longFunc"), &longFunctionCallback);

    return Tool.run(newFrontendActionFactory(&Finder).get());
}
