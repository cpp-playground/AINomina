#pragma once

#include <filesystem>
#include <set>
#include <string>
#include <unordered_map>

namespace ain::gemini
{
/**
 * @brief A prompt is a string that contains variables that can be replaced by
 * values. Variables are enclosed in angle brackets, e.g. <variable>.
 * Only strings can be used as values.
 */
class [[nodiscard]] Prompt
{
  public:
    /**
     * @brief Constructs a prompt from a string.
     * The existing variables are found and saved for later replacement.
     *
     * @param raw_text The string to construct the prompt from.
     */
    explicit Prompt(const std::string& raw_text) noexcept;
    Prompt(const Prompt& prompt) noexcept = default;
    Prompt(Prompt&& prompt) noexcept = default;
    Prompt& operator=(const Prompt& prompt) noexcept = default;
    Prompt& operator=(Prompt&& prompt) noexcept = default;

    /**
     * @brief Returns true if all variables have been replaced.
     */
    [[nodiscard]] bool is_fully_replaced() const noexcept
    {
        return variables_.empty();
    }

    /**
     * @brief Returns the text of the prompt, with all the variables replaced so far.
     */
    [[nodiscard]] auto text() const
    {
        return text_;
    }

    /**
     * @brief Returns the set of variables that have not been replaced yet.
     */
    [[nodiscard]] auto variables() const
    {
        return variables_;
    }

    /**
     * @brief Replaces the variable with the given value.
     * @param variable The variable to replace.
     * @param value The value to replace the variable with.
     * @return true if the variable was found and replaced, false otherwise.
     */
    bool replace(const std::string& variable, const std::string& value);

    /**
     * @brief Replaces the variables with the given values.
     * @param variables_map A map of variables and their values.
     * @return A map of variables and whether they were replaced.
     */
    std::unordered_map<std::string, bool>
    replace(const std::unordered_map<std::string, std::string>& variables_map);

    /**
     * @brief Loads a prompt from a file.
     * @param path The path to the file.
     * @return The prompt.
     */
    [[nodiscard]] static Prompt load_from_file(const std::filesystem::path& path);

    /**
     * @brief Loads all the prompts from a directory. Only files with the extension .prompt are
     * loaded.
     * @param path The path to the directory.
     * @return A map of prompt names and loaded prompts.
     */
    [[nodiscard]] static std::unordered_map<std::string, Prompt>
    load_from_directory(const std::filesystem::path& path);

  private:
    std::string text_;
    std::set<std::string> variables_;
};

}  // namespace ain::gemini
