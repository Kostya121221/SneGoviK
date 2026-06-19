cmake_minimum_required(VERSION 3.10)
project(ElGamalLibrary VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

add_library(elgamal SHARED 
    ../ciphers/elgamal.cpp 
    ../scripts/crypto_math.cpp
)
add_library(des SHARED 
    ../ciphers/des.cpp 
    ../scripts/crypto_math.cpp
)
add_library(rc4 SHARED 
    ../ciphers/rc4.cpp 
    ../scripts/crypto_math.cpp
)
add_library(rc5 SHARED 
    ../ciphers/rc5.cpp 
    ../scripts/crypto_math.cpp
)
add_library(rsa SHARED 
    ../ciphers/rsa.cpp 
    ../scripts/crypto_math.cpp
)
add_library(shamir SHARED 
    ../ciphers/shamir.cpp 
    ../scripts/crypto_math.cpp
)
add_library(crypto_math SHARED 
    ../scripts/crypto_math.cpp
)
add_executable(main_app main.cpp scripts/input_output.cpp scripts/crypto_math.cpp)

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
