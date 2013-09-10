// Name: BinaryStreamFormatter.c
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

#include "BinaryStreamFormatter.h"
#include "string.h"

const int    s_LenHdr = 2;
const int    s_LenFtr = 2;

const byte s_MsgHdr[] = {0xa0, 0xa4};
const byte s_MsgFtr[] = {0xb0, 0xb2};

/**
 * The initializer creates sets up the formatter object.
 */
void Dci_Bsf_Init( Dci_Bsf *pfmttr, byte* pBuff, int sizeBuff)
{
	pfmttr->pBuff	 = pBuff;
	pfmttr->sizeBuff = sizeBuff;
	Dci_Bsf_Reset(pfmttr);
}



/**
 * <summary>
 * Encodes payload and calculates the checksum. Returns the checksum value.
 * </summary>
 */
short Dci_Bsf_EncodePayloadType1( 
	byte* pOut, short* pLenOutput, const byte* pDciMsg, short lenMsg)
{
	byte *pend, *ptr;
	int chksum, nLen;
	Dci_Hdr* phdr;

	//Set the message encoding to type 1.
	phdr = (Dci_Hdr*) pDciMsg;
	Dci_Hdr_SetEncoding(phdr, 1);

	chksum	= 0;
	nLen	= lenMsg;

	//Encode Header, payload and calculate checksum in the same pass.
	ptr = pOut;
	for (pend = (byte*) (pDciMsg+nLen); pDciMsg < pend; pDciMsg++, ptr++)
	{   
		*ptr  = *pDciMsg; 
		chksum += (*pDciMsg);

		//Insert character 
		if( *pDciMsg == 0xA0 &&( pDciMsg[1] == 0xA4 || pDciMsg[1] == 0x00))
		{
			ptr++;
			*ptr= 0x00;	
		}	
	}

	//Save the length.
	*pLenOutput = (short)(ptr - pOut);

	return (short) (chksum & 0x7FFF);
}

/**
 * <summary>
 * Decodes payload and calculates the checksum. Returns the checksum value.
 * </summary>
 */
short Dci_Bsf_DecodePayloadType1( byte* pOut, int* pLenOutput, 
		byte* pHdr, byte* pbuf, short nLen )
{
	byte *pend, *ptr;
	int chksum = 0;

	//Calculate Header Checksum.
	for (pend = pHdr+4; pHdr < pend; pHdr++)
	{ 
		chksum += (*pbuf); 
	}

	//Decode payload and calculate checksum in the same pass.
	ptr = pOut;

	for (pend = pbuf+nLen; pbuf < pend; pbuf++, ptr++)
	{   
		*ptr = *pbuf; 
		chksum += (*pbuf);
		if( *pbuf == 0xA0 &&( pbuf[1] == 0x00))
		{
			pbuf++;
		}	
	}

	//Save the length.
	*pLenOutput = ptr - pOut;

	return (short) (chksum & 0x7FFF);
}

/**
 *<summary>
 * Formats a DCI Message in accordance to Binary Serial Stream Transport requirements.
 *</summary>	
 */
int Dci_Bsf_FormatMessage( 
	byte* pOutBuff, int nMaxBuffLen, const byte* pDciMsg, short lenMsg)
{
	short nlenMsg=0;
	short chksum;
	if( 12 <= nMaxBuffLen )  // 12=Message Size
	{
		pOutBuff[0] = 0xA0;
		pOutBuff[1] = 0xA4;

		nlenMsg = 0;
		chksum = Dci_Bsf_EncodePayloadType1(  pOutBuff+4, &nlenMsg, pDciMsg, lenMsg);
		nlenMsg-=4;
		
		//Save in Big Endian Format.
		//NOTE REDO FOR ARM.
		pOutBuff[2] = (byte)((nlenMsg >> 8) & 0xff);
		pOutBuff[3] = (byte)(nlenMsg & 0xff);
		
		nlenMsg+=8;
		pOutBuff[nlenMsg]   = (byte) ((chksum >> 8) & 0xff);
		pOutBuff[nlenMsg+1] = (byte) (chksum & 0xff);
		pOutBuff[nlenMsg+2] = 0xB0;
		pOutBuff[nlenMsg+3] = 0xB2;
		nlenMsg+=4;
	}	
	return nlenMsg;
}

/**
 *<summary>
 * Calculates the checksum for a payload and header.
 *</summary>	
 */
short Dci_Bsf_CalcChecksum( const byte* pDciMsg, short nLen )
{
	byte *pbuff, *pend;
	int chksum;

	pbuff  = (byte*) pDciMsg;
    chksum = 0;

	//Calculate Encoded Payload Checksum
    for (pend = pbuff+nLen; pbuff < pend; pbuff++)
    {   
		chksum += *pbuff; 
	}

    return (short) (chksum & 0x7FFF);
}




/**
 * Helper method used to reset the formatter for the next message.
 */
void Dci_Bsf_Reset(Dci_Bsf *pfmttr )
{
	memset( pfmttr->pBuff, 0, pfmttr->sizeBuff);
	pfmttr->pNext	 = pfmttr->pBuff;
	pfmttr->iPayload = -1;
	pfmttr->ct		 = 0;
	pfmttr->mode	 = FME_FrameSynch;
}

/**
 * <summary>
 * Call this to get Received Message when state is MessageReady.  
 * This only works once, so it is a good idea to copy the message 
 * if needed for long-term. as Buffers will be overriden in next
 * processing.
 *
 * Returns the length including the header.
 * </summary>
 */
int Dci_Bsf_GetReceivedMessage(
	Dci_Bsf *pfmttr, bool bUnencode, Dci_Hdr** ppMsg)
{
	int encoding, len;
	if( pfmttr->mode == FME_MsgReady )
	{
		encoding = (pfmttr->pBuff[0] >> 2) &0x3;
		len		 = pfmttr->pNext - pfmttr->pBuff - 2;
		
		if( bUnencode && encoding == 1)
		{
			//Decode in place.  This corrupts the buffer, but we don't care.
			Dci_Bsf_DecodePayloadType1(pfmttr->pBuff + 4, &len,
				pfmttr->pBuff, pfmttr->pBuff+4, (short)(len-4));
		}

		*ppMsg = (Dci_Hdr*) pfmttr->pBuff;
		return len+4; //Add back the Header length.
	}
	else
		return 0; //No message to decode.
}

/**
 * This is called by the <code>MessageTransport</code> instance in order to
 * determine if a message is ready - i.e. the formatter has determined that
 * all elements of the message are present and the header, checksum, and
 * footer are valid.  When this method returns true, it indicates that the
 * <code>getMessage()</code> method can be safely called without returning
 * <code>null</code>.
 * @return <code>true</code> if a message is ready; <code>false</code>
 * otherwise
 */
bool Dci_Bsf_MessageReady( Dci_Bsf *pfmttr )
{
	return pfmttr->mode == FME_MsgReady;
}	

bool Dci_Bsf_HaveSynch(Dci_Bsf *pfmttr)
{
	return pfmttr->mode > FME_FrameSynch;
}


/**
 * <summary>
 * Method called to process received data from a provider.  Returns the total number of 
 * bytes processed, if less than ctBytes, means a message is ready or an error occurred.
 * </summary>
 */
int Dci_Bsf_ProcessReceivedData( Dci_Bsf *pfmttr, 
		byte* pbuff, int ctBytes )
{
	byte *pend, *ptr;
	short idxChecksum, chksumCalc, chksumRx;
	
	pend = pbuff + ctBytes;
	ptr  = pbuff;

	if( pfmttr->mode == FME_MsgReady)
		Dci_Bsf_Reset(pfmttr);

	for( ; ptr < pend && pfmttr->mode != FME_MsgReady; ptr++)
	{
		switch (pfmttr->mode)
		{
		case FME_FrameSynch:
			{
				if (*ptr == s_MsgHdr[pfmttr->ct] && pfmttr->ct < s_LenHdr)
				{
					pfmttr->ct++;
					if (pfmttr->ct == s_LenHdr)
					{
						pfmttr->ct = 0; 
						pfmttr->mode = FME_ReadingLength;
					}
				}
				else 
				{
					pfmttr->ct = 0;
					if (*ptr == s_MsgHdr[pfmttr->ct])
						pfmttr->ct++;
				}
			}
			break;
		case FME_ReadingLength:
			{
				//Save the byte.
				*(pfmttr->pNext) = *ptr;
				pfmttr->pNext++;

				pfmttr->ct++;
				if( pfmttr->ct == 2)
				{
					pfmttr->iPayload = (((short) pfmttr->pBuff[0]) <<8) | ((short)pfmttr->pBuff[1]);
					pfmttr->pNext = pfmttr->pBuff;
					pfmttr->mode	=	FME_ReadingHeader; 
					pfmttr->ct = 0;
				}
			}
			break;
		case FME_ReadingHeader:
			{ 
				*(pfmttr->pNext) = *ptr;
				pfmttr->pNext++;

				pfmttr->ct++;
				if( pfmttr->ct == 4)
				{ 
					pfmttr->mode = (pfmttr->iPayload > 0) ? FME_ReadingPayload : FME_ReadingChecksum;
					pfmttr->ct =0;
				}
			}
			break;
		case FME_ReadingPayload:
			{
				*(pfmttr->pNext) = *ptr;
				pfmttr->pNext++;
				pfmttr->ct++;

				if( pfmttr->ct == pfmttr->iPayload)
				{
					pfmttr->mode = FME_ReadingChecksum;
					pfmttr->ct=0;
				}
			}
			break;
		case FME_ReadingChecksum:
			{
				*(pfmttr->pNext) = *ptr;
				pfmttr->pNext++;
				pfmttr->ct++;

				if( pfmttr->ct == 2)
				{
					pfmttr->mode= FME_EndSeq;
					pfmttr->ct=0;
				}
			}
			break; 
		case FME_EndSeq:
			// we are now trying to locate the message footer - the end
			// of the message body
			{
				if (*ptr == s_MsgFtr[pfmttr->ct] && pfmttr->ct < s_LenFtr)
				{
					pfmttr->ct++;
					if (pfmttr->ct == s_LenFtr)
					{
						pfmttr->mode = FME_MsgReady;
					}
				}
				else //Failed, reset.
				{
					Dci_Bsf_Reset(pfmttr);
				}	
			}
			break;
		case FME_MsgReady:
			break;
		}

		// if our message is ready, but the checksum is bad, then we need to try and
		// recover by searching through the current message looking for a new header
		// section (ignoring the current header at index 0)
		if (pfmttr->mode == FME_MsgReady )
		{
			idxChecksum	= (short)(pfmttr->pNext - pfmttr->pBuff -2);
			chksumCalc  = Dci_Bsf_CalcChecksum( pfmttr->pBuff, idxChecksum);
			chksumRx    = (((short) pfmttr->pBuff[idxChecksum]) <<8) | ((short)pfmttr->pBuff[idxChecksum+1]); 
			
			if( chksumCalc != chksumRx )
			{	
				Dci_Bsf_Reset(pfmttr);
			}
		}
	}

	// Return the number of bytes processed.
	return ptr - pbuff;
}
