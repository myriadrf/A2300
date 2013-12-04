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
# Common additions
##############################################################

INCLUDE_DIRECTORIES(
    ${UHD_INCLUDE_DIRS}
    ${LIBUSB_INCLUDE_DIRS}
)

##############################################################
# Register "RxPortToFile" component,
# dependends on A2300USB; on by default.
##############################################################

LIBUHD_REGISTER_COMPONENT("RxPortToFile App"
    ENABLE_RXPORTTOFILE_APP ON "ENABLE_A2300USB" OFF)

IF(ENABLE_RXPORTTOFILE_APP)

##############################################################
# Create executables
##############################################################

    ADD_EXECUTABLE(RxPortToFile RxPortToFile.cpp)

##############################################################
# Conditionally embed the A2300 UHD Adapter
##############################################################

    TARGET_LINK_LIBRARIES(RxPortToFile A2300Usb)

##############################################################
# On Linux, link with the real time clock
##############################################################

    IF(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        FIND_LIBRARY(RT_LIBRARIES rt
            PATHS /${LIBRARY_DIR} /usr/${LIBRARY_DIR}
        )
        IF(RT_LIBRARIES)
            SET(CMAKE_REQUIRED_LIBRARIES ${RT_LIBRARIES})
        ENDIF()

        INCLUDE(FindPackageHandleStandardArgs)
        FIND_PACKAGE_HANDLE_STANDARD_ARGS(RT DEFAULT_MSG
            RT_LIBRARIES)
        MARK_AS_ADVANCED(RT_LIBRARIES)
        TARGET_LINK_LIBRARIES(RxPortToFile ${RT_LIBRARIES})
    ENDIF()

##############################################################
# Install information
##############################################################

    UHD_INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/RxPortToFile
        DESTINATION ${RUNTIME_DIR} COMPONENT utilities)

ENDIF()
