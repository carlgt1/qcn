#ifndef _QCN_EARTH_H
#define _QCN_EARTH_H

// CMC earthquake map in opengl, parts lifted from:
// earth-simulation by Ohad Eder Pressman, 2001
// ohad@visual-i.com / ohad.visual-i.com
// also climateprediction.net project ref: Tolu Aina  tolu@atm.ox.ac.uk

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
#include "country.h"

// vector-file
struct VxpFeature {
        vector<Point3f> p3f;
};

class CEarth
{
  private:
    // constants
    static const int iMaxTextures; // note the static - so can use in array declaration size
    const GLsizei cxTexture;  // generated textures need to be powers of 2
    const GLsizei cyTexture;

    // variables
    //enum eTextureEarth { TEXTURE_DAY, TEXTURE_NIGHT, TEXTURE_MASK_NIGHT, TEXTURE_MASK, TEXTURE_MASK_INVERT };
    enum eTextureEarth { TEXTURE_DAY, TEXTURE_NIGHT, TEXTURE_MASK };
    //qcn_util::tImageJPG* ptJPG[2];   // JPG structs for the Day & Night Earth views
    //GLuint tidCombined;  // the texture id for the Combined day/night texture
    TEXTURE_DESC texture[3];   // BOINC/api/gutil.cpp TEXTURE_DESC JPG structs for the Day & Night Earth views

    SQuake* psqActive; // pointer to the active earthquake selected, if any
    vector<SQuake*> vpsqc; // a vector of pointers to SQuake's in the hit test
    int iCluster; // pointer to the last cluster of quakes, so we can cycle around via HitTest

    GLdouble modelview_matrix[16], projection_matrix[16];
    GLint    viewport[4];
    
    bool bProjection; // using 2D projection?
    Point3f ptEq;
    Point3f	vertices[EARTH_LON_RES+1][EARTH_LAT_RES+1];
    //Point3f	outer_vertices[EARTH_LON_RES+1][EARTH_LAT_RES+1];
    Mapping2f   mapping[EARTH_LON_RES+1][EARTH_LAT_RES+1];
    Point3f	country_names_pos[NUM_COUNTRY];
  
    /*
    GLUquadricObj* Q[4];
    GLuint m_textureID[4];   // a texture ID to map to a quad
    */

    bool bHaveMultitexture; // a flag to let us know if we can do multitexturing, i.e. for a nice mixed earth day/night view
    bool bShowSunLight;
    bool bMouseProcessed;
    GLdouble mouseX, mouseY;
    GLdouble mousePX, mousePY, mousePZ;
    int mouseLeftButtonDown, mouseRightButtonDown, isCtrlDown, isShiftDown;
    int iKey, iKeySpecial;

    float rotationSpeed;
    float autoRotX, autoRotY;
    float scaleAll;
    GLsizei m_iWidth, m_iHeight;
    GLfloat m_viewpoint;

    GLfloat coastline_rgb[4];

    bool bAutoRotate;
    unsigned int  uiAutoQuake;
    Point3f rot, auto_rot;

    int drawWireframe;
    int drawTexture;
    int drawCountries;
    int lineWidth;

    // Vxp structures for graphic coastline/plate boundary etc data
    int showVxp[MAX_VXP];
    vector<VxpFeature> vxp_data[MAX_VXP];

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
	
    #ifdef _DEBUG
        char strDebug[_MAX_PATH];
    #endif

  // private methods
    void DrawVxp();
    void DrawGrid();
    void DrawEarth();
    void DrawQuake(vector<SQuake>::iterator psq);
    void DrawQuakeSphere(const double dRadius, const int iLat, const int iLng);
    void Earth_2D();
    void Earth_3D();

    void Render2DProjection(int ii);
    void LoadEarthTexture();

    void GetSunriseSunset();
    void GetSunPosition(float& sub_solar_latitude, float& sub_solar_longitude);
    void GetSunLightSource();

    void SetViewpoint(double x = 0.0f, double y = 0.0f, double z = 1.0f);
    void OrthographicMatrix();

    void BindEarthTexture(int iSelect = -1);
    void MapEarthTexture(bool bMulti = false);
    void Idle();

    void InitGraphicStructs();
    void InitCoordinate(vector<VxpFeature>* vxp, char cType);
    void sprintGL(float x, float y, float z, int fontType, char * str, GLfloat * col); 
 
    void InitCamera();
    void InitLights();

    void LatLon2Point(const float lat, const float lon, Point3f& pos, const float pad = 0.002);

    void HitTestEarthquake(SQuake* psq, bool& bHaveActive);
    void GetMouseProjection();  // this gets the mousePXYZ proj on the globe

  public:
    CEarth();
    ~CEarth(); 
    
    void Cleanup();
    void Init();
    void Resize( GLsizei iWidth, GLsizei iHeight ); 
    void RenderScene( GLsizei iWidth, GLsizei iHeight, GLfloat viewpoint, GLfloat pitch, GLfloat roll ); 
    void RenderText();

    bool IsShown(); // is the earth view active/shown
    e_view ViewCombined();  // try and show the combined view if available, otherwise show the earth day view by default

    void SetTime(double dt = 0.0f);

    void MouseButton(int x, int y, int which, int is_down);
    void MouseMotion(int x, int y, int left, int middle, int right);
    void KeyDown(int key1, int key2);
    void KeyUp(int key1, int key2);

    //void SelectFromMenu(int idCommand);
    //int BuildPopupMenu (void);
    //void Keyboard(unsigned char key, int x, int y);

    void ShowSunLight(bool bShow = true);
    void SetMapNight();
    void SetMapDay();
    void SetMapCombined();
    void RecalculateEarthquakePositions();
	
    bool IsAutoRotate();
    void AutoRotate(bool bRotate = true);

    bool IsEarthquakeSelected();
    void EarthquakeSelectedString(char* strQuakeInfo, int iLen);
	
    void checkURLClick(bool bShift = false);
    void ResetEarthquakeNumber(); // start at the top (most recent) earthquake
};

#endif

