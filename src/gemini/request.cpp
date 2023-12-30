
#include "AINomina/gemini/request.hpp"

#include <chrono>
#include <iostream>

#include <httplib.h>
#include <nlohmann/json.hpp>

using namespace std::string_literals;

namespace ain::gemini
{

gemini_client::gemini_client(std::string_view api_key,
                             std::optional<unsigned int> max_request_per_minute) :
  m_api_key{ api_key },
  m_max_request_per_minute{ max_request_per_minute },
  m_last_request_time{ std::chrono::system_clock::now() }
{}

void gemini_client::querry(const std::string& request)
{
    if (m_max_request_per_minute)
    {
        const auto now = std::chrono::system_clock::now();
        const auto elapsed = now - m_last_request_time;

        if (elapsed < std::chrono::milliseconds{ 60000 } / *m_max_request_per_minute)
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds{ 6000 } / *m_max_request_per_minute - elapsed);
        }
    }

    httplib::Client https_client("https://generativelanguage.googleapis.com");

    nlohmann::json request_json;
    request_json["generationConfig"]["temperature"] = 0.1;
    request_json["generationConfig"]["topK"] = 1;
    request_json["generationConfig"]["topP"] = 1;
    request_json["generationConfig"]["maxOutputTokens"] = 2048;
    request_json["contents"]["parts"] = { { "text", request }, { "text", request } };

    auto res = https_client.Post("/v1beta/models/gemini-pro:generateContent?key="s + m_api_key,
                                 request_json.dump(),
                                 "application/json");

    auto rep = nlohmann::json::parse(res->body);
    std::cout << rep["candidates"][0]["content"].dump() << std::endl;
}

}  // namespace ain::gemini
