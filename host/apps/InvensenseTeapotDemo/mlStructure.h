#ifndef ML_MOUSE_H
#define ML_MOUSE_H

#ifdef __cplusplus
extern "C"
{
#endif

#define mEULAR_ANGLE_Z_MOUSE(q)	( atan2( 1-2*(q[1]*q[1]+q[2]*q[2]),  2*(q[1]*q[3]-q[0]*q[2])) )
#define mEULAR_ANGLE_Y_MOUSE(q)	( asin( 2*(q[0]*q[1]+q[2]*q[3]) ) )
#define mEULAR_ANGLE_X_MOUSE(q)	( atan2( 1-2*(q[1]*q[1]+q[3]*q[3]), 2*(q[1]*q[2]-q[0]*q[3]) ) )

enum _cursor_state 
{
    CURSOR_INIT = 0,
    CURSOR_IDLE = 1,
    CURSOR_POINTING =2,
    CURSOR_POINTING_BUTTON =21,
    CURSOR_POINTING_DOUBLE_CLICK =22,
    CURSOR_MOVING_READY =32,
    CURSOR_MOVING =30,
    CURSOR_DRAWING =50,
};


typedef struct
{
    float grid[2];
    float gridChange[2][2];
    float output[2];
    short outFx[2];

    unsigned char state;
    unsigned char ButtonPrev;
    unsigned int sampleCount;
    unsigned int ButtonPushTimeThreshold;
    unsigned int DoubleClickTimeThreshold;
    unsigned int PointingDetectTimeThreshold;
    float HandJitterEnterThreshold;
    float HandJitterExitThreshold;
} tCursorData;

typedef struct
{
    long mlQuaternion[4];
    float quat[4];
    float quatPrev[4];
    float angle[3];
    float grid[3][2];
    float gridChange[3];
    float gridLinear[3];
    float gridLinearAvg[3];
    float gridGain[3];
    float pitchSin;
    unsigned char button;
    unsigned char packetCnt;
    unsigned char packetCntPrev;
    unsigned char buttonTopBit;
    unsigned char buttonTopBitPrev;
    tCursorData cursor;
} tQuatPacket;

void IMUgetQuaternion (tQuatPacket *qData);
void IMUquaternionInit(tQuatPacket *qData);

#ifdef __cplusplus
}
#endif

#endif