// Name: DciUtils.c
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

#include "../System/DataTypes.h"
#include "DciUtils.h"
#include "StandardMsgs.h"
#include "WcaMsgs.h"

//*****************************************************************************
// Forward declarations and static declarations.
//*****************************************************************************

#define min( a,b) (((a) < (b)) ? (a) : (b))
#define WCA_COMPONENT_INDEX 4

//Declare standard message Type pre-process handlers.
bool OnTypedDataMsg( Dci_MapEntry* pentry,  Dci_Context* pctxt);
bool OnTypedDataQueryMsg( Dci_MapEntry* pentry, Dci_Context* pctxt);
bool OnWcaMsg( Dci_MapEntry* pentry, Dci_Context* pctxt);
bool OnWcaProp( Dci_MapEntry* pentry, Dci_Context* pctxt);
bool OnWcaPropQuery( Dci_MapEntry* pentry, Dci_Context* pctxt);

typedef bool (*Dci_MsgTypeHandler)( Dci_MapEntry* pentry, Dci_Context* pctxt);

//Array of typed handlers avoids if/else set.
static Dci_MsgTypeHandler s_HandlerTypes[] =
{
	NULL,
	&OnTypedDataMsg,
	&OnTypedDataQueryMsg,
	&OnWcaMsg,
	&OnWcaProp,
	&OnWcaPropQuery
};
 

//*****************************************************************************
// Implementation
//*****************************************************************************

/**
* Function initializes a DCI Map Manager structure to a known state.
*/
void Dci_MapMgr_Init(Dci_MapMgr* pmgr)
{
	pmgr->proot =  NULL;
} 

 

/**
* Function adds a handler to the Map manager.
*/
void Dci_MapMgr_Add( Dci_MapMgr* pmgr,  Dci_MapEntry* pentry)
{
	//Initialize Root if not done already.
	if( pmgr->proot == NULL)
	{
		pmgr->proot = pentry;
		pentry++; //move to next entry.
	}

	//Iterate map adding entries at appropriate location
	//in the tree.  This is the fastest way to traverse
	//message handlers.
	while( pentry->pNextCategory != DCI_MAP_ENDTOKEN )
	{
		Dci_MapEntry* pprev = NULL;
		Dci_MapEntry* pcur = pmgr->proot;

		//Traverse map looking for a place to stick it.
		while(1)
		{
			//Insert before node if less than.
			if( pcur->idCategory > pentry->idCategory)
			{
				//Insert before current node or root if that's where
				//we are.
				if( pprev != NULL) pprev->pNextCategory = pentry;
				else			   pmgr->proot = pentry;

				//Add current node as next node.
				pentry->pNextCategory = pcur;
				break;
			}
			else if( pcur->idCategory < pentry->idCategory)
			{
				//If there isn't a next node add it.
				if( pcur->pNextCategory == NULL)
				{
					pcur->pNextCategory = pentry;
					break; // done with this entry.
				}
				else // move to next node and try again.
				{
					pprev = pcur;
					pcur = pcur->pNextCategory;
				}
			}
			else // We are in the same category.
			{
				if( pcur->idType > pentry->idType)
				{
					//Insert at current node or root if that is 
					//where we are.
					if( pprev != NULL) pprev->pNextType = pentry;
					else			   pmgr->proot = pentry;
			
					//If pcur is a CategoryNode, move next category
					//to pentry;
					if( pcur->pNextCategory != NULL)
					{
						pentry->pNextCategory = pcur->pNextCategory;
						pcur->pNextCategory = NULL;
					}

					//Add current node as next type node.
					pentry->pNextType= pcur;
					break;
				}
				else if( pcur->idType < pentry->idType)
				{
					//If there isn't a next node add it.
					if( pcur->pNextType == NULL)
					{
						pcur->pNextType = pentry;
						break; // done with this entry.
					}
					else // move to next node and try again.
					{
						pprev = pcur;
						pcur = pcur->pNextType;
					}
				}
				else // We are in the same Type. move to end of list.
				{
					if( pcur->pNextHandler == NULL)
					{
						pcur->pNextHandler = pentry;
						break; // done with this entry.
					}
					else
					{
						pprev = pcur;
						pcur = pcur->pNextHandler;
					}
				}
			}
		}

		//Move to next entry.
		pentry++;
	}
}

/**
* Function processes a DCI message context with the specified map manager.
*/
void Dci_MapMgr_Process( Dci_MapMgr* pmgr, Dci_Context* pctxt)
{
	byte idc, idt;
	Dci_MapEntry* pnode = pmgr->proot;
	idc = pctxt->pMsg->idCategory;
	idt = pctxt->pMsg->idType;

	//Locate node with appropriate handler.
	while( pnode != NULL)
	{
		if( pnode->idCategory == idc)
		{
			//traverse type nodes.
			while( pnode != NULL)
			{
				if( pnode->idType == idt)
				{	
					//Traverse all handlers defined for this message
					//category and type.
					while( pnode != NULL)
					{
						//If basic message handler or specialized
						//handler returned true, call message handler.
						if( pnode->idHandlerType == 0 
							|| (*(s_HandlerTypes[ pnode->idHandlerType]))( pnode, pctxt))
						{
							(*(pnode->fncHandler))(pctxt);
						}
						
						//All done if handled.
						if( pctxt->bHandled )
							return;
						else //goto next handler.
							pnode = pnode->pNextHandler;
					}

					return; // nothing more todo.
				}
				else //Goto next message type.
					pnode = pnode->pNextType;
			}
		}
		else //goto next category.
			pnode = pnode->pNextCategory;
	}
}

bool OnTypedDataMsg( Dci_MapEntry* pentry, Dci_Context* pctxt)
{
	//Cast to a 20,13 message.
	Dci_TypedDataRecord* ptd = (Dci_TypedDataRecord*) pctxt->pMsg;
	
	uint16 idtype = pentry->idComponent | (pentry->idPropStart<<8);
	return ptd->idtype == idtype;
}

bool OnTypedDataQueryMsg( Dci_MapEntry* pentry, Dci_Context* pctxt)
{
	//Cast to a 20,93 message.
	Dci_TypedDataRecordQuery* ptdq = (Dci_TypedDataRecordQuery*) pctxt->pMsg;
	
	uint16 idtype = pentry->idComponent | (pentry->idPropStart<<8);
	return ptdq->idtype == idtype;
}

bool OnWcaMsg( Dci_MapEntry* pentry, Dci_Context* pctxt)
{
	//Test to see if component id matches.
	byte* pdata = (byte*) pctxt->pMsg;
	return pdata[WCA_COMPONENT_INDEX] == pentry->idComponent;
}

bool OnWcaProp( Dci_MapEntry* pentry, Dci_Context* pctxt)
{
	bool bResult;
	int i;

	//Test to see if component id matches.
	byte* pdata = (byte*) pctxt->pMsg;
	bResult = (bool)(pdata[WCA_COMPONENT_INDEX] == pentry->idComponent);
	if( bResult)
	{
		Dci_TypedProperties* pProps = (Dci_TypedProperties*) pctxt->pMsg;

		//Validate range on properties.
		Dci_Property* props = Dci_TypedProperties_GetProperties( pProps);
		for( i = 0;i < pProps->ctProperties && bResult; i++)
		{
			//Test to see if within range
			if(props[i].idprop < pentry->idPropStart || props[i].idprop > pentry->idPropEnd)
			{
				bResult = false;
				break;
			}
		}
	}
	return bResult;
}

bool OnWcaPropQuery( Dci_MapEntry* pentry, Dci_Context* pctxt)
{
	bool bResult;
	int i;

	//Test to see if component id matches.
	byte* pdata = (byte*) pctxt->pMsg;
	bResult = (pdata[WCA_COMPONENT_INDEX] == pentry->idComponent);
	if( bResult)
	{
		Dci_TypedPropertiesQuery* ppq = (Dci_TypedPropertiesQuery*) pctxt->pMsg;

		//Validate range on properties.
		byte* ids = Dci_TypedPropertiesQuery_GetPropIds( ppq);

		for( i = 0;i < ppq->ctProperties && bResult; i++)
		{
			//Test to see if within range
			if(ids[i] < pentry->idPropStart || ids[i] > pentry->idPropEnd)
			{
				bResult = false;
				break;
			}
		}
	}
	return bResult;
}
