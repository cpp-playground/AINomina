

#include "AINomina/gemini/request.hpp"

int main(int, char** argv)
{
    ain::gemini::request request;
    request.m_code_context = "code_context";
    request.m_entity_name = "entity_name";

    ain::gemini::gemini_client client{ argv[1], 60 };
    client.querry(request);
}
