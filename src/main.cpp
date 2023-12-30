
#include <iostream>

#include "AINomina/gemini/prompt.hpp"
#include "AINomina/gemini/request.hpp"

int main(int, char** argv)
{
    auto prompts = ain::gemini::Prompt::load_from_directory(argv[1]);
    ain::gemini::gemini_client client(argv[2], 1);

    for (auto& [name, prompt] : prompts)
    {
        std::cout << "Promting " << name << std::endl;
        // for (auto& var : prompt.variables())
        // {
        //     std::cout << var << std::endl;
        // }

        // std::cout << "Now replacing" << std::endl;
        prompt.replace("variable", "Ain");
        prompt.replace("code", "int main(int Ain, char** argv) { return 0; }");

        // std::cout << prompt.text() << std::endl;
        // for (auto& var : prompt.variables())
        // {
        //     std::cout << var << std::endl;
        // }

        client.querry(prompt.text());
    }
}
