#========================================================================================

include_directories( ${CMAKE_CURRENT_LIST_DIR} )

#========================================================================================

include( ${CMAKE_CURRENT_LIST_DIR}/sl/sl.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/sa_pdu/sa_pdu.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/crypt/crypt.cmake )

#========================================================================================

set( LIB_SL
     ${SL}
     ${SA_PDU}
     ${CRYPT} )

#========================================================================================