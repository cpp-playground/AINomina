#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <string_view>

#include <boost/asio.hpp>

namespace ain::gemini
{

class gemini_client
{
  public:
    gemini_client(std::string_view api_key, std::optional<unsigned int> max_request_per_minute);

    void querry(const std::string& request);

  private:
    std::string m_api_key;
    std::optional<unsigned int> m_max_request_per_minute;
    std::chrono::system_clock::time_point m_last_request_time;

    boost::asio::io_context m_io_context;
};

}  // namespace ain::gemini
