##############################################################
# Project setup
##############################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(PROJECT_NAME A2300)
PROJECT(${PROJECT_NAME} CXX C)
ENABLE_TESTING()

# select the release build type by default to get optimization flags

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Release")
    MESSAGE(STATUS "Build type not specified: defaulting to release.")
ENDIF()
SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

# Set the version information here

SET(VERSION_MAJOR 0)
SET(VERSION_API_COMPAT 1)
SET(VERSION_MINOR 0)
SET(VERSION_MAINT git)

SET(${PROJECT_NAME}_VERSION_FULL
    ${VERSION_MAJOR}.${VERSION_API_COMPAT}.${VERSION_MINOR}
)
SET(${PROJECT_NAME}_VERSION_PATCH
    ${VERSION_MAJOR}.${VERSION_API_COMPAT}
)

##############################################################
# Define install directories
##############################################################

# when the library suffix should be 64 (applies to redhat linux family)
IF(NOT DEFINED LIB_SUFFIX AND CMAKE_SYSTEM_PROCESSOR MATCHES "64$")
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
# UHD MODULE DIR
SET(UHD_MODULE_DIR need_to_define_this)

##############################################################
# Compiler specific setup
##############################################################

INCLUDE(CheckCXXCompilerFlag)
MACRO(ADD_OPTIONAL_CXX_COMPILER_FLAG flag)
    CHECK_CXX_COMPILER_FLAG(${flag} HAVE_DUMMY)
    IF(HAVE_DUMMY)
        ADD_DEFINITIONS(${flag})
    ENDIF()
    UNSET(HAVE_DUMMY)
ENDMACRO(ADD_OPTIONAL_CXX_COMPILER_FLAG)

# http://gcc.gnu.org/wiki/Visibility
ADD_OPTIONAL_CXX_COMPILER_FLAG(-fvisibility=hidden)
ADD_OPTIONAL_CXX_COMPILER_FLAG(-fvisibility-inlines-hidden)

IF(CMAKE_COMPILER_IS_GNUCXX AND
    NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    ADD_OPTIONAL_CXX_COMPILER_FLAG(-s)
ENDIF()

ADD_OPTIONAL_CXX_COMPILER_FLAG(-Wall)
ADD_OPTIONAL_CXX_COMPILER_FLAG(-Wextra)
ADD_OPTIONAL_CXX_COMPILER_FLAG(-Wsign-compare)
ADD_OPTIONAL_CXX_COMPILER_FLAG(-Wconversion)
ADD_OPTIONAL_CXX_COMPILER_FLAG(-pedantic)
ADD_OPTIONAL_CXX_COMPILER_FLAG(-ansi)

ADD_OPTIONAL_CXX_COMPILER_FLAG(-fPIC)

##############################################################
# Setup common include paths within this build
##############################################################

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/host/include
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)

##############################################################
# Add subdirectories
##############################################################

ADD_SUBDIRECTORY(common)
ADD_SUBDIRECTORY(host)
