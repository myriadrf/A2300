##############################################################
# Most of this file is copied directly from the top-level
# uhd/hosts/CMakeLists.txt file; changes are made to allow for the
# relative build needed to include UHD within the A2300 project
##############################################################

##############################################################
# Setup CMake Modules path
##############################################################

SET(UHD_REL_DIR uhd/host)
LIST(APPEND CMAKE_MODULE_PATH
${CMAKE_CURRENT_SOURCE_DIR}/${UHD_REL_DIR}/cmake/Modules)

##############################################################
# Include the UHD version directly; it is indirect with the UHD build
##############################################################

INCLUDE(UHDVersion)

##############################################################
# Add libusb full header path
##############################################################

INCLUDE_DIRECTORIES(${LIBUSB_INCLUDE_DIRS}/${LIBUSB_INC_DIR})

##############################################################
# Define install directories
##############################################################

# when the library suffix should be 64 (applies to redhat linux family)
IF(NOT DEFINED LIB_SUFFIX AND REDHAT AND CMAKE_SYSTEM_PROCESSOR MATCHES "64$")
    SET(LIB_SUFFIX 64)
ENDIF()
SET(LIB_SUFFIX ${LIB_SUFFIX} CACHE STRING "lib directory suffix")
SET(RUNTIME_DIR bin)
SET(LIBRARY_DIR lib${LIB_SUFFIX})
SET(INCLUDE_DIR include)
SET(PKG_DATA_DIR share/uhd)
IF(NOT DEFINED PKG_LIB_DIR)
    SET(PKG_LIB_DIR ${LIBRARY_DIR}/uhd)
ENDIF()
SET(PKG_DOC_DIR share/doc/uhd)
SET(PKG_MAN_DIR share/man/man1)

##############################################################
# Local include directory
##############################################################

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/${UHD_REL_DIR}/include)

##############################################################
# Optional Compiler Flags
##############################################################

INCLUDE(CheckCXXCompilerFlag)
MACRO(UHD_ADD_OPTIONAL_CXX_COMPILER_FLAG flag have)
    CHECK_CXX_COMPILER_FLAG(${flag} ${have})
    IF(${have})
        ADD_DEFINITIONS(${flag})
    ENDIF(${have})
ENDMACRO(UHD_ADD_OPTIONAL_CXX_COMPILER_FLAG)

#force UHD_RELEASE_MODE to be a string for cmake-gui
SET(UHD_RELEASE_MODE "${UHD_RELEASE_MODE}" CACHE STRING "UHD Release Mode")

IF(CMAKE_COMPILER_IS_GNUCXX)
    IF(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s")
    ENDIF(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    ADD_DEFINITIONS(-Wall)
    ADD_DEFINITIONS(-Wextra)
    ADD_DEFINITIONS(-Wsign-compare)
    #ADD_DEFINITIONS(-Wconversion)
    #ADD_DEFINITIONS(-pedantic)
    #ADD_DEFINITIONS(-ansi)
    IF(NOT WIN32)
        #only export symbols that are declared to be part of the uhd api (non dll platforms)
        UHD_ADD_OPTIONAL_CXX_COMPILER_FLAG(-fvisibility=hidden HAVE_VISIBILITY_HIDDEN)
        UHD_ADD_OPTIONAL_CXX_COMPILER_FLAG(-fvisibility-inlines-hidden HAVE_VISIBILITY_INLINES_HIDDEN)
    ENDIF(NOT WIN32)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

IF(MSVC)
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/${UHD_REL_DIR}/cmake/msvc)
    ADD_DEFINITIONS( #stop all kinds of compatibility warnings
        -D_SCL_SECURE_NO_WARNINGS
        -D_SCL_SECURE_NO_DEPRECATE
        -D_CRT_SECURE_NO_WARNINGS
        -D_CRT_SECURE_NO_DEPRECATE
        -D_CRT_NONSTDC_NO_WARNINGS
        -D_CRT_NONSTDC_NO_DEPRECATE
    )
    ADD_DEFINITIONS(/MP) #multi-threaded build
ENDIF(MSVC)

IF(CYGWIN)
    ADD_DEFINITIONS(-D__USE_W32_SOCKETS) #boost asio says we need this
ENDIF(CYGWIN)

IF(WIN32)
    ADD_DEFINITIONS(-D_WIN32_WINNT=0x0501) #minimum version required is windows xp
    ADD_DEFINITIONS(-DNOMINMAX) #disables stupidity and enables std::min and std::max
ENDIF(WIN32)

##############################################################
# Setup Boost
##############################################################

MESSAGE(STATUS "")
MESSAGE(STATUS "Configuring Boost C++ Libraries...")
SET(BOOST_REQUIRED_COMPONENTS
    date_time
    filesystem
    program_options
    regex
    system
    thread
    unit_test_framework
)

IF(UNIX AND NOT BOOST_ROOT AND EXISTS "/usr/lib64")
    LIST(APPEND BOOST_LIBRARYDIR "/usr/lib64") #fedora 64-bit fix
ENDIF(UNIX AND NOT BOOST_ROOT AND EXISTS "/usr/lib64")

IF(MSVC)
    SET(BOOST_ALL_DYN_LINK "${BOOST_ALL_DYN_LINK}" CACHE BOOL "boost enable dynamic linking")
    IF(BOOST_ALL_DYN_LINK)
        ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK) #setup boost auto-linking in msvc
    ELSE(BOOST_ALL_DYN_LINK)
        UNSET(BOOST_REQUIRED_COMPONENTS) #empty components list for static link
    ENDIF(BOOST_ALL_DYN_LINK)
ENDIF(MSVC)

SET(Boost_ADDITIONAL_VERSIONS
    "1.35.0" "1.35" "1.36.0" "1.36" "1.37.0" "1.37" "1.38.0" "1.38" "1.39.0" "1.39"
    "1.40.0" "1.40" "1.41.0" "1.41" "1.42.0" "1.42" "1.43.0" "1.43" "1.44.0" "1.44"
    "1.45.0" "1.45" "1.46.0" "1.46" "1.47.0" "1.47" "1.48.0" "1.48" "1.49.0" "1.49"
    "1.50.0" "1.50" "1.51.0" "1.51" "1.52.0" "1.52" "1.53.0" "1.53" "1.54.0" "1.54"
    "1.55.0" "1.55" "1.56.0" "1.56" "1.57.0" "1.57" "1.58.0" "1.58" "1.59.0" "1.59"
    "1.60.0" "1.60" "1.61.0" "1.61" "1.62.0" "1.62" "1.63.0" "1.63" "1.64.0" "1.64"
    "1.65.0" "1.65" "1.66.0" "1.66" "1.67.0" "1.67" "1.68.0" "1.68" "1.69.0" "1.69"
)
FIND_PACKAGE(Boost 1.36 COMPONENTS ${BOOST_REQUIRED_COMPONENTS})

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

MESSAGE(STATUS "Boost include directories: ${Boost_INCLUDE_DIRS}")
MESSAGE(STATUS "Boost library directories: ${Boost_LIBRARY_DIRS}")
MESSAGE(STATUS "Boost libraries: ${Boost_LIBRARIES}")

##############################################################
# Check Python Modules
##############################################################

INCLUDE(UHDPython)

PYTHON_CHECK_MODULE(
    "Python version 2.6 or greater"
    "platform" "platform.python_version() >= '2.6'"
    HAVE_PYTHON_PLAT_MIN_VERSION
)

PYTHON_CHECK_MODULE(
    "Cheetah templates 2.0.0 or greater"
    "Cheetah" "Cheetah.Version >= '2.0.0'"
    HAVE_PYTHON_MODULE_CHEETAH
)

##############################################################
# Images download directory for utils/uhd_images_downloader.py
##############################################################

SET(UHD_IMAGES_MD5SUM "1825066668276cb502ee3db62726bc0e")
SET(UHD_IMAGES_DOWNLOAD_SRC "http://files.ettus.com/binaries/master_images/archive/uhd-images_003.005.004-139-g56f391ad.zip")

##############################################################
# Register top level components
##############################################################

LIBUHD_REGISTER_COMPONENT("LibUHD" ENABLE_LIBUHD ON "Boost_FOUND;HAVE_PYTHON_PLAT_MIN_VERSION;HAVE_PYTHON_MODULE_CHEETAH" OFF)
LIBUHD_REGISTER_COMPONENT("Examples" ENABLE_EXAMPLES ON "ENABLE_LIBUHD" OFF)
LIBUHD_REGISTER_COMPONENT("Utils" ENABLE_UTILS ON "ENABLE_LIBUHD" OFF)
LIBUHD_REGISTER_COMPONENT("Tests" ENABLE_TESTS ON "ENABLE_LIBUHD" OFF)

##############################################################
# Add the subdirectories
##############################################################

ADD_SUBDIRECTORY(${UHD_REL_DIR}/docs)

ADD_SUBDIRECTORY(${UHD_REL_DIR}/include)

IF(ENABLE_EXAMPLES)
    ADD_SUBDIRECTORY(${UHD_REL_DIR}/examples)
ENDIF(ENABLE_EXAMPLES)

IF(ENABLE_LIBUHD)
    ADD_SUBDIRECTORY(${UHD_REL_DIR}/lib)
ENDIF(ENABLE_LIBUHD)

IF(ENABLE_TESTS)
    ADD_SUBDIRECTORY(${UHD_REL_DIR}/tests)
ENDIF(ENABLE_TESTS)

IF(ENABLE_UTILS)
    ADD_SUBDIRECTORY(${UHD_REL_DIR}/utils)
ENDIF(ENABLE_UTILS)

##############################################################
# Create Pkg Config File
##############################################################

FOREACH(inc ${Boost_INCLUDE_DIRS})
    LIST(APPEND UHD_PC_CFLAGS "-I${inc}")
ENDFOREACH(inc)

FOREACH(lib ${Boost_LIBRARY_DIRS})
    LIST(APPEND UHD_PC_LIBS "-L${lib}")
ENDFOREACH(lib)

#use space-separation format for the pc file
STRING(REPLACE ";" " " UHD_PC_REQUIRES "${UHD_PC_REQUIRES}")
STRING(REPLACE ";" " " UHD_PC_CFLAGS "${UHD_PC_CFLAGS}")
STRING(REPLACE ";" " " UHD_PC_LIBS "${UHD_PC_LIBS}")

#unset these vars to avoid hard-coded paths to cross environment
IF(CMAKE_CROSSCOMPILING)
    UNSET(UHD_PC_CFLAGS)
    UNSET(UHD_PC_LIBS)
ENDIF(CMAKE_CROSSCOMPILING)

IF(NOT LIBUHD_PKG AND NOT UHDHOST_PKG)
    CONFIGURE_FILE(
        ${CMAKE_CURRENT_SOURCE_DIR}/${UHD_REL_DIR}/uhd.pc.in
        ${CMAKE_CURRENT_BINARY_DIR}/${UHD_REL_DIR}/uhd.pc
    @ONLY)

    INSTALL(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${UHD_REL_DIR}/uhd.pc
        DESTINATION ${LIBRARY_DIR}/pkgconfig
        COMPONENT libraries
    )
ENDIF(NOT LIBUHD_PKG AND NOT UHDHOST_PKG)

##############################################################
# Handle pre-built images
##############################################################

IF(DEFINED UHD_IMAGES_DIR AND EXISTS "${UHD_IMAGES_DIR}")
    FILE(GLOB _image_files "${UHD_IMAGES_DIR}/*.*")
    MESSAGE(STATUS "Using images:")
    FOREACH(_img ${_image_files})
        MESSAGE(STATUS "  ${_img}")
    ENDFOREACH(_img)
    UHD_INSTALL(FILES ${_image_files} DESTINATION ${PKG_DATA_DIR}/images COMPONENT images)
ENDIF(DEFINED UHD_IMAGES_DIR AND EXISTS "${UHD_IMAGES_DIR}")

##############################################################
# Print Summary
##############################################################

IF(LIBUHD_PKG)
    MESSAGE(STATUS "")
    SET(PRINT_APPEND " (Debian libuhd003 package configuration)")
ELSEIF(LIBUHDDEV_PKG)
    MESSAGE(STATUS "")
    SET(PRINT_APPEND " (Debian libuhd-dev package configuration)")
ELSEIF(UHDHOST_PKG)
    MESSAGE(STATUS "")
    SET(PRINT_APPEND " (Debian uhd-host package configuration)")
ENDIF(LIBUHD_PKG)
UHD_PRINT_COMPONENT_SUMMARY()
MESSAGE(STATUS "Building version: ${UHD_VERSION}${PRINT_APPEND}")
MESSAGE(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
MESSAGE(STATUS "Compatible images can be downloaded from: ${UHD_IMAGES_DOWNLOAD_SRC}")

##############################################################
# set variables used by the A2300 build system for UHD
##############################################################

SET(ENABLE_UHD ${ENABLE_LIBUHD} CACHE INTERNAL "" FORCE)
SET(UHD_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/${UHD_REL_DIR}/include CACHE INTERNAL "" FORCE)
