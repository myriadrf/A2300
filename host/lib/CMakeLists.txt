##############################################################
# Register "UHD Internal" component, no dependencies; off by default.
# If ON, then build and use the internal UHD repository; if off, use a
# pre-installed version as found by pkg-config
##############################################################

LIBUHD_REGISTER_COMPONENT("UHD Internal"
    ENABLE_UHD_INTERNAL OFF "" OFF)

##############################################################
# Configure or find UHD, depending on user's selection
##############################################################

IF(ENABLE_UHD_INTERNAL)

    # internal UHD build: just add the subdiretory 
    ADD_SUBDIRECTORY(Uhd)

ELSE()

    # External UHD; have to find and verify.
    # Make sure uhd.pc is in the PKG_CONFIG_PATH; fail if not found.

    SET(PC_PATH $ENV{PKG_CONFIG_PATH})

    FIND_PATH(UHD_PC_DIR uhd.pc
        HINTS ${PC_PATH}
            ${CMAKE_INSTALL_PREFIX}/${LIBRARY_DIR}/pkgconfig
        PATHS /usr/lib${LIB_SUFFIX}/pkgconfig
            /lib${LIB_SUFFIX}/pkgconfig
            /usr/local/lib${LIB_SUFFIX}/pkgconfig
            /opt/local/lib${LIB_SUFFIX}/pkgconfig
    )

    IF(NOT UHD_PC_DIR)
        MESSAGE(FATAL_ERROR "uhd.pc required (for now) to compile A2300UhdAdapter.")
    ENDIF()

    SET(PC_PATH "${UHD_PC_DIR}:${PC_PATH}")
    SET(ENV{PKG_CONFIG_PATH} ${PC_PATH})

    # Find UHD using pkg-config

    PKG_CHECK_MODULES(PC_UHD QUIET uhd)

    # Verify the includes path returned by pkg-config

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

    # Verify the library path returned by pkg-config

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

    IF(UHD_INCLUDE_DIRS)
        SET(CMAKE_REQUIRED_INCLUDES ${UHD_INCLUDE_DIRS})
    ENDIF()
    IF(UHD_LIBRARIES)
        SET(CMAKE_REQUIRED_LIBRARIES ${UHD_LIBRARIES})
    ENDIF()

    INCLUDE(FindPackageHandleStandardArgs)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS(UHD DEFAULT_MSG
        UHD_LIBRARIES UHD_INCLUDE_DIRS)
    MARK_AS_ADVANCED(UHD_INCLUDE_DIRS UHD_LIBRARIES)

    IF(UHD_FOUND)
        SET(LIBUHD_FOUND ON)
    ELSE()
        SET(LIBUHD_FOUND OFF)
    ENDIF()

    LIBUHD_REGISTER_COMPONENT("UHD" ENABLE_UHD ON
        "LIBUHD_FOUND;LIBUSB_FOUND" OFF)

ENDIF()

##############################################################
# Add subdirectories
##############################################################

ADD_SUBDIRECTORY(A2300Usb)
ADD_SUBDIRECTORY(UhdAdaptor)
