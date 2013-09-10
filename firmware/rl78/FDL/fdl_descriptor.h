/*================================================================================================*/
/* Project      = Flash Data Access Library (T01)                                                 */
/* Module       = fdl_descriptor.h                                                                */
/* Device:      = RL78                                                                            */
/* Version      = V1.10                                                                           */
/* Date         = 09.12.2010 09:46:36                                                             */
/*================================================================================================*/
/*                                  COPYRIGHT                                                     */
/*================================================================================================*/
/* Copyright (c) 2010 by Renesas Electronics Europe GmbH, a company of the Renesas Electronics    */
/* Corporation. All rights reserved.                                                              */
/*================================================================================================*/
/*                                                                                                */
/* Purpose:     contains user configuration of the FAL pool                                       */
/*                                                                                                */
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



#ifndef __FAL_DESCRIPTOR_H_INCLUDED
#define __FAL_DESCRIPTOR_H_INCLUDED

#include "fdl_types.h"

/* specify the CPU frequency in [Hz]                                        */
#define   FDL_SYSTEM_FREQUENCY      32000000

/* programming voltage mode                                                 */
/* #define   FDL_WIDE_VOLTAGE_MODE  */

/* specify the size of the FAL pool expressed in number of blocks           */
#define   FAL_POOL_SIZE               4

/* specify the size of the EEL pool inside the FAL pool expressed in blocks */
#define   EEL_POOL_SIZE               0

/* external reference to descriptor */
extern    __far const fal_descriptor_t  fal_descriptor_str;

#endif
