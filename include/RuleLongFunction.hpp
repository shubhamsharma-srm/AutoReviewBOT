#ifndef RULE_LONG_FUNCTION_H
#define RULE_LONG_FUNCTION_H

#include <clang/ASTMatchers/ASTMatchFinder.h>

class LongFunctionCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
    virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &Result) override;
};

#endif // RULE_LONG_FUNCTION_H
