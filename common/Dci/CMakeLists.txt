##############################################################
# Setup Shared Library
##############################################################

list(APPEND Dci_sources
	    ${CMAKE_CURRENT_SOURCE_DIR}/BinaryStreamFormatter.c
	    ${CMAKE_CURRENT_SOURCE_DIR}/DciConversation.c
	    ${CMAKE_CURRENT_SOURCE_DIR}/DciMsg.c
	    ${CMAKE_CURRENT_SOURCE_DIR}/DciUtils.c
	    ${CMAKE_CURRENT_SOURCE_DIR}/InfrastructureMsgs.c
	    ${CMAKE_CURRENT_SOURCE_DIR}/StandardMsgs.c
	    ${CMAKE_CURRENT_SOURCE_DIR}/WcaMsgs.c
)

add_library(Dci SHARED ${Dci_sources})

set_target_properties(Dci PROPERTIES
    VERSION ${${PROJECT_NAME}_VERSION_FULL}
)

if (UNIX)
    set_target_properties(Dci PROPERTIES
        SOVERSION ${${PROJECT_NAME}_VERSION_PATCH}
    )
endif()

if (APPLE)
        set_target_properties(Dci PROPERTIES
	        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib$"
	)
endif()
