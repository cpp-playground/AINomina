find_package(Threads REQUIRED)

add_library(gemini STATIC gemini/request.cpp)

target_include_directories(gemini PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(gemini httplib::httplib nlohmann_json::nlohmann_json)

add_executable(AINomina main.cpp)
target_link_libraries(AINomina PRIVATE gemini)
