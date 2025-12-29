# Добавление заголовочных файлов
set(HEADER_FILES
        ../include/converter_json.h
        ../include/inverted_index.h
        ../include/search_server.h
        ../include/text_utils.h
)

# Добавление исходных файлов
set(SOURCE_FILES
        converter_json.cpp
        inverted_index.cpp
        main.cpp
        search_server.cpp
)

# Создание исполняемого файла
add_executable(search_engine ${SOURCE_FILES} ${HEADER_FILES})

# Установка целевых свойств
target_include_directories(search_engine PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Подключение системных библиотек
find_package(Threads REQUIRED)
target_link_libraries(search_engine PRIVATE Threads::Threads)

# Настройка компилятора
if(MSVC)
    target_compile_options(search_engine PRIVATE /W4 /WX)
else()
    target_compile_options(search_engine PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Подключение JSON библиотеки (если используется)
# ВАЖНО: Выберите один из вариантов ниже

# Вариант 1: Скачивание nlohmann/json автоматически
include(FetchContent)
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)
target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json)

# Вариант 2: Если json.hpp уже в include/
# target_include_directories(search_engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Вариант 3: Использовать встроенную библиотеку (если есть)
# find_package(nlohmann_json 3.2.0 REQUIRED)
# target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json)