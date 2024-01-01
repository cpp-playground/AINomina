
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/CommandLine.h>

#include "AINomina/evaluate_names_action.hpp"

int main(int a, const char** argv)
{
    llvm::cl::OptionCategory ainomina_options("AINomina options");
    llvm::cl::extrahelp common_help(clang::tooling::CommonOptionsParser::HelpMessage);

    llvm::cl::opt<std::string> api_key("k",
                                       llvm::cl::desc("Gemini API key to use for the requests"),
                                       llvm::cl::value_desc("API-KEY"),
                                       llvm::cl::Required);
    api_key.addCategory(ainomina_options);

    llvm::cl::opt<int> rate_limiting(
        "r", llvm::cl::desc("Maximum Gemin request per minutes"), llvm::cl::value_desc("RPM"));
    rate_limiting.addCategory(ainomina_options);
    rate_limiting.setInitialValue(60);
    llvm::cl::extrahelp ainomina_help("More help\n");

    auto expected_parser = clang::tooling::CommonOptionsParser::create(a, argv, ainomina_options);
    if (!expected_parser)
    {
        // Fail gracefully for unsupported options.
        llvm::errs() << expected_parser.takeError();
        return 1;
    }

    auto& option_parser = expected_parser.get();
    clang::tooling::ClangTool ainomina(option_parser.getCompilations(),
                                       option_parser.getSourcePathList());

    auto action_factory = ain::new_evaluate_names_action_factory<ain::EvaluateNamesAction>(
        api_key.getValue(), rate_limiting.getValue());

    return ainomina.run(action_factory.get());
}
