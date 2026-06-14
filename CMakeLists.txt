cmake_minimum_required(VERSION 3.12)
project(SnegovikCrypto CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. СНАЧАЛА настраиваем правила поиска библиотек (экранируем $ знаком \ )
set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

# 2. ТЕПЕРЬ создаем исполняемый файл (он автоматически впитает RPATH)
add_executable(main_app main.cpp scripts/input_output.cpp)

target_include_directories(main_app PRIVATE ciphers scripts)
target_link_libraries(main_app PRIVATE ${CMAKE_DL_LIBS})
