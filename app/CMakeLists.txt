cmake_minimum_required(VERSION 3.10.2)
project("snakegame")

# Подключаем Raylib для Android (используем стандартные флаги)
add_library(snakegame SHARED
    src/main/cpp/main.cpp
)

find_library(log-lib log)
find_library(android-lib android)
find_library(opengl-lib GLESv2)
find_library( EGL-lib EGL)

target_link_libraries(snakegame
    ${log-lib}
    ${android-lib}
    ${opengl-lib}
    ${EGL-lib}
)