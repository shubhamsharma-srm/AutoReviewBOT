#include "../include/RuleLongFunction.hpp"
#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Stmt.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Lex/Lexer.h>
#include<sstream>
#include<string>
#include<regex> 
#include <llvm/Support/raw_ostream.h>
#include<iostream>

using namespace clang;
using namespace clang::ast_matchers;


void LongFunctionCallback::run(const MatchFinder::MatchResult &Result) {
    const FunctionDecl *func = Result.Nodes.getNodeAs<FunctionDecl>("longFunc");
    if (!func || !func->hasBody()) return;

    const SourceManager &SM = *Result.SourceManager;
    SourceLocation startLoc = func->getSourceRange().getBegin();
    SourceLocation endLoc = func->getSourceRange().getEnd();

    // Get raw source text of the function
    SourceLocation expandedEnd = Lexer::getLocForEndOfToken(endLoc, 0, SM, Result.Context->getLangOpts());
    CharSourceRange charRange = CharSourceRange::getCharRange(startLoc, expandedEnd);
    StringRef funcCode = Lexer::getSourceText(charRange, SM, Result.Context->getLangOpts());

    // Count only non-blank, non-comment lines
    std::istringstream stream(funcCode.str());
    std::string line;
    int meaningfulLineCount = 0;
    std::regex codeRegex(R"([^{};\s])");  // Matches if line has actual code

    while (std::getline(stream, line)) {
        std::string trimmed = llvm::StringRef(line).trim().str();

        if (trimmed.empty()) continue;
        if (trimmed.find("//") == 0) continue;
        if (trimmed.find("/*") == 0 || trimmed.find("*") == 0) continue;
        if (!std::regex_search(trimmed, codeRegex)) continue;

        ++meaningfulLineCount;
    }

    // std::cout << "Matched function: " << func->getNameAsString()
    //           << " (Executable lines: " << meaningfulLineCount << ")\n";

    // Skip functions in system headers
    if (SM.isInSystemHeader(startLoc)) return;
    if (meaningfulLineCount > 0) {
        std::cout << "Violation: Function '" << func->getNameAsString()
                  << "' has " << meaningfulLineCount << " executable lines "
                  << "(File: " << SM.getFilename(startLoc).str() << ").\n";
    }
}

