// #include "../include/RuleLongFunction.hpp"
// #include <clang/Tooling/CommonOptionsParser.h>
// #include <clang/Tooling/Tooling.h>
// #include <clang/ASTMatchers/ASTMatchFinder.h>
// #include <llvm/Support/CommandLine.h>

// using namespace clang::tooling;
// using namespace clang::ast_matchers;
// using namespace llvm;

// // Define a category for command-line options
// static cl::OptionCategory AutoCheckCategory("AutoCheckCpp Options");

// int main(int argc, const char **argv) {
//     auto ExpectedParser = CommonOptionsParser::create(argc, argv, AutoCheckCategory);
//     if (!ExpectedParser) {
//         llvm::errs() << "Failed to parse options: "
//                      << llvm::toString(ExpectedParser.takeError()) << "\n";
//         return 1;
//     }
//     CommonOptionsParser &OptionsParser = ExpectedParser.get();

//     // Get the list of source files
//     ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

//     // Set up the matcher and callback
//     LongFunctionCallback longFunctionCallback;
//     MatchFinder finder;
//     finder.addMatcher(functionDecl(isDefinition()).bind("longFunc"), &longFunctionCallback);

//     // Run the tool
//     return Tool.run(newFrontendActionFactory(&finder).get());
// }


#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CompilationDatabase.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include "../include/RuleLongFunction.hpp"

#include <filesystem>
#include <vector>
#include <string>
#include <iostream>

namespace fs = std::filesystem;
using namespace clang::tooling;
using namespace clang::ast_matchers;

std::vector<std::string> getSourceFiles(const std::string& dir) {
    std::vector<std::string> sources;

    // Canonical paths to exclude (tool's own source)
    std::vector<std::string> excludeDirs = {
        fs::canonical("../build").string(),
        fs::canonical("../src").string(),
        fs::canonical("../include").string(),
    };

    for (const auto& entry : fs::recursive_directory_iterator(dir)) {
        if (!entry.is_regular_file())
            continue;

        std::string path = fs::canonical(entry.path()).string();

        // Skip build/include/src directories
        bool skip = false;
        for (const auto& exclude : excludeDirs) {
            if (path.find(exclude) == 0) {
                skip = true;
                break;
            }
        }
        if (skip) continue;

        if (entry.path().extension() == ".cpp" || entry.path().extension() == ".hpp") {
            sources.push_back(path);
        }
    }

    return sources;
}

int main(int argc, const char **argv) {
    std::string targetDir = "..";  // Default source root
    if (argc > 1) {
        targetDir = argv[1];
    }

    std::vector<std::string> sources = getSourceFiles(targetDir);
    if (sources.empty()) {
        std::cerr << "No .cpp/.hpp files found.\n";
        return 1;
    }

    // Use a fixed dummy compilation command (adjust as needed)
    std::vector<std::string> compileArgs = {"-std=c++17"};
    FixedCompilationDatabase Compilations(".", compileArgs);

    ClangTool Tool(Compilations, sources);

    LongFunctionCallback callback;
    MatchFinder finder;
    finder.addMatcher(functionDecl(isDefinition()).bind("longFunc"), &callback);

    return Tool.run(newFrontendActionFactory(&finder).get());
}
