#========================================================================================

set( VLIBS_DIR ${CMAKE_CURRENT_LIST_DIR}/vlibs )

#========================================================================================

include_directories( ${VLIBS_DIR} )

#========================================================================================

include( ${VLIBS_DIR}/vsettings/vsettings.cmake       )
include( ${VLIBS_DIR}/vbyte_buffer/vbyte_buffer.cmake )
include( ${VLIBS_DIR}/vmacroses/vmacroses.cmake       )
include( ${VLIBS_DIR}/vchrono/vchrono.cmake           )
include( ${VLIBS_DIR}/vgit/vgit.cmake                 )
include( ${VLIBS_DIR}/vapplication/vapplication.cmake )
include( ${VLIBS_DIR}/vnetwork/vnetwork.cmake         )
include( ${VLIBS_DIR}/vsignal/vsignal.cmake           )
include( ${VLIBS_DIR}/vtimer/vtimer.cmake             )
include( ${VLIBS_DIR}/vposix/vposix.cmake             )
include( ${VLIBS_DIR}/vlog/vlog.cmake                 )
include( ${VLIBS_DIR}/vlog/vfilelog.cmake             )
include( ${VLIBS_DIR}/vzcm/vzcm.cmake                 )

#========================================================================================

set( V_LIBS
     ${V_HEADERS}
     ${V_SOURCES} )

#========================================================================================