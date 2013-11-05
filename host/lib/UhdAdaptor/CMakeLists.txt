##############################################################
# Register A2300 UHD Adapter: to be ON by default, and depend on UHD
# and USB being enabled.
##############################################################

LIBUHD_REGISTER_COMPONENT("A2300 Uhd Adapter"
    ENABLE_A2300UhdAdapter ON "ENABLE_UHD;ENABLE_LIBUSB" OFF)

##############################################################
# Conditionally configure the A2300 UHD Adapter
##############################################################

IF(ENABLE_A2300UhdAdapter)

    INCLUDE_DIRECTORIES(
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${UHD_INCLUDE_DIRS}
        ${LIBUSB_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
    )

    SET(A2300UhdAdapter_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/a2300_iface.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/a2300_impl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/a2300_io_impl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DciProperty.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DspConfig.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RadioPeripheral.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RfConfig.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Uhd/usrp/usrp1/soft_time_ctrl.cpp
    )

    ADD_LIBRARY(A2300UhdAdapter SHARED ${A2300UhdAdapter_sources})

    # Dci is always internal to this build, so include it first

    TARGET_LINK_LIBRARIES(A2300UhdAdapter Dci)

    # UHD may or not be internal to this build, so include it after all
    # other internal libraries

    IF(ENABLE_UHD_INTERNAL)
        TARGET_LINK_LIBRARIES(A2300UhdAdapter uhd)
    ELSE()
        TARGET_LINK_LIBRARIES(A2300UhdAdapter ${UHD_LIBRARIES})
    ENDIF()

    # all external libraries go last

    IF(NOT ENABLE_UHD_INTERNAL)

        # have to explicitly link to boost and libusb only if UHD is
        # not an internal build.

        TARGET_LINK_LIBRARIES(A2300UhdAdapter
            ${Boost_LIBRARIES}
            ${LIBUSB_LIBRARIES}
        )

    ENDIF()

    # set library properties

    SET_TARGET_PROPERTIES(A2300UhdAdapter PROPERTIES
        VERSION ${${PROJECT_NAME}_VERSION_FULL}
    )

    IF(UNIX)
        SET_TARGET_PROPERTIES(A2300UhdAdapter PROPERTIES
            SOVERSION ${${PROJECT_NAME}_VERSION_PATCH}
        )
    ENDIF(UNIX)

    IF(APPLE)
        SET_TARGET_PROPERTIES(A2300UhdAdapter PROPERTIES
            INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/${LIBRARY_DIR}"
        )
    ENDIF(APPLE)

    # Install information

    INSTALL(TARGETS A2300UhdAdapter
        LIBRARY DESTINATION ${UHD_MODULE_DIR} COMPONENT libraries # .so file
        ARCHIVE DESTINATION ${UHD_MODULE_DIR} COMPONENT libraries # .lib file
        RUNTIME DESTINATION ${UHD_MODULE_DIR} COMPONENT libraries # .dll file
    )

ENDIF(ENABLE_A2300UhdAdapter)
