cmake_minimum_required(VERSION 3.26)
project(CardsProbabilities)

set(CMAKE_CXX_STANDARD 20)

add_executable(CardsProbabilities main.cpp)


# VT 2023-10-06
# ---------------------------------------------------------------------
# Host specific configurations goes here.
# ---------------------------------------------------------------------
# https://cmake.org/cmake/help/v3.27/command/cmake_host_system_information.html
# ---------------------------------------------------------------------
cmake_host_system_information(RESULT VT_HOST QUERY HOSTNAME)
#message(STATUS "VT_HOST is ${VT_HOST}")

if (VT_HOST STREQUAL "pingu.local")
    message(STATUS "Host is pingu:")
elseif (VT_HOST STREQUAL "turing.local")
    message(STATUS "Host is turing:")
elseif (VT_HOST STREQUAL "macaron")
    message(STATUS "Host is macaron:")
else ()
    message(STATUS "generic host environment for ${VT_HOST}:")
endif ()
# ---------------------------------------------------------------------

# VT 2023-07-16
# ---------------------------------------------------------------------
# wxWidgets
# ---------------------------------------------------------------------
# For wxWidgets environment setting, collect information by running
# run: wx-config --cxxflags
# run: ex-config --libs
# to consolidate and update this CMakeLists.txt accordingly:
#    target_include_directories()
#    add_compile_definitions()
#    target_link_directories()
#    target_link_libraries()
# ---------------------------------------------------------------------

# ---------------------------------------------------------------------
# POCO framework
# ---------------------------------------------------------------------
# POCO helper classes are in "\\plovercove.local\home\Programming\Clippings\POCO Application\"
# ---------------------------------------------------------------------
# Properties file management
set(APP_PROPERTIES ${CMAKE_PROJECT_NAME}.properties)
unset(${CMAKE_SOURCE_DIR}/App.properties CACHE)
if (EXISTS ${CMAKE_SOURCE_DIR}/App.properties)
    configure_file(${CMAKE_SOURCE_DIR}/App.properties ${CMAKE_BINARY_DIR}/${APP_PROPERTIES} COPYONLY)
endif ()
# ---------------------------------------------------------------------


# VT 2023-06-27
# CLion template in Settings > Editor > File and Code Templates > Other
#


target_link_libraries(${PROJECT_NAME}
        fmt
        # PocoFoundation
        # "-framework Accelerate"
)

# https://stackoverflow.com/questions/10046114/in-cmake-how-can-i-test-if-the-compiler-is-clang
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # using regular Clang or AppleClang
    #if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # using Clang
    target_include_directories(${PROJECT_NAME}
            PUBLIC /opt/homebrew/include
            PUBLIC /opt/local/include
            PUBLIC /usr/local/include
            #        PUBLIC /usr/local/include/eigen3
    )

    target_link_directories(${PROJECT_NAME}
            PUBLIC /opt/homebrew/lib
            PUBLIC /opt/local/lib
            PUBLIC /usr/local/lib
    )

    add_compile_options(-Wall -Wextra -Wpedantic -fexperimental-library)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC
    target_include_directories(${PROJECT_NAME}
            PUBLIC /opt/gcc/include
            PUBLIC /opt/homebrew/include
            PUBLIC /opt/local/include
            PUBLIC /usr/local/include
            #        PUBLIC /usr/local/include/eigen3
    )
    target_link_directories(${PROJECT_NAME}
            PUBLIC /opt/gcc/lib
            PUBLIC /opt/homebrew/lib
            PUBLIC /opt/local/lib
            PUBLIC /usr/local/lib
    )
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    # using Intel C++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # using Visual Studio C++
endif ()

# === end ===


