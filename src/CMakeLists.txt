find_package(Threads REQUIRED)
find_package(Clang REQUIRED CONFIG)

add_definitions(${LLVM_DEFINITIONS})
if(NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

add_library(gemini STATIC gemini/prompt.cpp gemini/gemini_client.cpp)

target_include_directories(gemini PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(gemini httplib::httplib nlohmann_json::nlohmann_json)

add_executable(AINomina main.cpp)
target_include_directories(AINomina PRIVATE SYSTEM ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
target_link_libraries(AINomina PRIVATE gemini)
target_link_libraries(AINomina PRIVATE clangTooling)
