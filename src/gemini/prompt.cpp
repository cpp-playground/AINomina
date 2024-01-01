#include "AINomina/gemini/prompt.hpp"

#include <algorithm>
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

Prompt::Prompt(const std::string& raw_text) noexcept :
  text_{ raw_text }, variables_{ find_variables(raw_text) }
{}

// NOLINTNEXTLINE
bool Prompt::replace(const std::string& variable, const std::string& value)
{
    if (const auto it = std::ranges::find(variables_, variable); it != variables_.end())
    {
        const std::regex variable_regex("<" + variable + ">");
        text_ = std::regex_replace(text_, variable_regex, value);
        return true;
    }
    return false;
}

std::unordered_map<std::string, bool>
Prompt::replace(const std::unordered_map<std::string, std::string>& variables_map)
{
    std::unordered_map<std::string, bool> is_replaced_map;
    std::ranges::transform(variables_map,
                           std::inserter(is_replaced_map, is_replaced_map.end()),
                           [this](const auto& item) {
                               const auto& [variable, value] = item;
                               return std::make_pair(variable, replace(variable, value));
                           });
    return is_replaced_map;
}

Prompt Prompt::load_from_file(const std::filesystem::path& path)
{
    std::ifstream file{ path };
    if (!file.is_open())
    {
        throw std::runtime_error{ "Could not open file " + path.string() };
    }

    std::string text{ std::istreambuf_iterator<char>{ file }, std::istreambuf_iterator<char>{} };
    return Prompt{ text };
}

std::unordered_map<std::string, Prompt>
Prompt::load_from_directory(const std::filesystem::path& path)
{
    std::unordered_map<std::string, Prompt> prompts;
    for (const auto& entry : std::filesystem::directory_iterator{ path })
    {
        const auto& file_path = entry.path();
        if (file_path.extension() == ".prompt")
        {
            const auto& filename = file_path.filename().string();
            const auto& name = file_path.stem().string();
            prompts.emplace(name, load_from_file(file_path));
        }
    }
    return prompts;
}

}  // namespace ain::gemini
