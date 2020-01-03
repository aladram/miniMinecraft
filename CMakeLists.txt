# Project definition
cmake_minimum_required(VERSION 3.12)
project(opengl-demo)

set(CMAKE_CXX_STANDARD 17)

set(BIN_NAME "${PROJECT_NAME}")
set(LIB_DIR "libs")

add_custom_command(OUTPUT vertex_shader.c
		DEPENDS ${CMAKE_SOURCE_DIR}/src/vertex.shd
		COMMAND ${CMAKE_SOURCE_DIR}/compile_shader.sh ${CMAKE_SOURCE_DIR}/src/vertex.shd vertex_shader.c)
add_custom_command(OUTPUT fragment_shader.c
		DEPENDS ${CMAKE_SOURCE_DIR}/src/fragment.shd
		COMMAND ${CMAKE_SOURCE_DIR}/compile_shader.sh ${CMAKE_SOURCE_DIR}/src/fragment.shd fragment_shader.c)
add_custom_command(OUTPUT terrain_png.c
		DEPENDS ${CMAKE_SOURCE_DIR}/res/terrain.png
		COMMAND ${CMAKE_SOURCE_DIR}/export_file.sh ${CMAKE_SOURCE_DIR}/res/terrain.png terrain_png.c)

add_executable(${BIN_NAME}
		vertex_shader.c
		fragment_shader.c
		terrain_png.c
		src/main.cc
		src/my-shaders.cc
		src/utils/shaders.cc
		src/window.cc
		src/camera.cc
		src/primitives/cube.cc
		src/world/world.cc
		src/world/entity.cc
		src/world/aabb.cc
		src/renderer.cc
		src/world/chunk.cc
		src/world/block.cc
		src/world/generator.cc
		src/program.cc
        )

target_include_directories(${BIN_NAME} PRIVATE "include/")
target_compile_definitions(${BIN_NAME} PRIVATE OPENGL_VERSION_MAJOR=3 OPENGL_VERSION_MINOR=3)
target_compile_options(${BIN_NAME} PRIVATE -Wall -Wextra -Ofast -march=native)

# OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(${BIN_NAME} OpenMP::OpenMP_CXX)
endif()

# GLFW
set(GLFW_DIR "${LIB_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory("${GLFW_DIR}")
target_link_libraries(${BIN_NAME} "glfw" "${GLFW_LIBRARIES}")
target_include_directories(${BIN_NAME} PRIVATE "${GLFW_DIR}/include")
target_compile_definitions(${BIN_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# glad
set(GLAD_DIR "${LIB_DIR}/glad")
add_library(glad "${GLAD_DIR}/src/glad.c")
target_include_directories(glad PUBLIC "${GLAD_DIR}/include")
target_link_libraries(${BIN_NAME} "glad" "${CMAKE_DL_LIBS}")

# glm
target_include_directories(${BIN_NAME} PRIVATE "${LIB_DIR}/glm/")

# assimp
#add_subdirectory("${LIB_DIR}/assimp" EXCLUDE_FROM_ALL)
#target_link_libraries(${BIN_NAME} assimp::assimp)

# stb
set(STB_DIR "${LIB_DIR}/stb")
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "${STB_DIR}")
target_link_libraries(${BIN_NAME} "stb")

# Perlin noise
set(PERLIN_DIR "${LIB_DIR}/perlin")
add_library(perlin INTERFACE)
target_include_directories(perlin INTERFACE "${PERLIN_DIR}")
target_link_libraries(${BIN_NAME} "perlin")
