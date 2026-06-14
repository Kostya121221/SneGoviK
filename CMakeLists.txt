cmake_minimum_required(VERSION 3.12)
project(SnegovikCrypto CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Создаем исполняемый файл из ваших cpp-скриптов
add_executable(main_app main.cpp scripts/input_output.cpp)

# Подключаем папки с хедерами (.h)
target_include_directories(main_app PRIVATE ciphers scripts)

# 2. НАСТРОЙКА ДЛЯ LINUX (Ищем готовые .so)
if(NOT WIN32)
    # Настраиваем RPATH, чтобы main_app искал библиотеки в соседней папке lib/
    set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

    # Указываем линковщику ГДЕ искать библиотеки (-L) и КАКИЕ именно (-l)
    target_link_directories(main_app PRIVATE ${CMAKE_SOURCE_DIR}/lib)
    target_link_libraries(main_app PRIVATE rsa shamir ${CMAKE_DL_LIBS})
endif()

# 3. НАСТРОЙКА ДЛЯ WINDOWS (Ищем готовые .dll)
if(WIN32)
    # Указываем, где лежат ваши готовые файлы .lib / .dll
    target_link_directories(main_app PRIVATE ${CMAKE_SOURCE_DIR}/lib)
    target_link_libraries(main_app PRIVATE rsa shamir)
    
    # Автоматически копируем .dll файлы в папку к готовому .exe при сборке,
    # иначе Windows просто не запустит исполняемый файл.
    add_custom_command(TARGET main_app POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/lib/librsa.dll"
        "${CMAKE_SOURCE_DIR}/lib/libshamir.dll"
        $<TARGET_FILE_DIR:main_app>
    )
endif()