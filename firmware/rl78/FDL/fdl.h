/*================================================================================================*/
/* Project      = Flash Data Access Library (T01)                                                 */
/* Module       = fdl.h                                                                           */
/* Device:      = RL78                                                                            */
/* Version      = V1.10                                                                           */
/* Date         = 09.12.2010 09:45:51                                                             */
/*================================================================================================*/
/*                                  COPYRIGHT                                                     */
/*================================================================================================*/
/* Copyright (c) 2010 by Renesas Electronics Europe GmbH, a company of the Renesas Electronics    */
/* Corporation. All rights reserved.                                                              */
/*================================================================================================*/
/* Purpose:     contains the API definition of the Flash Access Library (data flash only)         */
/*================================================================================================*/
/*                                                                                                */
/* Warranty Disclaimer                                                                            */
/*                                                                                                */
/* Because the Product(s) is licensed free of charge, there is no warranty of any kind whatsoever */
/* and expressly disclaimed and excluded by Renesas, either expressed or implied, including but   */
/* not limited to those for non-infringement of intellectual property, merchantability and/or     */
/* fitness for the particular purpose.                                                            */
/* Renesas shall not have any obligation to maintain, service or provide bug fixes for the        */
/* supplied Product(s) and/or the Application.                                                    */
/*                                                                                                */
/* Each User is solely responsible for determining the appropriateness of using the Product(s)    */
/* and assumes all risks associated with its exercise of rights under this Agreement, including,  */
/* but not limited to the risks and costs of program errors, compliance with applicable laws,     */
/* damage to or loss of data, programs or equipment, and unavailability or interruption of        */
/* operations.                                                                                    */
/*                                                                                                */
/* Limitation of Liability                                                                        */
/*                                                                                                */
/* In no event shall Renesas be liable to the User for any incidental, consequential, indirect,   */
/* or punitive damage (including but not limited to lost profits) regardless of whether such      */
/* liability is based on breach of contract, tort, strict liability, breach of warranties,        */
/* failure of essential purpose or otherwise and even if advised of the possibility of such       */
/* damages. Renesas shall not be liable for any services or products provided by third party      */
/* vendors, developers or consultants identified or referred to the User by Renesas in            */
/* connection with the Product(s) and/or the Application.                                         */
/*                                                                                                */
/*================================================================================================*/


#ifndef __FDL_H_INCLUDED
#define __FDL_H_INCLUDED

/*==============================================================================================*/
/* include files list                                                                           */
/*==============================================================================================*/
#include "fdl_types.h"



/* ---------------------------------------------------------------------------------------------------------------*/
/*  Block type:   standard function                                                                               */
/* ---------------------------------------------------------------------------------------------------------------*/
/*  Purpose:      Enable data flash access.                                                                       */
/*                Pre-condition: The initialization of the library must be done by the FAL_Init function.         */
/*  Input:        -                                                                                               */
/*  Output:       -                                                                                               */
/*  Return:       -                                                                                               */
/* ---------------------------------------------------------------------------------------------------------------*/
extern  void  __far FAL_Open(void);


/* ---------------------------------------------------------------------------------------------------------------*/
/*  Block type:   standard function                                                                               */
/* ---------------------------------------------------------------------------------------------------------------*/
/*  Purpose:      Disable data flash access.                                                                      */
/*                Pre-condition: The initialization of the library must be done by the FAL_Init function.         */
/*  Input:        -                                                                                               */
/*  Output:       -                                                                                               */
/*  Return:       -                                                                                               */
/* ---------------------------------------------------------------------------------------------------------------*/
extern  void  __far FAL_Close(void);



/* ---------------------------------------------------------------------------------------------------------------*/
/*  Block type:   standard function                                                                               */
/* ---------------------------------------------------------------------------------------------------------------*/
/*  Purpose:      Initialization of the Flash Access Library.                                                     */
/*                Based on the descriptor data several checks are performed:                                      */
/*                - plausibility check of the FAL pool configuration                                              */
/*                - initialization of all internal variables                                                      */
/*                After initialization the FAL remains passive.                                                   */
/*                FAL_Open() has to be executed to open access to the FAL pool                                    */
/*  Input:        &descriptor_pstr far pointer to the descriptor                                                  */
/*  Output:       -                                                                                               */
/*  Return:       status: configuration related error code                                                        */
/*                - FAL_OK:                 initialization done without problems                                  */
/*                - FAL_ERR_CONFIGURATION:  FAL pool-configuration error                                          */
/* ---------------------------------------------------------------------------------------------------------------*/
extern  fal_status_t  __far FAL_Init(const __far fal_descriptor_t* descriptor_pstr);



/* ---------------------------------------------------------------------------------------------------------------*/
/*  Block type:   standard function                                                                               */
/* ---------------------------------------------------------------------------------------------------------------*/
/*  Purpose:      Initiating execution of the FAL request described in the passed request reference.              */
/*                The corresponding request variable has to be defined before starting the execution.             */
/*                                                                                                                */
/*                                                 WCMD (word command)            BCMD (block command)            */
/*                                                 -------------------------------------------------------------- */
/*                my_request->data_u32         :   read/write data                no meaning                      */
/*                my_request->index_u16        :   word-index within pool         block index within pool         */
/*                my_request->command_enu      :   command code                   command code                    */
/*                my_request->status_enu       :   request status                 request status                  */
/*                                                                                                                */
/*                Each client who is using FAL_Execute() has to use two private variables of initiating requests. */
/*                One of them is exclusively reserved for suspendable block-oriented commands (BCMD).             */
/*                The other one defines the non-suspendable word-oriented commands (WCMD).                        */
/*                Please refer to the users manual for details of the suspend-/resume-mechanism.                  */
/*                                                                                                                */
/*  Input:        &my_request_pstr - pointer to privat request variable  (WCMD or BCMD)                           */
/*  Output:       status inside the request variable                                                              */
/*  Return:       -                                                                                               */
/* ---------------------------------------------------------------------------------------------------------------*/
extern  void  __far FAL_Execute(__near fal_request_t* request_pstr);



/* ---------------------------------------------------------------------------------------------------------------*/
/*  Block type:   standard function                                                                               */
/* ---------------------------------------------------------------------------------------------------------------*/
/*  Purpose:      Continuing of not finished commands.                                                            */
/*                                                                                                                */
/*  Input:        -                                                                                               */
/*  Output:       status inside the request variable for command executed via FAL_Execute                         */
/*  Return:       -                                                                                               */
/* ---------------------------------------------------------------------------------------------------------------*/
extern  void  __far FAL_Handler(void);



/* ---------------------------------------------------------------------------------------------------------------*/
/*  Block type:   standard function                                                                               */
/* ---------------------------------------------------------------------------------------------------------------*/
/*  Purpose:      Return the start address of the library version string                                          */
/*                                                                                                                */
/*  Input:        -                                                                                               */
/*  Output:       -                                                                                               */
/*  Return:       starting pointer to the zero-terminated version string                                          */
/* ---------------------------------------------------------------------------------------------------------------*/
extern  __far fal_u08* __far FAL_GetVersionString(void);


#endif
