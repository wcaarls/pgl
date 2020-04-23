project(primgl)
cmake_minimum_required(VERSION 2.8.3)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -Wall -Wextra")

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
  message("-- Set default build type: ${CMAKE_BUILD_TYPE}")
else()
  message("-- Build type: ${CMAKE_BUILD_TYPE}")
endif()

# Find GLFW
find_package(OpenGL)
find_package(GLFW)
if(OPENGL_FOUND AND GLFW_FOUND)
  add_executable(example ${CMAKE_CURRENT_SOURCE_DIR}/src/example.cpp)

  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${GLFW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
  target_link_libraries(example ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES})
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/pgl DESTINATION include FILES_MATCHING PATTERN "*.h")
