#ifndef _QCN_GAME_MATCH_H
#define _QCN_GAME_MATCH_H

// bouncing cube for QCN -- should have the plots on each face and size dependent on fmag
#ifdef _WIN32
#include "qcn_config_win.h"
#else
#include "config.h"
#endif
#include "define.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include "qcn_graphics.h"


class CGameMatch
{
private:
    // variables
    static const int MAX_PLOT_POINTS;
    static const int iMaxTextures;
    TEXTURE_DESC textureBackground;


    GLdouble modelview_matrix[16], projection_matrix[16];
    GLint    viewport[4];
    bool bProjection; // using 2D projection?
	
    bool bVar;
	bool bIsQCNLive;
    float fMean[4];
    float fM2[4];
    float fDelta[4];
    float fVariance[4];
    float fMin[4], fMax[4];
    float fStdDev[4];
	
    //GLUquadricObj* Q[6];
    //GLuint m_textureID[6];   // a texture ID to map to a quad
    //GLubyte m_face[6][64][64][3];
	
    bool bMouseProcessed;
    int mouseX, mouseY;
    GLdouble mousePX, mousePY, mousePZ;
    int mouseLeftButtonDown, mouseRightButtonDown, isCtrlDown, isShiftDown;
    int iKey, iKeySpecial;
    float fMouseFactor; // use thte mouse to control size of cube
	
    float rotationSpeed;
    float autoRotX, autoRotY;
    float scaleAll;
    GLsizei m_iWidth, m_iHeight;
    GLfloat m_viewpoint;
	
    bool bAutoRotate;
    unsigned int  uiAutoQuake;
    Point3f rot, auto_rot;
	
    int lineWidth;
	
    GLfloat ortho;
    GLfloat diffuse[4];
    GLfloat ambient[4];
    GLfloat specular[4];
    GLfloat light0_pos[4];
    GLfloat attenuation;
    GLfloat shine;
    Point3f sunshine;
	
    double m_dTime;
    struct tm m_tmTime;
	
	// private methods
    void SetViewpoint(double x = 0.0f, double y = 0.0f, double z = 1.0f);
    void OrthographicMatrix();
    void Idle();
	
    void InitCamera();
    void InitLights();
	
public:
    CGameMatch();
    ~CGameMatch(); 
    
    void Init();
    void Resize( GLsizei iWidth, GLsizei iHeight ); 
    void RenderScene( GLsizei iWidth, GLsizei iHeight, GLfloat viewpoint, GLfloat pitch, GLfloat roll ); 
    void RenderText();
    void RenderCube(const GLfloat* asize);
	
    void MouseButton(int x, int y, int which, int is_down);
    void MouseMotion(int x, int y, int left, int middle, int right);
    void KeyDown(int key1, int key2);
    void KeyUp(int key1, int key2);
	
};

#endif

