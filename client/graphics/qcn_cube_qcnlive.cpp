// cube 

#ifdef _WIN32
  #pragma warning( disable : 4244 )  // Disable warning messages for double to float conversion
  #pragma warning( disable : 4305 )  // Disable warning messages for double to GLfloat conversion
  #pragma warning( disable : 4800 )  // Disable warning messages for "forcing value to bool"
#endif

#include "qcn_cube.h"
#include "qcn_shmem.h"

const int CCube::iMaxTextures = 4;
const int CCube::MAX_PLOT_POINTS = 500;;

// constructor
CCube::CCube()  
{ 
       //ptEq.x = ptEq.y = ptEq.z = 0;
       bVar = false;
       fMean[0] = fMean[1] = fMean[2] = fMean[3] = 0.0f;
       fM2[0] = fM2[1] = fM2[2] = fM2[3] = 0.0f;
       fDelta[0] = fDelta[1] = fDelta[2] = fDelta[3] = 0.0f;
       fVariance[0] = fVariance[1] = fVariance[2] = fVariance[3] = 0.0f;
       fStdDev[0] = fStdDev[1] = fStdDev[2] = fStdDev[3] = 0.0f;

       fMouseFactor = 0.0f;
       mouseX = mouseY = -1;
       mousePX = mousePY = mousePZ = -1;
       iKey = iKeySpecial = mouseRightButtonDown = isShiftDown = mouseLeftButtonDown = isCtrlDown = 0;
       rot.x = 0, rot.y = 0;  // note rot.x is our startup angle
       psqActive = NULL; 
       iCluster = -1;

       //bEarthDay = true;
       bMouseProcessed = false;
       bAutoRotate = true;
       uiAutoQuake = 0;
       autoRotX = 0, autoRotY = 0;
       scaleAll = 1.8;
       lineWidth = 1;

       memset(&modelview_matrix, 0x00, 16 * sizeof(GLdouble));
       memset(&projection_matrix, 0x00, 16 * sizeof(GLdouble));
       memset(&viewport, 0x00, 4 * sizeof(GLint));

       m_dTime = 0.0;

       m_iWidth = 640; 
       m_iHeight = 480; 

       /*
       for (int i = 0; i < 6; i++) {
          m_textureID[i] = 0;
          memset(m_face[i], 0x00, sizeof(64*64*3));
       }
       */

       rotationSpeed = ROTATION_SPEED_DEFAULT;

       diffuse[0] = 0.8, diffuse[1] = 0.8, diffuse[2] = 0.8, diffuse[3] = 1.0;
       ambient[0] = 0.4, ambient[1] = 0.4, ambient[2] = 0.4, ambient[3] = 1.0;
       specular[0] = 0.0, specular[1] = 0.0, specular[2] = 0.0, specular[3] = 1.0;

       light0_pos[0] = 0.0, light0_pos[1] = 0.0, light0_pos[2] = LIGHT_INFINITY, light0_pos[3] = 1.0;

       attenuation  =  1.f;
       shine        =  1.f;
       bProjection = false;
       ortho = 1.0f;
    
       //Q[0] = Q[1] = Q[2] = Q[3] = NULL;
}

// destructor
CCube::~CCube()
{
  //if (m_textureID[0])
  //  glDeleteTextures(6, m_textureID);

}

void CCube::Resize( GLsizei iWidth, GLsizei iHeight )
{ 
        m_iWidth = iWidth;
        m_iHeight = iHeight;

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

void CCube::Idle()
{
}

void CCube::Init()
{
    // set the background colour black
    glColor4fv(black);

    // enable hidden-surface-removal
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_FLAT);
    glEnable(GL_TEXTURE_2D);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

}

void CCube::RenderText()
{  // this draws the necessary text for the cube view
   // draw text on top

#ifndef QCNLIVE
   char buf[256];
   // left of window informative text
   if (!qcn_graphics::g_bFullScreen)  { // don't show the button press text in fullscreen/screensaver mode
        mode_unshaded();
        mode_ortho();
        sprintf(buf, "Press 'Q' to return to");
        TTFont::ttf_render_string(.1, 0, .32, 0, MSG_SIZE_NORMAL, yellow, 0, buf);
        sprintf(buf, "seismic sensor view");
        TTFont::ttf_render_string(.1, 0, .30, 0, MSG_SIZE_NORMAL, yellow, 0, buf);
        ortho_done();
   }
#endif

   // print a "legend"
   mode_unshaded();
   mode_ortho();
   TTFont::ttf_render_string(.1, 0, .25, 0, MSG_SIZE_NORMAL, blue, 0, "Blue is Z-axis");
   TTFont::ttf_render_string(.1, 0, .23, 0, MSG_SIZE_NORMAL, yellow, 0, "Yellow is Y-axis");
   TTFont::ttf_render_string(.1, 0, .21, 0, MSG_SIZE_NORMAL, green, 0, "Green is X-axis");
//Commented out by JL: Don't show these:
/*   TTFont::ttf_render_string(.1, 0, .27, 0, MSG_SIZE_NORMAL, red, 0, "Red is Significance");
   TTFont::ttf_render_string(.1, 0, .19, 0, MSG_SIZE_NORMAL, cyan, 0, "Cyan is Magnitude");
   TTFont::ttf_render_string(.1, 0, .17, 0, MSG_SIZE_NORMAL, magenta, 0, "Magenta is Variance");
   ortho_done();*/

   // NB: draw_text_user called automatically from qcn_graphics to show BOINC username, CPU time etc
}

void CCube::RenderScene( GLsizei iWidth, GLsizei iHeight, GLfloat viewpoint, GLfloat pitch, GLfloat roll )
{ 
    static int iRecompute = 0;
    static float size[4] = {.3,.3,.3,.3};
    //static float newsize[4] = {.3,.3,.3,.3};
	
    m_iWidth = iWidth;
    m_iHeight = iHeight;
    m_viewpoint = viewpoint;
 
    bVar = (bool)(iRecompute++ % 10);

    // cube size depends on our variance, have it range from .3 to .8, default to .4
	// sample the last 1 second to find a max, and then see if our value is the max for that range

    float fTest[4]  = {0,0,0,0};
    float fSubsample[4]  = {0,0,0,0};
    long lMaxOffset = sm ? sm->lOffset-2 : 0;
    float fCtr = 0;
    float fTest2[4] = {0,0,0,0};

    const float cfNumSeconds = .2f;
    const float cfNumSecondsMean = 2.0f;
    long lStartOffset = sm ? (long)(cfNumSeconds/sm->dt) : 0;
    long lStartMean   = sm ? (long)(cfNumSecondsMean/sm->dt) : 0;

    // compute the on-line variance by Knuth / Welford  (http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance (III))
    if (sm && lMaxOffset > lStartOffset && lMaxOffset > lStartMean) { // get the max for a 3-second window
           if (!bVar) { // compute baseline, note these values are member vars so just set once
              iRecompute = 1;
 	      for (long i = lMaxOffset; i >= lMaxOffset - lStartMean; i--) {
                  fCtr++;
                  fTest[E_DX] = sm->x0[i];
                  fTest[E_DY] = sm->y0[i];
                  fTest[E_DZ] = sm->z0[i];
                  for (int j = E_DX; j <= E_DZ ; j++) {
                     fDelta[j] = fTest[j] - fMean[j];
                     fMean[j]  += (fDelta[j]/fCtr);
                     fM2[j]    += (fDelta[j] * (fTest[j] - fMean[j]));
                  }
              }
              for (int j = E_DX; j <= E_DZ ; j++) {
                 fVariance[j] = fM2[j]/(fCtr == 0.0f ? 1.0f : fCtr);
                 //fVarianceN[j] = fM2[j]/(fCtr-1);
                 fStdDev[j] = sqrt(fVariance[j]);
	      }
	   }

           fCtr = 0;
           // now sample the last second
 	   for (long i = lMaxOffset; i >= lMaxOffset - lStartOffset; i--) {
              fCtr++;
              fTest[E_DX] = sm->x0[i];
              fTest[E_DY] = sm->y0[i];
              fTest[E_DZ] = sm->z0[i];
              for (int j = E_DX; j <= E_DZ ; j++) {
                  fSubsample[j] += fTest[j];
              }
	   }
    }

    for (int j = E_DX; j <= E_DZ ; j++) {
        fSubsample[j] /= fCtr;
//Changed by JL: Not normalized by Standard Deviation.
        fTest[j] = (fSubsample[j] - fMean[j]); // fStdDev[j]; // more than +4 is huge, +2 is big, with stddev is normal, -2 smaller, -4 tiny
/*
        newsize[j] = 0.30f + ((10.0f*(fTest[j]/4.0f) + fMouseFactor;
        // don't do abrupt changes, so if newsize is much bigger than size just do a little change (.1)
        if (newsize[j] < size[j] + .1f) size[j] -= .1f;     
        else if (newsize[j] > size[j] + .1f) size[j] += .1f;     
*/
//Changed by JL: Was divide by 4. No particular reason for 20.
        size[j] = 0.30f + fTest[j]/20.0f + fMouseFactor;

//Commented out by JL: We need negative values.
/*        if (size[j] > 1.3f) 
            size[j] = 1.3f;
        else if (size[j] < .05f)
            size[j] = .05f;   
*/
    }
    RenderCube(size);
}

void CCube::MouseButton(int x, int y, int which, int is_down)
{
	mouseX = x;
	mouseY = y;
	if (which == GLUT_LEFT_BUTTON && is_down) {
            bMouseProcessed = (bool) isShiftDown;
	    mouseLeftButtonDown = 1;
        fMouseFactor += 0.1f;
        if (fMouseFactor > 1.00f) fMouseFactor = 1.00f;
	}
        else {
	    mouseLeftButtonDown = 0;
        }
	if (which == GLUT_RIGHT_BUTTON && is_down) {
            bMouseProcessed = (bool) isShiftDown;
	    mouseRightButtonDown = 1;
        fMouseFactor -= 0.1f;
        if (fMouseFactor < -1.00f) fMouseFactor = -1.00f;
	}
    else {
	    mouseRightButtonDown = 0;
    }
}

void CCube::MouseMotion(int x, int y, int left, int middle, int right)
{
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

void CCube::KeyDown(int key1, int key2)
{ 
    iKey = key1;
    iKeySpecial = key2;
    switch(iKey)
    {
        //case 'p':
        //case 'P':  //bProjection = !bProjection;  break;
        case 'a':
        case 'A':  bAutoRotate = ! bAutoRotate; break;
        //case 45: glptr[0]++; if (glptr[0]>11) glptr[0] = 0; break;
        //case 61: glptr[1]++; if (glptr[1]>11) glptr[1] = 0; break;
    }
}

void CCube::KeyUp(int key1, int key2)
{ 
    iKey = key1;
    iKeySpecial = key2;
}

// note can pass in your light position
void CCube::InitLights()
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
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
        // local viewer for specular light?
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
        // enable smoooth shading (multi-coloured polygons)
        glShadeModel(GL_FLAT);
        // fill polygons
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void CCube::InitCamera()  
{
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        // orthographic projection
        glOrtho(-2.8, 2.8, -2.05, 2.05, -10, 10.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        SetViewpoint();
}


void CCube::OrthographicMatrix()
{
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
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

void CCube::SetViewpoint(double x, double y, double z)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        x, y, z,        // eye position
        -0.80,-.80,-.80,        // where we're looking
        0.0, 1.0, 0.    // up is in positive Y direction
    );
}

void CCube::DrawPlot(const GLfloat* pfEnd, const GLfloat* asize, const eCubeFace face)
{
  if (!pfEnd) return; // no plot if no sensor or we just started!

  GLfloat size = 0.0f;

  // select the right size depending on which face we're drawing 
        switch(face) {
           case CUBE_LEFT:
           case CUBE_RIGHT:
               size = asize[E_DX];
               break;
           case CUBE_TOP:
           case CUBE_BOTTOM:
               size = asize[E_DY];
               break;
           case CUBE_FRONT:
           case CUBE_BACK:
               size = asize[E_DZ];
               break;
        }

  // if here we're guaranteed that we have at least 100 (MAX_PLOT_POINTS) points from (pfEnd-100) to pfEnd
  const GLfloat *pfBegin = pfEnd - MAX_PLOT_POINTS;

  // get the maximum value for scaling
  float fmaxfactor = -1e9f, fmin = 1e9f, fmax = -1e9f; 
  for (float *pfIt = (float*) pfBegin; pfIt <= pfEnd; pfIt++) {
     if (*pfIt > fmax) fmax = *pfIt;
     if (*pfIt < fmin) fmin = *pfIt;
  }
  if ((fmax-fmin) == 0.0f) fmax = fmin+0.01f; // no divide by zero!
  
  if (fmax < 0.0f) { // if negative, all values are negative, so it will be off screen unless correct, so use -fmin
        fmaxfactor = size / (-fmin*2.0f); 
  } 
  else {
        fmaxfactor = size / (fmax*2.0f);   // 1.2f is a scaling factor that looks good  
  }

  // fprintf(stdout, "drawing plot for %f to %f - fmaxfactor=%f  size=%f\n", *pfBegin, *pfEnd, fmaxfactor, size);

  glLineWidth(1);

/*
  // set the color to the RGB "inverse" of the current (cube face) color
  GLfloat afcolor[4] = {0,0,0,0};
  glGetFloatv(GL_CURRENT_COLOR, afcolor);
 
  for (int j = 0; j < 3; j++)  {
     if (afcolor[j] == 1.0f) 
         afcolor[j] = 0.0f;
     else if (afcolor[j] == 0.0f) 
         afcolor[j] = 1.0f;
  } 
  // invert
  glColor4fv(afcolor);  

fprintf(stdout, "face color = %f %f %f %f\n", 
  afcolor[0],
  afcolor[1],
  afcolor[2],
  afcolor[3]);
*/

  glColor4fv(black);  

  glBegin(GL_LINE_STRIP);
  int i = 0;
  //GLfloat Xabs, Yabs, x = 0.0f, y = 0.0f, z = 0.0f;
  GLfloat x = 0.0f, y = 0.0f, z = 0.0f;
  for (float *pfIt = (float*) pfBegin; pfIt <= pfEnd; pfIt++) {
        // the "absolute" X & Y values -- i.e. before we figure out the translation to the vertex etc
        //Xabs =  // absolute x ranges from -size to +size
        //Yabs = ;  // absolute Y is scaled to the fmaxfactor calculated above to ensure we're on the cube face

        switch(face) {
           case CUBE_TOP:
               x = (((float) ++i / (float) (MAX_PLOT_POINTS)) * -asize[E_DX] * 2) + asize[E_DX];  // the top of the cube, we start at x is 0
               y = size;
               z = ((*pfIt-fmin)/(fmax-fmin)) * asize[E_DZ];
               break;
           case CUBE_BOTTOM:
               x = (((float) ++i / (float) (MAX_PLOT_POINTS)) * -asize[E_DX] * 2) + asize[E_DX];;
               y = -size;
               z = ((*pfIt-fmin)/(fmax-fmin)) * asize[E_DZ];
               break;
           case CUBE_FRONT:
               x = (((float) ++i / (float) (MAX_PLOT_POINTS)) * -asize[E_DX] * 2) + asize[E_DX];;
               y = ((*pfIt-fmin)/(fmax-fmin)) * asize[E_DY];
               z = size;
               break;
           case CUBE_BACK:
               x = (((float) ++i / (float) (MAX_PLOT_POINTS)) * -asize[E_DX] * 2) + asize[E_DX];;
               y = ((*pfIt-fmin)/(fmax-fmin)) * asize[E_DY];
               z = -size;
               break;
           case CUBE_LEFT:
               x = size;
               y = ((*pfIt-fmin)/(fmax-fmin)) * asize[E_DY];
               z = (((float) ++i / (float) (MAX_PLOT_POINTS)) * -asize[E_DZ] * 2) + asize[E_DZ];;
               break;
           case CUBE_RIGHT:
               x = -size;
               y = ((*pfIt-fmin)/(fmax-fmin)) * asize[E_DY];
               z = (((float) ++i / (float) (MAX_PLOT_POINTS)) * -asize[E_DZ] * 2) + asize[E_DZ];;
               break;
        }

         // the vertex depends on which face we're drawing from the TopLeft & BottomRight values passed in above
        glVertex3f(x, y, z);
   }
   glEnd();
}

void CCube::RenderCube(const GLfloat* asize)
{
  static double dOff[3] = {-1.0f, -1.0f, -.5f};
  static const double dMin[3] = {-1.0f, -1.0f, -.5f};
  static const double dMax[3] = {2.0f, 1.5f, 1.0f};
  static const double dFactor[3] = {.01f, .01f, .01f};
  static bool bDec[3] = { false, false, false};

  InitCamera();

  glLineWidth(3);

  diffuse[0] = diffuse[1] = diffuse[2] = 0.8 + scaleAll;
  specular[0] = specular[1] = specular[2] = 1.0;

  glPushMatrix();   // light matrix
  OrthographicMatrix();


//Changed by JL: MOVES box according to accelleration.
  glTranslatef(0.5 - asize[E_DX]/4.f, 
               0.5 - asize[E_DZ]/4.f, 
	       0.5 + asize[E_DY]*2.f);

  for (int i = 0; i < 3; i++)  {
     if (dOff[i]>dMax[i]) { bDec[i] = true; }
     else if (dOff[i]<dMin[i]) { bDec[i] = false; }
     dOff[i] += (dFactor[i] * (bDec[i] ? -1.0 : 1.0));
  } 

//Changed by JL: Constant Rotation:  Need non-demeaned values of x,y,z.
  glRotatef(rot.x, rot.y, rot.z, 0.0);
  rot.x = 20.f;  //Didn't work well: atan((asize[E_DZ])/asize[E_DY])*18.f/3.14;//rotationSpeed * 1.9134f;
  rot.y = 20.f;  //Didn't work well: atan((asize[E_DZ])/asize[E_DX])*18.f/3.14+20.;//rotationSpeed * 2.1234f;
  rot.z  = 20.f;

  glPushMatrix();

  //glTranslatef(0.0f, 0.0f,-7.0f);	// Translate Into The Screen 7.0 Units
  //glRotatef(rotqube,0.0f,1.0f,0.0f);	// Rotate The cube around the Y axis
  //glRotatef(rotqube,1.0f,1.0f,1.0f);

  // get an appropriate lOffset if available - we want at least 100 points + 2 for "padding" 
  long lOffset = 0; 
  if (sm && sm->bSensorFound) {
     if (sm->lOffset > MAX_PLOT_POINTS+2) { // note the two point "padding" so we are reading the latest value that isn't in use by the sensor (i.e. writing)
        lOffset = sm->lOffset-2;
     }
  }
  
//Changed by JL: Use Size for in and out of screen (Y AXIS).  Size of all axes is same now.
  float asize_ave = sqrt(sqrt(pow(2.f,asize[E_DY])))/3.f;

  glBegin(GL_QUADS);            // Draw The Cube Using quads
    glColor4fv(blue);          // Color Green 
    glVertex3f( asize_ave, asize_ave,-asize_ave);      // Top Right Of The Quad (Top)
    glVertex3f(-asize_ave, asize_ave,-asize_ave);      // Top Left Of The Quad (Top)
    glVertex3f(-asize_ave, asize_ave, asize_ave);      // Bottom Left Of The Quad (Top)
    glVertex3f( asize_ave, asize_ave, asize_ave);      // Bottom Right Of The Quad (Top)
  glEnd();
  // note the asize in DrawPlot below should be the "y-axis" for whichever face we're looking at
//  DrawPlot(lOffset ? (const GLfloat*) &(sm->x0[lOffset]) : NULL, asize, CUBE_TOP);  // x-axis is green

  glBegin(GL_QUADS);            // Draw The Cube Using quads
    glColor4fv(green);         // Color Yellow
    glVertex3f( asize_ave,-asize_ave, asize_ave);      // Top Right Of The Quad (Bottom)
    glVertex3f(-asize_ave,-asize_ave, asize_ave);      // Top Left Of The Quad (Bottom)
    glVertex3f(-asize_ave,-asize_ave,-asize_ave);      // Bottom Left Of The Quad (Bottom)
    glVertex3f( asize_ave,-asize_ave,-asize_ave);      // Bottom Right Of The Quad (Bottom)
  glEnd();
//  DrawPlot(lOffset ? (const float*) &(sm->y0[lOffset]) : NULL, asize, CUBE_BOTTOM);   // y-axis is yellow

  glBegin(GL_QUADS);            // Draw The Cube Using quads
    glColor4fv(yellow);           // Color Blue 
    glVertex3f( asize_ave, asize_ave, asize_ave);      // Top Right Of The Quad (Front)
    glVertex3f(-asize_ave, asize_ave, asize_ave);      // Top Left Of The Quad (Front)
    glVertex3f(-asize_ave,-asize_ave, asize_ave);      // Bottom Left Of The Quad (Front)
    glVertex3f( asize_ave,-asize_ave, asize_ave);      // Bottom Right Of The Quad (Front)
  glEnd();
//  DrawPlot(lOffset ? (const float*) &(sm->z0[lOffset]) : NULL, asize, CUBE_FRONT);  // z-axis is blue

  glBegin(GL_QUADS);            // Draw The Cube Using quads
    glColor4fv(yellow);            // Color Red
    glVertex3f( asize_ave,-asize_ave,-asize_ave);      // Top Right Of The Quad (Back)
    glVertex3f(-asize_ave,-asize_ave,-asize_ave);      // Top Left Of The Quad (Back)
    glVertex3f(-asize_ave, asize_ave,-asize_ave);      // Bottom Left Of The Quad (Back)
    glVertex3f( asize_ave, asize_ave,-asize_ave);      // Bottom Right Of The Quad (Back)
  glEnd();
//  DrawPlot(lOffset ? (const float*) &(sm->fsig[lOffset]) : NULL, asize, CUBE_BACK);   // fsig/significance is red

  glBegin(GL_QUADS);            // Draw The Cube Using quads
    glColor4fv(green);           // Color Cyan
    glVertex3f(-asize_ave, asize_ave, asize_ave);      // Top Right Of The Quad (Left)
    glVertex3f(-asize_ave, asize_ave,-asize_ave);      // Top Left Of The Quad (Left)
    glVertex3f(-asize_ave,-asize_ave,-asize_ave);      // Bottom Left Of The Quad (Left)
    glVertex3f(-asize_ave,-asize_ave, asize_ave);      // Bottom Right Of The Quad (Left)
  glEnd();
//  DrawPlot(lOffset ? (const float*) &(sm->fmag[lOffset]) : NULL, asize, CUBE_LEFT);   // magnitude

  glBegin(GL_QUADS);            // Draw The Cube Using quads
    glColor4fv(blue);        // Color Magenta
    glVertex3f( asize_ave, asize_ave,-asize_ave);      // Top Right Of The Quad (Right)
    glVertex3f( asize_ave, asize_ave, asize_ave);      // Top Left Of The Quad (Right)
    glVertex3f( asize_ave,-asize_ave, asize_ave);      // Bottom Left Of The Quad (Right)
    glVertex3f( asize_ave,-asize_ave,-asize_ave);      // Bottom Right Of The Quad (Right)
  glEnd();
//  DrawPlot(lOffset ? (const float*) &(sm->vari[lOffset]) : NULL, asize, CUBE_RIGHT);   // variance

  glPopMatrix();
  glPopMatrix(); // rotation matrix

  glFlush();
}

