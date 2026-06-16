cmake_minimum_required(VERSION 3.12)
project(SnegovikCrypto CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main_app main.cpp scripts/input_output.cpp)

target_include_directories(main_app PRIVATE ciphers scripts)


if(NOT WIN32)
    set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

    target_link_directories(main_app PRIVATE ${CMAKE_SOURCE_DIR}/lib)
    target_link_libraries(main_app PRIVATE rsa shamir ${CMAKE_DL_LIBS})
endif()

if(WIN32)
    target_link_directories(main_app PRIVATE ${CMAKE_SOURCE_DIR}/lib)
    target_link_libraries(main_app PRIVATE rsa shamir)
    
    add_custom_command(TARGET main_app POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/lib/librsa.dll"
        "${CMAKE_SOURCE_DIR}/lib/libshamir.dll"
        $<TARGET_FILE_DIR:main_app>
    )
endif()