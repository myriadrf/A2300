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
# Setup common System library as static
##############################################################

SET(A2300Threading_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/Mutex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Thread_Linux.cpp
)

ADD_LIBRARY(A2300Threading STATIC ${A2300Threading_sources})

##############################################################
# Install information
##############################################################

INSTALL(TARGETS A2300Threading
    LIBRARY DESTINATION ${LIBRARY_DIR} COMPONENT libraries # .so file
    ARCHIVE DESTINATION ${LIBRARY_DIR} COMPONENT libraries # .lib file
    RUNTIME DESTINATION ${RUNTIME_DIR} COMPONENT libraries # .dll file
)

UHD_INSTALL(FILES
    Mutex.hpp
    Thread.hpp
    ThreadSafeQueue.hpp
    DESTINATION ${INCLUDE_DIR}/A2300/Threading
    COMPONENT headers
)
