##############################################################
# Find libusb1 and libuhd
##############################################################

if (ENABLE_UHD_INTERNAL)

   add_subdirectory(Uhd)
   set(ENABLE_UHD ${ENABLE_LIBUHD})

else()

   FIND_PACKAGE(USB1 REQUIRED)

   LIBUHD_REGISTER_COMPONENT("USB" ENABLE_USB ON
      "LIBUSB_FOUND" OFF)

   PKG_CHECK_MODULES(PC_UHD QUIET uhd)

   FIND_PATH(UHD_INCLUDE_DIRS
      uhd/version.hpp
      HINT ${PC_UHD_INCLUDEDIR}
      NO_DEFAULT_PATH
      NO_CMAKE_ENVIRONMENT_PATH
      NO_CMAKE_PATH
      NO_SYSTEM_ENVIRONMENT_PATH
      NO_CMAKE_SYSTEM_PATH
      NO_CMAKE_FIND_ROOT_PATH
    )

    FIND_LIBRARY(UHD_LIBRARIES
      uhd
      HINTS ${PC_UHD_LIBDIR}
      NO_DEFAULT_PATH
      NO_CMAKE_ENVIRONMENT_PATH
      NO_CMAKE_PATH
      NO_SYSTEM_ENVIRONMENT_PATH
      NO_CMAKE_SYSTEM_PATH
      NO_CMAKE_FIND_ROOT_PATH
   )

   # FIXME: should really check that a header and some functions work ...

   if(UHD_INCLUDE_DIRS)
       set(CMAKE_REQUIRED_INCLUDES ${UHD_INCLUDE_DIRS})
   endif()
   if(UHD_LIBRARIES)
       set(CMAKE_REQUIRED_LIBRARIES ${UHD_LIBRARIES})
   endif()

    include(FindPackageHandleStandardArgs)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(UHD DEFAULT_MSG UHD_LIBRARIES UHD_INCLUDE_DIRS)
    MARK_AS_ADVANCED(UHD_INCLUDE_DIRS UHD_LIBRARIES)

   if(UHD_FOUND)
      set(LIBUHD_FOUND ON)
   else()
      set(LIBUHD_FOUND OFF)
   endif()

   LIBUHD_REGISTER_COMPONENT("UHD" ENABLE_UHD ON
   "LIBUHD_FOUND;LIBUSB_FOUND" OFF)

endif()

##############################################################
# Add subdirectories
##############################################################

add_subdirectory(UhdAdaptor)
#add_subdirectory(A2300Usb)
