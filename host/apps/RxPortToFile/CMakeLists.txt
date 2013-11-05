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
# Add definition for OS type
##############################################################

#message("CMAKE_SYSTEM_NAME is '${CMAKE_SYSTEM_NAME}'")
    #ADD_DEFINITIONS(-D${CMAKE_SYSTEM_NAME})

##############################################################
# Create executables
##############################################################

    ADD_EXECUTABLE(RxPortToFile RxPortToFile.cpp DciProperty.cpp)

##############################################################
# Conditionally embed the A2300 UHD Adapter
##############################################################

    TARGET_LINK_LIBRARIES(RxPortToFile A2300Usb)

##############################################################
# Install information
##############################################################

    UHD_INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/RxPortToFile
        DESTINATION ${RUNTIME_DIR} COMPONENT utilities)

ENDIF()
