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
