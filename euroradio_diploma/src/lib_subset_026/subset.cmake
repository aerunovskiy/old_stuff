#========================================================================================

include_directories( ${CMAKE_CURRENT_LIST_DIR} )

#========================================================================================

include( ${CMAKE_CURRENT_LIST_DIR}/subset_026/subset_026.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/interfaces/interfaces.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/messages/messages.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/binary_message/binary_message.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/parsers/parsers.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/channel_state/channel_state.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/connection_table/connection_table.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/lag_controller/lag_controller.cmake )

#========================================================================================

set( LIB_SUBSET
     ${SUBSET_026}
     ${INTERFACES}
     ${MESSAGES}
     ${BINARY_MESSAGE}
     ${PARSERS}
     ${CHANNEL_STATE}
     ${CONNECTION_TABLE}
     ${LAG_CONTROLLER} )

#========================================================================================