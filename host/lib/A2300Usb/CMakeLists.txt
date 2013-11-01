##############################################################
# Register A2300 Usb: no dependencies, to be ON by default.
##############################################################

LIBUHD_REGISTER_COMPONENT("A2300 Usb" ENABLE_A2300USB ON "" OFF)

##############################################################
# Conditionally configure A2300 Usb.
##############################################################

IF(ENABLE_A2300USB)

    INCLUDE_DIRECTORIES(
        ${LIBUSB_INCLUDE_DIRS}
    )

    SET(A2300Usb_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/PortBase.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/UsbDevice_core.cpp
    )

    IF(WIN32)
        LIST(APPEND A2300Usb_sources
            ${CMAKE_CURRENT_SOURCE_DIR}/BulkDataPort_cypressUsb.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/UsbDevice_cypressusb.cpp
        )
    ELSEIF(UNIX AND ENABLE_USB)
        LIST(APPEND A2300Usb_sources
            ${CMAKE_CURRENT_SOURCE_DIR}/BulkDataPort_libusb.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/UsbDevice_libusb.cpp
        )
    ELSE()
        MESSAGE(FATAL_ERROR "A2300Usb requires a USB framework to function; none found.")
    ENDIF()

    ADD_LIBRARY(A2300Usb SHARED ${A2300Usb_sources})

    # Dci is always internal to this build, so include it first

    TARGET_LINK_LIBRARIES(A2300Usb Dci)

    # all external libraries go last

    TARGET_LINK_LIBRARIES(A2300Usb
        ${LIBUSB_LIBRARIES}
    )

    # set library properties

    SET_TARGET_PROPERTIES(A2300Usb PROPERTIES
        VERSION ${${PROJECT_NAME}_VERSION_FULL}
    )

    IF(UNIX)
        SET_TARGET_PROPERTIES(A2300Usb PROPERTIES
            SOVERSION ${${PROJECT_NAME}_VERSION_PATCH}
        )
    ENDIF(UNIX)

    IF(APPLE)
        SET_TARGET_PROPERTIES(A2300Usb PROPERTIES
            INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib$"
        )
    ENDIF(APPLE)

ENDIF(ENABLE_A2300USB)
