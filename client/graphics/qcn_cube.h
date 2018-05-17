#ifndef _QCN_CUBE_H
#define _QCN_CUBE_H

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

/*
#ifndef GL_EXT_texture_cube_map 
#define GL_EXT_texture_cube_map             1 
#define GL_NORMAL_MAP_EXT                   0x8511 
#define GL_REFLECTION_MAP_EXT               0x8512 
#define GL_TEXTURE_CUBE_MAP_EXT             0x8513 
#define GL_TEXTURE_BINDING_CUBE_MAP_EXT     0x8514 
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT  0x8515 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT  0x8516 
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT  0x8517 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT  0x8518 
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT  0x8519 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT  0x851A 
#define GL_PROXY_TEXTURE_CUBE_MAP_EXT       0x851B 
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE_EXT    0x851C 
#endif
*/

class CCube
{
  private:
    // variables
    enum eCubeFace { CUBE_TOP, CUBE_BOTTOM, CUBE_FRONT, CUBE_BACK, CUBE_LEFT, CUBE_RIGHT };
    static const int iMaxTextures;
    static const int MAX_PLOT_POINTS;
    TEXTURE_DESC earthTexture;
    SQuake* psqActive; // pointer to the active earthquake selected, if any
    vector<SQuake*> vpsqc; // a vector of pointers to SQuake's in the hit test
    int iCluster; // pointer to the last cluster of quakes, so we can cycle around via HitTest

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
    CCube();
    ~CCube(); 
    
    void Init();
    void Resize( GLsizei iWidth, GLsizei iHeight ); 
    void RenderScene( GLsizei iWidth, GLsizei iHeight, GLfloat viewpoint, GLfloat pitch, GLfloat roll ); 
    void RenderText();
    void DrawPlot(const GLfloat* pfEnd, const GLfloat* asize, const eCubeFace face);
    void RenderCube(const GLfloat* asize);

    void MouseButton(int x, int y, int which, int is_down);
    void MouseMotion(int x, int y, int left, int middle, int right);
    void KeyDown(int key1, int key2);
    void KeyUp(int key1, int key2);

};

#endif

