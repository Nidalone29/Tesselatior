cmake_minimum_required(VERSION 3.14)

message("Building with CMake version: ${CMAKE_VERSION}")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# TODO clean this https://stackoverflow.com/questions/32411963/why-is-cmake-file-glob-evil
file(GLOB_RECURSE Sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp") 

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/tools/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(Tesselatior LANGUAGES CXX VERSION 0.0.1)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

find_package(Stb REQUIRED)


find_package(assimp CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)

find_package(glfw3 CONFIG REQUIRED)

find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)

set(GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only")

add_executable(${PROJECT_NAME} ${Sources})
target_link_libraries(${PROJECT_NAME} PRIVATE 
  assimp::assimp 
  glfw 
  GLEW::GLEW 
  glm::glm 
  EnTT::EnTT 
  imgui::imgui)

target_include_directories(${PROJECT_NAME} PRIVATE ${Stb_INCLUDE_DIR})
# Dirty way
# TODO improve (for install for example...)
add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/models ${CMAKE_CURRENT_BINARY_DIR}/models
)
# These should be in a shaders folder
add_custom_target(copy_shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders ${CMAKE_CURRENT_BINARY_DIR}/shaders
)
# These? it should depend of the model but maybe i have to clean the code because i do not see it used
add_custom_target(copy_textures
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/src/texture ${CMAKE_CURRENT_BINARY_DIR}
)
add_dependencies(${PROJECT_NAME} copy_assets copy_shaders copy_textures)
