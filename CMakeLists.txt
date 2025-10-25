cmake_minimum_required(VERSION 3.16)

project(SolarSystem LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch GLFW (works with MSVC and MinGW)
include(FetchContent)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.9
)
FetchContent_MakeAvailable(glfw)

find_package(OpenGL REQUIRED)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/Solar System")
set(DEPS_DIR "${CMAKE_SOURCE_DIR}/Dependencies")

add_executable(solar_system
  "${SRC_DIR}/Actor.cpp"
  "${SRC_DIR}/Camera.cpp"
  "${SRC_DIR}/Game.cpp"
  "${SRC_DIR}/main.cpp"
  "${SRC_DIR}/Mesh.cpp"
  "${SRC_DIR}/objload.cpp"
  "${SRC_DIR}/Planet.cpp"
  "${SRC_DIR}/ShaderProgram.cpp"
  "${SRC_DIR}/Texture.cpp"
  "${SRC_DIR}/stb_image.cpp"
  "${SRC_DIR}/Window.cpp"
  "${DEPS_DIR}/GLAD/src/glad.c"
)

# Keep the produced exe name and location familiar
set_target_properties(solar_system PROPERTIES
  OUTPUT_NAME "Solar System"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Solar System"
)

target_include_directories(solar_system PRIVATE
  "${SRC_DIR}"
  "${DEPS_DIR}/GLAD/include"
  "${DEPS_DIR}/GLM"
)

target_link_libraries(solar_system PRIVATE glfw OpenGL::GL)

# Copy Resources next to the runtime dir so the app's "..\\Resources\\..." paths work
add_custom_command(
  TARGET solar_system POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_SOURCE_DIR}/Resources"
          "${CMAKE_BINARY_DIR}/Resources"
)


