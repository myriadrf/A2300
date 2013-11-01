##############################################################
# Setup common Dci library as static
##############################################################

SET(Dci_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/BinaryStreamFormatter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/DciConversation.c
    ${CMAKE_CURRENT_SOURCE_DIR}/DciMsg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/DciUtils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/InfrastructureMsgs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/StandardMsgs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/WcaMsgs.c
)

ADD_LIBRARY(Dci STATIC ${Dci_sources})

##############################################################
# Install information
##############################################################

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
