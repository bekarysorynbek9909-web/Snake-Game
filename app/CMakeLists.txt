cmake_minimum_required(VERSION 3.18.1)
project(snakegame)

# Указываем путь к заголовочным файлам raylib
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/raylib/include)

# Создаем разделяемую библиотеку из вашего файла main.cpp
add_library(
    snakegame
    SHARED
    src/main/cpp/main.cpp
)

# Линкуем библиотеку с системными компонентами Android и графикой OpenGL ES
target_link_libraries(
    snakegame
    android
    log
    GLESv2
    EGL
)
