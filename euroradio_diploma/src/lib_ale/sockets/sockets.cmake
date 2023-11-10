#========================================================================================

include_directories( ${CMAKE_CURRENT_LIST_DIR} )

set( SOCKETS
     ${CMAKE_CURRENT_LIST_DIR}/tcp_socket.h
     ${CMAKE_CURRENT_LIST_DIR}/tcp_socket.cpp
     ${CMAKE_CURRENT_LIST_DIR}/socket_pair.h
     ${CMAKE_CURRENT_LIST_DIR}/socket_pair.cpp )

#========================================================================================