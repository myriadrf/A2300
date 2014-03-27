
/* ------------------ */
/* - Include Files. - */
/* ------------------ */

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "mlStructure.h"

#ifdef ML_DATA_LOG
#include "mlDataLog.h"

#define NUM_DIAG_FILES  3
typedef struct
{
    FILE *fp[NUM_DIAG_FILES];
    unsigned char logFile[80];
    int fileCounter;
    unsigned int index;
    unsigned char control;

}ML_DATA_LOG_t;

ML_DATA_LOG_t mlDataLog;

extern tQuatPacket qData;

/* -------------- */
/* - Functions. - */
/* -------------- */

/**********************************************************************/
/*                                                                    */
/* Function Name: mlDataLogInit                                       */
/*                                                                    */
/*   Description: Data log initialization                             */
/*                                                                    */
/*    Parameters:                                                     */
/*                                                                    */
/*       Returns:                                                     */
/*                                                                    */
/**********************************************************************/
void mlDataLogInit(void)
{
    // set the dataLog file name
    strcpy_s(mlDataLog.logFile,sizeof("mouseDataLog"), "mouseDataLog");
    mlDataLog.fileCounter = 0;
    mlDataLog.index = 0;
    mlDataLog.control = DATA_LOG_INIT;
}
/**********************************************************************/
/*                                                                    */
/* Function Name: mlDataLogOpen                                       */
/*                                                                    */
/*   Description: Data log file open                                  */
/*                                                                    */
/*    Parameters:                                                     */
/*                                                                    */
/*       Returns: 1: Success 0: Fail                                  */
/*                                                                    */
/**********************************************************************/
int mlDataLogOpen(void)
{
    char i;
    char fileName[80];

    mlDataLog.fileCounter++;

    for (i=0; i<NUM_DIAG_FILES; i++)
    {
        sprintf_s(fileName, sizeof(fileName), "%s%d_%03d.txt", mlDataLog.logFile, i, mlDataLog.fileCounter);
        fopen_s(&mlDataLog.fp[i], fileName, "w");
        if (mlDataLog.fp[i] == NULL)
        {
            return 0;
        }
    }
    return 1;
}
/**********************************************************************/
/*                                                                    */
/* Function Name: mlDataLogClose                                      */
/*                                                                    */
/*   Description: Data log file close                                 */
/*                                                                    */
/*    Parameters:                                                     */
/*                                                                    */
/*       Returns:                                                     */
/*                                                                    */
/**********************************************************************/
void mlDataLogClose(void)
{
    char i;
    for (i=0; i<NUM_DIAG_FILES; i++)
    {
        if( mlDataLog.fp[i] )
        {
            fclose(mlDataLog.fp[i]);
        }
    }
}
/**********************************************************************/
/*                                                                    */
/* Function Name: mlDataLogEnable                                     */
/*                                                                    */
/*   Description: Enable data log function                            */
/*                                                                    */
/*    Parameters:                                                     */
/*                                                                    */
/*       Returns:                                                     */
/*                                                                    */
/**********************************************************************/
void mlDataLogEnable(void)
{
    if( mlDataLog.control == DATA_LOG_INIT )
    {
        mlDataLog.control = DATA_LOG_ENABLE;
    }
}
/**********************************************************************/
/*                                                                    */
/* Function Name: mlDataLogDisable                                    */
/*                                                                    */
/*   Description: Disable data log function                           */
/*                                                                    */
/*    Parameters:                                                     */
/*                                                                    */
/*       Returns:                                                     */
/*                                                                    */
/**********************************************************************/
void mlDataLogDisable(void)
{
    if( mlDataLog.control == DATA_LOG_PROCESS )
    {
        mlDataLog.control = DATA_LOG_DISABLE;
    }
}
/**********************************************************************/
/*                                                                    */
/* Function Name: mlDataLogQuat                                       */
/*                                                                    */
/*   Description: Output quaternion data to file                      */
/*                                                                    */
/*    Parameters:                                                     */
/*                                                                    */
/*       Returns:                                                     */
/*                                                                    */
/**********************************************************************/
void mlDataLogQuat(void)
{
    char i;
    FILE *outFile = mlDataLog.fp[0];

    fprintf(outFile, "%8.6d ", mlDataLog.index);
    fprintf(outFile, "%8.6d ", qData.packetCnt);

    for (i=0; i<4; i++)
    {
        fprintf(outFile, "%8.6f ",
                        qData.quat[i]);
    }

    for (i=0; i<4; i++)
    {
        fprintf(outFile, "0x%08x ",
                        qData.mlQuaternion[i]);
    }

    for (i=0; i<2; i++)
    {
        fprintf(outFile, "%12.8f ",
                        qData.angle[i]);
    }

    for (i=0; i<2; i++)
    {
        fprintf(outFile, "%12.8f ",
                        qData.gridLinear[i]);
    }

    fprintf(outFile, "\r\n");
}
/**********************************************************************/
/*                                                                    */
/* Function Name: mlDataLogMouse                                      */
/*                                                                    */
/*   Description: Output mouse data to file                           */
/*                                                                    */
/*    Parameters:                                                     */
/*                                                                    */
/*       Returns:                                                     */
/*                                                                    */
/**********************************************************************/
void mlDataLogMouse(void)
{
    char i;
    FILE *outFile = mlDataLog.fp[1];

    fprintf(outFile, "%8.6d ", mlDataLog.index);
    fprintf(outFile, "%8.6d ", qData.packetCnt);
    fprintf(outFile, "%5.3d ", qData.button);
    fprintf(outFile, "%5.3d ", qData.cursor.state);
    fprintf(outFile, "%8.6d ", qData.cursor.sampleCount);
    for (i=0; i<2; i++)
    {
        fprintf(outFile, "%12.8f ",
                        qData.gridLinear[i]);
    }

    for (i=0; i<2; i++)
    {
        fprintf(outFile, "%12.8f ",
                        qData.cursor.grid[i]);
    }

    for (i=0; i<2; i++)
    {
        fprintf(outFile, "%12.8f ",
                        qData.cursor.output[i]);
    }

    fprintf(outFile, "\r\n");
}
/**********************************************************************/
/*                                                                    */
/* Function Name: mlDataLogCursor                                     */
/*                                                                    */
/*   Description: Output cursor data to file                          */
/*                                                                    */
/*    Parameters:                                                     */
/*                                                                    */
/*       Returns:                                                     */
/*                                                                    */
/**********************************************************************/
void mlDataLogCursor(void)
{
    char i;
    FILE *outFile = mlDataLog.fp[2];

    fprintf(outFile, "%8.6d ", mlDataLog.index);
    fprintf(outFile, "%5.3d ", qData.button);
    fprintf(outFile, "%5.3d ", qData.cursor.state);
    fprintf(outFile, "%8.6d ", qData.cursor.sampleCount);
    for (i=0; i<2; i++)
    {
        fprintf(outFile, "%12.8f ",
                        qData.gridLinear[i]);
    }

    for (i=0; i<2; i++)
    {
        fprintf(outFile, "%12.8f ",
                        qData.cursor.grid[i]);
    }

    for (i=0; i<2; i++)
    {
        fprintf(outFile, "%5.3d ",
                        qData.cursor.outFx[i]);
    }

    for (i=0; i<2; i++)
    {
        fprintf(outFile, "%12.8f ",
                        qData.cursor.output[i]);
    }

    fprintf(outFile, "\r\n");
}
/**********************************************************************/
/*                                                                    */
/* Function Name: mlDataLogProcess                                    */
/*                                                                    */
/*   Description: Handle data log process                             */
/*                                                                    */
/*    Parameters:                                                     */
/*                                                                    */
/*       Returns:                                                     */
/*                                                                    */
/**********************************************************************/
void mlDataLogProcess(void)
{
    switch (mlDataLog.control)
    {
        case DATA_LOG_ENABLE:
            mlDataLogOpen();
            mlDataLog.control = DATA_LOG_PROCESS;
            break;
        case DATA_LOG_PROCESS:
            mlDataLog.index++;
            mlDataLogQuat();
            mlDataLogMouse();
            mlDataLogCursor();
            break;
        case DATA_LOG_DISABLE:
            mlDataLogClose();
            mlDataLog.control = DATA_LOG_INIT;
            break;
        case DATA_LOG_INIT:
        default:
            break;
    }
}
#endif
