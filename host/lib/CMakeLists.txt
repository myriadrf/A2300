##############################################################
# Find libusb1 and libuhd
##############################################################

if (ENABLE_UHD_INTERNAL)

   add_subdirectory(Uhd)

else()

   FIND_PACKAGE(USB1 REQUIRED)

   LIBUHD_REGISTER_COMPONENT("USB" ENABLE_USB ON
      "LIBUSB_FOUND" OFF)

   PKG_CHECK_MODULES(UHD QUIET uhd)

   FIND_PATH(UHD_INCLUDE_DIRS
      NAMES uhd.h
      HINTS $ENV{UHD_DIR}/include ${PC_UHD_INCLUDEDIR}
      PATHS /usr/local/include/uhd /usr/local/include
      /usr/include/uhd /usr/include
      ${CMAKE_INSTALL_PREFIX}/include/uhd

      #non-conforming naming convention,
      #backwards compatible with old script
      ${UHD_INCLUDE_DIR}
    )

    FIND_LIBRARY(UHD_LIBRARIES
      NAMES libuhd
      HINTS $ENV{UHD_DIR}/lib ${PC_UHD_LIBDIR}
      PATHS /usr/local/lib /usr/lib ${CMAKE_INSTALL_PREFIX}/lib
   )

   if(UHD_FOUND)
      set(ENABLE_LIBUHD ON)
   else()
      set(ENABLE_LIBUHD OFF)
   endif()

   LIBUHD_REGISTER_COMPONENT("UHD" ENABLE_UHD ON
   "ENABLE_LIBUHD;LIBUSB_FOUND" OFF)

endif()

##############################################################
# Add subdirectories
##############################################################

add_subdirectory(UhdAdaptor)
#add_subdirectory(A2300Usb)
