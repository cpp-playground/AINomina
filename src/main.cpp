
#include <iostream>

#include "AINomina/gemini/gemini_client.hpp"
#include "AINomina/gemini/prompt.hpp"

int main(int, char** argv)
{
    auto prompts = ain::gemini::Prompt::load_from_directory(argv[1]);
    ain::gemini::GeminiClient client(argv[2], 10);

    std::ifstream file{ argv[3] };
    if (!file.is_open())
    {
        throw std::runtime_error{ "Could not open file " + std::string{ argv[3] } };
    }
    std::string code{ std::istreambuf_iterator<char>{ file }, std::istreambuf_iterator<char>{} };

    for (auto& [name, prompt] : prompts)
    {
        std::cout << "Promting " << name << std::endl;
        // for (auto& var : prompt.variables())
        // {
        //     std::cout << var << std::endl;
        // }

        // std::cout << "Now replacing" << std::endl;
        prompt.replace("code", code);

        // std::cout << prompt.text() << std::endl;
        // for (auto& var : prompt.variables())
        // {
        //     std::cout << var << std::endl;
        // }

        if (auto res = client.querry(ain::gemini::QuerryParameters{}, prompt))
        {
            std::cout << res->dump(4) << std::endl;
        }
        else
        {
            std::cout << "Failed to querry" << std::endl;
        }
    }
}
