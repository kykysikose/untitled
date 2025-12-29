# Проверяем, нужно ли собирать тесты
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    # Загрузка Google Test
    include(FetchContent)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.12.1
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # Добавление тестовых файлов
    set(TEST_SOURCES
            test_inverted_index.cpp
            test_search_server.cpp
    )

    # Создание тестового исполняемого файла
    add_executable(search_engine_tests
            ${TEST_SOURCES}
            ../src/converter_json.cpp
            ../src/inverted_index.cpp
            ../src/search_server.cpp
    )

    # Настройка тестового таргета
    target_include_directories(search_engine_tests PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/../src
            ${CMAKE_CURRENT_SOURCE_DIR}/../include
            ${gtest_SOURCE_DIR}/include
            ${gmock_SOURCE_DIR}/include
    )

    # Подключение библиотек
    target_link_libraries(search_engine_tests PRIVATE
            gtest
            gtest_main
            Threads::Threads
            nlohmann_json::nlohmann_json  # Если используется JSON
    )

    # Настройка компилятора для тестов
    if(MSVC)
        target_compile_options(search_engine_tests PRIVATE /W4)
    else()
        target_compile_options(search_engine_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    # Регистрация тестов
    include(GoogleTest)
    gtest_discover_tests(search_engine_tests
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    )

    # Настройка для запуска тестов с правильной рабочей директорией
    set_target_properties(search_engine_tests PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
endif()