#include "AINomina/gemini/prompt.hpp"

#include <fstream>
#include <regex>

namespace
{
std::set<std::string> find_variables(std::string text)
{
    std::set<std::string> variables;

    std::regex variable_pattern("<([^<>]+)>");
    for (std::smatch match; std::regex_search(text, match, variable_pattern);)
    {
        variables.insert(match[1]);
        text = match.suffix();
    }

    return variables;
}

}  // namespace

namespace ain::gemini
{

prompt::prompt(const std::string& text) noexcept :
  m_text{ text }, m_unreplaced_variables{ find_variables(m_text) }
{}

prompt::status prompt::replace(const std::string& variable, const std::string& value)
{
    if (auto it = std::find(m_unreplaced_variables.begin(), m_unreplaced_variables.end(), variable);
        it != m_unreplaced_variables.end())
    {
        m_unreplaced_variables.erase(it);
        m_text = std::regex_replace(m_text, std::regex("<" + variable + ">"), value);
        return status::ok;
    }
    else
    {
        return status::variable_not_found;
    }
}

std::tuple<std::string, prompt::status> prompt::get_text() const
{
    if (m_unreplaced_variables.empty())
    {
        return { m_text, status::ok };
    }
    else
    {
        return { m_text, status::unreplaced_variable };
    }
}

std::unordered_map<std::string, prompt> load_prompts(const std::filesystem::path& path)
{
    std::unordered_map<std::string, prompt> prompts;

    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        if (entry.is_regular_file())
        {
            std::ifstream file{ entry.path() };
            std::string text{ std::istreambuf_iterator<char>(file),
                              std::istreambuf_iterator<char>() };
            prompts.emplace(entry.path().stem().string(), text);
        }
    }

    return prompts;
}

}  // namespace ain::gemini
