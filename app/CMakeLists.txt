cmake_minimum_required(VERSION 3.18.1)
project(snakegame)

# Указываем точный путь к папке include с учетом вложенности в src/main/cpp/
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/raylib/include)

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
