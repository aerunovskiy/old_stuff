#========================================================================================

include_directories( ${CMAKE_CURRENT_LIST_DIR} )

#========================================================================================

include( ${CMAKE_CURRENT_LIST_DIR}/ale/ale.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/ale_controller/ale_controller.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/alepkt/alepkt.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/basic_ale/basic_ale.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/sockets/sockets.cmake )

#========================================================================================

set( LIB_ALE
     ${ALE}
     ${ALE_CONTROLLER}
     ${ALEPKT}
     ${BASIC_ALE}
     ${SOCKETS} )

#========================================================================================