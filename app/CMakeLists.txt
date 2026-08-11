cmake_minimum_required(VERSION 3.18.1)
project(snakegame)

# 1. Указываем путь к заголовочным файлам raylib
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/raylib/include)

# 2. Находим все исходные файлы .c raylib (если они лежат в папке src внутри raylib)
# Убедитесь, что у вас в репозитории есть папка со исходниками raylib (например, raylib/src/*.c)
# Либо добавьте их явно. Самый простой способ для Android — собрать raylib как статическую библиотеку:

# Устанавливаем флаги для сборки raylib под Android
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

# Если у вас в репозитории лежит вся папка raylib с исходниками (.c файлами в raylib/src):
add_subdirectory(src/main/cpp/raylib)

# 3. Создаем вашу библиотеку игры
add_library(
    snakegame
    SHARED
    src/main/cpp/main.cpp
)

# 4. Линкуем вашу игру с raylib и системными библиотеками Android
target_link_libraries(
    snakegame
    raylib
    android
    log
    GLESv2
    EGL
    m
    dl
    atomic
)
