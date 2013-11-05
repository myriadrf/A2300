##############################################################
# Find boost
##############################################################

IF(UNIX AND EXISTS "/usr/lib64")
    LIST(APPEND BOOST_LIBRARYDIR "/usr/lib64") #fedora 64-bit fix
ENDIF(UNIX AND EXISTS "/usr/lib64")
SET(Boost_ADDITIONAL_VERSIONS
    "1.35.0" "1.35" "1.36.0" "1.36" "1.37.0" "1.37" "1.38.0" "1.38" "1.39.0" "1.39"
    "1.40.0" "1.40" "1.41.0" "1.41" "1.42.0" "1.42" "1.43.0" "1.43" "1.44.0" "1.44"
    "1.45.0" "1.45" "1.46.0" "1.46" "1.47.0" "1.47" "1.48.0" "1.48" "1.49.0" "1.49"
    "1.50.0" "1.50" "1.51.0" "1.51" "1.52.0" "1.52" "1.53.0" "1.53" "1.54.0" "1.54"
    "1.55.0" "1.55" "1.56.0" "1.56" "1.57.0" "1.57" "1.58.0" "1.58" "1.59.0" "1.59"
    "1.60.0" "1.60" "1.61.0" "1.61" "1.62.0" "1.62" "1.63.0" "1.63" "1.64.0" "1.64"
    "1.65.0" "1.65" "1.66.0" "1.66" "1.67.0" "1.67" "1.68.0" "1.68" "1.69.0" "1.69"
)
FIND_PACKAGE(Boost "1.35" COMPONENTS system thread program_options)

IF(NOT Boost_FOUND)
    MESSAGE(FATAL_ERROR "Boost required to compile A2300UhdAdapter and related projects")
ENDIF()

ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK)

##############################################################
# Find libusb1
##############################################################

FIND_PACKAGE(USB1)

LIBUHD_REGISTER_COMPONENT("LIBUSB" ENABLE_LIBUSB ON
    "LIBUSB_FOUND" OFF)

IF(LIBUSB_FOUND)

    # we use <@LIBUSB_INC_DIR@/libusb.h>, so have to define both
    # header include path and LIBUSB_INC_DIR, taking from
    # LIBUSB_INCLUDE_DIRS

    STRING(FIND "${LIBUSB_INCLUDE_DIRS}" "/" NDX REVERSE)
    MATH(EXPR NDXp1 "${NDX}+1")
    STRING(SUBSTRING "${LIBUSB_INCLUDE_DIRS}" ${NDXp1} -1 LIBUSB_INC_DIR)
    STRING(SUBSTRING "${LIBUSB_INCLUDE_DIRS}" 0 ${NDX} LIBUSB_INCLUDE_DIRS)
    UNSET(NDX)
    UNSET(NDXp1)

    # macro used for compiling

    ADD_DEFINITIONS(-DHAVE_LIBUSB)

ELSE()

    MESSAGE(FATAL_ERROR "LIBUSB1 required to compile A2300UhdAdapter and related projects")

ENDIF()

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

IF(NOT ENABLE_UHD_INTERNAL)

    # External UHD; have to find and verify.
    # Make sure uhd.pc is in the PKG_CONFIG_PATH; fail if not found.

    SET(PC_PATH $ENV{PKG_CONFIG_PATH})

    FIND_PATH(UHD_PC_DIR uhd.pc
        HINTS ${PC_PATH}
            ${CMAKE_INSTALL_PREFIX}/${LIBRARY_DIR}/pkgconfig
        PATHS /usr/${LIBRARY_DIR}/pkgconfig
            /${LIBRARY_DIR}/pkgconfig
            /usr/local/${LIBRARY_DIR}/pkgconfig
            /opt/local/${LIBRARY_DIR}/pkgconfig
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

    # FIXME: should really check that a header and some functions
    # work, for example, USB support in the already-installed UHD.

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

    LIBUHD_REGISTER_COMPONENT("UHD" ENABLE_UHD ON
        "UHD_FOUND;LIBUSB_FOUND" OFF)

ENDIF()

##############################################################
# Add subdirectories
##############################################################

ADD_SUBDIRECTORY(extern)
ADD_SUBDIRECTORY(include)
ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(gnuradio)
ADD_SUBDIRECTORY(apps)
