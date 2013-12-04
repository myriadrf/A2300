##############################################################
# Copyright(c) 2013 Loctronix Corporation
# http://www.loctronix.com
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
##############################################################

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
        ${CMAKE_CURRENT_SOURCE_DIR}/BitUtil.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ConfigDevice.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ConfigDsp.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ConfigRf.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/TransportDci.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/PortBase.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/UsbDevice_core.cpp
    )

    IF(WIN32)
        LIST(APPEND A2300Usb_sources
            ${CMAKE_CURRENT_SOURCE_DIR}/BulkDataPort_cypressUsb.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/UsbDevice_cypressusb.cpp
        )
    ELSEIF(UNIX AND ENABLE_LIBUSB)
        LIST(APPEND A2300Usb_sources
            ${CMAKE_CURRENT_SOURCE_DIR}/BulkDataPort_libusb.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/UsbDevice_libusb.cpp
        )
    ELSE()
        MESSAGE(FATAL_ERROR "A2300Usb requires a USB framework to function; none found.")
    ENDIF()

    ADD_LIBRARY(A2300Usb SHARED ${A2300Usb_sources})

    # A2300Dci is always internal to this build, so include it first

    TARGET_LINK_LIBRARIES(A2300Usb A2300Dci)

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
            INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/${LIBRARY_DIR}"
        )
    ENDIF(APPLE)

    # Install information

    INSTALL(TARGETS A2300Usb
        LIBRARY DESTINATION ${LIBRARY_DIR} COMPONENT libraries # .so file
        ARCHIVE DESTINATION ${LIBRARY_DIR} COMPONENT libraries # .lib file
        RUNTIME DESTINATION ${RUNTIME_DIR} COMPONENT libraries # .dll file
    )

ENDIF(ENABLE_A2300USB)
