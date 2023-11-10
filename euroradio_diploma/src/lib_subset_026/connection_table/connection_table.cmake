#========================================================================================

include_directories( ${CMAKE_CURRENT_LIST_DIR} )

set( CONNECTION_TABLE
     ${CMAKE_CURRENT_LIST_DIR}/connection_table.h
     ${CMAKE_CURRENT_LIST_DIR}/connection_table.cpp
     ${CMAKE_CURRENT_LIST_DIR}/pending_cs_init.h
     ${CMAKE_CURRENT_LIST_DIR}/pending_cs_init.cpp
     ${CMAKE_CURRENT_LIST_DIR}/connection_parameters.cpp
     ${CMAKE_CURRENT_LIST_DIR}/watchdog_timers.cpp )

#========================================================================================