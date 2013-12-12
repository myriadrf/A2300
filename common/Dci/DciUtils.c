/** Name: DciUtils.c
*
* Copyright(c) 2013 Loctronix Corporation
* http://www.loctronix.com
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#include "../System/DataTypes.h"
#include "DciUtils.h"
#include "StandardMsgs.h"
#include <string.h>

/*****************************************************************************
* Forward declarations and static declarations.
*****************************************************************************/
#define WCA_COMPONENT_INDEX ((byte)4)

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
					//Insert at current node or root if that is					//where we are.
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
						if( pnode->idHandlerType == 0							|| (*(s_HandlerTypes[ pnode->idHandlerType]))( pnode, pctxt))
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

	uint16 idtype = ((uint16)(pentry->idComponent)) | ((uint16)(pentry->idPropStart<<8));
	return ptd->idtype == idtype;
}

bool OnTypedDataQueryMsg( Dci_MapEntry* pentry, Dci_Context* pctxt)
{
	//Cast to a 20,93 message.
	Dci_TypedDataRecordQuery* ptdq = (Dci_TypedDataRecordQuery*) pctxt->pMsg;

	uint16 idtype = ((uint16)(pentry->idComponent)) | ((uint16)(pentry->idPropStart<<8));
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
	bResult = false;
	bResult = (bool)(pdata[WCA_COMPONENT_INDEX] == pentry->idComponent);
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

//*****************************************************************************
// DCI BIT Framework.
//*****************************************************************************

/**
* Initializes the DCI BIT Client to a safe state.  Function
* callbacks must still be set or it will not work.
*/
void Dci_BitClient_Init( Dci_BitClient* pclient, byte idComponent)
{
	memset( pclient, 0, sizeof( Dci_BitClient));
	pclient->idComponent = idComponent;
}

/**
* Function initializes Bit Operations Manager structure.
*/
void Dci_BitOperationMgrInit( Dci_BitOperationMgr* pmgr, Dci_BitSendMessage fncSendDciMessage)
{
	pmgr->plistClients = NULL;
	pmgr->fncSendDciMessage = fncSendDciMessage;
	memset( pmgr->aBitOps, 0, sizeof( Dci_BitOperation) * DCI_BITMAXOPS);
}

/**
* Function regsters a Bit client with the specified operations manager.
*/
void Dci_BitRegisterClient( Dci_BitOperationMgr* pmgr, Dci_BitClient* pclient)
{
	//Get the address of the client list pointer and iterate
	//until we find one that is not NULL.
	Dci_BitClient** ppiter = &(pmgr->plistClients);
	while( *ppiter != NULL)
	{
		ppiter = &((*ppiter)->pNextClient);
	}

	//Set the client pointer.
	*ppiter  = pclient;
}

/**
* Helper function looks up BitClient
*/
Dci_BitClient* GetBitClient( Dci_BitOperationMgr* pmgr, byte idComponent )
{
	Dci_BitClient* piter = pmgr->plistClients;
	while( piter != NULL)
	{
		if( piter->idComponent == idComponent)			break;
		else
			piter = piter->pNextClient;
	}
	return piter;
}

/**
* Functions processes BIT DCI messages within the context of the specified operations manager.
*/
bool Dci_BitProcessDciMsg( Dci_BitOperationMgr* pmgr, Dci_Context* pctxt)
{
	byte idTransfer = 0;
	byte idStatus  = 0;
	Dci_BitOperation* pbop = NULL;
	Dci_BitClient* pClient = NULL;
	bool bHandled = true;
	int lenMsg;
	byte buff[MAX_MSG_SIZE];

	//If this is not a WCA message or not associated with registered clients
	//leave.
	if( pctxt->idComponent == 0xFF || ((Dci_Hdr*)(pctxt->pMsg))->idCategory != Dci_WcaCategoryId		|| (pClient = GetBitClient( pmgr, pctxt->idComponent)) == NULL)
		return false;

	//Process any BIT messages
	switch( pctxt->idMessage & 0xFF)
	{
		////////////////////////////////////////////////////
		// Process Target Transfer Operation.
		////////////////////////////////////////////////////
		case Dci_BinaryImageTransfer_Id:
		{
			Dci_BinaryImageTransfer *pBit = (Dci_BinaryImageTransfer*) pctxt->pMsg;

			//Look up requested operation and make sure it is inactive
			//and Client supports the type of operation.
			idStatus = BSE_OperationNotAvailable; //Set default status.
			idTransfer = pBit->idTransfer;
			if(    ( idTransfer < DCI_BITMAXOPS)				&& (pmgr->aBitOps[ idTransfer ].state == DCI_BOS_IDLE)				&& (pClient->fncInitiateTargetTransfer != NULL)	)
			{
				//Set up the operation.
				pbop = pmgr->aBitOps + idTransfer;
				pbop->bitinfo = *pBit;

				//Let Client get a whack at initializing.
				idStatus = (*(pClient->fncInitiateTargetTransfer))( pbop);

				//If the client is initiating. Mark the operation
				//as active.
				if( idStatus == BSE_InitiatingTransfer)
				{
				 	pbop->idFrame = 0;					pbop->state = DCI_BOS_TARGET_TRANSFER;
				}
			}

			//Send a response that we are initiating.
			lenMsg = Dci_BinaryImageTransferStatus_Init(				buff, pbop->bitinfo.idComponent,				idTransfer, pbop->idFrame, 0, idStatus);

			(*(pmgr->fncSendDciMessage))(buff, lenMsg, false, pctxt);
		}
		break;

		////////////////////////////////////////////////////
		// Process Request to Initiate Source Transfer.
		////////////////////////////////////////////////////
		case Dci_BinaryImageTransferQuery_Id:
		{
			bool bResetState = false;
			Dci_BinaryImageTransferQuery* pbitq = (Dci_BinaryImageTransferQuery*)pctxt->pMsg;

			//Look up requested operation and make sure it is inactive
			//and Client supports the type of operation.
			idStatus = BSE_OperationNotAvailable; //Set default status.
			idTransfer = pbitq->idTransfer;
			pbop 	   = pmgr->aBitOps + idTransfer; //don't use until validated.

			if(    ( idTransfer < DCI_BITMAXOPS)				&& (pmgr->aBitOps[ idTransfer].state == DCI_BOS_IDLE)				&& (pClient->fncInitiateSourceTransfer != NULL)	)
			{
				//Initialize the BIT Transfer information
				Dci_BinaryImageTransfer* pbit = &(pbop->bitinfo);
				Dci_BinaryImageTransfer_Init( pbit, pbitq->idComponent,
					pbitq->flags, NULL,	NULL, 0, 256, 0, idTransfer);

				//Request Client fill out the rest of the information.
				idStatus = (*(pClient->fncInitiateSourceTransfer))( pbop);
				bResetState = true;
			}

			//If we are able to initiate, send BIT info message to			//target.
			if( idStatus == BSE_InitiatingTransfer)
			{
				//Send a BIT Transfer Message telling target we are going to
				//send data.
				(*(pmgr->fncSendDciMessage))( (byte*) &(pbop->bitinfo),					sizeof( Dci_BinaryImageTransfer), false, pctxt);

				//Mark this operation as in use.
				pbop->idFrame = 0;
				pbop->state = DCI_BOS_SOURCE_TRANSFER;
			}
			else //Report error
			{
				//Send a response that we are initiating.
				lenMsg = Dci_BinaryImageTransferStatus_Init(					buff, pbop->bitinfo.idComponent,					idTransfer, pbop->idFrame, 0, idStatus);

				(*(pmgr->fncSendDciMessage))(buff, lenMsg, false, pctxt);

				//Make sure we reset the operation.
				if(bResetState)
					pbop->state = DCI_BOS_IDLE;
			}
		}
		break;

		////////////////////////////////////////////////////
		//Process Frame transfer Message.
		////////////////////////////////////////////////////
		case Dci_BinaryImageTransferFrame_Id:
		{
			byte *pdata;
			Dci_BinaryImageTransferFrame* pBitf = (Dci_BinaryImageTransferFrame*) pctxt->pMsg;

			//Initialize local variables.
			idStatus 	= BSE_FrameError; //Set default status.
			idTransfer 	= pBitf->idTransfer;
			pbop 		= pmgr->aBitOps + idTransfer; //don't use until validated.

			//Look up requested operation and make sure it is in target transfer
			//and Client supports the type of operation.
			if(    ( idTransfer < DCI_BITMAXOPS )				&& (pbop->state == DCI_BOS_TARGET_TRANSFER)				&& (pClient->fncSetFrameData != NULL)	)
			{
				// Check frame id and set up status.
				idStatus = (pBitf->idFrame != pbop->idFrame)
					? BSE_FrameError					: ( pbop->idFrame < (pbop->bitinfo.ctFrames-1))						? BSE_ReadyNextFrame : BSE_TransferComplete;

				//Let the client set the frame data. If they do not				//return the number of bytes in the frame, then a write_error
				//occured.
				pdata = Dci_BinaryImageTransferFrame_GetData(pBitf);
				if( (*(pClient->fncSetFrameData))( pbop, pdata, pBitf->ctBytes)	!= pBitf->ctBytes)
					idStatus = BSE_WriteError;

				//Send the current status so process can continue.
				lenMsg = Dci_BinaryImageTransferStatus_Init( buff,					pbop->bitinfo.idComponent,					pBitf->idTransfer, pbop->idFrame, pBitf->ctBytes, idStatus);
				(*(pmgr->fncSendDciMessage))(buff, lenMsg, false, pctxt);

				//If ready for next frame set the expected id.
				if( idStatus == BSE_ReadyNextFrame)
				{
					++(pbop->idFrame);
				}
				else // We are stopping for some reason either complete or error.
				{
					if( pClient->fncTransferComplete != NULL)
						(*(pClient->fncTransferComplete))( pbop, idStatus);

					pbop->idFrame = 0;
					pbop->state = DCI_BOS_IDLE;
				}
			}
		}
		break;

		////////////////////////////////////////////////////
		//Process Frame transfer status Message.
		////////////////////////////////////////////////////
		case Dci_BinaryImageTransferStatus_Id:
		{
			//Switch on the Status Message ID.
			Dci_BinaryImageTransferStatus *pMsg = (Dci_BinaryImageTransferStatus *) pctxt->pMsg;

			//Initialize local variables.
			idStatus 	= BSE_FrameError; //Set default status.
			idTransfer 	= pMsg->idTransfer;
			pbop 		= &(pmgr->aBitOps[0]) + idTransfer; //don't use until validated.

			//Look up requested operation and make sure it not inactive
			//and Client supports the type of operation.
			if(    ( idTransfer < DCI_BITMAXOPS )				&& (pbop->state == DCI_BOS_SOURCE_TRANSFER)				&& (pClient->fncGetFrameData != NULL)	)
			{
				switch(pMsg->idStatus)
				{
					case(BSE_InitiatingTransfer): //Target ready to receive.
						pbop->ctBytesRemaining = pbop->bitinfo.sizeImg;
					case(BSE_ReadyNextFrame):			// Send next frame.
					// Send a frame of data.
					{
						//Determine how much data to send.
						byte *pdata;
						uint16 ctBytes = (pbop->ctBytesRemaining > pbop->bitinfo.sizeFrame)								? pbop->bitinfo.sizeFrame
								: (uint16) pbop->ctBytesRemaining;

						// Create Frame message and get point to read data into it.
						lenMsg = Dci_BinaryImageTransferFrame_Init( buff, pbop->bitinfo.idComponent,									idTransfer, 0, pbop->idFrame, ctBytes, NULL);
						pdata  = Dci_BinaryImageTransferFrame_GetData( (Dci_BinaryImageTransferFrame *)buff );

						//Request the client populate the frame with data.
						if( (*(pClient->fncGetFrameData))( pbop, pdata, ctBytes) == ctBytes)
						{
							//Send the buffer frame to the target.
							(*(pmgr->fncSendDciMessage))(buff, lenMsg, false, pctxt);

							//Update internal state.
							++(pbop->idFrame);
							pbop->ctBytesRemaining -= ctBytes;

							//Determine if we are done or ready for next frame.
							idStatus = (pbop->idFrame >= pbop->bitinfo.ctFrames)									 ? BSE_TransferComplete									 : BSE_ReadyNextFrame;
						}
						else //Read Error, send notification.
						{
							idStatus = BSE_ReadError;

							lenMsg = Dci_BinaryImageTransferStatus_Init(										buff, pbop->bitinfo.idComponent,										idTransfer, pbop->idFrame, 0, idStatus);
							(*(pmgr->fncSendDciMessage))(buff, lenMsg, false, pctxt);
						}
					}
					break;

					// Default conditions hault transfer
					case(BSE_FrameError):
					case(BSE_OperationNotAvailable):
					case(BSE_WriteError):
					default:
						idStatus = pMsg->idStatus;
						break;
				} //End status switch

				//If we are not waiting for next frame,
				//terminate the BIT operation.
				if( idStatus != BSE_ReadyNextFrame)
				{
					//Notifiy client we are complete.
					if( pClient->fncTransferComplete != NULL)
						(*(pClient->fncTransferComplete))( pbop, idStatus);

					pbop->idFrame = 0;
					pbop->state = DCI_BOS_IDLE;
				}
			} //End handle status.
		} //End receive BitStatus mesage.

		break;

		default:
			bHandled = false;
			break;
	}
	return bHandled;
}

/**
* Functions initiates transfer from source to target.  By calling this funtion, the caller is designating themselves the source of the operation.
*/
byte Dci_BitInitiateTargetTransfer( Dci_BitOperationMgr* pmgr, Dci_BitClient* pClient, byte idComponentTarget, byte flags, byte idTransfer, Dci_Context* pctxt)
{
  Dci_BitOperation* pbop = NULL;
  byte idStatus  = 0;

  //Set up the operation.
  pbop = pmgr->aBitOps + idTransfer;

  //Look up requested operation and make sure it is inactive
  //and Client supports the type of operation.
  idStatus = BSE_OperationNotAvailable; //Set default status.
  if(    ( idTransfer < DCI_BITMAXOPS)	 && (pmgr->aBitOps[ idTransfer].state == DCI_BOS_IDLE)	 && (pClient->fncInitiateSourceTransfer != NULL)	)
    {
      //Initialize the BIT Transfer information

      Dci_BinaryImageTransfer_Init( &(pbop->bitinfo),
				    idComponentTarget, flags, "",
				    "", 0, 256, 0, idTransfer);

      //Request Client fill out the rest of the information.
      idStatus = (*(pClient->fncInitiateSourceTransfer))( pbop);
    }

  if( idStatus == BSE_InitiatingTransfer)
    {
      //Send a BIT Transfer Message telling target we are going to
      //send data.
      (*(pmgr->fncSendDciMessage))( (byte*) &(pbop->bitinfo),				    sizeof( Dci_BinaryImageTransfer), false, pctxt);

      //Mark this operation as in use.
      pbop->idFrame = 0;
      pbop->state = DCI_BOS_SOURCE_TRANSFER;
    }
  return( idStatus);
}

/**
* Functions requests peer to initiate a transfer.  By calling this funtion, the caller is designating themselves the target of the operation.
*/
bool Dci_BitRequestSourceTransfer( Dci_BitOperationMgr* pmgr, Dci_BitClient* pClient, byte idComponentSource, byte flags, byte idTransfer, Dci_Context* pctxt)
{

  Dci_BitOperation* pbop = NULL;
  byte buff[MAX_MSG_SIZE];

  //Set up the operation.
  pbop = pmgr->aBitOps + idTransfer;

  //Look up requested operation and make sure it is inactive
  //and Client supports the type of operation.

  if(    ( idTransfer < DCI_BITMAXOPS)
	 && (pmgr->aBitOps[ idTransfer].state == DCI_BOS_IDLE))
    {
      int msgLen = Dci_BinaryImageTransferQuery_Init(buff, idComponentSource, idTransfer, flags);

      //Send request to initiate a BIT Transfer
      (*(pmgr->fncSendDciMessage))( buff, msgLen, false, pctxt);

      return (true);
    }

  return( false);
}
