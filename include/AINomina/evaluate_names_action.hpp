#pragma once

#include <fstream>

#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>

#include "AINomina/gemini/gemini_client.hpp"

namespace ain
{

struct NameQuality
{
    std::string name_;
    std::string quality_;
    std::string alternative_;
    std::string explanation_;
};

constexpr auto promt_text = R"(Context:
You a code reviewer and are tasked with evaluating variables name quality in a C++ snippet.
Ignore functions and types names.

Quality can only be one of the following:
- GOOD
- BAD

Consider the following C++ code:
```cpp
<code>
```

Output:
The output must be formatted as a JSON list. For example the following are valid outputs:
[{"name" : "request_payload","quality": "GOOD", "explanation" : "", "alternative" : ""}]
[{"name" : "wc", "quality": "BAD", "explanation" : "The variable name is too short", "alternative" : "word_count"},{"name" : "s","quality": "BAD", "explanation" : "Very short names are reserved to indexes", "alternative" : "size"}]

Task:
Find the names of all variables in the snippet.
For each found name, answer the following questions:
Is that name good enough? Can it be improved? Provide a short explanation and if possible an alternative. If the quality is BAD, always propose an alternative name.
)";

class EvaluateVariableNameVisitor : public clang::RecursiveASTVisitor<EvaluateVariableNameVisitor>
{
  public:
    explicit EvaluateVariableNameVisitor(clang::ASTContext* context,
                                         const std::vector<NameQuality>& evaluated_names,
                                         const std::string& file) :
      context_(context), evaluated_names_(evaluated_names), file_(file)
    {}

    bool VisitVarDecl(clang::VarDecl* declaration)
    {
        if (declaration->isFirstDecl() && declaration->getNameAsString() != "")
        {
            clang::FullSourceLoc full_location = context_->getFullLoc(declaration->getBeginLoc());

            if (full_location.isValid() && full_location.getFileEntry())
            {
                if (full_location.getFileEntry()->tryGetRealPathName().str() == file_)
                {
                    auto it = std::ranges::find(
                        evaluated_names_, declaration->getNameAsString(), &NameQuality::name_);
                    if (it != evaluated_names_.end())
                    {
                        if (it->quality_ == "BAD")
                        {
                            llvm::outs().changeColor(llvm::raw_ostream::Colors::YELLOW)
                                << it->name_;
                            llvm::outs().resetColor() << " should probably renamed ";
                            llvm::outs().changeColor(llvm::raw_ostream::Colors::GREEN)
                                << it->alternative_;
                            llvm::outs().resetColor() << " (" << it->explanation_ << ")\n";
                        }
                    }
                }
            }
        }
        return true;
    }

  private:
    clang::ASTContext* context_;
    std::vector<NameQuality> evaluated_names_;
    const std::string file_;
};

class EvaluateVariableNameConsumer : public clang::ASTConsumer
{
  public:
    explicit EvaluateVariableNameConsumer(clang::ASTContext* context,
                                          const std::vector<NameQuality>& names,
                                          const std::string& file) :
      visitor_(context, names, file)
    {}

    virtual void HandleTranslationUnit(clang::ASTContext& context)
    {
        visitor_.TraverseDecl(context.getTranslationUnitDecl());
    }

  private:
    EvaluateVariableNameVisitor visitor_;
};

class EvaluateNamesAction : public clang::ASTFrontendAction
{
  public:
    EvaluateNamesAction(const std::string& key, int rate_limit) : client_(key, rate_limit)
    {}

    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& compiler,
                                                                  llvm::StringRef file) override
    {
        llvm::outs() << "\nProcessing " << file << "\n";

        std::ifstream ifs(file.str());
        std::string content((std::istreambuf_iterator<char>(ifs)),
                            (std::istreambuf_iterator<char>()));

        llvm::outs() << "Querrying Gemini...\n";
        auto prompt = ain::gemini::Prompt(promt_text);
        prompt.replace("code", content);

        std::vector<NameQuality> names;
        if (auto response = client_.querry(ain::gemini::QuerryParameters{}, prompt))
        {
            for (auto& result : *response)
            {
                NameQuality name_quality;
                name_quality.name_ = result["name"].get<std::string>();
                name_quality.quality_ = result["quality"].get<std::string>();
                name_quality.alternative_ = result["alternative"].get<std::string>();
                name_quality.explanation_ = result["explanation"].get<std::string>();
                names.push_back(name_quality);
            }
        }

        llvm::outs() << "Gemini results retrieved.\n";
        return std::make_unique<EvaluateVariableNameConsumer>(
            &compiler.getASTContext(), names, file.str());
    }

  private:
    ain::gemini::GeminiClient client_;
};

template <typename T, typename... Ts>
std::unique_ptr<clang::tooling::FrontendActionFactory> new_evaluate_names_action_factory(Ts... args)
{
    class SimpleFrontendActionFactory : public clang::tooling::FrontendActionFactory
    {
      public:
        SimpleFrontendActionFactory(Ts... args) : args_(args...)
        {}
        std::tuple<Ts...> args_;

        std::unique_ptr<clang::FrontendAction> create()
        {
            return std::make_unique<T>(std::get<Ts>(args_)...);
        }
    };

    return std::make_unique<SimpleFrontendActionFactory>(args...);
}

}  // namespace ain
