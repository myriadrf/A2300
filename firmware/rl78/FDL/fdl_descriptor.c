/*================================================================================================*/
/* Project      = Flash Data Access Library (T01)                                                 */
/* Module       = fdl_descriptor.c                                                                */
/* Device:      = RL78                                                                            */
/* Version      = V1.10                                                                           */
/* Date         = 09.12.2010 09:44:33                                                             */
/*================================================================================================*/
/*                                  COPYRIGHT                                                     */
/*================================================================================================*/
/* Copyright (c) 2010 by Renesas Electronics Europe GmbH, a company of the Renesas Electronics    */
/* Corporation. All rights reserved.                                                              */
/*================================================================================================*/
/*                                                                                                */
/* Purpose:     Contains FAL pool descriptor. Please don't change this file!!!!                   */
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




/*==============================================================================================*/
/* compiler settings                                                                            */
/*==============================================================================================*/
#pragma name FAL_DESCR

/* constant segment definition */
#pragma section @@CNSTL FAL_CNST

/*==============================================================================================*/
/* include files list                                                                           */
/*==============================================================================================*/
#include "fdl_types.h"
#include "fdl_descriptor.h"

/* ----------------------------------------------------------------------------------------------------- */
/* ------                                                                                          ----- */
/* ------     B E G I N    O F    U N T O U C H A B L E     D E C L A R A T I O N    A R E A       ----- */
/* ------                                                                                          ----- */
/* ----------------------------------------------------------------------------------------------------- */

#if (EEL_POOL_SIZE>FAL_POOL_SIZE)
  #error "EEL pool exceeds the FAL pool! Please adapt EEL_POOL_SIZE definition in fdl_descriptor.h"
#endif

/* specify the flash block size expressed in bytes */
#define   FAL_BLOCK_SIZE                (1*1024)

/* data flash start and end address                */
#define   FAL_DATA_FLASH_START_ADDR     0x000F1000
#define   FAL_DATA_FLASH_END_ADDR       0x000F1FFF



/* FAL pool configuration */

#define FAL_POOL_FIRST_BLOCK            0

#if (FAL_POOL_SIZE>0)
  #define FAL_POOL_LAST_BLOCK           (FAL_POOL_SIZE - 1)
  #define FAL_POOL_FIRST_ADDRESS        FAL_DATA_FLASH_START_ADDR
  #define FAL_POOL_LAST_ADDRESS         ( FAL_POOL_FIRST_ADDRESS + (FAL_BLOCK_SIZE*FAL_POOL_LAST_BLOCK) + (FAL_BLOCK_SIZE-1) )
  #define FAL_POOL_WSIZE                (FAL_POOL_SIZE*FAL_BLOCK_SIZE/4)
  #define FAL_POOL_FIRST_WIDX           0
  #define FAL_POOL_LAST_WIDX            (FAL_POOL_WSIZE - 1)
  #define FAL_BLOCK_WSIZE               (FAL_BLOCK_SIZE/4)
#else
  #define FAL_POOL_LAST_BLOCK           0
  #define FAL_POOL_FIRST_ADDRESS        0
  #define FAL_POOL_LAST_ADDRESS         0
  #define FAL_POOL_WSIZE                0
  #define FAL_POOL_FIRST_WIDX           0
  #define FAL_POOL_LAST_WIDX            0
  #define FAL_BLOCK_WSIZE               0
#endif



/* EEL pool configuration */

#define EEL_POOL_FIRST_BLOCK            0

#if (EEL_POOL_SIZE>0)
  #define EEL_POOL_LAST_BLOCK           (EEL_POOL_FIRST_BLOCK + EEL_POOL_SIZE - 1)
  #define EEL_POOL_FIRST_ADDRESS        FAL_POOL_FIRST_ADDRESS
  #define EEL_POOL_LAST_ADDRESS         ( FAL_POOL_FIRST_ADDRESS + ( FAL_BLOCK_SIZE*EEL_POOL_LAST_BLOCK ) + (FAL_BLOCK_SIZE-1) )
  #define EEL_POOL_WSIZE                (EEL_POOL_SIZE*FAL_BLOCK_SIZE/4)
  #define EEL_POOL_FIRST_WIDX           0
  #define EEL_POOL_LAST_WIDX            (EEL_POOL_WSIZE - 1)
#else
  #define EEL_POOL_LAST_BLOCK           0
  #define EEL_POOL_FIRST_ADDRESS        0
  #define EEL_POOL_LAST_ADDRESS         0
  #define EEL_POOL_WSIZE                0
  #define EEL_POOL_FIRST_WIDX           0
  #define EEL_POOL_LAST_WIDX            0
#endif


/* USER pool configuration */

#define USER_POOL_SIZE                  (FAL_POOL_SIZE - EEL_POOL_SIZE)

#if (USER_POOL_SIZE>0)
  #define USER_POOL_FIRST_BLOCK         0
  #define USER_POOL_LAST_BLOCK          (USER_POOL_FIRST_BLOCK + USER_POOL_SIZE - 1)

  #if (EEL_POOL_SIZE>0)
    #define USER_POOL_FIRST_ADDRESS     (EEL_POOL_LAST_ADDRESS + 1)
  #else
    #define USER_POOL_FIRST_ADDRESS     FAL_POOL_FIRST_ADDRESS
  #endif

  #define USER_POOL_LAST_ADDRESS        ( USER_POOL_FIRST_ADDRESS + ( FAL_BLOCK_SIZE*USER_POOL_LAST_BLOCK ) + (FAL_BLOCK_SIZE-1) )
  #define USER_POOL_WSIZE               (USER_POOL_SIZE*FAL_BLOCK_SIZE/4)
  #define USER_POOL_FIRST_WIDX          0
  #define USER_POOL_LAST_WIDX           (USER_POOL_WSIZE - 1)
#else
  #define USER_POOL_FIRST_BLOCK         0
  #define USER_POOL_LAST_BLOCK          0
  #define USER_POOL_FIRST_ADDRESS       0
  #define USER_POOL_LAST_ADDRESS        0
  #define USER_POOL_WSIZE               0
  #define USER_POOL_FIRST_WIDX          0
  #define USER_POOL_LAST_WIDX           0
#endif

/* calculate f_MHz = round-up(FDL_SYSTEM_FREQUENCY)   */
#define FDL_FX_MHZ                      ((FDL_SYSTEM_FREQUENCY+999999)/1000000)

#ifdef FDL_WIDE_VOLTAGE_MODE
  #define FDL_VOLTAGE_MODE  0x01
#else
  #define FDL_VOLTAGE_MODE  0x00
#endif

__far const fal_descriptor_t  fal_descriptor_str = {                              \
                                                (fal_u32)FAL_POOL_FIRST_ADDRESS,  \
                                                (fal_u32)EEL_POOL_FIRST_ADDRESS,  \
                                                (fal_u32)USER_POOL_FIRST_ADDRESS, \

                                                (fal_u32)FAL_POOL_LAST_ADDRESS,   \
                                                (fal_u32)EEL_POOL_LAST_ADDRESS,   \
                                                (fal_u32)USER_POOL_LAST_ADDRESS,  \

                                                (fal_u16)FAL_POOL_FIRST_BLOCK,    \
                                                (fal_u16)EEL_POOL_FIRST_BLOCK,    \
                                                (fal_u16)USER_POOL_FIRST_BLOCK,   \

                                                (fal_u16)FAL_POOL_LAST_BLOCK,     \
                                                (fal_u16)EEL_POOL_LAST_BLOCK,     \
                                                (fal_u16)USER_POOL_LAST_BLOCK,    \

                                                (fal_u16)FAL_POOL_FIRST_WIDX,     \
                                                (fal_u16)EEL_POOL_FIRST_WIDX,     \
                                                (fal_u16)USER_POOL_FIRST_WIDX,    \

                                                (fal_u16)FAL_POOL_LAST_WIDX,      \
                                                (fal_u16)EEL_POOL_LAST_WIDX,      \
                                                (fal_u16)USER_POOL_LAST_WIDX,     \

                                                (fal_u16)FAL_POOL_WSIZE,          \
                                                (fal_u16)EEL_POOL_WSIZE,          \
                                                (fal_u16)USER_POOL_WSIZE,         \

                                                (fal_u16)FAL_BLOCK_SIZE,          \
                                                (fal_u16)FAL_BLOCK_WSIZE,         \

                                                (fal_u08)FAL_POOL_SIZE,           \
                                                (fal_u08)EEL_POOL_SIZE,           \
                                                (fal_u08)USER_POOL_SIZE,          \

                                                (fal_u08)FDL_FX_MHZ,              \
                                                (fal_u08)FDL_VOLTAGE_MODE,        \
                                       };

/* ----------------------------------------------------------------------------------------------------- */
/* ------                                                                                          ----- */
/* ------     E N D    O F    U N T O U C H A B L E     D E C L A R A T I O N    A R E A           ----- */
/* ------                                                                                          ----- */
/* ----------------------------------------------------------------------------------------------------- */
