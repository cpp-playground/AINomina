
#include <iostream>

#include "AINomina/gemini/prompt.hpp"
#include "AINomina/gemini/request.hpp"

int main(int, char** argv)
{
    auto prompts = ain::gemini::load_prompts(argv[1]);
    ain::gemini::gemini_client client(argv[2], 1);

    for (auto& [name, prompt] : prompts)
    {
        prompt.replace("variable", "Ain");
        prompt.replace("code", "int main(int Ain, char** argv) { return 0; }");

        auto [text, status] = prompt.get_text();
        if (status == ain::gemini::prompt::status::ok)
        {
            client.querry(text);
        }
    }
}
