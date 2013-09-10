#
# Copyright 2012-2013 Ettus Research LLC
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

########################################################################
# This file included, use CMake directory variables
########################################################################

########################################################################
# Conditionally configure the A2300 support
########################################################################
LIBUHD_REGISTER_COMPONENT("A2300" ENABLE_A2300 ON "ENABLE_LIBUHD;ENABLE_USB" OFF)

IF(ENABLE_A2300)
    LIBUHD_APPEND_SOURCES(
        ${CMAKE_CURRENT_SOURCE_DIR}/a2300_impl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/a2300_iface.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/a2300_io_impl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/a2300_uart.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Uhd/usrp/usrp1/soft_time_ctrl.cpp
    )
ENDIF(ENABLE_A2300)
