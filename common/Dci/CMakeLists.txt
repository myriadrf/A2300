##############################################################
# Setup common Dci library as static
##############################################################

SET(A2300Dci_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/BinaryStreamFormatter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/DciConversation.c
    ${CMAKE_CURRENT_SOURCE_DIR}/DciMsg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/DciUtils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/InfrastructureMsgs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/StandardMsgs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/WcaMsgs.c
)

ADD_LIBRARY(A2300Dci STATIC ${A2300Dci_sources})

##############################################################
# Install information
##############################################################

INSTALL(TARGETS A2300Dci
    LIBRARY DESTINATION ${LIBRARY_DIR} COMPONENT libraries # .so file
    ARCHIVE DESTINATION ${LIBRARY_DIR} COMPONENT libraries # .lib file
    RUNTIME DESTINATION ${RUNTIME_DIR} COMPONENT libraries # .dll file
)

UHD_INSTALL(FILES
    BinaryStreamFormatter.h
    DciConversation.h
    DciMsg.h
    DciUtils.h
    InfrastructureMsgs.h
    StandardMsgs.h
    WcaMsgs.h
    DESTINATION ${INCLUDE_DIR}/A2300/Dci
    COMPONENT headers
)
