cmake_minimum_required(VERSION 3.12)
project(Figures_Optimal_Route)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(Figures_Optimal_Route
        polygons.cpp
        polygons.hpp)

include_directories( ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

target_link_libraries(Figures_Optimal_Route ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )

include_directories(.)

