#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <vector>

#include <httplib.h>
#include <nlohmann/json.hpp>

#include "AINomina/gemini/prompt.hpp"

namespace ain::gemini
{

struct QuerryParameters
{
    double temperature_{ 0.0 };
    unsigned int top_k_{ 1 };
    double top_p_{ 1.0 };
    unsigned int max_output_tokens_{ 2048 };
};

class GeminiClient
{
  public:
    GeminiClient(const std::string& api_key,
                 std::optional<unsigned int> max_request_per_minute) noexcept;

    std::optional<nlohmann::json> querry(const QuerryParameters& params,
                                         const ain::gemini::Prompt& prompt);

  private:
    void apply_rate_limitation();

    std::string api_key_;
    bool use_rate_limiter_;
    std::size_t max_request_per_minute_;
    std::vector<std::chrono::time_point<std::chrono::system_clock>> request_times_;
    httplib::Client http_client_;
};
}  // namespace ain::gemini
