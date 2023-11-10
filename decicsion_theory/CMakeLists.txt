#========================================================================================

cmake_minimum_required(VERSION 3.17)

#========================================================================================

project(decicsion_theory)

#========================================================================================

set(CMAKE_CXX_STANDARD 20)

#========================================================================================

include_directories( ${CMAKE_CURRENT_LIST_DIR}/include/)

#========================================================================================

add_executable( ${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp      )
add_executable( mercedes        ${CMAKE_CURRENT_LIST_DIR}/src/mercedes.cpp  )
add_executable( two_years       ${CMAKE_CURRENT_LIST_DIR}/src/two_years.cpp )

#========================================================================================