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
# Register "A2300 Calibrate App" component,
# dependends on A2300USB; on by default.
##############################################################

LIBUHD_REGISTER_COMPONENT("A2300 A2300 Sensor Demo"
    ENABLE_A2300_SENSORDEMO_APP ON "ENABLE_A2300USB" OFF)

IF(ENABLE_A2300_SENSORDEMO_APP)

##############################################################
# Create executables
##############################################################

    ADD_EXECUTABLE(A2300SensorDemo A2300SensorDemo.cpp)

##############################################################
# Conditionally embed the A2300 UHD Adapter
##############################################################

    TARGET_LINK_LIBRARIES(A2300SensorDemo A2300Usb)

##############################################################
# Install information
##############################################################

    UHD_INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/A2300SensorDemo
        DESTINATION ${RUNTIME_DIR} COMPONENT utilities)

ENDIF()
