cmake_minimum_required(VERSION 3.21)

message("Building with CMake version: ${CMAKE_VERSION}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(Sources
	./src/application.cpp
	./src/camera.cpp
	./src/light.cpp
	./src/main.cpp
	./src/material.cpp
	./src/mesh.cpp
	./src/model.cpp
	./src/object.cpp
	./src/renderer.cpp
	./src/scene.cpp
	./src/shader.cpp
	./src/framebuffer.cpp
	./src/texture.cpp
	./src/transform.cpp
	./src/vertex.cpp
)

set(Headers 
	./src/my_imguiconfig.h 
)

add_compile_definitions(IMGUI_USER_CONFIG="${CMAKE_CURRENT_SOURCE_DIR}/src/my_imguiconfig.h")

project(Tesselatior LANGUAGES CXX VERSION 0.0.1)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

find_package(glfw3 CONFIG REQUIRED)

find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)

find_package(assimp CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

find_package(Stb REQUIRED)

find_package(spdlog CONFIG REQUIRED)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "GLFW lib only")

add_executable(${PROJECT_NAME} ${Sources} ${Headers})

target_link_libraries(${PROJECT_NAME} PRIVATE
  glfw 
  GLEW::GLEW 
  glm::glm 
  imgui::imgui
  assimp::assimp
  spdlog::spdlog
)

target_include_directories(${PROJECT_NAME} PRIVATE ${Stb_INCLUDE_DIR})

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/models ${CMAKE_CURRENT_BINARY_DIR}/${PRESET_NAME}/models
)
# These should be in a shaders folder
add_custom_target(copy_shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders ${CMAKE_CURRENT_BINARY_DIR}/${PRESET_NAME}/shaders
)
# These? it should depend of the model but maybe i have to clean the code because i do not see it used
add_custom_target(copy_textures
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/src/texture ${CMAKE_CURRENT_BINARY_DIR}/${PRESET_NAME}
)

add_custom_target(copy_imgui_layout
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/imgui-layout.ini ${CMAKE_CURRENT_BINARY_DIR}/${PRESET_NAME}
)

if(NOT $ENV{CI})
  add_dependencies(${PROJECT_NAME} copy_assets copy_shaders copy_textures copy_imgui_layout)
endif()

#TODO: refactor
# if(MSVC)
#   target_compile_options(${PROJECT_NAME} PRIVATE /W4)
# else()
#   target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wno-unused -Wno-unused-parameter -Wno-sign-compare)
# endif()
