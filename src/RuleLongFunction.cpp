#include "../include/RuleLongFunction.hpp"
#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Stmt.h>
#include <clang/Basic/SourceManager.h>
#include <llvm/Support/raw_ostream.h>

using namespace clang;
using namespace clang::ast_matchers;

void LongFunctionCallback::run(const MatchFinder::MatchResult &Result) {
    if (const FunctionDecl *Func = Result.Nodes.getNodeAs<FunctionDecl>("longFunc")) {
        const Stmt *Body = Func->getBody();
        if (!Body) return;

        const SourceManager &SM = *Result.SourceManager;
        unsigned startLine = SM.getSpellingLineNumber(Body->getBeginLoc());
        unsigned endLine = SM.getSpellingLineNumber(Body->getEndLoc());
        unsigned lineCount = endLine - startLine + 1;

        clang::FullSourceLoc FullLocation(Func->getBeginLoc(), SM);

if (FullLocation.isValid() &&
    lineCount > 30) {

    std::string filename = SM.getFilename(Func->getBeginLoc()).str();

    // Only analyze files from test_files directory
    if (filename.find("/test_files/") != std::string::npos) {
        llvm::errs() << "Violation: Function '" << Func->getNameInfo().getName().getAsString()
                     << "' has " << lineCount << " lines (File: " << filename << ").\n";
    }
}
        {
            llvm::errs() << "Violation: Function '" << Func->getNameInfo().getName().getAsString()
                         << "' has " << lineCount << " lines.\n";
        }
    }
}



