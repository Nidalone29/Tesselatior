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
	./src/mesh/mesh.cpp
	./src/mesh/model_importer.cpp
	./src/mesh/object.cpp
	./src/renderer.cpp
	./src/scene.cpp
	./src/shader.cpp
	./src/framebuffer.cpp
	./src/texture.cpp
	./src/transform.cpp
	./src/mesh/vertex.cpp
	./src/mesh/halfedge.cpp
	./src/mesh/staticmodel.cpp
	./src/mesh/subdivmesh.cpp
	./src/mesh/terrain.cpp
	./src/mesh/importer.cpp
	./src/mesh/assimp_importer.cpp
	./src/subdiv/subdivision.cpp
	./src/subdiv/loop.cpp
	./src/subdiv/sqrt3.cpp
	./src/subdiv/catmullclark.cpp
)

set(Headers 
	./src/my_imguiconfig.h 
)

add_compile_definitions(IMGUI_USER_CONFIG="${CMAKE_CURRENT_SOURCE_DIR}/src/my_imguiconfig.h")

if(NOT DEFINED ENV{VCPKG_ROOT})
  message(FATAL_ERROR "VCPKG_ROOT is not set!")
else()
  message(STATUS "VCPKG_ROOT is set to: $ENV{VCPKG_ROOT}")
endif()

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

add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/models ${CMAKE_CURRENT_BINARY_DIR}/${PRESET_NAME}/models
)
# These should be in a shaders folder
add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders ${CMAKE_CURRENT_BINARY_DIR}/${PRESET_NAME}/shaders
)
# These? it should depend of the model but maybe i have to clean the code because i do not see it used
add_custom_target(copy_textures ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/src/texture ${CMAKE_CURRENT_BINARY_DIR}/${PRESET_NAME}
)

add_custom_target(copy_imgui_layout ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/imgui-layout.ini ${CMAKE_CURRENT_BINARY_DIR}/${PRESET_NAME}
)

if(NOT $ENV{CI})
  add_dependencies(${PROJECT_NAME} copy_assets copy_shaders copy_textures copy_imgui_layout)
endif()

#TODO: refactor
# if(MSVC)
#   target_compile_options(${PROJECT_NAME} PRIVATE /W4)
# else()
#   target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wno-unused -Wno-unused-parameter -Wno-sign-compare) #-fsanitize=undefined
#   # target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=undefined)
# endif()
#
# message(STATUS "CXX Flags for RelWithDebInfo: ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
