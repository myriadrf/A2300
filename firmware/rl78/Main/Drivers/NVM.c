// Name:  NVM.c
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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.


/*---- determine how to compile ----*/
#include "Compile.h"

/*---- Custom include files ----*/
#include "r_cg_macrodriver.h"
#include "r_cg_timer.h"
#include "r_cg_userdefine.h"
#include "Main.h"
#include "Globals.h"
#include "NVM.h"

/*---- static function declarations ----*/
static fal_status_t   NVM_Execute( void );

/*---- static variable declarations ----*/

/*---- global variable declarations ----*/



/****************************************************************/
/****************************************************************/
/***                 NVM PROGRAMMING ROUTINES                 ***/
/****************************************************************/
/****************************************************************/

extern __far const fal_descriptor_t fal_descriptor_str;
fal_status_t         my_fal_status_enu;
__near fal_request_t request;

/***************************************************************

   CALL:    NVM_Init()

   INTRO:   This routine initializes the non-volatile routines.
            The actual routines are in the Renesas FDL library.

   INPUT:   nothing
   OUTPUT:  nothing

****************************************************************/
void NVM_Init( void )
{

	/* initialization */
	my_fal_status_enu = FAL_Init( (__far fal_descriptor_t*)&fal_descriptor_str );
	if( my_fal_status_enu != FAL_OK ) {
		/*---- indicate an error in the status ----*/
		if( status.Error[ERROR_NVM] < 255 ) {
			status.Error[ERROR_NVM]++;
		}
	}

	FAL_Open();

}

/***************************************************************

   CALL:    NVM_EraseBlock()

   INTRO:   This routine erases the specified 1k block in NVM.
            There are 4 blocks in the NVM, thus 'blocks' can
            be 0 to 3.

   INPUT:   block          - NVM block to erase
   OUTPUT:  error code

****************************************************************/
fal_status_t   NVM_EraseBlock( WORD block )
{

   /*---- erase specified block ----*/
   request.index_u16 = block;
   request.command_enu = FAL_CMD_ERASE_BLOCK;

   /*---- send command to FDL and wait for it to finish ----*/
   return( NVM_Execute() );
}

/***************************************************************

   CALL:    NVM_Write()

   INTRO:   This routine writes the specified data into NVM.
            The NVM must be erased before using this command.

   INPUT:   address        - DWORD offset within 1k block to start write
            length         - length to write in bytes
            dwordPtr       - pointer to start of data
   OUTPUT:  error code

****************************************************************/
fal_status_t   NVM_Write( WORD address, int length, DWORD* dwordPtr )
{
int   i;
int   ec;

   for( i=0 ; i<(length+3)/4 ; i++ ) {

      /*---- write word to specified address ----*/
      request.index_u16 = address++;      // set the address (in long word sizes)
      request.data_u32 = *dwordPtr++;     // set the 4-bytes of data
      request.command_enu = FAL_CMD_WRITE_WORD;

      /*---- send command to FDL and wait for it to finish ----*/
      ec = NVM_Execute();

      /*---- stop if error ----*/
      if( ec != FAL_OK ) {
         break;
      }

   }

   return( ec );
}

/***************************************************************

   CALL:    NVM_Execute()

   INTRO:   This routine executes the command in the Renesas
            FDL as specified by the 'request' structure.

   INPUT:   nothing
   OUTPUT:  error code

****************************************************************/
static fal_status_t   NVM_Execute( void )
{
	FAL_Execute(&request);
	while( request.status_enu == FAL_BUSY ) {
		FAL_Handler();
	}
	if( request.status_enu != FAL_OK ) {
		/*---- indicate an error in the status ----*/
		if( status.Error[ERROR_NVM] < 255 ) {
			status.Error[ERROR_NVM]++;
		}
	}
	/*---- return result ----*/
	return( request.status_enu );
}
