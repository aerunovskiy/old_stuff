#========================================================================================

include_directories( ${CMAKE_CURRENT_LIST_DIR} )

#========================================================================================

include(${CMAKE_CURRENT_LIST_DIR}/lib_ale/lib_ale.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/lib_sl/lib_sl.cmake)
include( ${CMAKE_CURRENT_LIST_DIR}/lib_subset_026/subset.cmake )

#========================================================================================

set( SRC
     ${LIB_ALE}
     ${LIB_SL}
     ${LIB_SUBSET} )

#========================================================================================