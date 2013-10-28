##############################################################
# common additions for subdirectory applications
##############################################################

LINK_LIBRARIES(A2300UhdAdapter)

if (ENABLE_UHD_INTERNAL)
   LINK_LIBRARIES(uhd)
else()
   LINK_LIBRARIES(${UHD_LIBRARIES})
endif()

include_directories(
    ${UHD_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

##############################################################
# Add subdirectories
##############################################################

add_subdirectory(uhd_A2300_probe_shared)
