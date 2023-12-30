#include "AINomina/gemini/gemini_client.hpp"

#include <thread>

#include <boost/algorithm/string.hpp>

namespace
{
/**
 * @brief Unescape a string.
 * Received JSON contains escaped characters, this function unescapes them.
 * Taken and adapted from
 * https://stackoverflow.com/questions/5612182/convert-string-with-explicit-escape-sequence-into-relative-character
 *
 * @param escaped_string The string to unescape.
 * @return std::string The unescaped string.
 */
std::string unescape(const std::string& escaped_string)
{
    std::string res;
    auto it = escaped_string.begin();
    while (it != escaped_string.end())
    {
        char character = *it++;
        if (character == '\\' && it != escaped_string.end())
        {
            switch (*it++)
            {
                case '\\':
                    character = '\\';
                    break;
                case 'n':
                    character = '\n';
                    break;
                case 't':
                    character = '\t';
                    break;
                case 'r':
                    character = '\r';
                    break;
                // all other escapes
                default:
                    continue;
            }
        }
        res += character;
    }

    // Remove leading and trailing whitespaces
    boost::trim(res);

    // If res starts by ```json, remove it
    if (res.size() >= 7 && res.substr(0, 7) == "```json")
    {
        res = res.substr(7);
    }

    // If res ends by ```, remove it
    if (res.size() >= 3 && res.substr(res.size() - 3, 3) == "```")
    {
        res = res.substr(0, res.size() - 3);
    }

    return res;
}

nlohmann::json to_json(const ain::gemini::QuerryParameters parameters)
{
    nlohmann::json json;
    json["temperature"] = parameters.temperature_;
    json["topK"] = parameters.top_k_;
    json["topP"] = parameters.top_p_;
    json["maxOutputTokens"] = parameters.max_output_tokens_;
    return json;
}

nlohmann::json to_json(const ain::gemini::Prompt& prompt)
{
    nlohmann::json json;
    json["parts"] = { { "text", prompt.text() } };
    return json;
}

std::optional<nlohmann::json> parse_response(const nlohmann::json& response)
{
    if (!response.contains("candidates"))
    {
        return std::nullopt;
    }
    const auto& candidates = response["candidates"];
    if (candidates.empty())
    {
        return std::nullopt;
    }

    const auto& result = candidates[0];
    if (result["finishReason"] != "STOP")
    {
        return std::nullopt;
    }

    auto text = result["content"]["parts"][0]["text"].get<std::string>();
    return nlohmann::json::parse(unescape(text));
}
}  // namespace

namespace ain::gemini
{

GeminiClient::GeminiClient(const std::string& api_key,
                           std::optional<unsigned int> max_request_per_minute) noexcept :
  api_key_{ api_key },
  use_rate_limiter_{ max_request_per_minute.has_value() },
  max_request_per_minute_{ max_request_per_minute.value_or(0) },
  http_client_{ "https://generativelanguage.googleapis.com" }
{
    http_client_.set_read_timeout(60, 0);
}

std::optional<nlohmann::json> GeminiClient::querry(const QuerryParameters& params,
                                                   const ain::gemini::Prompt& prompt)
{
    if (use_rate_limiter_)
    {
        apply_rate_limitation();
    }

    nlohmann::json request_json;
    request_json["generationConfig"] = to_json(params);
    request_json["contents"] = to_json(prompt);

    auto res = http_client_.Post("/v1beta/models/gemini-pro:generateContent?key=" + api_key_,
                                 request_json.dump(),
                                 "application/json");

    if (res && res->status == 200)
    {
        return parse_response(nlohmann::json::parse(res->body));
    }

    return std::nullopt;
}

void GeminiClient::apply_rate_limitation()
{
    auto now = std::chrono::system_clock::now();
    std::erase_if(request_times_, [now](const auto& time_point) {
        return time_point < now - std::chrono::minutes{ 1 };
    });

    if (request_times_.size() >= max_request_per_minute_)
    {
        // Note, by construction, the first element is always the oldest one.
        auto time_to_wait = std::chrono::minutes{ 1 } - (now - request_times_.front());
        std::this_thread::sleep_for(time_to_wait);
    }

    request_times_.push_back(now);
}
}  // namespace ain::gemini
