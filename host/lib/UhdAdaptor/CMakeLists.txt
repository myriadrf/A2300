########################################################################
# Conditionally configure the A2300 UHD Adapter support
########################################################################
LIBUHD_REGISTER_COMPONENT("A2300UhdAdapter" ENABLE_A2300UhdAdapter ON "ENABLE_UHD;ENABLE_USB" OFF)

IF(ENABLE_A2300UhdAdapter)

include_directories(
    ${UHD_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${LIBUSB_INCLUDE_DIRS}
)

list(APPEND A2300UhdAdapter_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/a2300_impl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/a2300_iface.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/a2300_io_impl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DciProperty.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Uhd/usrp/usrp1/soft_time_ctrl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Uhd/usrp/usrp1/dsp_core_a2300.cpp
)

# ${CMAKE_CURRENT_SOURCE_DIR}/a2300_uart.cpp

add_library(A2300UhdAdapter SHARED ${A2300UhdAdapter_sources})

TARGET_LINK_LIBRARIES(A2300UhdAdapter Dci)

if (ENABLE_UHD_INTERNAL)
   TARGET_LINK_LIBRARIES(A2300UhdAdapter uhd)
else()
   TARGET_LINK_LIBRARIES(A2300UhdAdapter ${UHD_LIBRARIES})
endif()

TARGET_LINK_LIBRARIES(A2300UhdAdapter
	${Boost_LIBRARIES}
	${LIBUSB_LIBRARIES}
)

set_target_properties(A2300UhdAdapter PROPERTIES
    VERSION ${${PROJECT_NAME}_VERSION_FULL}
)

if (UNIX)
    set_target_properties(A2300UhdAdapter PROPERTIES
        SOVERSION ${${PROJECT_NAME}_VERSION_PATCH}
    )
endif()

if (APPLE)
        set_target_properties(A2300UhdAdapter PROPERTIES
	        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib$"
	)
endif()

ENDIF()
