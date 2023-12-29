
#include "AINomina/gemini/request.hpp"

#include <iostream>

#include <httplib.h>
#include <nlohmann/json.hpp>

using namespace std::string_literals;

namespace ain::gemini
{

gemini_client::gemini_client(std::string_view api_key,
                             std::optional<unsigned int> max_request_per_minute) :
  m_api_key{ api_key }, m_max_request_per_minute{ max_request_per_minute }
{}

void gemini_client::querry(const ain::gemini::request&)
{
    httplib::Client https_client("https://generativelanguage.googleapis.com");

    nlohmann::json j;
    j["generationConfig"]["temperature"] = 0.1;
    j["generationConfig"]["topK"] = 1;
    j["generationConfig"]["topP"] = 1;
    j["generationConfig"]["maxOutputTokens"] = 2048;

    nlohmann::json j2;
    j2["parts"] = { { "text",
                      "Output:\nThe output must be formatted as JSON text. For example the "
                      "following are valid outputs:\n- {\"name\" : "
                      "\"request_payload\",\"quality\": \"GOOD\", \"explanation\" : \"\", "
                      "\"alternative\" : \"\"}\n- {\"name\" : \"wc\", \"quality\": \"BAD\", "
                      "\"explanation\" : \"The variable name is too short\", \"alternative\" : "
                      "\"word_count\"}\n- {\"name\" : \"s\",\"quality\": \"BAD\", \"explanation\" "
                      ": \"Very short names are reserved to indexes\", \"alternative\" : "
                      "\"size\"}\n\nContext:\nQuality can only be one of the following:\n- GOOD\n- "
                      "NEUTRAL\n- BAD\n\nConsider the following C++ code when deciding the quality "
                      "of a variable name:\nint main()\n{\nfor(auto i = 0; i < 10; "
                      "++i)\nstd::cout << i ;\n}\n\nAdditionally consider those rules:\n- "
                      "Variable names should be explicit\n- Variable names should be coherent with "
                      "their usage\n- Single letter variables names are forbidden, except if they "
                      "represent counters.\n\nInput:\nDoes the variable i follow the ru"
                      "les?\nDoes it have a good name within the context of the provided co"
                      "de?\nIf the quality is BAD or NEUTRAL, always propose an alternative na"
                      "me." } };

    j["contents"] = j2;

    auto res = https_client.Post("/v1beta/models/gemini-pro:generateContent?key="s + m_api_key,
                                 j.dump(),
                                 "application/json");

    std::cout << res->status << std::endl;
    std::cout << res->body << std::endl;
}

}  // namespace ain::gemini
