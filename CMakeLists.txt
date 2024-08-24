cmake_minimum_required(VERSION 3.14)
project(template C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Adding Raylib
include(FetchContent)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games
FetchContent_Declare(raylib GIT_REPOSITORY https://github.com/raysan5/raylib.git GIT_TAG 5.0)
FetchContent_MakeAvailable(raylib)

# Add header files
set(HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/raygui.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/rmem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/rres.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/rres-raylib.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/reasings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/external/aes.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/external/lz4.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/external/monocypher.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/external/qoi.h
)

# Add source files
set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/include/external/aes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/include/external/lz4.c
    ${CMAKE_CURRENT_SOURCE_DIR}/include/external/monocypher.c
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

# Create source groups (filters for Visual Studio)
source_group("Header Files" FILES ${HEADER_FILES})
source_group("Source Files" FILES ${SOURCE_FILES})

# Link raylib
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/external
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

# Copy assets to build directory
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# Setting ASSETS_PATH
target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/") # Set the asset path macro to the absolute path on the dev machine
#target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="./assets") # Set the asset path macro in release mode to a relative path that assumes the assets folder is in the same directory as the game executable