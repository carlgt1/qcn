// cube 

#ifdef _WIN32
#pragma warning( disable : 4244 )  // Disable warning messages for double to float conversion
#pragma warning( disable : 4305 )  // Disable warning messages for double to GLfloat conversion
#pragma warning( disable : 4800 )  // Disable warning messages for "forcing value to bool"
#endif

#include "qcn_game_match.h"
#include "qcn_shmem.h"

const int CGameMatch::iMaxTextures = 4;
const int CGameMatch::MAX_PLOT_POINTS = 500;

// constructor
CGameMatch::CGameMatch()  
{ 
	//ptEq.x = ptEq.y = ptEq.z = 0;
	bVar = false;
	fMean[0] = fMean[1] = fMean[2] = fMean[3] = 0.0f;
	fM2[0] = fM2[1] = fM2[2] = fM2[3] = 0.0f;
	fDelta[0] = fDelta[1] = fDelta[2] = fDelta[3] = 0.0f;
	fMin[0] = fMin[1] = fMin[2] = fMin[3] = 0.0f;
	fMax[0] = fMax[1] = fMax[2] = fMax[3] = 0.0f;
	fVariance[0] = fVariance[1] = fVariance[2] = fVariance[3] = 0.0f;
	fStdDev[0] = fStdDev[1] = fStdDev[2] = fStdDev[3] = 0.0f;
	
#ifdef QCNLIVE
	//#ifdef _DEBUG
	//	   bIsQCNLive = false;
	//#else
	bIsQCNLive = true;
	//#endif
#else
	bIsQCNLive = false;
#endif
	
	fMouseFactor = 0.0f;
	mouseX = mouseY = -1;
	mousePX = mousePY = mousePZ = -1;
	iKey = iKeySpecial = mouseRightButtonDown = isShiftDown = mouseLeftButtonDown = isCtrlDown = 0;
	
		rot.x = 0, rot.y = 0;  // note rot.x is our startup angle
		rotationSpeed = ROTATION_SPEED_DEFAULT;
		bAutoRotate = true;  
	
	
	//bEarthDay = true;
	bMouseProcessed = false;
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
CGameMatch::~CGameMatch()
{
	//if (m_textureID[0])
	//  glDeleteTextures(6, m_textureID);
	
}

void CGameMatch::Resize( GLsizei iWidth, GLsizei iHeight )
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

void CGameMatch::Idle()
{
}

void CGameMatch::Init()
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

void CGameMatch::RenderText()
{  // this draws the necessary text for the cube view
	// draw text on top
	
		char buf[256];
		// left of window informative text
		if (!qcn_graphics::g_bFullScreen)  { // don't show the button press text in fullscreen/screensaver mode
			mode_unshaded();
			qcn_graphics::mode_ortho_qcn();
			sprintf(buf, "Press 'Q' to return to");
			TTFont::ttf_render_string(qcn_graphics::cfTextAlpha, 0, .32, 0, MSG_SIZE_NORMAL, yellow, TTF_ARIAL, buf);
			sprintf(buf, "seismic sensor view");
			TTFont::ttf_render_string(qcn_graphics::cfTextAlpha, 0, .30, 0, MSG_SIZE_NORMAL, yellow, TTF_ARIAL, buf);
			ortho_done();
		}
		
		// print a "legend"
		mode_unshaded();
		qcn_graphics::mode_ortho_qcn();
		TTFont::ttf_render_string(qcn_graphics::cfTextAlpha, 0, .27, 0, MSG_SIZE_NORMAL, red, TTF_ARIAL, "Red is Significance");
		TTFont::ttf_render_string(qcn_graphics::cfTextAlpha, 0, .25, 0, MSG_SIZE_NORMAL, blue, TTF_ARIAL, "Blue is Z-axis");
		TTFont::ttf_render_string(qcn_graphics::cfTextAlpha, 0, .23, 0, MSG_SIZE_NORMAL, yellow, TTF_ARIAL, "Yellow is Y-axis");
		TTFont::ttf_render_string(qcn_graphics::cfTextAlpha, 0, .21, 0, MSG_SIZE_NORMAL, green, TTF_ARIAL, "Green is X-axis");
		TTFont::ttf_render_string(qcn_graphics::cfTextAlpha, 0, .19, 0, MSG_SIZE_NORMAL, cyan, TTF_ARIAL, "Cyan is Magnitude");
		TTFont::ttf_render_string(qcn_graphics::cfTextAlpha, 0, .17, 0, MSG_SIZE_NORMAL, magenta, TTF_ARIAL, "Magenta is Variance");
		ortho_done();
	// NB: draw_text_user called automatically from qcn_graphics to show BOINC username, CPU time etc
}

void CGameMatch::RenderScene( GLsizei iWidth, GLsizei iHeight, GLfloat viewpoint, GLfloat pitch, GLfloat roll )
{ 
    static int iRecompute = 0;
    static float size[4] = {.3,.3,.3,.3};
    //static float newsize[4] = {.3,.3,.3,.3};
	
    //glColor4fv(black);
    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
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
	
    const float cfNumSeconds = .2f;
    const float cfNumSecondsMean = 2.0f;
    long lStartOffset = sm ? (long)(cfNumSeconds/sm->dt) : 0;
    long lStartMean   = sm ? (long)(cfNumSecondsMean/sm->dt) : 0;
	
    if (sm && lMaxOffset > lStartOffset && lMaxOffset > lStartMean) { // get the max for a 3-second window
		if (!bVar) { // compute baseline, note these values are member vars so just set once
			iRecompute = 1;
			qcn_util::ComputeMeanStdDevVarianceKnuth((const float*) sm->x0, MAXI, lMaxOffset - lStartMean, lMaxOffset, &fMean[E_DX], &fStdDev[E_DX], &fVariance[E_DX], &fMin[E_DX], &fMax[E_DX]);
			qcn_util::ComputeMeanStdDevVarianceKnuth((const float*) sm->y0, MAXI, lMaxOffset - lStartMean, lMaxOffset, &fMean[E_DY], &fStdDev[E_DY], &fVariance[E_DY], &fMin[E_DY], &fMax[E_DY]);
			qcn_util::ComputeMeanStdDevVarianceKnuth((const float*) sm->z0, MAXI, lMaxOffset - lStartMean, lMaxOffset, &fMean[E_DZ], &fStdDev[E_DZ], &fVariance[E_DZ], &fMin[E_DZ], &fMax[E_DZ]);
		}
		
		fCtr = 0;
		// now sample the last second
		for (long i = lMaxOffset; i >= lMaxOffset - lStartOffset; i--) {
			fCtr++;
			fTest[E_DX] = sm->x0[i];
			fTest[E_DY] = sm->y0[i];
			fTest[E_DZ] = sm->z0[i];
			for (int j = E_DX; j <= E_DZ ; j++) {
				fSubsample[j] += fTest[j];//Changed by Jesse Lawrence-JUST Y DIRECTION NOW.
				//                    fSubsample[j]=fTest[E_DY];
			}
		}
    }
	
    for (int j = E_DX; j <= E_DZ ; j++) {
        fSubsample[j] /= fCtr;
        fTest[j] = (fSubsample[j] - fMean[j]); // fStdDev[E_DY]; // more than +4 is huge, +2 is big, with stddev is normal, -2 smaller, -4 tiny
		/*
		 newsize[j] = 0.30f + ((10.0f*(fTest[j]/4.0f) + fMouseFactor;
		 // don't do abrupt changes, so if newsize is much bigger than size just do a little change (.1)
		 if (newsize[j] < size[j] + .1f) size[j] -= .1f;     
		 else if (newsize[j] > size[j] + .1f) size[j] += .1f;     
		 */
        size[j] = 0.30f + fTest[j]/4.0f + fMouseFactor;
        if (size[j] > 2.0f) 
            size[j] = 2.0f;
		
        else if (size[j] < -1.3f)
            size[j] = -1.3f; //Jesse Lawrence Added - Maximum negative value
		
        else if (size[j] < .05f && size[j] > 0.f) 
            size[j] = .05f;   
		
        else if (size[j] > -.05f && size[j] <= 0.f)
            size[j] = -.05f;//Jesse Lawrence Added - Minimum negative value
    }
    size[E_DY] = ( (size[E_DY]+1.3f) /6.f );//Jesse Lawrence Added - Y to non-negative number
    RenderCube(size);
}

void CGameMatch::MouseButton(int x, int y, int which, int is_down)
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

void CGameMatch::MouseMotion(int x, int y, int left, int middle, int right)
{
	int	deltX, deltY;
	
	// calculate mouse movement since click
	deltX = x - mouseX;
	deltY = y - mouseY;
	
	// store new mouse position
	mouseX = x;
	mouseY = y;
	
	//mouseLeftButtonDown = left;
	//mouseRightButtonDown = right;
	
	if (left) {
		// rotate
		rot.x = x; //+= deltX; //*0.25f/scaleAll;
		//if (rot.x > 50) rot.x = -50;
		rot.y = y; // += (deltY*10); //*0.25f/scaleAll;
		//if (rot.y > 1000) rot.y = -1000;
		
		// save values for auto rotation
		//autoRotX = deltX*0.25f;
		//autoRotY = deltY*0.25f;
	} 
	else if (right)  { //zoom
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

void CGameMatch::KeyDown(int key1, int key2)
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

void CGameMatch::KeyUp(int key1, int key2)
{ 
    iKey = key1;
    iKeySpecial = key2;
}

// note can pass in your light position
void CGameMatch::InitLights()
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

void CGameMatch::InitCamera()  
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// orthographic projection
	glOrtho(-2.8, 2.8, -2.05, 2.05, -10, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	SetViewpoint();
}


void CGameMatch::OrthographicMatrix()
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

void CGameMatch::SetViewpoint(double x, double y, double z)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
			  x, y, z,        // eye position
			  -0.80,-.80,-.80,        // where we're looking
			  0.0, 1.0, 0.    // up is in positive Y direction
			  );
}

void CGameMatch::RenderCube(const GLfloat* asize)
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
	
	// get an appropriate lOffset if available - we want at least 100 points + 2 for "padding" 
	long lOffset = 0; 
	if (sm && sm->bSensorFound) {
		if (sm->lOffset > MAX_PLOT_POINTS+2) { // note the two point "padding" so we are reading the latest value that isn't in use by the sensor (i.e. writing)
			lOffset = sm->lOffset-2;
		}
	}
	
		glTranslatef(-.50 + dOff[0], dOff[1], dOff[2]);
	
	for (int i = 0; i < 3; i++)  {
		if (dOff[i]>dMax[i]) { bDec[i] = true; }
		else if (dOff[i]<dMin[i]) { bDec[i] = false; }
		dOff[i] += (dFactor[i] * (bDec[i] ? -1.0 : 1.0));
	} 
	
	glRotatef(rot.x, rot.y, rot.y/3.0f, 0.0);
	rot.x += rotationSpeed * 1.9134f;
	rot.y += rotationSpeed * 2.1234f;
	
	glPushMatrix();
	
	//glTranslatef(0.0f, 0.0f,-7.0f);	// Translate Into The Screen 7.0 Units
	//glRotatef(rotqube,0.0f,1.0f,0.0f);	// Rotate The cube around the Y axis
	//glRotatef(rotqube,1.0f,1.0f,1.0f);
	// screensaver mode
		glBegin(GL_QUADS);            // Draw The Cube Using quads
		glColor4fv(green);          // Color Green 
		glVertex3f( asize[E_DX], asize[E_DY], -asize[E_DZ]);      // Top Right Of The Quad (Top)
		glVertex3f(-asize[E_DX], asize[E_DY],-asize[E_DZ]);      // Top Left Of The Quad (Top)
		glVertex3f(-asize[E_DX], asize[E_DY], asize[E_DZ]);      // Bottom Left Of The Quad (Top)
		glVertex3f( asize[E_DX], asize[E_DY], asize[E_DZ]);      // Bottom Right Of The Quad (Top)
		glEnd();
		
		glBegin(GL_QUADS);            // Draw The Cube Using quads
		glColor4fv(yellow);         // Color Yellow
		glVertex3f( asize[E_DX],-asize[E_DY], asize[E_DZ]);      // Top Right Of The Quad (Bottom)
		glVertex3f(-asize[E_DX],-asize[E_DY], asize[E_DZ]);      // Top Left Of The Quad (Bottom)
		glVertex3f(-asize[E_DX],-asize[E_DY],-asize[E_DZ]);      // Bottom Left Of The Quad (Bottom)
		glVertex3f( asize[E_DX],-asize[E_DY],-asize[E_DZ]);      // Bottom Right Of The Quad (Bottom)
		glEnd();
		
		glBegin(GL_QUADS);            // Draw The Cube Using quads
		glColor4fv(blue);           // Color Blue 
		glVertex3f( asize[E_DX], asize[E_DY], asize[E_DZ]);      // Top Right Of The Quad (Front)
		glVertex3f(-asize[E_DX], asize[E_DY], asize[E_DZ]);      // Top Left Of The Quad (Front)
		glVertex3f(-asize[E_DX],-asize[E_DY], asize[E_DZ]);      // Bottom Left Of The Quad (Front)
		glVertex3f( asize[E_DX],-asize[E_DY], asize[E_DZ]);      // Bottom Right Of The Quad (Front)
		glEnd();
		
		glBegin(GL_QUADS);            // Draw The Cube Using quads
		glColor4fv(red);            // Color Red
		glVertex3f( asize[E_DX],-asize[E_DY],-asize[E_DZ]);      // Top Right Of The Quad (Back)
		glVertex3f(-asize[E_DX],-asize[E_DY],-asize[E_DZ]);      // Top Left Of The Quad (Back)
		glVertex3f(-asize[E_DX], asize[E_DY],-asize[E_DZ]);      // Bottom Left Of The Quad (Back)
		glVertex3f( asize[E_DX], asize[E_DY],-asize[E_DZ]);      // Bottom Right Of The Quad (Back)
		glEnd();
		
		glBegin(GL_QUADS);            // Draw The Cube Using quads
		glColor4fv(cyan);           // Color Cyan
		glVertex3f(-asize[E_DX], asize[E_DY], asize[E_DZ]);      // Top Right Of The Quad (Left)
		glVertex3f(-asize[E_DX], asize[E_DY],-asize[E_DZ]);      // Top Left Of The Quad (Left)
		glVertex3f(-asize[E_DX],-asize[E_DY],-asize[E_DZ]);      // Bottom Left Of The Quad (Left)
		glVertex3f(-asize[E_DX],-asize[E_DY], asize[E_DZ]);      // Bottom Right Of The Quad (Left)
		glEnd();
		
		glBegin(GL_QUADS);            // Draw The Cube Using quads
		glColor4fv(magenta);        // Color Magenta
		glVertex3f( asize[E_DX], asize[E_DY],-asize[E_DZ]);      // Top Right Of The Quad (Right)
		glVertex3f( asize[E_DX], asize[E_DY], asize[E_DZ]);      // Top Left Of The Quad (Right)
		glVertex3f( asize[E_DX],-asize[E_DY], asize[E_DZ]);      // Bottom Left Of The Quad (Right)
		glVertex3f( asize[E_DX],-asize[E_DY],-asize[E_DZ]);      // Bottom Right Of The Quad (Right)
		glEnd();
	
	glPopMatrix();
	glPopMatrix(); // rotation matrix
	
	glFlush();
}
