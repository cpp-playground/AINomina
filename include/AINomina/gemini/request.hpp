#pragma once

#include <optional>
#include <string>
#include <string_view>

#include <boost/asio.hpp>

namespace ain::gemini
{
struct request
{
    std::string m_code_context;
    std::string m_entity_name;
};

enum class entity_name_quality
{
    good,
    bad,
};

struct reply
{
    ain::gemini::request m_request;
    entity_name_quality m_entity_name_quality;

    std::optional<std::string> m_explanation;
    std::optional<std::string> m_alternative_name;
};

class gemini_client
{
  public:
    gemini_client(std::string_view api_key, std::optional<unsigned int> max_request_per_minute);

    void querry(const ain::gemini::request& request);

  private:
    std::string m_api_key;
    std::optional<unsigned int> m_max_request_per_minute;

    boost::asio::io_context m_io_context;
};

}  // namespace ain::gemini
