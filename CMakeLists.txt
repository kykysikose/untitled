cmake_minimum_required(VERSION 3.20)
project(search_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include(FetchContent)

# -------- nlohmann/json --------
FetchContent_Declare(
  nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

# -------- GoogleTest --------
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# Для MinGW иногда нужно (можете раскомментировать при проблемах):
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# set(gtest_disable_pthreads ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)
enable_testing()

# -------- Library (логика движка) --------
add_library(search_engine_lib
  src/converter_json.cpp
  src/inverted_index.cpp
  src/search_server.cpp
)

target_include_directories(search_engine_lib PUBLIC include)
target_link_libraries(search_engine_lib PUBLIC nlohmann_json::nlohmann_json)

# -------- App --------
add_executable(search_engine_app src/main.cpp)
target_link_libraries(search_engine_app PRIVATE search_engine_lib)

# -------- Tests --------
add_executable(search_engine_tests
  tests/test_inverted_index.cpp
  tests/test_search_server.cpp
)

add_subdirectory(src)
add_subdirectory(tests)

configure_file(config.json ${CMAKE_BINARY_DIR}/config.json COPYONLY)
configure_file(requests.json ${CMAKE_BINARY_DIR}/requests.json COPYONLY)
configure_file(answers.json ${CMAKE_BINARY_DIR}/answers.json COPYONLY)


target_link_libraries(search_engine_tests PRIVATE search_engine_lib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(search_engine_tests)