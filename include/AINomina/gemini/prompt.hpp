#pragma once

#include <filesystem>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>

namespace ain::gemini
{

class prompt
{
  public:
    enum class status
    {
        ok,
        error,
        variable_not_found,
        unreplaced_variable,
    };

    explicit prompt(const std::string& text) noexcept;

    status replace(const std::string& variable, const std::string& value);

    [[nodiscard]] std::tuple<std::string, status> get_text() const;

  private:
    std::string m_text;
    std::set<std::string> m_unreplaced_variables;
};

std::unordered_map<std::string, prompt> load_prompts(const std::filesystem::path& path);

}  // namespace ain::gemini
