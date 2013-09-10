// Name: DciUtils.h
//
// Copyright(c) 2013 Loctronix Corporation
// http://www.loctronix.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#ifndef DciUtils_h
#define DciUtils_h

#include "../System/DataTypes.h"


#ifdef  __cplusplus
extern "C" {
#endif

// Forward declare basic structures.
struct Dci_Hdr;
struct Dci_Conversation;


//*****************************************************************************
// DCI Message Map Framework.
//*****************************************************************************
/*
	DCI message map framework provides filtered message handlers that efficiently
	processes supplied messages and routes them to the appropriate handler.  Use of this
	framework significantly reduces the code overhead to validate and determine if a particular message
	is to be processed.  This replaces the use of switch/case structures and is significantly faster at the 
	expense of 16 bytes per message handler defined.  For an application of 30 handlers, this constitutes a memory penalty
	about 480 bytes, which is likely offset by reduced code space requirements, eliminating lots of redundant code. 

	To use you must implement two items:  1) a Message Map binding handlers to messages and 2) a message map Manager.  Both examples are
	shown below.

	1) Message map.  In your various source files supporting message processing define message handlers as follows:

	void OnHandler1( Dci_Context* pctxt) {...}				//Standard DCI Message Handler 1
	void OnHandler2( Dci_Context* pctxt) {...}				//Standard DCI Message Handler 2
	void OnWcaHandler( Dci_Context* pctxt) {...}			//WCA Message Handlers support component ids
	void OnWcaPropHandler( Dci_Context* pctxt) {...}		//WCA Message Property Handlers support component ids and properties
	void OnWcaPropQueryHandler( Dci_Context* pctxt) {...}	//WCA Property Query Message Handlers support component ids and properties

	//Then define the Message Map to bind them to particular messages, components and properties.
	DCI_BEGIN_MSGMAP( idMyMsgMap )
		DCI_MSG_HANDLER				( 0x20,0x02, &OnHandler1)		// Binds Version Info to Handler 1.
		DCI_TYPEDDATA_HANDLER		( 0x21,&OnHandler2)				// Binds typed data message with typeid=0x21 to OnHandler2.
		DCI_TYPEDDATAQUERY_HANDLER	( 0x21, &OnHandler2)			// Binds TypedData Query with typeid=0x21 to OnHandler2 as well.
		DCI_WCAMSG_HANDLER			( 0x03, 0x18, &OnWcaHandler)	// Binds WCA BIT Info message (21,03) for component 0x18 to OnWcaHandler.
		DCI_WCAPROP_HANDLER			( 0x18, 0x1, 0x4, OnWcaPropHandler )  // Binds a WCA Typed Properties(21,01) message for props 0x1 through 0x4.
		DCI_WCAPROPQUERY_HANDLER	( 0x18, 0x5, 0x8, OnWcaPropQueryHandler) // Binds a WCA Type Properties Query (21,81) for properties 0x5 through 0x8.
	DCI_END_MSGMAP

	//Multiple message maps can be defined and then added to a message map manager.
	
	2) Message Map Manager.  In one source file, handling the initial DCI message processing implement a message map manager and add all 
							 defined messages maps to it.  Once completed, you can then use the map to handle message processing.

	 Create a Message Map Manager instance on the heap and then initialize during application initialization.

	 Dci_MapMgr s_mapmgr;
	 
	 void Init()
	 {
		 Dci_MapMgr_Init(&mapmgr);		//Initializes  the mapmgr instance.
		 Dci_MapMgr_Add( &idMyMsgMap);  //Adds the message map to the manager.
		 Dci_MapMgr_Add( &idMyOtherMsgMap); //Adds another map as well.
	 }
	3) Using the Map Manager.  Once initialized and maps are added, it can be used for processing. For Example:

	 void OnDciMessageReceived( DciContext* pctxt)
	 {
		//Process message using the Manager from step 2.
		Dci_MapMgr_Process( ctxt);
	 }
*/

/**
* Context pass into message handlers to that process DCI Messages.
*/
typedef struct Dci_Context
{
	uint16					idMessage;			// 16 bit version of the message ID.
	byte					idComponent;		// 0xFF if undefined.
	struct Dci_Hdr			*pMsg;				// pointer to message header 
	int						lenMsg;				// length of message
	struct Dci_Conversation	*pConv;				// pointer to conversation
	bool					bHandled;			// indicator of whether the message is handled
} Dci_Context;

/**
* Default DCI Message Handler
*/
typedef void (*Dci_MsgHandler)( Dci_Context* pctxt);


/**
* Structure defines a message map entry. Used by the message map macroxs below.
*/
typedef struct Dci_MapEntry 
{
	struct Dci_MapEntry* pNextCategory;
	struct Dci_MapEntry* pNextType;
	struct Dci_MapEntry* pNextHandler;
	Dci_MsgHandler fncHandler;
	byte idCategory, idType, idComponent, idPropStart, idPropEnd, idHandlerType; 
} Dci_MapEntry;

//Define macros for building message maps.

/**
* Macro is used to denote the start of a message map.
*/
#define DCI_BEGIN_MSGMAP( idMap ) Dci_MapEntry idMap[] = {

/**
* Macro is used to bind an infrastructure or standard DCI message.
*/
#define DCI_MSG_HANDLER( idCategory, idType, fncHandler) \
		{ NULL, NULL, NULL, fncHandler, idCategory, idType, 0xFF, 0xFF, 0xFF, 0x00},

/*
* Macro binds handler to a TypedData (20,13) message with specified typeid = idDataType.
*/
#define DCI_TYPEDDATA_HANDLER( idDataType, fncHandler) \
		{ NULL, NULL, NULL, fncHandler, 0x20, 0x13, (idDataType)&0xFF, (idDataType)>>8, 0xFF, 0x01},

/*
* Macro binds handler to a TypedData (20,83) message with specified typeid = idDataType.
*/
#define DCI_TYPEDDATAQUERY_HANDLER( idDataType, fncHandler) \
		{ NULL, NULL, NULL, fncHandler, 0x20, 0x93, (idDataType)&0xFF, (idDataType)>>8, 0xFF, 0x02},

/*
* Macro binds handler to a WCA message(21,xx) message with specified component ID.
*/
#define DCI_WCAMSG_HANDLER( idType, idComponent, fncHandler) \
		{ NULL, NULL, NULL, fncHandler, 0x21, idType, idComponent, 0x00, 0x00, 0x03},

/*
* Macro binds handler to a WCA TypedProperty message(21,01) message with specified component ID and property range.
*/
#define DCI_WCAPROP_HANDLER( idComponent, idPropStart, idPropEnd, fncHandler) \
		{ NULL, NULL, NULL, fncHandler, 0x21, 0x01, idComponent, idPropStart, idPropEnd, 0x04},

/*
* Macro binds handler to a WCA TypedPropertyQuery message(21,81) message with specified component ID and property range.
*/
#define DCI_WCAPROPQUERY_HANDLER( idComponent, idPropStart, idPropEnd, fncHandler) \
		{ NULL, NULL, NULL, fncHandler, 0x21, 0x81, idComponent, idPropStart, idPropEnd, 0x05},

#define DCI_MAP_ENDTOKEN ((void*)0x01)

/**
* Macro denotes the end of the message map.
*/
#define DCI_END_MSGMAP { DCI_MAP_ENDTOKEN, NULL, NULL, NULL, 0,0,0,0,0,0}};

/**
* Map Manager structure.
*/
typedef struct Dci_MapMgr
{
	Dci_MapEntry* proot;
} Dci_MapMgr;

/**
* Function initializes a DCI Map Manager structure to a known state.
*/
void Dci_MapMgr_Init(Dci_MapMgr* pmgr);		

/**
* Function adds a handler to the Map manager.
*/
void Dci_MapMgr_Add( Dci_MapMgr* pmgr,  Dci_MapEntry* pentry);  

/**
* Function processes a DCI message context with the specified map manager.
*/
void Dci_MapMgr_Process( Dci_MapMgr* pmgr, Dci_Context* pctxt);

#ifdef __cplusplus
}
#endif

#endif /* DciUtils_h */

