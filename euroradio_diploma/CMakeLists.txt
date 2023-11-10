#========================================================================================

project( euroradio LANGUAGES CXX )

#========================================================================================

cmake_minimum_required( VERSION 3.2 )

#========================================================================================

set( CMAKE_CXX_STANDARD 11                               )
set( CMAKE_CXX_STANDARD_REQUIRED ON                      )
set( CMAKE_EXPORT_COMPILE_COMMANDS ON                    )
set( LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR}/libs/ )
set( EURORADIO_DIR ${CMAKE_CURRENT_SOURCE_DIR}           )

#========================================================================================

include_directories( ${CMAKE_SOURCE_DIR} )

#========================================================================================

include( ${CMAKE_CURRENT_LIST_DIR}/external/external.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/src/src.cmake           )

#========================================================================================

add_library( ${PROJECT_NAME} SHARED
             ${EXTERNAL}
             ${SRC} )

target_link_libraries( ${PROJECT_NAME}
                       ${V_LIBRARIES}
                       -lstdc++fs
                       -ldl )

#========================================================================================