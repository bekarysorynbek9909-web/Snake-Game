cmake_minimum_required(VERSION 3.18.1)
project(snakegame)

# Автоматически скачиваем исходники Raylib прямо во время сборки на сервере
include(FetchContent)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.zip
)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

# Создаем библиотеку вашей игры
add_library(
    snakegame
    SHARED
    src/main/cpp/main.cpp
)

# Линкуем игру с raylib и системными библиотеками Android
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
