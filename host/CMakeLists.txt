##############################################################
# Setup CMake Module Path to include local files
##############################################################

set(UHD_REL_DIR uhd/host)

LIST(APPEND CMAKE_MODULE_PATH
${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

##############################################################
# Register "UHD Internal" component, no dependencies
##############################################################
 
INCLUDE(UHDComponent)

LIBUHD_REGISTER_COMPONENT("UHD Internal" ENABLE_UHD_INTERNAL ON "" OFF)

##############################################################
# Set UHD include dirs if doing internal build only
##############################################################

if (ENABLE_UHD_INTERNAL)

   set(UHD_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/lib/Uhd/${UHD_REL_DIR}/include)

endif()

##############################################################
# Add subdirectories
##############################################################

add_subdirectory(lib)
add_subdirectory(gnuradio)
