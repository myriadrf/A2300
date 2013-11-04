##############################################################
# Replace LIBUSB_INC_DIR in this file
##############################################################

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/libusb_ext.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/libusb_ext.h
@ONLY)

##############################################################
# Install information
##############################################################

UHD_INSTALL(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/libusb_ext.h
    ${CMAKE_CURRENT_SOURCE_DIR}/A2300InterfaceDefs.h
    ${CMAKE_CURRENT_SOURCE_DIR}/A2300_Defs.h
    ${CMAKE_CURRENT_SOURCE_DIR}/BulkDataPort.h
    ${CMAKE_CURRENT_SOURCE_DIR}/UsbDevice.h
    DESTINATION ${INCLUDE_DIR}/A2300/A2300
    COMPONENT headers
)
