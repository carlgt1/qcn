// earthquake map in opengl as a C++ class
// some C parts lifted from: 
// earth-simulation by Ohad Eder Pressman, 2001
// ohad@visual-i.com / ohad.visual-i.com
// and of course climateprediction.net / Tolu Aina <tolu.aina@oerc.ox.ac.uk>

#ifdef _WIN32
  #pragma warning( disable : 4244 )  // Disable warning messages for double to float conversion
  #pragma warning( disable : 4800 )  // Disable warning messages for "forcing value to bool"
#endif

// define or undef to allow the GL_ARB OpenGL extensions for multitexturing
// this looks cool but may not be supported nicely on many cards, so safest now to disable
//#define _EARTH_COMBINE_

#include "qcn_graphics.h"
#include "qcn_earth.h"
#include "qcn_shmem.h"

// coastline & boundary info now moved into static h/cpp files
#include "nation_boundary.h"
#include "country.h"
#include "coastline.h"
#include "plate_boundary.h"

#include "filesys.h"   // for boinc_file_exists fn

// setup OpenGL 1.3 extensions - required for multitexturing
#ifdef _EARTH_COMBINE_
#ifdef _WIN32
  #include "glext.win.h"
  PFNGLACTIVETEXTUREARBPROC          glActiveTextureARB          = NULL;
  PFNGLMULTITEXCOORD2FVARBPROC       glMultiTexCoord2fvARB       = NULL;
#endif
#endif // earth_combine

#ifdef _EARTH_COMBINE_
const int CEarth::iMaxTextures = 3; 
#else
const int CEarth::iMaxTextures = 2; 
#endif

// constructor
CEarth::CEarth()  
   : cxTexture(TEXTURE_X), cyTexture(TEXTURE_Y)
{ 
       ptEq.x = ptEq.y = ptEq.z = 0;
       mouseX = mouseY = -1.0;
       mousePX = mousePY = mousePZ = -1.0;
       iKey = iKeySpecial = mouseRightButtonDown = isShiftDown = mouseLeftButtonDown = isCtrlDown = 0;
       rot.x = 100, rot.y = 0;  // note rot.x is our startup angle
       psqActive = NULL; 
       iCluster = -1;

       bHaveMultitexture = false;
       bShowSunLight = false;

       bMouseProcessed = false;
       bAutoRotate = true;
       uiAutoQuake = 0;
       autoRotX = 0, autoRotY = 0;
       scaleAll = 1.8;
       drawWireframe = 0;
       drawTexture = 1;
       drawCountries = 0;
       lineWidth = 1;

       memset(&modelview_matrix, 0x00, 16 * sizeof(GLdouble));
       memset(&projection_matrix, 0x00, 16 * sizeof(GLdouble));
       memset(&viewport, 0x00, 4 * sizeof(GLint));

       showVxp[NATION] = 1;        // show nation boundaries
       showVxp[PLATE]  = 1;        // show plate boundaries
       showVxp[COUNTRY]  = 0;      // show countries
       showVxp[COASTLINE]  = 1;    // show coastline

       m_dTime = 0.0;

       m_iWidth = 640; 
       m_iHeight = 480; 

       rotationSpeed = ROTATION_SPEED_DEFAULT;

       diffuse[0] = 0.8, diffuse[1] = 0.8, diffuse[2] = 0.8, diffuse[3] = 1.0;
       ambient[0] = 0.4, ambient[1] = 0.4, ambient[2] = 0.4, ambient[3] = 1.0;
       specular[0] = 0.0, specular[1] = 0.0, specular[2] = 0.0, specular[3] = 1.0;

       light0_pos[0] = 0.0, light0_pos[1] = 0.0, light0_pos[2] = LIGHT_INFINITY, light0_pos[3] = 1.0;

       attenuation  =  1.f;
       shine        =  1.f;
       bProjection = false;
       ortho = 1.0f;
    
       // Q[0] = Q[1] = Q[2] = Q[3] = NULL;

       #ifdef _DEBUG
          memset(strDebug, 0x00, sizeof(char)*_MAX_PATH);
       #endif

       memset(texture, 0x00, 3 * sizeof(TEXTURE_DESC));
}

void CEarth::Cleanup()
{
   for (int i = 0; i < iMaxTextures; i++) {
       if (texture[i].id || texture[i].present) { // free earth day texture if required
          glDeleteTextures(1, (const GLuint*) &(texture[i].id));
		  texture[i].id = 0;
       }
    }

    // probably not necessary to clear out std::vectors
    vector<VxpFeature>::iterator itFeat;
    //vector<Point3f>::iterator itVect;
    for (int i = 0; i < MAX_VXP; i++) { // have to delete dynamically allocated structures within vxp, then the vxp_data point itself!
       for (itFeat = vxp_data[i].begin(); itFeat < vxp_data[i].end(); itFeat++) {
          itFeat->p3f.clear();
       }
       vxp_data[i].clear();
    }
}

// destructor
CEarth::~CEarth()
{
   Cleanup();
}

void CEarth::ShowSunLight(bool bShow)
{ // show sun lighting on the earth view (only the day view)
   bShowSunLight = bShow;
}

bool CEarth::IsShown() // is the earth view active/shown
{
   return (bool)(qcn_graphics::g_eView == VIEW_EARTH_DAY || qcn_graphics::g_eView == VIEW_EARTH_NIGHT || qcn_graphics::g_eView == VIEW_EARTH_COMBINED);
}

e_view CEarth::ViewCombined()  // try and show the combined view if available, otherwise show the earth day view by default
{ // checks for multitexture capability
   return bHaveMultitexture ? VIEW_EARTH_COMBINED : VIEW_EARTH_DAY;
}

void CEarth::SetMapCombined()
{ // note only change if it's on night, as Init would have defaulted to day, and they may have hit 'E' anyway
   if (bHaveMultitexture && qcn_graphics::g_eView == VIEW_EARTH_COMBINED) return;
   qcn_graphics::g_eView = ViewCombined(); // checks that we have multitexture capability
}

void CEarth::SetMapDay()
{ // note only change if it's on night, as Init would have defaulted to day, and they may have hit 'E' anyway
   if (qcn_graphics::g_eView == VIEW_EARTH_DAY) return;
   qcn_graphics::g_eView = VIEW_EARTH_DAY;
}

void CEarth::SetMapNight()
{ // note only change if it's on night, as Init would have defaulted to day, and they may have hit 'E' anyway
   if (qcn_graphics::g_eView == VIEW_EARTH_NIGHT) return;
   qcn_graphics::g_eView = VIEW_EARTH_NIGHT;
}

void CEarth::Resize( GLsizei iWidth, GLsizei iHeight )
{ 
        m_iWidth = iWidth;
        m_iHeight = iHeight;

        //glViewport(0, 0, m_iWidth, m_iHeight);
        //glViewport(200, 200, m_iWidth-200, m_iHeight-200);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, (GLfloat) m_iWidth / (GLfloat) m_iHeight, 1.0, 100.0);

        // orthographic projection

        ortho =  static_cast<GLfloat> (m_iHeight) / static_cast<GLfloat> (m_iWidth);
        if ( m_iWidth <= m_iHeight ){
                glOrtho(-2.0, 2.0, -2.0 * ortho, 2.0 * ortho, -10.0, 10.0);
        }else{
                glOrtho(-2.0 / ortho, 2.0 / ortho, -2.0, 2.0, -10.0, 10.0);
        }

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
}

void CEarth::Idle()
{
}

void CEarth::LoadEarthTexture()
{
   char strImg[_MAX_PATH];
#ifdef _EARTH_COMBINE_
    const char* pstrEarth[iMaxTextures] = { IMG_EARTH_DAY, IMG_EARTH_NIGHT, IMG_EARTH_MASK };
#else
    const char* pstrEarth[iMaxTextures] = { IMG_EARTH_DAY, IMG_EARTH_NIGHT };
#endif

    // common texture settings
    for (int i = 0; i < iMaxTextures; i++) {
#ifdef QCNLIVE
            strcpy(strImg, pstrEarth[i]);
#else
            boinc_resolve_filename(pstrEarth[i], strImg, sizeof(char)*_MAX_PATH);
#endif
            if (!boinc_file_exists(strImg)) {
                fprintf(stderr, "Graphics file %s does not exist\n", strImg);
                continue;
            }
#ifdef _DEBUG
            else {
                fprintf(stdout, "Graphics file %s loading...\n", strImg);
            }
#endif
            if (i<2) { // first two earth views are JPG textures, not RGB alpha map which is handled below
                texture[i].load_image_file(strImg);
            }
            else {     // masks are RGB, but use my special "RGB to Alpha" convertor function in qcn_util
                //texture[i].load_image_file(strImg);
                texture[i].id = qcn_util::CreateRGBAlpha(strImg);
            }
            if (!texture[i].present)  {
                fprintf(stderr, "Could not load %s image file\n", strImg);
                bHaveMultitexture = false;
            }
#ifdef _DEBUG
            else {
                fprintf(stdout, "Loaded OpenGL Texture from %s\n", strImg);
            }
#endif

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    }
}

void CEarth::Init()
{

#ifdef _EARTH_COMBINE_
 // look for OpenGL extensions - in particular the GL ARB_multitexture
#ifdef _WIN32
    glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
    glMultiTexCoord2fvARB = (PFNGLMULTITEXCOORD2FVARBPROC) wglGetProcAddress("glMultiTexCoord2fvARB");
    bHaveMultitexture = (bool) (glActiveTextureARB && glMultiTexCoord2fvARB);
#else
    bHaveMultitexture = true; // default to multitexturing (hopeful!)
    const char* strExtensions =  (const char*) glGetString(GL_EXTENSIONS);
    if (strstr(strExtensions, "GL_ARB_multitexture") == 0) {
        fprintf(stderr, "OpenGL GL_ARB_multitexture extensions not found.\n");
        bHaveMultitexture = false;
    }
    //fprintf(stdout, "Available OpenGL extensions: %s\n", strExtensions);
    GLint nNumTextureUnits = 0;
    glGetIntegerv( GL_MAX_TEXTURE_UNITS_ARB, &nNumTextureUnits );
    if (nNumTextureUnits < iMaxTextures) { // need three textures at least - one mask & two earth pics
        fprintf(stderr, "Insufficient texture resources available for OpenGL ARB multitexture\n");
        bHaveMultitexture = false;
    }
#endif  // WIN32 proc addresses or Mac/Linux ARB extensions available
#endif  // GL_ARB_EXT

    bAutoRotate = true;
		
    // enable hidden-surface-removal
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

//  setup sphere textures to wrap around the base sphere (earth map)
/*
        for (int i = 0 ; i < iMaxTextures ; i++) {
#ifndef __APPLE_CC__
                if (!Q[i])
#endif
                {
                        Q[i] = gluNewQuadric();
                        gluQuadricDrawStyle(Q[i], GLU_FILL);
                        gluQuadricNormals  (Q[i], GLU_SMOOTH);
                        gluQuadricTexture  (Q[i], GL_TRUE);
                }
        }
        for (int i = 0 ; i < iMaxTextures; i++ )
                glGenTextures(1, &m_[i]);
*/

	// generate our sphere -- we'll map the texture on to it later
	for (int x = 0; x <= EARTH_LON_RES; x++) {
		for (int y = 0; y <= EARTH_LAT_RES; y++) {
			float	angX, angY;

			angX = ((float) x * 360.f / (float) EARTH_LON_RES) * PI / 180.f;
			angY = (90.0f + ((float) y * 180.f / (float) EARTH_LAT_RES)) * PI / 180.f;

			vertices[x][y].x = fabsf(cosf(angY)) * EARTH_RADIUS * sinf(angX);
			vertices[x][y].y = EARTH_RADIUS * sinf(angY);
			vertices[x][y].z = fabsf(cosf(angY)) * EARTH_RADIUS * cosf(angX);

			mapping[x][y].u = (float) x / (float) EARTH_LON_RES;
			mapping[x][y].v = (float) y / (float) EARTH_LAT_RES;
		}
	}

     InitGraphicStructs();
     LoadEarthTexture();
}

void CEarth::Earth_2D()
{
                //if (show_panel) glScalef(SHOW_PANEL_W_FACTOR,1.0,0.0);
                glBegin(GL_POLYGON);
                glTexCoord2f(1.f,.0f);
                glVertex3f(-2.3f,-1.05f,0.5);
                glTexCoord2f(0.f,.0f);
                glVertex3f(2.3f,-1.05f,0.5);
                glTexCoord2f(0.f,1.f);
                glVertex3f(2.3f,1.05f,0.5);
                glTexCoord2f(1.f,1.f);
                glVertex3f(-2.3f,1.05f,0.5);
                glEnd();

        glPushMatrix();
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glEnable(GL_DEPTH_TEST);
        glScalef(0.0128,0.0117,0.0);
        glColor4fv(white);
        glLineWidth(0.1);

        //if (scene == RELIEF_SCENE || scene == CLOUD_SCENE){
                glPopMatrix();
                return;
        //}
/*
                // 2 outline rect
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glRectf(-180.f, 90.f, 180.f, -90.f);
                glRectf(-181.f, 91.f, 181.f, -91.f);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glEnable(GL_LINE_STIPPLE);
                glLineStipple(1,0x0101);    //  dotted
                glBegin(GL_LINES);

                glEnd();
                glDisable(GL_LINE_STIPPLE);
                glLineWidth(1.5);
                // longitude - horizontal
                glBegin(GL_LINES);
                glVertex2d(-180.5,90.);
                glVertex2d(-180.5,60.);
                glVertex2d(+180.5,60.);
                glVertex2d(+180.5,30.);
                glVertex2d(-180.5,30.);
                glVertex2d(-180.5,0.);
                glVertex2d(+180.5,0.);
                glVertex2d(+180.5,-30.);
                glVertex2d(-180.5,-30.);
                glVertex2d(-180.5,-60.);
                glVertex2d(+180.5,-60.);
                glVertex2d(+180.5,-90.);
                glEnd();
                glBegin(GL_LINES);
                glVertex2d(-135.0,90.5);
                glVertex2d(-90,90.5);
                glVertex2d(-180.,-90.5);
                glVertex2d(-135.,-90.5);
                glVertex2d(-45.0,90.5);
                glVertex2d(-0,90.5);
                glVertex2d(-90.0,-90.5);
                glVertex2d(-45.0,-90.5);
                glVertex2d(45.0,90.5);
                glVertex2d(90,90.5);
                glVertex2d(0.,-90.5);
                glVertex2d(45,-90.5);
                glVertex2d(135.0,90.5);
                glVertex2d(180,90.5);
                glVertex2d(90.,-90.5);
                glVertex2d(135.,-90.5);
                glEnd();

        for ( i = 0 ; i < 5; i++ ) {
                if ( nation_boundaries && i == 4 )
                        Render2DProjection(i);
                else if (i != 4)
                        Render2DProjection(i);
        }
        glPopMatrix();
*/

}

void CEarth::Render2DProjection(int ii)
{
/*
        glPushMatrix();
        //if (show_panel) glScalef(SHOW_PANEL_W_FACTOR,1.0,0.0);
        glRotatef(180,0.0,1.0,0.0);
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_DEPTH_TEST);
        glLineWidth(lineWidth);
        //if ( !use_default  && iCoastline > 0 && iCoastline <= NUM_RGB )
        //        glColor3d(rgb[iCoastline-1].r,rgb[iCoastline-1].g,rgb[iCoastline-1].b);
        //else
                glColor3f(coastline_rgb[0], coastline_rgb[1], coastline_rgb[2]);
        int i, limit;
        switch(ii){
        case 0: limit = NUM_BDY_EUROPE;  break;
        case 1: limit = NUM_BDY_AFRICA;  break;
        case 2: limit = NUM_BDY_AMERICA; break;
        case 3: limit = NUM_BDY_ASIA;    break;
        case 4: limit = NUM_NATION_BOUNDARIES;
        }
        glBegin(GL_LINE_STRIP);
        for ( i = 0; i < limit; i++ ){
                switch(ii){
                case 0: if ( bdy_europe[i].lat == -1 && bdy_europe[i].lon == -1 && i++){
                                        glEnd();
                                        glBegin(GL_LINE_STRIP);
                                }
                                glVertex2f(bdy_europe[i].lat, bdy_europe[i].lon);
                                break;
                case 1: if ( bdy_africa[i].lat == -1 && bdy_africa[i].lon == -1 && i++){
                                        glEnd();
                                        glBegin(GL_LINE_STRIP);
                                }
                                glVertex2f(bdy_africa[i].lat, bdy_africa[i].lon);
                                break;
                case 2: if ( bdy_americas[i].lat == -1 && bdy_americas[i].lon == -1 && i++){
                                        glEnd();
                                        glBegin(GL_LINE_STRIP);
                                }
                                glVertex2f(bdy_americas[i].lat, bdy_americas[i].lon);
                                break;
                case 3: if ( bdy_asia[i].lat == -1 && bdy_asia[i].lon == -1 && i++){
                                        glEnd();
                                        glBegin(GL_LINE_STRIP);
                                }
                                glVertex2f(bdy_asia[i].lat, bdy_asia[i].lon);
                               break;
                case 4: if ( nation[i].lat == -1 && nation[i].lon == -1 && i++ ){
                                        glEnd();
                                        glBegin(GL_LINE_STRIP);
                                }
                                glVertex2f(nation[i].lat,nation[i].lon);
                }
                if (i == limit-1) glEnd();
        }
        glEnd();
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glPopMatrix();
*/
}

void CEarth::Earth_3D()
{
}

void CEarth::DrawQuake(vector<SQuake>::iterator psq)
{
  GLfloat quake_color[4];
  GLfloat* pfcolor;
  switch (psq->eType)  {
    case QUAKE_CURRENT:
      pfcolor = red;
      break;
    case QUAKE_WORLD85:
      pfcolor = cyan;
      break;
    default:
      pfcolor = white;
  }

  glPushMatrix();
  psq->radius = psq->magnitude/QUAKE_MAGNITUDE_FACTOR;  //300.0f;  // note that the size of the sphere relates to the magnitude of the quake

  if (psq->bActive) {
    Point3f pt;
    LatLon2Point(psq->latitude, psq->longitude, pt, .007); // note the extra padding so the hemisphere is on top all others
    glTranslatef(pt.x, pt.y, pt.z);
    memcpy(quake_color, yellow, sizeof(GLfloat) * 4);
  }
  else {
    glTranslatef(psq->v.x, psq->v.y, psq->v.z);
    memcpy(quake_color, pfcolor, sizeof(GLfloat) * 4);
    //quake_color[0] = 1.0 - (.8 * ( (GLfloat) psq->num / (GLfloat) qcn_graphics::vsq.size()) );  // newer quakes should appear brighter than older ones
    quake_color[3] = 0.9 - (.4 * ( (GLfloat) psq->num / (GLfloat) qcn_graphics::vsq.size()) );  // newer quakes should appear brighter than older ones
  }

  // now draw the quake!
  glColor4fv(quake_color);
  glShadeModel(GL_SMOOTH);
  DrawQuakeSphere(psq->radius, 10, 10); 
 
  glPopMatrix();

/*
  if (psq->bActive) { // draw an arrow pointing to quake
    glPushMatrix();

    Point3f pt;
    LatLon2Point(psq->latitude, psq->longitude, pt, .005); // note the extra padding so the arrow isn't buried in the hemisphere
    glTranslatef(pt.x, pt.y, pt.z);


    //    glRotatef(lightAngle * -180.0 / M_PI, 0, 1, 0);
    //    glRotatef(atan(lightHeight/12) * 180.0 / M_PI, 0, 0, 1);
    glColor4fv(yellow);
    
    //glRotatef(rot.x, 0.0, 1.0, 0.0);
    //glRotatef(rot.y, 0.0, 0.0, 1.0);
    glRotatef(-180.0+rot.x, 0.0, 1.0, 0.0);
    glRotatef(rot.y, 0.0, 0.0, 1.0);

    const GLfloat vx = 0.04, vy = 0.02;
    glBegin(GL_TRIANGLE_FAN);
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(vx, vy, vy);
      glVertex3f(vx, -vy, vy);
      glVertex3f(vx, -.01, -vy);
      glVertex3f(vx, vy, -vy);
      glVertex3f(vx, vy, vy);
    glEnd();

    // white line
    glColor3fv(white);
    glBegin(GL_LINES);
      glVertex3f(0.0, 0.0, 0.0);
      glVertex3f(vx*2.0, 0.0, 0.0);
    glEnd();

    glPopMatrix();
  }
*/

}

void CEarth::DrawEarth()
{
        // the glClear is done in the qcn_graphics.cpp file
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        InitCamera();

        diffuse[0] = diffuse[1] = diffuse[2] = 0.8 + scaleAll;
        specular[0] = specular[1] = specular[2] = 1.0;

        glPushMatrix();   // light matrix
        OrthographicMatrix();
        glRotatef(180.0, 0.0, 1.0, 0.0);

        // light is in fixed position so has its own matrix
        InitLights();

        glTranslatef(-.50, 0.0, 0.0);

        static double dBlinker = 1.0;  // used for home location "blinking"

        glPushMatrix(); // rotation matrix

        if (!bProjection) {
           glScalef(scaleAll, scaleAll, scaleAll);
           if (bAutoRotate)  {
               rot.x += rotationSpeed;
               uiAutoQuake++;
               if (uiAutoQuake == (qcn_graphics::vsq.size() * QUAKE_AUTOROTATE_SCALE)) uiAutoQuake = 0;
           }
           glRotatef(-rot.y, 1.0, 0.0, 0.0);
           glRotatef(rot.x, 0.0, 1.0, 0.0);

           if (qcn_graphics::g_eView == VIEW_EARTH_DAY && bShowSunLight) GetSunLightSource();
        }


        // render_texture
        mode_shaded(white);
        glColor4f(1.0,1.0,1.0, 0.0);

        glEnable(GL_DEPTH_TEST);  // enable depth test so "back" of earth doesn't appear etc
        glDepthFunc(GL_LESS);   

        // draw from mapping & vertices
        //glDisable(GL_LIGHTING);
        //glDisable(GL_LIGHT0);
        if (drawTexture) { 
#ifdef _EARTH_COMBINE_
           if (qcn_graphics::g_eView == VIEW_EARTH_COMBINED && bHaveMultitexture)  { // two-pass, using the mask on each image to get the day/night
               // first output the mask, which is an RGB of alpha values (white & black) turned into an alpha map to 
               // block/allow subsequent textures

               (*glActiveTextureARB)(GL_TEXTURE0_ARB);
               glEnable(GL_TEXTURE_2D);
               glBindTexture(GL_TEXTURE_2D, texture[TEXTURE_DAY].id);

               // don't forget to translate the mask by going to TEXTURE mode
               // or probably just generate it programmatically every once in awhile to get the alpha values
               (*glActiveTextureARB)(GL_TEXTURE1_ARB);
               glEnable(GL_TEXTURE_2D);
               glBindTexture(GL_TEXTURE_2D, texture[TEXTURE_MASK].id);

               (*glActiveTextureARB)(GL_TEXTURE2_ARB);
               glEnable(GL_TEXTURE_2D);
               glBindTexture(GL_TEXTURE_2D, texture[TEXTURE_NIGHT].id);

               glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);

               glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_REPLACE);
               glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_REPLACE);

               glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS);
               glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_TEXTURE);

               glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
               glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_INTERPOLATE_ARB);

               glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB);
               glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR); 

               glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE);
               glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);

               glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, GL_PREVIOUS_ARB);
               glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, GL_DST_COLOR);

               //(*glActiveTextureARB)(GL_TEXTURE3_ARB);
               //glEnable(GL_TEXTURE_2D);
               //glBindTexture(GL_TEXTURE_2D, texture[TEXTURE_MASK_INVERT].id);

               // now draw the one-pass multitexture
               MapEarthTexture(true);  // pass in true to denote we want the multi geometry

               // seems to need to be disabled in reverse order here, otherwise screws up logo texture etc
               //(*glActiveTextureARB)(GL_TEXTURE3_ARB);
               //glDisable(GL_TEXTURE_2D);
               (*glActiveTextureARB)(GL_TEXTURE2_ARB);
               glDisable(GL_TEXTURE_2D);
               (*glActiveTextureARB)(GL_TEXTURE1_ARB);
               glDisable(GL_TEXTURE_2D);
               (*glActiveTextureARB)(GL_TEXTURE0_ARB);
               glDisable(GL_TEXTURE_2D);

           }
           else {
#endif // _EARTH_COMBINE_

               glEnable(GL_TEXTURE_2D);
               glDisable(GL_BLEND);
               BindEarthTexture();  // this will get the day or night earth view texture
               MapEarthTexture();
               glDisable(GL_TEXTURE_2D);
#ifdef _EARTH_COMBINE_
           }  // if drawcombined or draw normal
#endif // _EARTH_COMBINE_
        }  // if drawTexture

        if (drawWireframe) {
                DrawGrid();
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                glLineWidth(lineWidth);

                for (int y=0; y<=EARTH_LAT_RES; y++) {
                        glBegin(GL_LINE_STRIP);
                        for (int x=0; x<=EARTH_LON_RES; x++) {
                                glColor4f(0,0,0,0.2f);
                                glVertex3fv((float*)&vertices[x][y]);
                        }
                        glEnd();
                }

                for (int x=0; x<=EARTH_LON_RES; x++) {
                        glBegin(GL_LINE_STRIP);
                        for (int y=0; y<=EARTH_LAT_RES; y++) {
                                glColor4f(0,0,0,0.2f);
                                glVertex3fv((float*)&vertices[x][y]);
                        }
                        glEnd();
                }

                glDisable(GL_BLEND);

        }
        else {
           // draw all available coastlines, nation boundaries etc
           DrawVxp();
        }

        if (bProjection) { // maybe do a 2d projection someday?
          Earth_2D();
        }

        // draw earthquake hotspots!
        vector<SQuake>::iterator it;
        vector<SQuake*>::iterator itp;

        glPushMatrix(); // earthquake matrix
        mode_unshaded();
        bool bHaveActive = false; // reset the active earthquake flag, so only one active eq is drawn yellow

        // get view info for the hit test
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview_matrix);
        glGetDoublev(GL_PROJECTION_MATRIX, projection_matrix);
        glGetIntegerv(GL_VIEWPORT, viewport);

        //psqActive = NULL; // reset active marker
        static int ctr=0, iCount = -1;

        if (bAutoRotate) vpsqc.clear();  // reset quake hit list if auto-rotating, so different quakes are auto-selected

        if (!bAutoRotate && !bMouseProcessed) { // just need to make the vector of quakes etc every mouse click

          vpsqc.clear(); // reset our vector for the hit test -- this will get all in the cluster
          // first go through and see who are active
          for( it = qcn_graphics::vsq.begin(); it != qcn_graphics::vsq.end(); it++) {
             HitTestEarthquake((SQuake*) &(*it), bHaveActive);  // this sets it->active to true if mouse is clicked on top of an earthquake (within .05 tolerance)
          }

          ctr=0, iCount = -1;
          // now see if our iCluster matches one of the ones in the active vpsqc vector, only matters if >1
          if (vpsqc.size() > 1) { // two or more matched, i.e. a cluster of quakes, try and cycle around the group
            for (itp = vpsqc.begin(); itp != vpsqc.end(); itp++) {
               SQuake* psqt = *itp;
               if (psqt->num == iCluster) { // we are still in a cluster
                  iCount = ctr;
                  break;
               }
               ctr++;
            }
            if (iCount > -1) { // we are in a cluster and on a current one, so bump up or wrap around 
                if  (iCount >= (int)(vpsqc.size() - 1)) 
                    iCount = 0; // wrapping around -- reset iCount to 0
                else 
                    iCount++;

                // now set iCluster to this current one, whether it's the first or the next in the group
                iCluster = vpsqc.at(iCount)->num;
            }
            else { // no previous selection in this group, so set iCluster to the first one
                iCount = 0;
                iCluster = vpsqc.at(0)->num;
            }
          } 
          else {
             if (vpsqc.size() <= 1) iCluster = 0;
             iCount = 0;
          }
          // fprintf(stdout, "vpsqc.size=%d  iCluster=%d  iCount=%d\n", vpsqc.size(), iCluster, iCount);
          // fflush(stdout);
        }  // if bMouseProcessed

        // newer quakes bright red, older quakes light red
        unsigned int uiQCtr = 0;
        for( it = qcn_graphics::vsq.begin(); it != qcn_graphics::vsq.end(); it++) {
           if (vpsqc.size() == 0) { // this will always get hit in autorotate mode
               if (bAutoRotate) { // set the activequake only if uiQCtr = uiAutoQuake/QUAKE_AUTOROTATE_SCALE;
                  it->bActive = (bool) (uiQCtr == (unsigned int) (uiAutoQuake/QUAKE_AUTOROTATE_SCALE));
               } 
               else {
                  it->bActive = (bool) ((SQuake*) psqActive == (SQuake*) &(*it)); // may still count as active if it was psqActive before and they clicked on nothing else
               }
           }
           else {
               it->bActive = (bool) ((SQuake*) vpsqc.at(iCount) == (SQuake*) &(*it));
           }
           uiQCtr++;

           // note -- is there a better way to get pointer to SQuake from the iterator?  can't just cast SQuake* on the iterator!
           // HitTestEarthquake((SQuake*) it->this, bHaveActive);  // this sets it->active to true if mouse is clicked on top of an earthquake (within .05 tolerance)

           // note -- either a quake was just set active, or one is already active (psqActive!=NULL) but they clicked somewhere else
           // so if psqActive is set, but they clicked on an "empty" area -- still show psqActive
           if (it->bActive)  { // || psqActive == (SQuake*) it->this;
              psqActive = (SQuake*) &(*it); // note we'll draw this last so it shows up!
           }
           DrawQuake(it);
        }

        // maybe draw a little glyph to represent station if available?
        if (sm && sm->dMyLatitude != NO_LAT && sm->dMyLongitude != NO_LNG && sm->dMyLatitude != 0.0f && sm->dMyLongitude != 0.0f) {
             Point3f mypt;
             glColor4f(0.0, 1.0, 0.0, 0.3);  // translucent green?
             LatLon2Point(sm->dMyLatitude, sm->dMyLongitude, mypt, .002); // note the extra padding so the hemisphere is on top all others
             glTranslatef(mypt.x, mypt.y, mypt.z);
             DrawQuakeSphere(dBlinker++/QUAKE_MAGNITUDE_FACTOR, 10, 10);
             if (dBlinker>10.0f) dBlinker = 1.0f;  // need to reset, getting too big
        }

        glDisable(GL_DEPTH_TEST);  // end depth test

        bMouseProcessed = true; // set flag that we processed the last mouse click
        glPopMatrix(); // earthquake drawing

        glPopMatrix(); // rotation matrix
        glPopMatrix(); // light matrix

        glFlush();
}

// this sets it->active to true if mouse is clicked on top of an earthquake (within .05 tolerance)
void CEarth::HitTestEarthquake(SQuake* psq, bool& bHaveActive)
{
    gluProject(psq->v.x, psq->v.y, psq->v.z, modelview_matrix, projection_matrix, viewport,
            &mousePX, &mousePY, &mousePZ);

    mousePY = (GLdouble) m_iHeight - mousePY;
    const float fErr = psq->magnitude * scaleAll;  // error region to test, note mousePZ -- <.5 means it's "front" of screen

    if (mousePZ < 0.5
         && (mouseX >= mousePX-fErr  &&  mouseX <= mousePX+fErr)
         && (mouseY >= mousePY-fErr  &&  mouseY <= mousePY+fErr)
    ) { // note the last term -- this will allow us to cycle through earthquakes in a very close region!
       vpsqc.push_back(psq);
#ifdef _DEBUG
       sprintf(strDebug, "QU#: %d  X: %f <= %f <= %f  Y: %f <= %f <= %f  Z: %f",
            psq->num, mousePX-fErr, mouseX, mousePX+fErr, mousePY-fErr, mouseY, mousePY+fErr, mousePZ);
#endif
    }
} 

void CEarth::DrawVxp()
{
        glPushMatrix();
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);

        glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);

        // use iterators through the vectors to draw the coastlines etc
        vector<VxpFeature>::iterator itFeat;
        vector<Point3f>::iterator itVect;

        for (int i = 0; i < MAX_VXP; i++) { // have to delete dynamically allocated structures within vxp, then the vxp_data point itself!
           if (showVxp[i])  {
               switch(i)  {
                 case COUNTRY:
                 case COASTLINE:
                 case NATION:
	             glColor4f(0.0,0.0,0.0,0.5);
                     glLineWidth(lineWidth*3);
                     break;
                 case PLATE:
	             glColor4f(1.0,0.0,1.0,0.5);
                     glLineWidth(lineWidth*5);
                     break;
               }
               for (itFeat = vxp_data[i].begin(); itFeat < vxp_data[i].end(); itFeat++) {
		 glBegin(GL_LINE_STRIP);
                 for (itVect = itFeat->p3f.begin(); itVect < itFeat->p3f.end(); itVect++) {
		    glVertex3f(itVect->x, itVect->y, itVect->z);
                 }
		 glEnd(); // this will end this section
               }
           }
        }
	
        //glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glPopMatrix();
}

void CEarth::RecalculateEarthquakePositions()
{
        // need to redo the earthquake points
        // now set the earthquake data points, this was parsed via project_preferences XML in qcn_graphics::Init
        vector<SQuake>::iterator it;
        for( it = qcn_graphics::vsq.begin(); it != qcn_graphics::vsq.end(); it++) {
           LatLon2Point(it->latitude, it->longitude, it->v);
        }
}

void CEarth::LatLon2Point(const float lat, const float lon, Point3f& pos, const float pad)
{
	// lat -90..90
	// lon -180..180

	float	angX, angY;

	angX = (180.f+lon) * PI / 180.f;
	angY = lat * PI / 180.f;

        // note the pad as it seems (perhaps due to rounding errors) some of the borders aren't shown, i.e. rendered below the map texture
	pos.x = fabsf(cosf(angY)) * (EARTH_RADIUS + pad) * sinf(angX);
	pos.y = (EARTH_RADIUS + pad) * sinf(angY);
	pos.z = fabsf(cosf(angY)) * (EARTH_RADIUS + pad) * cosf(angX);
}


void CEarth::RenderScene( GLsizei iWidth, GLsizei iHeight, GLfloat viewpoint, GLfloat pitch, GLfloat roll )
{ 	
    m_iWidth = iWidth;
    m_iHeight = iHeight;
    m_viewpoint = viewpoint;
    SetTime();
    DrawEarth();
}

void CEarth::checkURLClick(bool bShift)
{
   // if shift key is down see if they clicked on the URL message
   // if they did click, open a browser with this URL
   //if (!bShift || !psqActive) return; // only go into this if shift key is down on a mouse down event
   if (!bShift) return;
	if (psqActive)
		qcn_util::launchURL(psqActive->strURL.c_str());
	else
		qcn_util::launchURL(URL_USGS);
	
   isShiftDown = 0;  // set shift state off
}

void CEarth::MouseButton(int x, int y, int which, int is_down)
{
	mouseX = x;
	mouseY = y;

	if (which == GLUT_LEFT_BUTTON && is_down) {
	        mouseLeftButtonDown = 1;
            bMouseProcessed = (bool) isShiftDown;
            checkURLClick(bMouseProcessed);
	}
	else {
	    mouseLeftButtonDown = 0;
		bMouseProcessed = true; // no proc if mouse not down
    }

	if (which == GLUT_RIGHT_BUTTON && is_down) {
			mouseRightButtonDown = 1;
            bMouseProcessed = (bool) isShiftDown;
            checkURLClick(bMouseProcessed);
	}
        else {
                mouseRightButtonDown = 0;
        }

   bMouseProcessed = mouseRightButtonDown || mouseLeftButtonDown; // no proc if mouse not down

   isShiftDown = isCtrlDown = 0; // we just care about shift & ctrl down during the lifetime of this call
}

void CEarth::RenderText()
{  // this draws the necessary text for the earth/quake view

   // draw text on top
   mode_unshaded();
   qcn_graphics::mode_ortho_qcn();
	
   char buf[256];

   // the following uncommented out will let the text bounce around!
    //static float x=0, y=0;
    //static float dx=0.0003, dy=0.0007;
    //x += dx;
    //y += dy;
    //if (x < 0 || x > .5) dx *= -1;
    //if (y < 0 || y > .5) dy *= -1;

   if (psqActive) { // draw quake info
       GLfloat* pfcolor;
       switch (psqActive->eType)  {
          case QUAKE_CURRENT:
              pfcolor = red;
              break;
          case QUAKE_WORLD85:
              pfcolor = cyan;
              break;
          default:
              pfcolor = yellow;
       }

       sprintf(buf, "Selected Earthquake # %d of %d:", psqActive->num, (int) qcn_graphics::vsq.size());
       TTFont::ttf_render_string(qcn_graphics::g_alphaText, 0, .30, 0, MSG_SIZE_NORMAL, yellow, TTF_ARIAL, buf);

       if (vpsqc.size()>1) {
         sprintf(buf, "(%d Quakes in this Area, Click to Cycle)", (int) vpsqc.size());
         TTFont::ttf_render_string(qcn_graphics::g_alphaText, 0, .282, 0, MSG_SIZE_SMALL, yellow, TTF_ARIAL, buf);
       }

       sprintf(buf, "Magnitude: %5.1f", psqActive->magnitude);
       TTFont::ttf_render_string(qcn_graphics::g_alphaText, 0, .255, 0, MSG_SIZE_NORMAL, pfcolor, TTF_ARIAL, buf);

       sprintf(buf, "Lat/Long: %8.3f, %8.3f", psqActive->latitude, psqActive->longitude);
       TTFont::ttf_render_string(qcn_graphics::g_alphaText, 0, .230, 0, MSG_SIZE_NORMAL, pfcolor, TTF_ARIAL, buf);

       if (psqActive->depth_km <= 0.0)
          sprintf(buf, "Depth (km): Not Available");
       else
          sprintf(buf, "Depth (km): %6.2f", psqActive->depth_km);
       TTFont::ttf_render_string(qcn_graphics::g_alphaText, 0, .205, 0, MSG_SIZE_NORMAL, pfcolor, TTF_ARIAL, buf);

       sprintf(buf, "Time (UTC): %04d/%02d/%02d %02d:%02d:%02d",
            psqActive->year, psqActive->month, psqActive->day,
              psqActive->hour, psqActive->minute, psqActive->second);
       TTFont::ttf_render_string(qcn_graphics::g_alphaText, 0, .180, 0, MSG_SIZE_NORMAL, pfcolor, TTF_ARIAL, buf);

       TTFont::ttf_render_string(qcn_graphics::g_alphaText, 0, .155, 0, MSG_SIZE_NORMAL, pfcolor, TTF_ARIAL, (const char*) psqActive->strDesc.c_str());
   }

   // draw mouse position info
   //sprintf(buf, "mouse %d,%d  %d,%d,%d", mouseX, mouseY, mousePX, mousePY, mousePZ);
   //sprintf(buf, "glptr %d %d", glptr[0], glptr[1]);
   //TTFont::ttf_render_string(qcn_graphics::g_alphaText, 0, .50, 0, MSG_SIZE_NORMAL, red, TTF_ARIAL, buf);

   if (!qcn_graphics::g_bFullScreen) {
     TTFont::ttf_render_string(qcn_graphics::g_alphaText, 0, .470, 0, MSG_SIZE_SMALL, yellow, TTF_ARIAL, (const char*) "Left Mouse & Drag to Rotate");
     TTFont::ttf_render_string(qcn_graphics::g_alphaText, 0, .450, 0, MSG_SIZE_SMALL, yellow, TTF_ARIAL, (const char*) "Ctrl+Left or Rt Mouse & Drag to Zoom");
     TTFont::ttf_render_string(qcn_graphics::g_alphaText, 0, .430, 0, MSG_SIZE_SMALL, yellow, TTF_ARIAL, (const char*) "Click on Quake to Select");
   }

#ifndef QCNLIVE
	//!defined(QCNLIVE) || defined(_DEBUG)
   if (!qcn_graphics::g_bFullScreen) {
     TTFont::ttf_render_string(qcn_graphics::g_alphaText, 0, .410, 0, MSG_SIZE_SMALL, yellow, TTF_ARIAL, (const char*) "Shift+Mouse for USGS Website Data");
     TTFont::ttf_render_string(qcn_graphics::g_alphaText, 0, .390, 0, MSG_SIZE_SMALL, yellow, TTF_ARIAL, (const char*) "Press 'A' to toggle auto-rotation");
     TTFont::ttf_render_string(qcn_graphics::g_alphaText, 0, .370, 0, MSG_SIZE_SMALL, yellow, TTF_ARIAL, (const char*) "Press 'E' to toggle earth map image");
	 // left of window informative text
	 TTFont::ttf_render_string(qcn_graphics::g_alphaText, 0, .350, 0, MSG_SIZE_SMALL, yellow, TTF_ARIAL, "Press 'Q' for sensor view");
   }
#endif

	
#ifdef _DEBUG
   if (strDebug[0]!=0x00) {
       TTFont::ttf_render_string(qcn_graphics::cfTextAlpha, 0, .7, 0, MSG_SIZE_NORMAL, red, TTF_ARIAL, strDebug);
   }
#endif

   // NB: draw_text_user called automatically from qcn_graphics to show BOINC username, CPU time etc
   ortho_done();
}

void CEarth::MouseMotion(int x, int y, int left, int middle, int right)
{ // note this is already corrected for windows coords which are absolute screen, not relative to window
	int	deltX, deltY;

	// calculate mouse movement since click
	deltX = x - mouseX;
	deltY = y - mouseY;

	// store new mouse position
	mouseX = x;
	mouseY = y;

	if (mouseLeftButtonDown) {
			// rotate
			rot.x += deltX*0.25f/scaleAll;
			rot.y += deltY*0.25f/scaleAll;

			// save values for auto rotation
			autoRotX = deltX*0.25f;
			autoRotY = deltY*0.25f;
	} 
        else if (mouseRightButtonDown)  { //zoom
			// scale
			float addition;
			addition = ((deltX+deltY) / 200.f);
			
			if (addition < 0 && scaleAll+addition > MIN_SCALE) {
				scaleAll += addition;
			}

			if (addition > 0 && scaleAll+addition < MAX_SCALE) {
				scaleAll += addition;
			}
        }
}

void CEarth::GetMouseProjection()
{
/*
     // note the weird 'y' since opengl reverses & have to factor in viewport size
     if (mouseX != -1 && mouseY != -1)
        gluUnProject(mouseX, m_iHeight - mouseY, 0.5, modelview_matrix, projection_matrix, viewport,
             &mousePX, &mousePY, &mousePZ);

        // go through and see if we hit an eathquake blob
        for (int i=0; i<NUM_COUNTRY-1; i++) {
           GLdouble winX, winY, winZ;
           gluProject(country_names_pos[i].x, country_names_pos[i].y, country_names_pos[i].z,
              modelview_matrix,
              projection_matrix,
              viewport,
              &winX, &winY, &winZ);

*/
}

/*
void CEarth::SelectFromMenu(int idCommand)
{
	switch (idCommand) {
	case MENU_WIREFRAME:
		drawWireframe = !drawWireframe;
		break;
	case MENU_TEXTURE:
		drawTexture = !drawTexture;
		break;
	case MENU_LINEWIDTH:
		if (lineWidth == 1) {
			lineWidth = 2;
		} else {
			lineWidth = 1;
		}
		break;
	case MENU_COUNTRY_NAMES:
		drawCountries = !drawCountries;
		break;

	case MENU_COAST:
	case MENU_ISLAND:
	case MENU_LAKE:
	case MENU_NATION:
	case MENU_RIVER:
	case MENU_USSTATE:
		showVxp[idCommand-MENU_COAST] = !showVxp[idCommand-MENU_COAST];
		break;

	case MENU_EXIT:
		exit(0);
		break;
	}
}

*/

void CEarth::KeyDown(int key1, int key2)
{ 
    iKey = key1;
    iKeySpecial = key2;
    switch(iKey)
    { 
    // note - KEY_SHIFT & KEY_CTRL only captures on Windows, Mac & Linux ??
	case KEY_SHIFT:  // shift key
	   isShiftDown = 1; break;
	case KEY_CTRL:  // ctrl key
           isCtrlDown = 1; break;
        case 'e':
        case 'E':   
               if (bHaveMultitexture) { // cycle through
                   switch (qcn_graphics::g_eView) {
                       case VIEW_EARTH_DAY:
                          qcn_graphics::g_eView = VIEW_EARTH_NIGHT;
                          break;
                       case VIEW_EARTH_NIGHT:
                          qcn_graphics::g_eView = VIEW_EARTH_COMBINED;
                          break;
                       case VIEW_EARTH_COMBINED:
                          qcn_graphics::g_eView = VIEW_EARTH_DAY;
                          break;
                       default:
                          qcn_graphics::g_eView = VIEW_EARTH_DAY;
                          break;
                   }
               }
               else { // just alternate night & day
                  if (qcn_graphics::g_eView == VIEW_EARTH_NIGHT) {
                     qcn_graphics::g_eView = VIEW_EARTH_DAY;
                  }
                  else {
                     qcn_graphics::g_eView = VIEW_EARTH_NIGHT;
                  }
               }
               break;
        case 'c':
        case 'C':  qcn_graphics::g_eView = VIEW_CUBE; break;
        case 'a':
        case 'A':  bAutoRotate = ! bAutoRotate; break;
        case 's':
        case 'S':  bShowSunLight = ! bShowSunLight; break;
        //case 45: glptr[0]++; if (glptr[0]>11) glptr[0] = 0; break;
        //case 61: glptr[1]++; if (glptr[1]>11) glptr[1] = 0; break;
    }
}

void CEarth::KeyUp(int key1, int key2)
{ 
    switch(key1)
    {
	    case KEY_SHIFT:  // shift key
		   isShiftDown = 0; break;
	    case KEY_CTRL:  // ctrl key
		   isCtrlDown = 0; break;
	}
}

/*
void CEarth::Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
		SelectFromMenu(MENU_COAST+key-'1');
		break;

	case 't':
		SelectFromMenu(MENU_TEXTURE);
		break;

	case 'w':
		SelectFromMenu(MENU_WIREFRAME);
		break;

	case 'l':
		SelectFromMenu(MENU_LINEWIDTH);
		break;

	case 'c':
		SelectFromMenu(MENU_COUNTRY_NAMES);
		break;

	case 27:	// escape
		SelectFromMenu(MENU_EXIT);
		break;
	}
}
*/

void CEarth::MapEarthTexture(bool bMulti)
{ // draws the earth sphere and maps the texture currently bound to it (which can be a multi texture if bMulti = true)
  // at this point gl-mode is texture & texture is bound
    for ( int x = 0; x < EARTH_LON_RES; x++ ) {
        glBegin(GL_QUAD_STRIP);
        for ( int y = 0; y < EARTH_LAT_RES; y++ ) {
            if (bMulti) {
#ifdef _EARTH_COMBINE_
               (*glMultiTexCoord2fvARB)(GL_TEXTURE0_ARB, (GLfloat*) &mapping[x][y]);
               (*glMultiTexCoord2fvARB)(GL_TEXTURE1_ARB, (GLfloat*) &mapping[x][y]);
               (*glMultiTexCoord2fvARB)(GL_TEXTURE2_ARB, (GLfloat*) &mapping[x][y]);
               //(*glMultiTexCoord2fvARB)(GL_TEXTURE3_ARB, (GLfloat*) &mapping[x][y]);
#endif
            }
            else {
               glTexCoord2fv((GLfloat*) &mapping[x][y]);
            }
            glNormal3fv  ((GLfloat*)&vertices[x][y]);
            glVertex3fv  ((GLfloat*)&vertices[x][y]);

            if (bMulti) {
#ifdef _EARTH_COMBINE_
               (*glMultiTexCoord2fvARB)(GL_TEXTURE0_ARB, (GLfloat*) &mapping[x][y+1]);
               (*glMultiTexCoord2fvARB)(GL_TEXTURE1_ARB, (GLfloat*) &mapping[x][y+1]);
               (*glMultiTexCoord2fvARB)(GL_TEXTURE2_ARB, (GLfloat*) &mapping[x][y+1]);
               //(*glMultiTexCoord2fvARB)(GL_TEXTURE3_ARB, (GLfloat*) &mapping[x][y+1]);
#endif
            }
            else {
               glTexCoord2fv((GLfloat*) &mapping[x][y+1]);
            }
            glNormal3fv  ((GLfloat*)&vertices[x][y+1]);
            glVertex3fv  ((GLfloat*)&vertices[x][y+1]);

            if (bMulti) {
#ifdef _EARTH_COMBINE_
               (*glMultiTexCoord2fvARB)(GL_TEXTURE0_ARB, (GLfloat*) &mapping[x+1][y]);
               (*glMultiTexCoord2fvARB)(GL_TEXTURE1_ARB, (GLfloat*) &mapping[x+1][y]);
               (*glMultiTexCoord2fvARB)(GL_TEXTURE2_ARB, (GLfloat*) &mapping[x+1][y]);
               //(*glMultiTexCoord2fvARB)(GL_TEXTURE3_ARB, (GLfloat*) &mapping[x+1][y]);
#endif
            }
            else {
               glTexCoord2fv((GLfloat*) &mapping[x+1][y]);
            }
            glNormal3fv  ((GLfloat*)&vertices[x+1][y]);
            glVertex3fv  ((GLfloat*)&vertices[x+1][y]);

            if (bMulti) {
#ifdef _EARTH_COMBINE_
               (*glMultiTexCoord2fvARB)(GL_TEXTURE0_ARB, (GLfloat*) &mapping[x+1][y+1]);
               (*glMultiTexCoord2fvARB)(GL_TEXTURE1_ARB, (GLfloat*) &mapping[x+1][y+1]);
               (*glMultiTexCoord2fvARB)(GL_TEXTURE2_ARB, (GLfloat*) &mapping[x+1][y+1]);
               //(*glMultiTexCoord2fvARB)(GL_TEXTURE3_ARB, (GLfloat*) &mapping[x+1][y+1]);
#endif
            }
            else {
               glTexCoord2fv((GLfloat*) &mapping[x+1][y+1]);
            }
            glNormal3fv  ((GLfloat*)&vertices[x+1][y+1]);
            glVertex3fv  ((GLfloat*)&vertices[x+1][y+1]);
         }
         glEnd();
    }
}

void CEarth::BindEarthTexture(int iSelect)
{
    // setup common params to use for the textures
    if (iSelect>-1 && texture[iSelect].id)  {
        glBindTexture(GL_TEXTURE_2D, texture[iSelect].id);
    }
    else if (qcn_graphics::g_eView == VIEW_EARTH_DAY && texture[TEXTURE_DAY].id)  {  // draw day earth texture
        glBindTexture(GL_TEXTURE_2D, texture[TEXTURE_DAY].id);
    }
    else if (qcn_graphics::g_eView == VIEW_EARTH_NIGHT && texture[TEXTURE_NIGHT].id) { // draw night earth texture
        glBindTexture(GL_TEXTURE_2D, texture[TEXTURE_NIGHT].id);
    }
}

// note can pass in your light position
void CEarth::InitLights()
{

        light0_pos[0] = light0_pos[1] = 0.0;
        light0_pos[2] = LIGHT_INFINITY;

        // enable lighting and one light source
        if (bProjection) {
           glDisable(GL_LIGHTING);
           glDisable(GL_LIGHT0);
        }
        else {
           glEnable(GL_LIGHTING);
           glEnable(GL_LIGHT0);
        }

        glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
        glLightfv(GL_LIGHT0, GL_CONSTANT_ATTENUATION, &attenuation);
        glMaterialfv(GL_FRONT, GL_SHININESS, &shine);

        // no two-sided polygons
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
        // local viewer for specular light?
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
        // enable smoooth shading (multi-coloured polygons)
        glShadeModel(GL_SMOOTH);
        // fill polygons
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void CEarth::InitCamera()  
{
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        // orthographic projection
        glOrtho(-2.8, 2.8, -2.05, 2.05, -10, 10.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        SetViewpoint();
}


void CEarth::OrthographicMatrix()
{
        glEnable(GL_DEPTH_TEST);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        ortho =  static_cast<GLfloat> (m_iHeight) / static_cast<GLfloat> (m_iWidth);
        if ( m_iWidth <= m_iHeight ){
                glOrtho(-2.0, 2.0, -2.0 * ortho, 2.0 * ortho, -10.0, 10.0);
        }else{
                glOrtho(-2.0 / ortho, 2.0 / ortho, -2.0, 2.0, -10.0, 10.0);
        }
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
}

void CEarth::SetViewpoint(double x, double y, double z)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        x, y, z,        // eye position
        0,-.8,0,        // where we're looking
        0.0, 1.0, 0.    // up is in positive Y direction
    );
}

void CEarth::DrawGrid()
{
        int i, j;
        glPushMatrix();
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT0);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glLineWidth(lineWidth);
        glColor3f(132.0/255.0, 132.0/255.0, 132.0/255.0);
        if(bProjection){
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glRectf(-2.3, 1.05, 2.3, -1.05);
                // longitude - horizontal
                double delta = 0.0;
                for( i = 0; i < EARTH_LAT_RES ; i++ ){
                        delta += 4.6 / EARTH_LAT_RES ;
                        glBegin(GL_LINES);
                        glVertex2d(-2.3+delta, 1.05);
                        glVertex2d(-2.3+delta,-1.05);
                        glEnd();
                }
                // latitude - vertical
                delta = 0.0;
                for( i = 0; i <= EARTH_LON_RES ; i++ ){
                        delta += 2.1 / EARTH_LON_RES ;
                        glBegin(GL_LINES);
                        glVertex2d( 2.3,1.05-delta);
                        glVertex2d(-2.3,1.05-delta);
                        glEnd();
                }
        }
        else {

/*
        glBegin(GL_LINE_STRIP);
        glVertex3f(-1.5,2.05, 0.5);
        glVertex3f(-1.5,-2.05, 0.5);
        glVertex3f(-1.5,3.05, 0.5);
        glVertex3f(1.5,-3.05, 0.5);
        glEnd();
*/

                for ( i = 0; i <= EARTH_LAT_RES ; i++ ) {
                        glBegin(GL_LINE_STRIP);
                        for ( j = 0; j <= EARTH_LON_RES ; j++ )
                                glVertex3fv((float*)&vertices[j][i]);
                        glEnd();
                }
                for ( i = 0; i <= EARTH_LON_RES ; i++ ) {
                        glBegin(GL_LINE_STRIP);
                        for ( j = 0; j <= EARTH_LAT_RES ; j++ ) {
                                glVertex3fv((float*)&vertices[i][j]);
                        }
                        glEnd();
                }
        }
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glPopMatrix();
}

// sets the time for sun location etc
void CEarth::SetTime(double dt)
{
     if (sm && dt == 0.0f)
         m_dTime = sm->update_time;  // this should be current within .2 seconds, whether they have a sensor or not
     else
         m_dTime = dt;

     if (m_dTime == 0.0f) m_dTime = dtime(); // if still 0 get it from the boinc function dtime()

     //struct tm m_tmTime;
     // now break out into component parts
     time_t tt = (time_t) m_dTime;  // note that this truncates the microseconds i.e. decimal part of dtime
#ifdef _WIN32 
	gmtime_s(&m_tmTime, &tt);  // note the "secure" Microsoft version flips the args around!
#else
	gmtime_r(&tt, &m_tmTime);
#endif

/*
     gmtime_r(&tt, &m_tmTime);
    // print out as month/day/year hour:min:sec
    sprintf(strTime, "%02d/%02d/%04d %02d:%02d:%02d",
       tmp.tm_mon, tmp.tm_mday, tmp.tm_year+1900,
       tmp.tm_hour, tmp.tm_min, tmp.tm_sec);  // don't need microseconds! , dtime - (double) tt);
*/
}

void CEarth::sprintGL(float x, float y, float z, int fontType, char * str, GLfloat * col) 
{
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glColor3fv(col);
        glPushMatrix();
        glTranslatef(0.f, 0.f, z);
        glRasterPos2f(x, y);
        int len;
        len = (int) strlen(str);
        glPopMatrix();
}

void CEarth::InitGraphicStructs()
{
        InitCoordinate(&vxp_data[NATION], 'N');     // nation boundaries
        InitCoordinate(&vxp_data[PLATE], 'P');      // plate boundaries
        InitCoordinate(&vxp_data[COASTLINE], 'C');  // coastline boundaries
        //InitCoordinate(&vxp_data[COUNTRY], 'R');  // country names/lat/long

        // now set the earthquake data points, this was parsed via project_preferences XML in qcn_graphics::Init
        RecalculateEarthquakePositions();  // put the below in a method we can call public
/*
        vector<SQuake>::iterator it;
        for( it = qcn_graphics::vsq.begin(); it != qcn_graphics::vsq.end(); it++) {
           LatLon2Point(it->latitude, it->longitude, it->v);
        }
*/
}

void CEarth::InitCoordinate(vector<VxpFeature>* vxp, char cType ) 
{
   // go through the appropriate array
   // basically x/y or lat/lon of 0,0 is a separator
        int iMax = 0;
        Point3f p3f;
        VxpFeature vxpf;
        vxp->clear();

        switch(cType) {
          case 'N':  iMax = NUM_NATION_BOUNDARY; break;
          case 'C':  iMax = NUM_COASTLINE; break;
          case 'P':  iMax = NUM_PLATE_BOUNDARY; break;
        }

        // the way we build the vector of vectors is to push_back onto vxp (vxp_data[NATION]) every time we hit a 0,0
        // in between these 0,0 sections we push_back vectors onto that section

        // each array is split by the 0,0 entry, and each ends on a 0,0 (but don't start with 0,0 !)
        for (int i = 0 ; i < iMax; i++ ) {
               switch(cType) { // 
                 case 'N' :  
                             if ((nation[i].lat == 0)  && (nation[i].lon == 0)) { // new section, add this vector
                                // note we're guaranteed to get the last one pushed since all arrays end with 0,
                                vxp->push_back(vxpf);
                                vxpf.p3f.clear(); // clears out for reuse
                             }
                             else { // it's a good point, not a 0,0 separator point
                                // set our temp Point3f struct of floats
                                LatLon2Point(nation[i].lat, nation[i].lon, p3f);
                                // now push this onto our temp vector
                                vxpf.p3f.push_back(p3f);
                             }
                             break;

                 case 'C' :  
                             if ((coastline[i].x == 0)  && (coastline[i].y == 0)) { // new section, add this vector
                                // note we're guaranteed to get the last one pushed since all arrays end with 0,0
                                vxp->push_back(vxpf);
                                vxpf.p3f.clear(); // clears out for reuse
                             }
                             else { // it's a good point, not a 0,0 separator point
                                // set our temp Point3f struct of floats -- note coastline array doesn't use LatLon2Point
                                p3f.x  = coastline[i].x * EARTH_RADIUS;
                                p3f.y  = coastline[i].y * EARTH_RADIUS;
                                p3f.z  = coastline[i].z * EARTH_RADIUS;
                                // now push this onto our temp vector
                                vxpf.p3f.push_back(p3f);
                             }
                             break;
                 case 'P' : 
                             if ((plate[i].lat == 0)  && (plate[i].lon == 0)) { // new section, add this vector
                                // note we're guaranteed to get the last one pushed since all arrays end with 0,0
                                vxp->push_back(vxpf);
                                vxpf.p3f.clear(); // clears out for reuse
                             }
                             else { // it's a good point, not a 0,0 separator point
                                // set our temp Point3f struct of floats
                                LatLon2Point(plate[i].lat, plate[i].lon, p3f);
                                // now push this onto our temp vector
                                vxpf.p3f.push_back(p3f);
                             }
                             break;
                   } // end switch block
        }
}

bool CEarth::IsAutoRotate()
{
   return bAutoRotate;
}

void CEarth::AutoRotate(bool bRotate)
{
   bAutoRotate = bRotate;
}

bool CEarth::IsEarthquakeSelected()
{
   return (bool)(psqActive != NULL);
}

void CEarth::EarthquakeSelectedString(char* strQuakeInfo, int iLen)
{ // note should probably checkf or minimum string size
   //memset(strQuakeInfo, 0x00, sizeof(char) * iLen);
   if (psqActive)  {
      sprintf(strQuakeInfo,
        "Selected Earthquake # %d of %d:"
         "  Magnitude: %5.1f"
		 "  Lat/Long: %8.3f, %8.3f"
		 "  Depth (km): %6.2f"
	     "  Time (UTC): %04d/%02d/%02d %02d:%02d:%02d"
		 "  Location: %s",
         psqActive->num, (int) qcn_graphics::vsq.size(),
		 psqActive->magnitude,
		 psqActive->latitude, psqActive->longitude,
         psqActive->depth_km,
         psqActive->year, psqActive->month, psqActive->day,
         psqActive->hour, psqActive->minute, psqActive->second,
		 psqActive->strDesc.c_str()
	   );
   }
}

void CEarth::ResetEarthquakeNumber()
{
   uiAutoQuake = 0;
}

// the below needs some work!
void CEarth::DrawQuakeSphere(const double dRadius, const int iLat, const int iLng)
{
    GLUquadricObj* pquad = gluNewQuadric();
    if (pquad) {
        //gluPartialDisk(pquad, dRadius, dRadius + (dRadius/2.0f), 5, 5, 0, 180);
        gluSphere(pquad, dRadius, 10, 10);
        gluDeleteQuadric(pquad);
    }

  /* GLUquadricObj *qobj,
  GLdouble innerRadius,
  GLdouble outerRadius,
  GLint slices,
  GLint loops,
  GLdouble startAngle,
  GLdouble sweepAngle
  */

  /*
    int i, j;
    double lat[2], z[2], zrad[2];
    double lng, x, y;

    for (i = 0; i <= iLat; i++)  {
        lat[0] = dRadius * PI * (-0.5 + (double) (i - 1) / iLat);
        z[0]  = sin(lat[0]);
        zrad[0] =  cos(lat[0]);

        lat[1] = dRadius * PI * (-0.5 + (double) i / iLat);
        z[1]  = sin(lat[1]);
        zrad[1] =  cos(lat[1]);

        glBegin(GL_QUAD_STRIP);
        for (j = 0; j <= iLng; j++)  {
            lng = 2 * PI * dRadius * (double) (j - 1) / iLng;
            x = cos(lng);
            y = sin(lng);
    
            glNormal3f(x * zrad[0], y * zrad[0], z[0]);
            glVertex3f(x * zrad[0], y * zrad[0], z[0]);
            glNormal3f(x * zrad[1], y * zrad[1], z[1]);
            glVertex3f(x * zrad[1], y * zrad[1], z[1]);
         }
         glEnd();
    }
    */
}

// GetSunrise & GetSunset
// basically we want to get a line on the 2D texture which will be 
// sunrise (orange blend) and sunset (purple blend) to serve as
// boundary points for blending the earth day & night textures
// note the time used is a struct tm m_tmTime, the member variable which stores
// a Greenwich Mean Time (UTC)-coordinated time structure (tm)

void CEarth::GetSunriseSunset() 
{

   // note: I am using the m_tmTime (struct tm from gmtime) to convert a dtime() to a full struct
   //           this has the correct day of year etc, but may not be needed for a 360-day climate model etc


}

void CEarth::GetSunLightSource()
{
    // from CPDN: calculate the sun lat/lng position, then set the OpenGL light source
    float sub_solar_latitude, sub_solar_longitude;
    GetSunPosition(sub_solar_latitude, sub_solar_longitude);

    sub_solar_longitude *= (float) PI/180.0f ;  // want it in radians for use in trig functions.
    sub_solar_latitude *= (float) PI/180.0f;  // -> radians

    // Now convert to a location on the earth (unit radius).  We use the same formula as
    // when making the quad sphere (Y axis is polar).
    sunshine.x =  (float)sin(sub_solar_longitude) * LIGHT_INFINITY * (float)sin(sub_solar_latitude);
    sunshine.y =  (float)cos(sub_solar_latitude)  * LIGHT_INFINITY ;
    sunshine.z =  -(float)cos(sub_solar_longitude) * LIGHT_INFINITY * (float)sin(sub_solar_latitude);

    // The sun direction is found by reversing the vector going from the origin
    // (which is where the centre of the earth is) to the location on the earth.
    // this doesn't seem right for QCN?
    GLfloat sunpos[4] = {sunshine.x, sunshine.y, sunshine.z, 1.0};
    glLightfv(GL_LIGHT0, GL_POSITION, sunpos);
    //glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);  // not needed, washes out the other light
}

// return the degree sun position lat/longitude for use in various calcs 
void CEarth::GetSunPosition(float& sub_solar_latitude, float& sub_solar_longitude)
{
   // note: I am using the m_tmTime (struct tm from gmtime) to convert a dtime() to a full struct
   //           this has the correct day of year etc, but may not be needed for a 360-day climate model etc
   //   for other uses you will want to declare it:
   //   struct tm m_tmTime
   //   and set each element either manually or by calls to time functions such as gmtime etc.

       // Routine to calculate direction of sunlight from date and time via
        // Myles' formulae for sub-solar latitude and longitude.
        float day_of_year;
        // Calculate the shifted day of the year from the month and day from Myles' formula.
        // Note that all months have 30 days (and the year is 360 days long).  The 201 shift
        // is to put the equinox at March 21 (day 111) and Sep 21 (day 291); these correspond
        // to shifted days of -90 and 90, whose cosine (see below) is 0 - this makes the
        // latitude 0 as well.
        // note - no need for 180 or 201 day shift, tm_yday is the real day of year
        day_of_year = (float) m_tmTime.tm_yday * (PI/180.0f); // (float) m_tmTime.tm_mon * 30.0f + (float) m_tmTime.tm_mday - 201.0f ;

        // Calculate sub-solar longitude from the time (in fraction of a day), which is in [0,360].
        // The 180 shift makes midday correspond to a longitude of 360 = 0 - i.e. the meridian.
        // note - no need for 180 shift?
        sub_solar_longitude = (360.0f*((float) m_tmTime.tm_hour +((float) m_tmTime.tm_min/60.0f))/24.0f);

        // Calculate sub-solar latitude from the day.  Note that the original formula had a
        // factor of cos(sub_solar_longitude), which was wrong (Myles 21-Oct-2002).
        // The latitude is in [-EARTH_TILT_ANGLE, EARTH_TILT_ANGLE], where 0 is the equator.
        sub_solar_latitude = (float) EARTH_TILT_ANGLE * (float) cos(day_of_year) ;

        // Since we take Y as cos(latitude), we want 0 to be the North Pole.  So we take it off 90.
        sub_solar_latitude = 90.0f - sub_solar_latitude ;

}
