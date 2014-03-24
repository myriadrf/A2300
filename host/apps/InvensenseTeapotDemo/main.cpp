/*******************************************************************************
* Copyright (c) 2008 InvenSense Corporation, All Rights Reserved.
*******************************************************************************/

#include <windows.h>
#include <iostream>
#include "math.h"
#include "conio.h"
#include "main.h"
#include "mlStructure.h"
#include "Asr2300Io.h"
#ifdef ML_DATA_LOG
#include "mlDataLog.h"
#endif

#include <gl.h>         // Header File For The OpenGL32 Library
#include <glu.h>        // Header File For The GLu32 Library
#include <glaux.h>
#include <glut.h>

#define VERSION "v1.01"
#define R2D     (57.2957795131)     //conversion parameter for radian to degree
#define mEULAR_ANGLE_Z_FROM_QUAT_YXZ_CONVNETION(q)  ( atan2( -2*(q[1]*q[2]-q[0]*q[3]), 1-2*(q[1]*q[1]+q[3]*q[3]) ) )

/* ------------ */
/* - Globals. - */
/* ------------ */

char flg_ResetRotate = 0;
MLFLT ResetQuat[4] =
{
    1, 0, 0, 0
};
extern "C" tQuatPacket qData;

 tQuatPacket qData;

/* -------------- */
/* - Functions. - */
/* -------------- */

/*
 For 32-bit signed integers.
*/
float four_bytes(byte d1,byte d2,byte d3,byte d4)
    {
    	 float  d = d1*(1<<24) + d2*(1<<16) + d3*(1<<8) + d4;
    	 if (d > 2147483648)
    		 d-= 4294967296;
    return d;
    }


/**********************************************************************/
/*                                                                    */
/* Function Name: DemoKeyboardUpFunc                                  */
/*                                                                    */
/*   Description:  Keyboard up callback function                      */
/*                                                                    */
/*    Parameters: key - keyboard ASCII character                      */
/*                x - mouse location in window x-axis                 */
/*                y - mouse location in window y-axis                 */
/*                                                                    */
/*       Returns:                                                     */
/*                                                                    */
/**********************************************************************/
void DemoKeyboardUpFunc(unsigned char key, int x, int y)
{
    //printf(">>>>>>handsetDemoKeyboardUP>>>> %c\n", key);
    switch( key )
    {
    case '8':
        break;
    case '2':
        break;
    case '4':
        break;
    case '6':
        break;
    case 0x0d: //'Enter'
        flg_ResetRotate = 1;
        printf("Re-align the sensor board and clear any calibration flag\n");
        break;
    case 0x08: //'Backspace'
        break;
#ifdef ML_DATA_LOG
    case 'l':  // enable sensor fusion datalogger
        mlDataLogEnable();
        printf("Sensor fusion data logger is running!\n");
        printf("Press key k to stop\n");
        break;
    case 'k':  // enable sensor fusion datalogger
        mlDataLogDisable();
        printf("Sensor fusion data is ready!\n");
        break;
#endif
    default:
        break;
    }
} // end of handsetDemoKeyboard

/**********************************************************************/
/*                                                                    */
/* Function Name: TeaPotDisplay                                       */
/*                                                                    */
/*   Description:  OpenGL display Tea Pot                             */
/*                                                                    */
/*    Parameters:                                                     */
/*                                                                    */
/*       Returns:                                                     */
/*                                                                    */
/**********************************************************************/
void TeaPotDisplay(void)
{
    GLfloat flt;

    //the light position at (0, 0, 5)
    float lightpos[] =
    {
        0.0, 0.0, 5.0, 0.0
    };

    //a diffuse light source with white light
    float light_diffuse[4] =
    {
        1.0, 1.0, 1.0, 1.0
    };

    //keep the qData.quat into ResetQuat
    //ResetQuat can help adjust the direction of teapot 
    if( flg_ResetRotate == 1 ) // if aligment button was pressed, the direction reset
    {
        flg_ResetRotate = 0;
        memcpy(ResetQuat, qData.quat, sizeof(ResetQuat));
    }

    glDepthMask(true);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    //give a diffuse light source with white light and at (0, 0, 5)
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
    glEnable(GL_LIGHT0);

    glShadeModel(GL_SMOOTH);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    //set the Projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //use perspective view
    gluPerspective(45.0f, 1.0f, 0.1f, 100.0f);

    //set the ModelView matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //set the observation position
    gluLookAt( 0, 0, 3, 0, 0, 0, 0, 1, 0);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor4f(0.7f, 0.7f, 1.0f, 1.0f);

    //----------------------------------------------------------------------------------
    //The direction of coordinate systems for OpenGL and Invensense are different
    //In OpenGL, the X axis is toward right on Screen, the Y axis is toward up
    //and the Z axis is from the center of the screen towards the viewer
    //In Invensesen definition, the X axis is toward right on motion device, the Y axis is toward front
    //and the Z axis is toward up
    //So, we have to transfer the direction of Invensense coordinate system to OpenGL system

    //In order to transfer coordinate system, have to choose a desired direction of the motion devie at first.
    //Assume the motino device puts on the table which is perpendicular with the screen, and the front of the motion device is toward screen
    //So, can have the following relation    
    //    Invensense              transfer result              OpenGL
    //        X(right)                  X(right)                 X(right)
    //        Y(to screen)              Z(up)                    Y(up)
    //        Z(up)                     -Y(from screen)          Z(from screen)
    //----------------------------------------------------------------------------------

    //the quaternion have four elements
    //the first element, quat[0], represents a rotation angle after using acos()
    //the remaining elememts quat[1], quat[2], quat[3] represnet the the rotation axis of X-Y-Z coordinate system

    //rotate teapot along Y axis, the rotation angle depends on ResetQuat which is used to adjust the direction
    glRotatef(-mEULAR_ANGLE_Z_FROM_QUAT_YXZ_CONVNETION(ResetQuat) * R2D, 0, 1, 0);

    //compute the rotation angle from quat[0] 
    flt = (GLfloat) acos(qData.quat[0]);

    //void glRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z); 
    //rotate object "angle" degrees along (x, y, z)
    //should transfer the Invensense coordinate system to OpenGL coordinate system for the rotation axis (x,y,z)
    //OpenGL X = Invensense X <=> quat[1]
    //OpenGL Y = Invensense Z <=> quat[3]
    //OpenGL Z = Invensense -Y <=> -quat[2]
    glRotatef((GLfloat)(2* flt * 180 / 3.1415), (GLfloat) qData.quat[1],
        (GLfloat) qData.quat[3], (GLfloat) - qData.quat[2]);

    //rotate teapot 90 degree along Y axis, let the direction of spout equal the front of the motion device
  //  glRotatef(90, 0, 1, 0); //MBM-Commented out to show side profile of spout.

    //draw a teapot by OpengGL library
    glutSolidTeapot(0.6);
}
/**********************************************************************/
/*                                                                    */
/* Function Name: ReshapeWindowSize                                   */
/*                                                                    */
/*   Description:  OpenGL reshape callback                            */
/*                                                                    */
/*    Parameters: w - width                                           */
/*                h - height                                          */
/*                                                                    */
/*       Returns:                                                     */
/*                                                                    */
/**********************************************************************/
void ReshapeWindowSize(int w, int h)
{
    glViewport(0, 0, w, h);
}
/**********************************************************************/
/*                                                                    */
/* Function Name: DemoDisplayMain                                     */
/*                                                                    */
/*   Description:  OpenGL display callback                            */
/*                                                                    */
/*    Parameters:                                                     */
/*                                                                    */
/*       Returns:                                                     */
/*                                                                    */
/**********************************************************************/
#ifndef DISABLE_TEAPOT
void DemoDisplayMain(void)
{
    TeaPotDisplay();
    glFlush ();
    glutSwapBuffers();
    Sleep(4);
}
#endif

/**********************************************************************/
/*                                                                    */
/* Function Name: DemoDisplayIdle                                     */
/*                                                                    */
/*   Description:  OpenGL global idle callback                        */
/*                                                                    */
/*    Parameters:                                                     */
/*                                                                    */
/*       Returns:                                                     */
/*                                                                    */
/**********************************************************************/
void DemoDisplayIdle(void)
{
    glutPostRedisplay();
}
/**********************************************************************/
/*                                                                    */
/* Function Name: DemoInit                                            */
/*                                                                    */
/*   Description:  OpenGL initialization                              */
/*                                                                    */
/*    Parameters:                                                     */
/*                                                                    */
/*       Returns:                                                     */
/*                                                                    */
/**********************************************************************/
void DemoInit(void)
{
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glDepthFunc(GL_LESS); //  # The Type Of Depth Test To Do
    glEnable(GL_DEPTH_TEST); //  # Enables Depth Testing
    glLoadIdentity(); //  # Reset The Projection Matrix

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
}// end of DemoInit

/**********************************************************************/
/*                                                                    */
/* Function Name: main                                                */
/*                                                                    */
/*   Description:                                                     */
/*                                                                    */
/*    Parameters:                                                     */
/*                                                                    */
/*       Returns:                                                     */
/*                                                                    */
/**********************************************************************/
void main(int argc, char *argv[])
{
    printf("InvenSense Tea Pot App %s %s %s\r\n", VERSION, __DATE__, __TIME__);

    if (ASR2300SensorProcessStart()!= 0)
    {
        MessageBox(NULL, TEXT("Unable to start ASR 2300 processing. Check connection."),
            TEXT("Error"), MB_OK);
    }
    else
    {
#ifndef DISABLE_TEAPOT
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

        glutInitWindowSize(480,480);
        glutInitWindowPosition (500, 0);
        glutCreateWindow("InvenSense Tea Pot Demo");

        DemoInit();
#endif
        //IMUquaternionInit(&qData);
#ifndef DISABLE_TEAPOT
        glutDisplayFunc(DemoDisplayMain);
        glutKeyboardUpFunc(DemoKeyboardUpFunc);
        glutReshapeFunc(ReshapeWindowSize);
        glutIdleFunc(DemoDisplayIdle);
        glEnable(GL_BLEND);
        glutMainLoop();
#else
        while(1)
        Sleep(4);
        
#endif
    }

}   // end of main()

