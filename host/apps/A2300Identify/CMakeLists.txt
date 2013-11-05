##############################################################
# Common additions
##############################################################


INCLUDE_DIRECTORIES(
    ${UHD_INCLUDE_DIRS}
    ${LIBUSB_INCLUDE_DIRS}
)

##############################################################
# Register "A2300 Identify App" component,
# dependends on A2300USB; on by default.
##############################################################

LIBUHD_REGISTER_COMPONENT("A2300 Identify App"
    ENABLE_A2300_IDENTIFY_APP ON "ENABLE_A2300USB" OFF)

IF(ENABLE_A2300_IDENTIFY_APP)

##############################################################
# Create executables
##############################################################

    ADD_EXECUTABLE(A2300Identify main.cpp)

##############################################################
# Conditionally embed the A2300 UHD Adapter
##############################################################

    TARGET_LINK_LIBRARIES(A2300Identify A2300Usb)

##############################################################
# Install information
##############################################################

    UHD_INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/A2300Identify
        DESTINATION ${RUNTIME_DIR} COMPONENT utilities)

ENDIF()
