cmake_minimum_required(VERSION 3.14)

message("Building with CMake version: ${CMAKE_VERSION}")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(Sources
	./src/main.cpp
	./src/boot.cpp
	./src/camera.cpp
	./src/common.cpp
	./src/cube.cpp
	./src/dragon.cpp
	./src/flower.cpp
	./src/light.cpp
	./src/marius.cpp
	./src/mesh.cpp
	./src/myshaderclass.cpp
	./src/octahedron.cpp
	./src/shaderclass.cpp
	./src/skull.cpp
	./src/teapot.cpp
	./src/texture2D.cpp
	./src/transform.cpp
	./src/utilities.cpp
)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/tools/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(Sandbox LANGUAGES CXX VERSION 0.0.1)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

find_package(Stb REQUIRED)

find_package(FreeGLUT CONFIG REQUIRED)

find_package(assimp CONFIG REQUIRED)

find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(${PROJECT_NAME} ${Sources})
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp GLUT::GLUT $<IF:$<TARGET_EXISTS:FreeGLUT::freeglut>,FreeGLUT::freeglut,FreeGLUT::freeglut_static> GLEW::GLEW glm::glm )

target_include_directories(${PROJECT_NAME} PRIVATE ${Stb_INCLUDE_DIR})
# Dirty way
# TODO improve (for install for example...)
add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/models ${CMAKE_CURRENT_BINARY_DIR}/models
)
# These should be in a shaders folder
add_custom_target(copy_shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/src/shaders ${CMAKE_CURRENT_BINARY_DIR}/shaders
)
# These? it should depend of the model but maybe i have to clean the code because i do not see it used
add_custom_target(copy_textures
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/src/texture ${CMAKE_CURRENT_BINARY_DIR}
)
add_dependencies(${PROJECT_NAME} copy_assets copy_shaders copy_textures)