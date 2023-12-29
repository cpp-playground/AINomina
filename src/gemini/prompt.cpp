#include "AINomina/gemini/prompt.hpp"

#include <algorithm>
#include <iostream>
#include <regex>

namespace
{
std::vector<std::string> find_variables(const std::string& text)
{
    std::vector<std::string> variables;

    std::regex variable_pattern("{{([^{}]+)}}");
    for (std::smatch variables_matches;
         std::regex_search(text, variables_matches, variable_pattern);)
    {
        std::ranges::transform(variables_matches, std::back_inserter(variables), [](const auto& m) {
            return m.str();
        });
    }

    return {};
}
}  // namespace

namespace ain::gemini
{

prompt::prompt(const std::string& text) noexcept :
  m_text{ text }, m_unreplaced_variables{ find_variables(m_text) }
{}

}  // namespace ain::gemini
