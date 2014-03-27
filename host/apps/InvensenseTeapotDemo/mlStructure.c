/* ------------------ */
/* - Include Files. - */
/* ------------------ */

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mlStructure.h"
#include "mlDataLog.h"

tQuatPacket qData;
/**********************************************************************/
/*                                                                    */
/* Function Name: MLQMult                                             */
/*                                                                    */
/*   Description: Quaternion multiplication                           */
/*                                                                    */
/*    Parameters: q1 - multiplicand                                   */
/*                q2 - multiplicator                                  */
/*                qProd - multiplication result                       */
/*                                                                    */
/*       Returns:                                                     */
/*                                                                    */
/**********************************************************************/
void MLQMult(float *q1, float *q2, float *qProd)
{
    qProd[0] = (q1[0] * q2[0] - q1[1] * q2[1] - q1[2] * q2[2] - q1[3] * q2[3]);
    qProd[1] = (q1[0] * q2[1] + q1[1] * q2[0] + q1[2] * q2[3] - q1[3] * q2[2]);
    qProd[2] = (q1[0] * q2[2] - q1[1] * q2[3] + q1[2] * q2[0] + q1[3] * q2[1]);
    qProd[3] = (q1[0] * q2[3] + q1[1] * q2[2] - q1[2] * q2[1] + q1[3] * q2[0]);
}
/**********************************************************************/
/*                                                                    */
/* Function Name: MLQInvert                                           */
/*                                                                    */
/*   Description: Calculate inverse of Quaternion                     */
/*                                                                    */
/*    Parameters: q - original quaternion                             */
/*                qInverted - inverse of quaternion                   */
/*                                                                    */
/*       Returns:                                                     */
/*                                                                    */
/**********************************************************************/
void MLQInvert(float *q, float *qInverted)
{
    qInverted[0] = q[0];
    qInverted[1] = -q[1];
    qInverted[2] = -q[2];
    qInverted[3] = -q[3];
}
/**********************************************************************/
/*                                                                    */
/* Function Name: IMUquaternionInit                                   */
/*                                                                    */
/*   Description: Data structure initialization                       */
/*                                                                    */
/*    Parameters: qData - data structure pointer                      */
/*                                                                    */
/*       Returns:                                                     */
/*                                                                    */
/**********************************************************************/
void IMUquaternionInit(tQuatPacket *qData)
{
    /* Write you code here to initialize any quaternion*/

#ifdef ML_DATA_LOG
    mlDataLogInit();
#endif
}

void IMUgetQuaternion(tQuatPacket *qData)
{
    int i;
    if( qData->packetCnt == qData->packetCntPrev )
    {
        return;
    }

    /* Do your quat processing if needed here for any applications*/

#ifdef ML_DATA_LOG
    mlDataLogProcess();
#endif

    for (i=0; i<4; i++)
    {
        qData->quatPrev[i] = qData->quat[i];
    }

}
