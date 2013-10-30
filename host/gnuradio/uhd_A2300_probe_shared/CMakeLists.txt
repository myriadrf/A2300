##############################################################
# Common additions
##############################################################

INCLUDE_DIRECTORIES(
    ${UHD_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

##############################################################
# Register "Embed A2300 Uhd Adapter" component,
# no dependencies; off by default
##############################################################

LIBUHD_REGISTER_COMPONENT("Embed A2300 Uhd Adapter"
    ENABLE_EMBED_A2300_UHD_ADAPTER OFF "" OFF)

##############################################################
# Create executables
##############################################################

ADD_EXECUTABLE(uhd_usrp_A2300_probe uhd_usrp_A2300_probe.cpp)

##############################################################
# Conditionally embed the A2300 UHD Adapter
##############################################################

IF(ENABLE_EMBED_A2300_UHD_ADAPTER)

    # A2300UhdAdapter links to boost and uhd, so no need to include
    # them further here
    TARGET_LINK_LIBRARIES(uhd_usrp_A2300_probe A2300UhdAdapter)

ELSE()

    # UHD may or not be internal to this build, so include it after
    # all other internal libraries and before external ones

     IF(ENABLE_UHD_INTERNAL)

        TARGET_LINK_LIBRARIES(uhd_usrp_A2300_probe uhd)

     ELSE()

        TARGET_LINK_LIBRARIES(uhd_usrp_A2300_probe
            ${UHD_LIBRARIES} ${Boost_LIBRARIES})

     ENDIF()

ENDIF()
