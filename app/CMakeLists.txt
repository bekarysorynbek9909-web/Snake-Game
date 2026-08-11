cmake_minimum_required(VERSION 3.18.1)
project(snakegame)

# Принудительно указываем, что собираем под Android и отключаем десктопные модули
set(PLATFORM "Android" CACHE STRING "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(WITH_GLFW OFF CACHE BOOL "" FORCE)

# Автоматически скачиваем исходники Raylib
include(FetchContent)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.zip
)
FetchContent_MakeAvailable(raylib)

# Создаем библиотеку вашей игры
add_library(
    snakegame
    SHARED
    src/main/cpp/main.cpp
)

# Линкуем игру с raylib и обязательными системными библиотеками Android
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
