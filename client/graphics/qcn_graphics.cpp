// CMC -- OpenGL Graphics for QCN project
// (c) 2007 Stanford University

#ifdef _WIN32
  #pragma warning( disable : 4244 )  // Disable warning messages for double to float conversion
#endif

#include "qcn_graphics.h"
#include "qcn_2dplot.h"
#include "qcn_earth.h"
#include "qcn_cube.h"
#ifdef QCNLIVE
#include "qcn_game_match.h"
#else
#include "qcn_signal.h"
CQCNShMem* volatile sm = NULL;             // the main shared memory pointer
#endif
#include "filesys.h"  // boinc include

using std::string;
using std::vector;

float fDiff2D = 0.0f; // mouse drag difference

// note the colors are extern'd in define.h, so keep outside the namespace qcn_graphics
GLfloat white[4] = {1., 1., 1., 1.};
GLfloat red[4] = {1., 0., 0., 1.};
GLfloat green[4] = {0., 1., 0., 1.};
GLfloat yellow[4] = {1., 1., 0., 1.};
GLfloat orange[4] = {1., .5, 0., 1.};
GLfloat blue[4] = {0., 0., 1., 1.};
GLfloat magenta[4] = {1., 0., 1., 1.};
GLfloat cyan[4] = {0., 1., 1., 1.};
GLfloat dark_blue[4] = {  1.0f/255.f,   1.0f/255.f, 101.0f/255.f, 1.0f};
GLfloat dark_green[4] = { 32.0f/255.f, 101.0f/255.f,   8.0f/255.f, 1.0f};
GLfloat black[4] = {0., 0., 0., 1.};
GLfloat black_trans[4] = {0., 0., 0., .8};
GLfloat trans_red[4] = {1., 0., 0., .5};
GLfloat trans_yellow[4] = {1., 1., 0., .5};
GLfloat grey[4] = {.4,.4,.4,.3};
GLfloat grey_trans[4] = {.6,.6,.6,.6};
GLfloat white_trans[4] = {1., 1., 1., 0.50f};
GLfloat light_blue[4] = {0., 0., .5f, .5f};

// an hour seems to be the max to vis without much delay
static double g_dtOrig = g_DT;

static long awinsize[MAX_KEY_WINSIZE+1]; 
static const double awinsize_seconds[MAX_KEY_WINSIZE+1] = {10.0, 60.0, 600.0, 3600.0};
static int key_winsize = 0; // points to an element of the above array to get the winsize i.e. 0=10 sec , 1=60 sec , 2= 10 min, 3 = hour
static int key_press = 0;
static int key_press_alt = 0;
static int key_up = 0;
static int key_up_alt = 0;
static int g_iTimeWindowWidth = 10;  // default to a 10 second window (fast)

static long g_lSnapshotPoint = 0L;
static long g_lSnapshotPointOriginal = 0L;
static long g_lSnapshotTimeBackSeconds = 0L;  // the minutes back in time we've gone for snapshot
static bool g_bSnapshot = false;
static bool g_bSnapshotArrayProcessed = false;

static int g_iTimePercent = 100;

FADER::FADER(double g, double n, double f, double o, double ma) {
	maxalpha = ma;
	grow = g;
	on = n;
	fade = f;
	off = o;
	start = 0;
	total = grow + on + fade + off;
}

bool FADER::value(const double& t, double& v, bool bReset) {
	if (t < 1.) return false;
	if (bReset) {
		start = t;
		v = maxalpha;
		return true;
	}
	if (!start) {
		start = t;
		v = maxalpha;
		return false;
	}
	double dt = t - start;
	if (dt < on) { // on
		v = maxalpha;
	} else if (dt < on + fade) {  // fading
		v = (1.0 - ((dt-on)/fade)) * maxalpha;
	} else if (dt < off + on + fade) {  // off
		v = 0;
	} else if (dt < total) {  // growing
		double x = dt-(off+on+fade);
		v = maxalpha * (x/grow);
	} else { // reset
		start = t;
		v = maxalpha;
		return true;
	}
	return false;
}


#ifdef QCNLIVE

void SMakeQuake::clear() 
{ 
	/*
	static bool bFirst = true;
	if (!bFirst) { 
		// check byte array is empty
		if (data) {
			// for example it's in the middle of a screenshot and they hit print
			delete [] data;
		}
	}
	*/
	bActive = false;
	bDisplay = false;
	bReceived = false;
	dStart = 0.0;
	iTime = 0;
	iCountdown = 0;
	memset(strName, 0x00, sizeof(char) * 64);
	//data = NULL;
	//bFirst = false;
}
	
#else 

void qcn_graphics_exit()
{
   qcn_graphics::Cleanup();
}

// install signal handlers
void graphics_signal_handler(int iSignal)
{
   fprintf(stderr, "Signal %d received, exiting...\n", iSignal);
   qcn_graphics::Cleanup();
   _exit(EXIT_SIGNAL);
}

#endif  // qcnlive

// the following are required by BOINC
void app_graphics_render(int xs, int ys, double time_of_day) 
{
    qcn_graphics::Render(xs, ys, time_of_day);
}

void app_graphics_resize(int w, int h)
{
    qcn_graphics::Resize(w, h);
}

void app_graphics_reread_prefs() 
{
   fprintf(stderr, "BOINC Graphics app_graphics_reread_prefs() request to reread project prefs at %f\n", dtime());
   qcn_graphics::getProjectPrefs();
}

// mouse drag w/ left button rotates 3D objects;
// mouse draw w/ right button zooms 3D objects
//
void boinc_app_mouse_move(int x, int y, int left, int middle, int right) 
{
    qcn_graphics::MouseMove(x, y, left, middle, right);
}

void boinc_app_mouse_button(int x, int y, int which, int is_down) 
{
    qcn_graphics::MouseButton(x, y, which, is_down);
}

void boinc_app_key_press(int k1, int k2)
{ 
   qcn_graphics::KeyDown(k1, k2);
}

void boinc_app_key_release(int k1, int k2)
{
   qcn_graphics::KeyUp(k1, k2);
}

void app_graphics_init() 
{
    qcn_graphics::Init();
}
//#endif

#ifndef QCNLIVE  
// the main entry point for BOINC standalone graphics (i.e. separate executable)
int main(int argc, char** argv) 
{
#ifndef _DEBUG
   atexit(qcn_graphics_exit);
   qcn_signal::InstallHandlers(graphics_signal_handler);   // note this is set to ignore SIGPIPE by default
#endif
   return qcn_graphics::graphics_main(argc, argv);
}

/*
#ifdef _WIN32   // transfer WinMain call in Windows to our regular main()
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR Args, int WinMode) {
    LPSTR command_line;
    char* argv[100];
    int argc;

    command_line = GetCommandLine();
    argc = parse_command_line( command_line, argv );
	return qcn_graphics::graphics_main(argc, argv);
}
#endif  // _WIN32
*/
#endif  // QCNLIVE

namespace qcn_graphics {

bool g_bThreadGraphics = false;
bool g_bInitGraphics   = false;
bool g_bViewHasStart   = false;
bool g_bFader          = true;  // fader is on by default
	
const float cfTextAlpha = 1.0f;

double g_alphaLogo = 1.0f;
double g_alphaText = 1.0f;
FADER g_faderLogo(5,15,5,2,g_alphaLogo);  // grow, on, fade, off, alpha max
//FADER g_faderText(5,5,5,2,g_alphaText);
		
int g_width, g_height;      // window dimensions
float g_aspect = 1.0f;

extern void setTimePercent(const int iPct)
{
   g_iTimePercent = iPct;
}

void Cleanup()
{
   bool bInHere = false;
   if (bInHere) return; // in case called more than once 
   fprintf(stderr, "Cleaning up graphics objects...\n");
  // free project prefs
   if (sm && sm->dataBOINC.project_preferences) {
	   free(sm->dataBOINC.project_preferences);
	   sm->dataBOINC.project_preferences = NULL;
	}
   earth.Cleanup();
   vsq.clear();
   TTFont::ttf_cleanup(); // cleanup fonts
#ifdef QCNLIVE
   g_MakeQuake.clear();
#endif
   bInHere = true;
}

#ifdef QCNLIVE

// make-a-quake vars
struct SMakeQuake g_MakeQuake;

// declare thread handles
#ifdef _WIN32
    HANDLE thread_handle_graphics = NULL;
#else
    pthread_t thread_handle_graphics = NULL;
#endif  // _WIN32
#endif

    /* below no longer necessary
 // below this comment will not be available externally
#ifdef _WIN32
  static HWND g_hWnd = NULL;   // need to get the handle of this window for screen coord/conversions
  inline void WinGetCoords(int& mx, int& my); // no longer necessary
#endif
*/

e_view g_eView = VIEW_PLOT_3D;  // default to 3d plots unless user prefs override below

bool g_bFullScreen = false;         // bool to denote if we're running in fullscreen (screensaver) mode
CEarth earth;   // earth object
CCube cube;     // cube object
#ifdef QCNLIVE
CGameMatch gameMatch; // the match-the-accelerometer game
#endif

vector<SQuake> vsq; // a vector of earthquake data struct (see qcn_graphics.h)

const float xax[2] = { -15.0, 44.0 };
const float yax[4] = { -25.0, -10.0, 8.0, 21.0 };
	
const float xax_2d[3] = { -47.0, 44.0, 49.0 };
// note the last is the very top of sig, so it's 15 + .5 padding for the sig axis which is .5 above next line
// also note 2d array as first element/array group is with no sig, second is to show sig
// size of each height (y) is 20 for x/y/z plot and 15 for x/y/z/s plot
	// get rid of the .5 padding
const float yax_2d[2][4] = { { -28.5, -8.5, 11.5, 31.5 }, { -28.5, -13.5, 1.5, 16.5 } } ; 

const float Y_TRIGGER_LAST[2] = { -30.0, yax_2d[0][3] }; // the Y of the trigger & timer tick line

int  iFullScreenView = 0;  // user preferred view, can be set on cmd line

GLfloat* colorsPlot[4] = { green, yellow, blue, red };
// time of the latest trigger, so we don't have them less than a second away, note unadjusted wrt server time!
// the "LastRebin" will be the actual displayed array offset position after the rebin
double dTriggerLastTime[MAX_TRIGGER_LAST];    
long lTriggerLastOffset[MAX_TRIGGER_LAST];
long lTimeLast[MAX_TICK_MARK];    
long lTimeLastOffset[MAX_TICK_MARK];
int g_iTimeCtr = 0;
int g_iZoomLevel = 0;

// an array of x/y/z for screensaver moving
//GLfloat jiggle[3] = {0., 0., 0.};

double dtw[2]; // time window

bool mouse_down = false;
int mouseX, mouseY;
int mouseSX, mouseSY;

double pitch_angle[4] = {0.0, 0.0, 0.0, 0.0}; 
double roll_angle[4] = {0.0, 0.0, 0.0, 0.0}; 
double viewpoint_distance[4]={ 10.0, 10.0, 10.0, 10.0};

//float color[4] = {.7, .2, .5, 1};

TEXTURE_DESC txLogo;   // customized version of the boinc/api/gutil.h TEXTURE_DESC

TEXTURE_DESC txAdd;  // optional additional image
TEXTURE_DESC txXYZAxes;  // legend for XYZ axes
TEXTURE_DESC txXYZAxesBlack;  // legend for XYZ axes for 3D and Cube view

RIBBON_GRAPH rgx, rgy, rgz, rgs; // override the standard boinc/api ribbon graph draw as it's making the earth red!

#ifndef QCNLIVE
bool bFirstShown = false;             // flags that the view hasn't been shown yet
#endif
bool bScaled = false;             // scaled is usually for 3D pics, but can also be done on 2D in the QCNLIVE
char* g_strFile = NULL;           // optional file of shared memory serialization
bool bResetArray = true;          // reset our plot memory array, otherwise it will just try to push a "live" point onto the array
float aryg[4][PLOT_ARRAY_SIZE];   // the data points for plotting -- DS DX DY DZ
float g_fAvg[4];  // keep an average of values to use for the middle of the scale

// current view is an enum i.e. { VIEW_PLOT_3D = 1, VIEW_PLOT_2D, VIEW_EARTH_DAY, VIEW_EARTH_NIGHT, VIEW_EARTH_COMBINED, VIEW_CUBE }; 
char g_strJPG[_MAX_PATH];
int g_iJPG = 0;

float g_fmax[4], g_fmin[4];

void getProjectPrefs()
{
    static bool bInHere = false;
    if (!sm || bInHere) return;
    bInHere = true;
    qcn_util::getBOINCInitData(WHERE_MAIN_PROJPREFS);
    qcn_graphics::parse_project_prefs();
    // tell the earth to regen earthquakes coords
    qcn_graphics::earth.RecalculateEarthquakePositions();
    qcn_graphics::earth.ResetEarthquakeNumber();
    bInHere = false;
}

// function to see if trigger lies on a point of interest we wish to plot
// this function also sets up the labels for the time display at the bottom
int getLastTrigger(const long lTriggerCheck, const int iWinSizeArray, const int iRebin, const bool bFirst)  // we may need to see if this trigger lies within a "rebin" range for aryg
{  // this sets up the vertical views, i.e. trigger & time marker
  static long lStartTime = 0L;
  static long lCheckTime = 0L;
  int iRet = 0;
  int i;
  bool bProc = false;

  if (bFirst) lStartTime = 0L;

  for (i = 0; i < MAX_TRIGGER_LAST; i++)  {
     // check if this offset matches a trigger and is within a rounding error for the time
     if (sm->lTriggerLastOffset[i] == lTriggerCheck && fabs(sm->dTriggerLastTime[i] - sm->t0[lTriggerCheck])<.05f) break;
  }
  if (i < MAX_TRIGGER_LAST)  { 
     // it must have matched, so add this to our graphics last trigger list
     iRet = iWinSizeArray / iRebin;  // iWinSize points into the graphics array, but have to divide by iRebin to plot onto PLOT_ARRAY_SIZE
     lTriggerLastOffset[i] = iRet;  // note have to factor in the bNewPoint decrement for pushing onto the aryg array
     dTriggerLastTime[i] = sm->dTriggerLastTime[i];
  }

//  if (g_eView == VIEW_PLOT_2D) { // only need the timing markers on 2d view
	// use a mod of the time interval with time - sm->t0start
    // first point is never a boundary, but mark second for next time
    //long lTimeTest = (long)(sm->t0[lTriggerCheck]);
	if (lStartTime == 0L) {  // it's our first time in and our point is a valid start time
	   /*
	   long lMod = 1L;
	   if (qcn_2dplot::GetTimerTick() == 1) { // just get the nearest "even" second point
	       //if ( (sm->t0[lTriggerCheck] - (float) ((long) sm->t0[lTriggerCheck])) <= 0.3f) lMod = 0L;
		   if (fmodf(sm->t0[lTriggerCheck] - floor(sm->t0start), 1.0f) < .1f) {
		      lMod = 0L;
		   }
	   }
	   else {
		   lMod = ( (long)(sm->t0[lTriggerCheck] - floor(sm->t0start)) ) % qcn_2dplot::GetTimerTick();
	   }
	   */
	   
	   // use fmodf function to get tick mark boundary
	   if (sm->t0[lTriggerCheck] >= ceil(sm->t0startSession) && fmodf(sm->t0[lTriggerCheck] - ceil(sm->t0startSession), (float) qcn_2dplot::GetTimerTick() ) < 0.1f) 
	   {
		  //&& (sm->t0[lTriggerCheck] - (float((long) sm->t0[lTriggerCheck]))) < 0.30f ) { 
           // get the even increment of sm->t0 from t0start 
		   //long lMult = (sm->t0[lTriggerCheck] - ceil(sm->t0start)) / g_TimerTick;
		   // we want two points here - the previous tick mark (i.e. before data starts) then the next tick mark
		   lCheckTime = sm->t0[lTriggerCheck] - qcn_2dplot::GetTimerTick();
	       lTimeLast[g_iTimeCtr] = lCheckTime; // g_iTimeCtr is 0 now, then 1
		   lTimeLastOffset[g_iTimeCtr] = (iWinSizeArray - ((float) qcn_2dplot::GetTimerTick() / sm->dt))/ iRebin;
		   g_iTimeCtr++;
		   lCheckTime = sm->t0[lTriggerCheck];
	       lTimeLast[g_iTimeCtr] = lCheckTime; // g_iTimeCtr incremented below
		   lStartTime = lCheckTime;
		   bProc = true;
	   }
	}
	else {
	   if (sm->t0[lTriggerCheck] > lCheckTime && g_iTimeCtr < MAX_TICK_MARK) {
	       lTimeLast[g_iTimeCtr] = lCheckTime;
		   bProc = true;
	   }
	}
	if (bProc && g_iTimeCtr < MAX_TICK_MARK) {  // we hit a timer interval, so setup the array
	   lTimeLastOffset[g_iTimeCtr] = iWinSizeArray / iRebin;
	   g_iTimeCtr++;
	   lCheckTime += qcn_2dplot::GetTimerTick();  // bump up to check next second
	}
//  }
  
  return iRet;

}

void getSharedMemory()
{
#ifndef QCNLIVE   // we don't need this in the GUI everything is "all in one" so sm is already setup by qcn_main
        if (sm) return; // already setup?
        // boinc_graphics_get_shmem() must be called after 
        // boinc_parse_init_data_file()
        boinc_parse_init_data_file();  // important to call this before the graphics shmem call
        sm = static_cast<CQCNShMem*>(boinc_graphics_get_shmem((char*) QCN_SHMEM));
/*
        if (sm && g_strFile && g_strFile[0] != 0x00) {
               strcpy((char*) sm->strCurFile, g_strFile);
               sm->deserialize(sm, sizeof(CQCNShMem), (const char*) sm->strCurFile);
               sm->bReadOnly = true; // put this in readonly mode
               fprintf(stdout, "QCN memory read from file %s\n", sm->strCurFile);
               fflush(stdout);
        }
*/
#endif
}

void set_viewpoint(double dist) 
{
    double x, y, z;
    x = 0.0;
    y = (g_eView == VIEW_PLOT_2D ? 0.0 : 3.0*dist); // note if in pseudo-2D mode don't shift the Y axis
    z = 11.0*dist;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(
          x, y, z,        // eye position
          0,-.8,0,        // where we're looking
          0.0, 1.0, 0.    // up is in positive Y direction
    );

    if (g_eView==VIEW_PLOT_3D) { // don't pitch or roll on the 2d view
      glRotated(pitch_angle[g_eView], 1., 0., 0);
      glRotated(roll_angle[g_eView], 0., 1., 0);    
    }
}

void init_camera(double dist, double field) 
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(
        field,       // field of view in degree
        1.0,        // aspect ratio
        1.0,        // Z near clip
        1000.0      // Z far
    );
    set_viewpoint(dist);
}

// set up lighting model
//
void init_lights() 
{
   GLfloat posl0[4] = {-13.0, 6.0, 20.0, 1.0};
   GLfloat dir[] = {-1, -.5, -3, 1.0};

   GLfloat ambient[] = {.2, .2, .2, 1.0};
   GLfloat diffuse[] = {.8, .8, .8, 1.0};
   GLfloat specular[]= {.0, .0, .0, 1.0};

   GLfloat shine = 1.f;
   GLfloat attenuation = 1.f;

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);

   glLightfv(GL_LIGHT0, GL_POSITION, posl0);
   
   glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

   glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);
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

   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);
}

void draw_logo(bool bExtraOnly) 
{
	mode_unshaded();
	mode_ortho_qcn();
	
	/*
	GLdouble diam = g_height > g_width ? g_height : g_width;
	GLdouble zNear = 1.0;
	GLdouble zFar = zNear + diam;
	GLdouble left;
	GLdouble right;
	GLdouble bottom;
	GLdouble top;
	
	if ( g_aspect < 1.0 ) { // window taller than wide
		bottom /= g_aspect;
		top /= g_aspect;
	} else {
		left *= g_aspect;
		right *= g_aspect;
	}
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(left, right, bottom, top, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	*/
																								  
																																											
    if (!bExtraOnly && txLogo.id) {

        float pos[3] = {0.0f, .50f, 0.f};
          float size[3] = {.21, .21, 0};
          txLogo.draw(pos, size, ALIGN_CENTER, ALIGN_CENTER, g_alphaLogo);
    }

    if (txAdd.id) {
        float pos[3] = {0.0f, 0.27f, 0.0f};
		if (bExtraOnly 
#ifdef QCNLIVE
			&& ! g_MakeQuake.bActive
#endif
			) {/*
			if (g_MakeQuake.bActive)  {  // make a quake - draw extra logo on the bottom right
			    pos[0] = 0.50f; 
				pos[1] = -0.10f;
			}
			else { */// move additional logo to the top
				pos[0] = -0.015f;
				pos[1] = 0.545f;
			//}
		}
        float size[3] = {.2, .2, 0};
        txAdd.draw(pos, size, ALIGN_CENTER, ALIGN_CENTER, g_alphaLogo);
    }
	
	// draw the xyz axes if available
	if (g_eView == VIEW_PLOT_2D || g_eView == VIEW_PLOT_3D || g_eView == VIEW_CUBE) {
		float pos[3] =  {1.00, 0.0, 0};
		float size[3] = {.05, .05, 0};
		if (g_eView == VIEW_PLOT_2D && qcn_2dplot::IsWhite() && txXYZAxes.id) {
			txXYZAxes.draw(pos, size, ALIGN_CENTER, ALIGN_CENTER, g_alphaLogo);
		}
		else if ((g_eView == VIEW_PLOT_3D || g_eView == VIEW_CUBE 
                  || (g_eView == VIEW_PLOT_2D && !qcn_2dplot::IsWhite())) && txXYZAxesBlack.id) {
			txXYZAxesBlack.draw(pos, size, ALIGN_CENTER, ALIGN_CENTER, g_alphaLogo);
		}
	}
	ortho_done();
}

void draw_text_sensor()
{
   char* buf = new char[128];
   memset(buf, 0x00, 128 * sizeof(char));

   //#ifdef QCNLIVE
//   int isize = MSG_SIZE_NORMAL;
//#else
   int isize = MSG_SIZE_SMALL;
//#endif

    if (sm) {
        if (!sm->bSensorFound) {
            TTFont::ttf_render_string(g_alphaText, 0.003, 0.01,0, isize, red, TTF_ARIAL, (const char*) "Demo Mode - Sensor Not Found");
        } else if (sm->lOffset >=0 && sm->lOffset < sm->iWindow ) {  // we're in our calibration window
            sprintf(buf, "%s sensor calibrating...", sm->strSensor);
            TTFont::ttf_render_string(g_alphaText, 0.003, 0.01, 0, isize, red, TTF_ARIAL, buf);
        } else if (sm->strSensor[0] != 0x00) {
            sprintf(buf, "Using %s sensor (Reset %d)", sm->strSensor, sm->iNumReset);
            TTFont::ttf_render_string(g_alphaText, 0.003, 0.01, 0, isize, red, TTF_ARIAL, buf);
        } else if (dtime()-sm->update_time > 5) {
            TTFont::ttf_render_string(g_alphaText, 0.003, 0.01, 0, isize, red, TTF_ARIAL, (const char*) "QCN Not Running");
        } else if (sm->statusBOINC.suspended) {
            TTFont::ttf_render_string(g_alphaText, 0.003, 0.01, 0, isize, red, TTF_ARIAL, (const char*) "QCN Suspended");
		}
    } 

#ifndef QCNLIVE  // QCNLIVE writes to the status bar on the window
      // if we wrote a JPG file, display a message for a little bit (200 frame refreshes ~ 7 seconds)
      if (++g_iJPG < 200 && g_strJPG[0] != 0x00) { // we have written a JPG file
        sprintf(buf, "Screenshot saved to: %s", g_strJPG);
        TTFont::ttf_render_string(g_alphaText, 0.003, 0.028, 0, MSG_SIZE_SMALL, orange, TTF_ARIAL, buf);
      }
#endif

	  delete [] buf;
}

void draw_text_user() 
{
   char* buf = new char[128];
   memset(buf, 0x00, 128 * sizeof(char));

   // draw text on top
   mode_unshaded();
   mode_ortho_qcn();

    if (!sm) {
       TTFont::ttf_render_string(g_alphaText, 0, 0, 0, 800, red, TTF_ARIAL, (const char*) "No shared memory, QCN not running?");
	   delete [] buf;
       return;
    }

/*
    sprintf(buf, "mouse x=%d  y=%d", mouseSX, mouseSY);
    TTFont::ttf_render_string(cf, 0, .04, 0, MSG_SIZE_NORMAL, red, TTF_ARIAL, buf);
*/

    // user info
#ifdef QCNLIVE
   if (strlen((const char*) sm->strMyStation)>0) {
      sprintf(buf, "Station: %s", (const char*) sm->strMyStation);
      TTFont::ttf_render_string(g_alphaText, 0, .118, 0, MSG_SIZE_NORMAL, green, TTF_ARIAL, buf);
   }

   if (sm && earth.IsShown() && sm->dMyLatitude != NO_LAT && sm->dMyLongitude != NO_LNG
	    && sm->dMyLatitude != 0.0f && sm->dMyLongitude != 0.0f) {
       sprintf(buf, "Location: %.4f, %.4f", sm->dMyLatitude, sm->dMyLongitude);
       TTFont::ttf_render_string(g_alphaText, 0, .094, 0, MSG_SIZE_NORMAL, green, TTF_ARIAL, buf);
   }

   if (sm) {
      char strTime[32];
      qcn_util::FormatElapsedTime((const double&) sm->clock_time, strTime, 32);

      sprintf(buf, "Run Time: %s", strTime);
      TTFont::ttf_render_string(g_alphaText, 0, 0.06, 0, MSG_SIZE_NORMAL, white, TTF_ARIAL, buf);

/*
	   sprintf(buf, "g_alphaText: %f", g_alphaText);
	   TTFont::ttf_render_string(g_alphaLogo, 0, 0.06, 0, MSG_SIZE_NORMAL, white, TTF_ARIAL, buf);
*/
      qcn_util::FormatElapsedTime((const double&) sm->cpu_time, strTime, 32);
      sprintf(buf, "CPU Time: %s", strTime);
      TTFont::ttf_render_string(g_alphaText, 0, 0.04, 0, MSG_SIZE_NORMAL, white, TTF_ARIAL, buf);
    }
#else
    if (sm) {
	  TTFont::ttf_render_string(g_alphaText, 0, .130, 0, MSG_SIZE_NORMAL, white, TTF_ARIAL, (const char*) sm->dataBOINC.user_name);
      //TTFont::ttf_render_string(g_alphaText, 0, 0.10, 0, MSG_SIZE_NORMAL, white, TTF_ARIAL, (const char*) sm->dataBOINC.team_name);

      sprintf(buf, "Workunit: %s", sm->dataBOINC.wu_name);
      TTFont::ttf_render_string(g_alphaText, 0, 0.110, 0, MSG_SIZE_NORMAL, white, 0, buf);

      char strTime[32];
      qcn_util::FormatElapsedTime((const double&) sm->clock_time, strTime, 32);
      sprintf(buf, "Run Time: %s", strTime);
      TTFont::ttf_render_string(g_alphaText, 0, 0.090, 0, MSG_SIZE_NORMAL, white, TTF_ARIAL, buf);

      qcn_util::FormatElapsedTime((const double&) sm->cpu_time, strTime, 32);
      sprintf(buf, "CPU Time: %s", strTime);
      TTFont::ttf_render_string(g_alphaText, 0, 0.070, 0, MSG_SIZE_NORMAL, white, TTF_ARIAL, buf);

      sprintf(buf, "%.2f Percent Complete", 100.0f * sm->fraction_done);
      TTFont::ttf_render_string(g_alphaText, 0, 0.050, 0, MSG_SIZE_NORMAL, white, TTF_ARIAL, buf);

      if (sm && earth.IsShown() && sm->dMyLatitude != NO_LAT && sm->dMyLongitude != NO_LNG
  	    && sm->dMyLatitude != 0.0f && sm->dMyLongitude != 0.0f) {
         sprintf(buf, "Home Map Location: %.3f, %.3f", sm->dMyLatitude, sm->dMyLongitude);
         TTFont::ttf_render_string(g_alphaText, 0, .030, 0, MSG_SIZE_NORMAL, green, TTF_ARIAL, buf);
      }
    }
#endif

#ifdef KEYVIEW
    sprintf(buf, "keys:  dn=%d  dnalt=%d  up=%d upalt=%d", key_press, key_press_alt, key_up, key_up_alt);
    TTFont::ttf_render_string(g_alphaText, 0, 0, 0, 800, red, TTF_ARIAL, buf);
#endif

    draw_text_sensor(); // sensor specific messages
    ortho_done();

	delete [] buf;
}

void draw_text_plot() 
{  // this draws the seismic sensor/accelerometer labels
    char buf[128];

   // draw text on top
   mode_unshaded();
   mode_ortho_qcn();

   // the following uncommented out will let the text bounce around!
    //static float x=0, y=0;
    //static float dx=0.0003, dy=0.0007;
    //x += dx;
    //y += dy;
    //if (x < 0 || x > .5) dx *= -1;
    //if (y < 0 || y > .5) dy *= -1;

    // left of window informative text

    // help messages
#ifndef QCNLIVE
    if (!g_bFullScreen) {
 	if (g_bSnapshot)  {
		sprintf(buf, "Press 'S' for live view"); 
		TTFont::ttf_render_string(g_alphaText, 0, .4, 0, MSG_SIZE_NORMAL, yellow, TTF_ARIAL, buf);
		sprintf(buf, "Use '<' & '>' keys to pan");
		TTFont::ttf_render_string(g_alphaText, 0, .38, 0, MSG_SIZE_NORMAL, yellow, TTF_ARIAL, buf);
	}
	else {
		sprintf(buf, "Press 'S' for snapshot view"); 
		TTFont::ttf_render_string(g_alphaText, 0, .4, 0, MSG_SIZE_NORMAL, yellow, TTF_ARIAL, buf);
	}

	//sprintf(buf, "Press 'C' for bouncy cube"); 
	//TTFont::ttf_render_string(cf, 0, .34, 0, MSG_SIZE_NORMAL, yellow, TTF_ARIAL, buf);
       
	sprintf(buf, "Press 'Q' for world earthquake map"); 
	TTFont::ttf_render_string(g_alphaText, 0, .34, 0, MSG_SIZE_NORMAL, yellow, TTF_ARIAL, buf);
       
	sprintf(buf, "Press 'L' to toggle 2D/3D Plot"); 
	TTFont::ttf_render_string(g_alphaText, 0, .32, 0, MSG_SIZE_NORMAL, yellow, TTF_ARIAL, buf);
       
	sprintf(buf, "Press +/- to change time window");
	TTFont::ttf_render_string(g_alphaText, 0, .30, 0, MSG_SIZE_NORMAL, yellow, TTF_ARIAL, buf);
   }
#endif

    const float fTop[4] = { 0.09, 0.28, 0.47, 0.62 };

	// graph labels
	sprintf(buf, "Significance");
	TTFont::ttf_render_string(qcn_graphics::g_alphaText, TEXT_PLOT_LEFT_AXES, fTop[E_DS], 0, MSG_SIZE_MEDIUM, colorsPlot[E_DS], TTF_ARIAL, buf);
        if (sm && g_fmax[E_DS] != SAC_NULL_FLOAT && g_fmin[E_DS] != SAC_NULL_FLOAT) {
           sprintf(buf, " max=%+6.3f", g_fmax[E_DS]);
           TTFont::ttf_render_string(qcn_graphics::g_alphaText, TEXT_PLOT_LEFT_AXES, fTop[E_DS] - 0.02, 0, MSG_SIZE_SMALL, colorsPlot[E_DS], TTF_ARIAL, buf);
           sprintf(buf, " min=%+6.3f", g_fmin[E_DS]);
           TTFont::ttf_render_string(qcn_graphics::g_alphaText, TEXT_PLOT_LEFT_AXES, fTop[E_DS] - 0.04, 0, MSG_SIZE_SMALL, colorsPlot[E_DS], TTF_ARIAL, buf);
        }

	sprintf(buf, "Z-amp");
	TTFont::ttf_render_string(qcn_graphics::g_alphaText, TEXT_PLOT_LEFT_AXES, fTop[E_DZ], 0, MSG_SIZE_MEDIUM, colorsPlot[E_DZ], TTF_ARIAL, buf);
        if (sm && g_fmax[E_DZ] != SAC_NULL_FLOAT && g_fmin[E_DZ] != SAC_NULL_FLOAT) {
           sprintf(buf, " max=%+6.3f", g_fmax[E_DZ]);
           TTFont::ttf_render_string(qcn_graphics::g_alphaText, TEXT_PLOT_LEFT_AXES, fTop[E_DZ] - 0.02, 0, MSG_SIZE_SMALL, colorsPlot[E_DZ], TTF_ARIAL, buf);
           sprintf(buf, " min=%+6.3f", g_fmin[E_DZ]);
           TTFont::ttf_render_string(qcn_graphics::g_alphaText, TEXT_PLOT_LEFT_AXES, fTop[E_DZ] - 0.04, 0, MSG_SIZE_SMALL, colorsPlot[E_DZ], TTF_ARIAL, buf);
        }

	sprintf(buf, "Y-amp");
	TTFont::ttf_render_string(qcn_graphics::g_alphaText, TEXT_PLOT_LEFT_AXES, fTop[E_DY], 0, MSG_SIZE_MEDIUM, colorsPlot[E_DY], TTF_ARIAL, buf);
        if (sm && g_fmax[E_DY] != SAC_NULL_FLOAT && g_fmin[E_DY] != SAC_NULL_FLOAT) {
           sprintf(buf, " max=%+6.3f", g_fmax[E_DY]);
           TTFont::ttf_render_string(qcn_graphics::g_alphaText, TEXT_PLOT_LEFT_AXES, fTop[E_DY] - 0.02, 0, MSG_SIZE_SMALL, colorsPlot[E_DY], TTF_ARIAL, buf);
           sprintf(buf, " min=%+6.3f", g_fmin[E_DY]);
           TTFont::ttf_render_string(qcn_graphics::g_alphaText, TEXT_PLOT_LEFT_AXES, fTop[E_DY] - 0.04, 0, MSG_SIZE_SMALL, colorsPlot[E_DY], TTF_ARIAL, buf);
        }

	sprintf(buf, "X-amp");
	TTFont::ttf_render_string(qcn_graphics::g_alphaText, TEXT_PLOT_LEFT_AXES, fTop[E_DX], 0, MSG_SIZE_MEDIUM, colorsPlot[E_DX], TTF_ARIAL, buf);
        if (sm && g_fmax[E_DX] != SAC_NULL_FLOAT && g_fmin[E_DX] != SAC_NULL_FLOAT) {
           sprintf(buf, " max=%+6.3f", g_fmax[E_DX]);
           TTFont::ttf_render_string(qcn_graphics::g_alphaText, TEXT_PLOT_LEFT_AXES, fTop[E_DX] - 0.02, 0, MSG_SIZE_SMALL, colorsPlot[E_DX], TTF_ARIAL, buf);
           sprintf(buf, " min=%+6.3f", g_fmin[E_DX]);
           TTFont::ttf_render_string(qcn_graphics::g_alphaText, TEXT_PLOT_LEFT_AXES, fTop[E_DX] - 0.04, 0, MSG_SIZE_SMALL, colorsPlot[E_DX], TTF_ARIAL, buf);
        }


	char strt[2][32];
	dtw[0] = dtw[1] - awinsize_seconds[key_winsize];
	if (g_bSnapshot) {
     	memset(strt, 0x00, sizeof(char) * 64);
	    qcn_util::dtime_to_string((const double) dtw[0], 'h', strt[0]);
	    qcn_util::dtime_to_string((const double) dtw[1], 'h', strt[1]);
        TTFont::ttf_render_string(cfTextAlpha, 0, 0.20, 0, 1500, white, TTF_ARIAL, (const char*) "Time In Hour UTC");
    }
	
    for (int jj = 0; jj < 4; jj++)  {
      if (g_bSnapshot) {  //snapshot time
     	  TTFont::ttf_render_string(cfTextAlpha, TEXT_PLOT_LEFT_AXES + 0.715f, fTop[jj] - 0.03, 0, MSG_SIZE_SMALL, white_trans, TTF_ARIAL, strt[1]);		
       	  TTFont::ttf_render_string(cfTextAlpha, TEXT_PLOT_LEFT_AXES + 0.070f, fTop[jj] - 0.03, 0, MSG_SIZE_SMALL, white_trans, TTF_ARIAL, strt[0]);
      }
	  else { // current time
     	  TTFont::ttf_render_string(qcn_graphics::g_alphaText, TEXT_PLOT_LEFT_AXES + 0.735f, fTop[jj] - 0.03, 0, MSG_SIZE_SMALL, white_trans, TTF_ARIAL, (const char*) "Now");	
		  switch(key_winsize) {
			  case 0:
				  sprintf(buf, "10 seconds ago");
				  break;
			  case 1:
				  sprintf(buf, "1 minute ago");
				  break;
			  case 2:
				  sprintf(buf, "10 minutes ago");
				  break;
			  case 3:
				  sprintf(buf, "1 hour ago");
				  break;
		  }		  
		  TTFont::ttf_render_string(qcn_graphics::g_alphaText, TEXT_PLOT_LEFT_AXES + 0.090f, fTop[jj] - 0.03, 0, MSG_SIZE_SMALL, white_trans, TTF_ARIAL, buf);
	   }
	}
    ortho_done();
}

bool setupPlotMemory(const long lOffset)  
{
    static bool bInHere = false;
    static long lLastDrawnOffset = -MAXI;
	
    if (bInHere || !sm) return false;
	
#ifdef _DEBUG_QCNLIVE_WRAP
	if (sm) {
		// these settings should just show the current stuff
		//sm->lWrap = 0; 
		
		// these settings should show the wrapped part & current part
		//sm->t0start = sm->t0[0] + 7200;
		sm->lWrap = 1; 
	}
#endif	

	if (g_bSnapshot && !g_bSnapshotArrayProcessed) {
	    lLastDrawnOffset = -MAXI;  // flag to force the regeneration of the aryg data array
		g_bSnapshotArrayProcessed = true;
    }
		
	bInHere = true;

    int iRebin;
    long ii, jj, kk;
    //float fAvg[4], fAvgCtr[4];
    float fLocalMax[4];
	
	if (g_dtOrig != sm->dt) { // problem -- dt must have changed
		// need to recalculate awinsize
		g_dtOrig = sm->dt; // set new dt
		if (sm->dt == 0. || g_dtOrig == 0.) {
			sm->dt = g_DT;
			g_dtOrig = g_DT; // this probably can't happen but just in case, so no change of divide by 0
		}
		for (int i = 0; i < MAX_KEY_WINSIZE + 1; i++) {
			awinsize[i] = (long) ceil(awinsize_seconds[i]/sm->dt);    // 10 seconds / dt // 500 pts @ 50Hz, but only 100 at 10Hz or 50 at 5Hz which is possible
		}
	}	
	
	iRebin = awinsize[key_winsize] / PLOT_ARRAY_SIZE;  // this size of our bin, i.e. how many avgs of points for each plot on the screen
	if (iRebin < 1) {
	  // shouldn't happen but just in case
	  bInHere = false;
      return false;
	}

    // it would be easiest if we shift our array on iRebin-divisable boundaries -- that way each recalc will have the same values
    // in each rebin "section" i.e. 0-5 / 6-11 / 12-17 / etc.  That way we shouldn't have to bother with the bNewPoint logic as the
    // previous rebin values will appear again identically to the previous rendering
    // return if lOffset isn't evenly divisible by iRebin, i.e. we haven't gone to a new point
    // CMC - we're getting sampling problems if the frame-rate of the rendering is too low,
	// we're not getting on the proper boundary of the rebin, so if not snapshot then override
	// bump up if just starting to get rid of the first few calibration elements in the array
	long lDrawableOffset = (lOffset / iRebin) * iRebin; // this is the "ideal" offset, i.e. the closet point in the rebin to what we want to view at the end
	
	// of course it's possible our lOffset in question matches this, if so we're OK below
	// we want to return if: 1) we aren't reforcing the array (bResetArray) and 2) lLastDrawn != -MAXI and  3) last drawn offset matches our lDrawableOffset above
	// #3 holds whether it was live or from a snapshot (not to be confused with the JPG "SnapShot"!)

    if (!bResetArray && lLastDrawnOffset != -MAXI && lLastDrawnOffset == lDrawableOffset) {  
	    bInHere = false;
	    return false; // if offset mod iRebin isn't 0, we return (unless we're in a snapshot view of course!)
    }

    long lOff = lDrawableOffset - awinsize[key_winsize];
/*
    // for hour, just use current pt - hour 
    if (key_winsize == 3) {  // for hour view, don't bother going back
       lOff = sm->lOffset - awinsize[key_winsize] - 1;
       lDrawableOffset = lOff;
    }
*/

    // if we made it here then we are either doing a bResetArray, or at a new offset/rebin point
    // set our lastdrawn to this point
    lLastDrawnOffset = lDrawableOffset;
 
    // set timestamps for window to be displayed in the draw_text
    dtw[1] = sm->t0[lOffset];   // timestamp for end of the window (which is the current point)
    earth.SetTime(dtw[1]);

    float *af[4] = {NULL,NULL,NULL,NULL};
    for (ii = 0; ii < 4; ii++) {
      af[ii] = new float[awinsize[key_winsize]];
    }
    if (!af[E_DX] || !af[E_DY] || !af[E_DZ] || !af[E_DS]) { 
      if (af[E_DX]) delete [] af[E_DX];
      if (af[E_DY]) delete [] af[E_DY];
      if (af[E_DZ]) delete [] af[E_DZ];
      if (af[E_DS]) delete [] af[E_DS];
	  bInHere = false;
      return false;
    }

    for (ii = 0; ii < 4; ii++) {
	  g_fAvg[ii] = 0.0f;
      memset(af[ii], SAC_NULL_FLOAT, sizeof(float) * awinsize[key_winsize]);
      memset((void*) aryg[ii], SAC_NULL_FLOAT, sizeof(float) * PLOT_ARRAY_SIZE);
    }

    // reset our trigger list & timer values
    memset(dTriggerLastTime, 0x00, sizeof(double) * MAX_TRIGGER_LAST);
    memset(lTriggerLastOffset, 0x00, sizeof(long) * MAX_TRIGGER_LAST);
    memset(lTimeLast, 0x00, sizeof(long) * MAX_TICK_MARK);
    memset(lTimeLastOffset, 0x00, sizeof(long) * MAX_TICK_MARK);
    g_iTimeCtr = 0;
	
    if (lOff > 0)  {  // all points exist in our window, so we can just go into lOffset - winsize and copy/scale from there
	  g_bViewHasStart = (bool) (sm->x0[lOff] == SAC_NULL_FLOAT || sm->t0[lOff] == 0.0);  // the current view does not have the start point, can rewind		
	  bool bStart = true;
      for (ii = 0; ii < awinsize[key_winsize]; ii++) { 
		if (sm->lWrap>0 || (sm->lWrap == 0 && sm->t0[lOff] >= sm->t0start)) { 
			getLastTrigger(lOff, ii, iRebin, bStart);  // we may need to see if this trigger lies within a "rebin" range for aryg
			// the point in question is needed if we wrapped around or it's in our current window
			dtw[0] = sm->t0[lOff];  // this will be the timestamp for the beginning of the window, i.e. "awinsize[key_winsize] ticks ago"
			bStart = false;
#ifdef QCNLIVE   // use the bScaled value, defaults to normal 2D/absolute & 3D/scaled, but user can change
			if (! bScaled) {
#else   // 2D is always absolute, 3D is scaled
			if (g_eView == VIEW_PLOT_2D || g_eView == VIEW_CUBE) {
#endif
					af[E_DX][ii] = sm->x0[lOff]; 
					af[E_DY][ii] = sm->y0[lOff]; 
					af[E_DZ][ii] = sm->z0[lOff]; 
			}
			else {
					af[E_DX][ii] = sm->x0[lOff] - sm->xa[lOff];   
					af[E_DY][ii] = sm->y0[lOff] - sm->ya[lOff];  
					af[E_DZ][ii] = sm->z0[lOff] - sm->za[lOff];
			}
			af[E_DS][ii] = sm->fsig[lOff];
		}  // t0start check
        lOff++;
      }
    }
    else { // we are wrapping around the array, lOff <= 0
      long lStart = MAXI + lOff + 1;   // start here, wrap around to 1+(awinsize-lStart) (skip 0 as that's baseline?)
      if (lStart >= MAXI || lStart < 1) lStart = 1;
	  g_bViewHasStart = (bool) (sm->x0[lStart-1] == SAC_NULL_FLOAT || sm->t0[lStart-1] == 0.0);  // the current view does not have the start point, can rewind		

      // Note that if the array (MAXI) is close to an hour, there's problems with overlapping points
      // i.e. the "live sensor" lOffset can get ahead of the graphics and start writing over times etc!
      // so make the MAXI 2 hours or more to be safe
      //fprintf(stdout, "lOffset = %ld  lstart = %ld   dtw0 = %f\n", lOffset, lStart, dtw[0]);
      //fflush(stdout);
	  bool bStart = true;
      for (ii = 0; ii < awinsize[key_winsize]; ii++) {
		if (sm->lWrap>0 || (sm->lWrap == 0 && sm->t0[lStart] >= sm->t0start)) {
			getLastTrigger(lStart, ii, iRebin, bStart);  // we may need to see if this trigger lies within a "rebin" range for aryg
			// the point in question is needed if we wrapped around or it's in our current window
			dtw[0] = sm->t0[lStart];  // this will be the timestamp for the beginning of the window, i.e. "awinsize[key_winsize] ticks ago"
			bStart = false;
#ifdef QCNLIVE   // use the bScaled value, defaults to normal 2D/absolute & 3D/scaled, but user can change
			if (! bScaled) {
#else   // 2D is always absolute, 3D is scaled
			if (g_eView == VIEW_PLOT_2D || g_eView == VIEW_CUBE) {
#endif
				af[E_DX][ii] = sm->x0[lStart]; 
				af[E_DY][ii] = sm->y0[lStart];
				af[E_DZ][ii] = sm->z0[lStart]; 
			}
			else {
				// now we can scale each axis according to fmin = 0 and fmax = 1
				af[E_DX][ii] = (sm->xa[lStart] != SAC_NULL_FLOAT) ? sm->x0[lStart] - sm->xa[lStart] : SAC_NULL_FLOAT;  
				af[E_DY][ii] = (sm->ya[lStart] != SAC_NULL_FLOAT) ? sm->y0[lStart] - sm->ya[lStart] : SAC_NULL_FLOAT;  
				af[E_DZ][ii] = (sm->za[lStart] != SAC_NULL_FLOAT) ? sm->z0[lStart] - sm->za[lStart] : SAC_NULL_FLOAT;  
			}
			af[E_DS][ii] = sm->fsig[lStart];
		} // t0start check
        if (++lStart >= MAXI || lStart < 1) lStart = 1;  // check for wrapping
      }
    }

	// note: setup a max min range per axis per rebin -- this doesn't really correspond to absolute max/min g_fmax/fmin values, but for display purposes
	g_fmax[E_DX] = g_fmax[E_DY] = g_fmax[E_DZ] = g_fmax[E_DS] = SAC_NULL_FLOAT;
	g_fmin[E_DX] = g_fmin[E_DY] = g_fmin[E_DZ] = g_fmin[E_DS] = -1.0f * SAC_NULL_FLOAT;

    // now try a simple averaging rebinning to get the array down to manageable size (1000 pts)
	// maybe get the max each rebin interval?
	float fAvgCtr = 0.0f;
	for (kk = E_DX; kk <= E_DS; kk++) {
		g_fAvg[kk] = 0.0f;
		fAvgCtr = 0.0f;
		for (ii = 0; ii < PLOT_ARRAY_SIZE; ii++) {
			//fAvgCtr[kk]  = 0.0f;
			//fAvg[kk]     = 0.0f;
			fLocalMax[kk]     = SAC_NULL_FLOAT;
		  if (iRebin == 1) {
			  if (af[kk][ii] != SAC_NULL_FLOAT && af[kk][ii] > fLocalMax[kk]) {
				//fAvg[kk] = af[kk][ii];
				//fAvgCtr[kk]++;
			    fLocalMax[kk] = af[kk][ii];
			  }
		  }
		  else {
			  for (jj = 0; jj < iRebin; jj++) {
				  if (af[kk][(ii*iRebin)+jj] != SAC_NULL_FLOAT && af[kk][(ii*iRebin)+jj] > fLocalMax[kk]) {
					//fAvg[kk] += af[kk][(ii*iRebin)+jj];
					//fAvgCtr[kk]++;
				    fLocalMax[kk] = af[kk][(ii*iRebin)+jj];
				  }
			  } // for jj
		  }
		  /*
		  if (fAvgCtr[kk] > 0.0f) {
			  aryg[kk][ii] = fAvg[kk] / fAvgCtr[kk];
		  }
		  else {
			  aryg[kk][ii] = 0.0f;
		  }
		  */
		  aryg[kk][ii] = fLocalMax[kk];
		  if (fLocalMax[kk] == SAC_NULL_FLOAT) {
			 fLocalMax[kk] = 0.0f; // force to 0 if no max found, this wixll prevent drawing crazy autoscale ranges etc
			 if (g_eView == VIEW_PLOT_3D) { // if null on the 3d, force to 0, since we just pass the aryg array in and it will try to draw SAC_NULL (-12345)
			     aryg[kk][ii] = 0.0f;
			 }
		  }

			// note: setup a max min range per axis per rebin -- this doesn't really correspond to absolute max/min g_fmax/fmin values, but for display purposes
			if (aryg[kk][ii] != SAC_NULL_FLOAT ) {
				if (aryg[kk][ii] < g_fmin[kk]) g_fmin[kk] = aryg[kk][ii];
				else if (aryg[kk][ii] > g_fmax[kk]) g_fmax[kk] = aryg[kk][ii];
				g_fAvg[kk] += aryg[kk][ii];
				fAvgCtr += 1.0f;
			}

		} // for ii
		if (fAvgCtr > 0.0f) g_fAvg[kk] /= fAvgCtr; // take the average for number of valid points
		g_fAvg[kk] = qcn_util::fround(g_fAvg[kk],1);  // round to 1 dec point so we're centered somewhere sensible, i.e. 0 rather than - .002 etc

	} // for kk
	  

    if (af[E_DX]) delete [] af[E_DX];
    if (af[E_DY]) delete [] af[E_DY];
    if (af[E_DZ]) delete [] af[E_DZ];
    if (af[E_DS]) delete [] af[E_DS];
   
    bResetArray = false; 
    bInHere = false;

    return true;
}

void draw_triggers()
{
	// not if they don't want to
	if (!sm->bMyVerticalTrigger) return;
	
    // show the triggers, if any
    glPushMatrix();
    for (int i = 0; i < MAX_TRIGGER_LAST; i++) {
       if (dTriggerLastTime[i] > 0.0f) { // there's a trigger here
	     float fWhere;
	     if (g_eView == VIEW_PLOT_2D) {
            fWhere = xax_2d[0] + ( ((float) (lTriggerLastOffset[i]) / (float) PLOT_ARRAY_SIZE) * (xax_2d[1]-xax_2d[0]));
		 }
		 else  {
            fWhere = xax[0] + ( ((float) (lTriggerLastOffset[i]) / (float) PLOT_ARRAY_SIZE) * (xax[1]-xax[0]));
         }
		 
         //fprintf(stdout, "%d  dTriggerLastTime=%f  lTriggerLastOffset=%ld  fWhere=%f\n",
         //    i, dTriggerLastTime[i], lTriggerLastOffset[i], fWhere);
         //fflush(stdout);
         glColor4fv((GLfloat*) magenta);
         glLineWidth(2);
         glLineStipple(4, 0xAAAA);
         glEnable(GL_LINE_STIPPLE);
         glBegin(GL_LINE_STRIP);
         glVertex2f(fWhere, Y_TRIGGER_LAST[0]);
         glVertex2f(fWhere, Y_TRIGGER_LAST[1]);
         glEnd();
         glDisable(GL_LINE_STIPPLE);
       }
    }
    glPopMatrix();
    glFlush();
}

void draw_plots_3d() 
{
    // setup arrays for the plots, as we'll have to pre-process the raw data from the sensor
    if (!sm) return; // not much point in continuing if shmem isn't setup!

    // use jiggle array to move around graph in x/y/z depending on values

    init_camera(viewpoint_distance[g_eView]);
    init_lights();
    scale_screen_qcn(g_width, g_height);  // boinc api/gutil function to get good aspect ratio

    rgx.draw((float*) aryg[E_DX], PLOT_ARRAY_SIZE, false);
    rgy.draw((float*) aryg[E_DY], PLOT_ARRAY_SIZE, false);
 	rgz.draw((float*) aryg[E_DZ], PLOT_ARRAY_SIZE, false);
    rgs.draw((float*) aryg[E_DS], PLOT_ARRAY_SIZE, false);

}

extern void ResetPlotArray()
{
   // must have switched view -- reset the graphics bool
   bResetArray = true;
   g_bSnapshotArrayProcessed = false;
}

// CMC Note --- change path to an "images" directory not the trigger directory
const char* ScreenshotJPG()
{
   double dblTime;
   if (sm && sm->strPathImage) {
	  dblTime = dtime();
      g_iJPG = 0;
      memset(g_strJPG, 0x00, sizeof(char) * _MAX_PATH);
	  sprintf(g_strJPG, "%s%c%ld_%ld_%d.jpg",
                sm->strPathImage,
                qcn_util::cPathSeparator(),
	         (long) dblTime, 
		      (long) ( (double) (dblTime - (long) dblTime) * 1000000.0f),
		      (int) g_eView			  
	 );
     if (! qcn_util::ScreenshotJPG(g_width, g_height, g_strJPG, 100)) {
        memset(g_strJPG, 0x00, sizeof(char) * _MAX_PATH);
        g_iJPG = 1000;
     }
   }
   return g_strJPG;
}

void parse_quake_info(char* strQuake, int ctr, e_quake eType)
{
   // each line of quake info is delimited by pipe (|) char -- 5 max
   // first bit is the magnitude of the quake
   // second is the UTC time
   // third is the latitude
   // fourth is the longitude
   // fifth is the depth in km
   // sixth is a text description
 
 
   const int iLenBuf = 256;
   const int iMax = 7; // max # of sections
   if ((int) strlen(strQuake) <= iMax) return; // must be an error, string too short!

   const char delim = '|'; // the pipe char is the delimiter
   // search point of next delim
   char* strWhere[iMax+1] = { strQuake, NULL, NULL, NULL, NULL, NULL, NULL, strQuake+strlen(strQuake) }; 
   SQuake sq; // a temp struct to use for the vector assign
   sq.num = ctr;
   char* buf = new char[iLenBuf];

/*
5.2|2007/11/21 08:09:15          |-32.901       | -179.221 |    53.9     |SOUTH OF THE KERMADEC ISLANDS
*/

   for (int i = 1; i <= iMax; i++)  {
     if (i<iMax)
        strWhere[i] = (char*) strchr((const char*) strWhere[i-1]+1, (int) delim);	
  
     if (strWhere[i-1]) { 
        memset(buf,0x00,iLenBuf);
        strncpy(buf, strWhere[i-1] + ((i==1) ? 0 : 1), strWhere[i] - strWhere[i-1] - ((i==1) ? 0 : 1));
        //fprintf(stdout, "buf[%d] = %s\n", i, buf);

        switch (i) {
          case 1: // mag
            sq.magnitude = atof(buf);
            break;
          case 2: // time in format YYYY/MM/DD HH:MI:SS  e.g. 2007/11/21 15:05:21
            //sq.utc_time.assign(buf); 
            if (strlen(buf) < 19) break;
            char strTmp[8];
            memset(strTmp, 0x00, 8);
            strncpy(strTmp, buf, 4);
            sq.year = atoi(strTmp);

            memset(strTmp, 0x00, 8);
            strncpy(strTmp, buf+5, 2);
            sq.month = atoi(strTmp);

            memset(strTmp, 0x00, 8);
            strncpy(strTmp, buf+8, 2);
            sq.day = atoi(strTmp);

            memset(strTmp, 0x00, 8);
            strncpy(strTmp, buf+11, 2);
            sq.hour = atoi(strTmp);

            memset(strTmp, 0x00, 8);
            strncpy(strTmp, buf+14, 2);
            sq.minute = atoi(strTmp);

            memset(strTmp, 0x00, 8);
            strncpy(strTmp, buf+17, 2);
            sq.second= atoi(strTmp);
            break;
          case 3: // lat
            sq.latitude  = atof(buf);
            break;
          case 4: // lon
            sq.longitude = atof(buf);
            break;
          case 5: // depth
            sq.depth_km  = atof(buf);
            break;
          case 6: // desc
            sq.strDesc.assign(buf);
            break;
          case 7: // URL
            sq.strURL.assign(buf);
            break;
        }
     }
   }
   sq.bActive = false;  // not actively selected yet!
   sq.eType = eType;

   if (sq.magnitude) { // we have magnitude, so we should add this
      vsq.push_back(sq);
   }
   delete [] buf;
}


int GetTimeWindowWidth()
{
	return g_iTimeWindowWidth;
}

int SetTimeWindowWidthInt(int iWidth)
{
	if (iWidth == 10 || iWidth == 60 || iWidth == 600 || iWidth == 3600)
	{
		g_iTimeWindowWidth = iWidth; // iWidth is a valid value
	}
	else {
		g_iTimeWindowWidth = 10; // default if no valid value set in
	}
	switch(g_iTimeWindowWidth) {
		case 10:
			key_winsize = 0;
			qcn_2dplot::SetTimerTick(1);  // 1 second tick marks
			break;
		case 60:
			key_winsize = 1;
			qcn_2dplot::SetTimerTick(5);  // 5 second tick marks
			break;
		case 600:
			key_winsize = 2;
			qcn_2dplot::SetTimerTick(60);  // 1 minute tick marks
			break;
		case 3600:
			key_winsize = 3;
			qcn_2dplot::SetTimerTick(300);  // 5 minute tick marks
			break;
	}
	bResetArray = true;
	g_bSnapshotArrayProcessed = false;
	return g_iTimeWindowWidth;
}
		

int SetTimeWindowWidth(bool bUp)
{
	if (bUp) { // increase window width
		switch(g_iTimeWindowWidth) {
		   case 10:
			   g_iTimeWindowWidth = 60;
			   key_winsize = 1;
			   qcn_2dplot::SetTimerTick(5);  // 1 second tick marks
			   break;
		   case 60:
			   g_iTimeWindowWidth = 600;
			   key_winsize = 2;
			   qcn_2dplot::SetTimerTick(60);  // 60 second tick marks
			   break;
		   case 600:
			   g_iTimeWindowWidth = 3600;
			   key_winsize = 3;
			   qcn_2dplot::SetTimerTick(300);  // 5 minute tick marks
			   break;
		}
	}
	else { // decrease window width if possible
		switch(g_iTimeWindowWidth) {
		   case 60:
			   g_iTimeWindowWidth = 10;
			   key_winsize = 0;
	  		   qcn_2dplot::SetTimerTick(1);  // 5 second tick marks
			   break;
		   case 600:
			   g_iTimeWindowWidth = 60;
			   key_winsize = 1;
			   qcn_2dplot::SetTimerTick(5);  // 60 second tick marks
			   break;
		   case 3600:
			   g_iTimeWindowWidth = 600;
			   key_winsize = 2;
			   qcn_2dplot::SetTimerTick(60);  // 5 minute tick marks
			   break;
		}
	}

	bResetArray = true;
	g_bSnapshotArrayProcessed = false;
	return g_iTimeWindowWidth;
}

long TimeWindowBack()
{
	return TimeWindowPercent(g_iTimePercent - 10); // use pct to go forward or back
}

long TimeWindowForward()
{
	return TimeWindowPercent(g_iTimePercent + 10); // use pct to go forward or back
}
		
long TimeWindowPercent(int iPct)
{
	static int iLastPct = 0;
	if (iPct < 0) iPct = 0;
	else if (iPct > 100) iPct = 100;
	g_iTimePercent = iPct;
	if (iPct <= iLastPct && g_bViewHasStart) { //we're going backwards but already at the start so just return
		return g_iTimePercent; //g_lSnapshotPoint;
	}
	iLastPct = iPct;
    if (!g_bSnapshot) TimeWindowStop();  // stop the live data stream and set bSnapshot to true
	if (g_bSnapshot) {  // note we can't go further forward than our sm->lOffset!
		// go a percentage of our array bounds
		int iTestMax = MAXI-1;
		int iMinSec = ceil(5.0f/sm->dt);
		float fTestOff = (float)(sm->lOffset-1);  // don't use lOffset as it may be getting written now, 1 back is safe
		const float fPct = (float) iPct / 100.0f;
		//if (sm->x0[iTestMax] == SAC_NULL_FLOAT || sm->t0[iTestMax] == 0) { // we haven't wrapped yet so just use lOffset = 1 as the start
		if (sm->lWrap == 0) {
			g_lSnapshotPoint = (long) ( fPct * fTestOff );
			if (g_lSnapshotPoint < iMinSec && iMinSec < fTestOff) {
				g_lSnapshotPoint = iMinSec; // keep at least a second on the screen
				g_bViewHasStart = true;
			}
		}
		else { // we've wrapped around so have to factor in the next MAXI-lOffset points
			long lOff = (long)((float) iTestMax * (1.0f - fPct)); // this is how many points back we have to go in our possible array
			g_lSnapshotPoint = fTestOff - lOff; // our tentative point is the 
			if (g_lSnapshotPoint < 0)  // our percentage point is part of the "wrap"
				g_lSnapshotPoint += MAXI; // this should be the pct point of the wrapped array
			if (g_lSnapshotPoint >= sm->lOffset && g_lSnapshotPoint < (sm->lOffset + awinsize[key_winsize])) {
				// wrapped around but effectively at the start
				g_bViewHasStart = true;
                g_lSnapshotPoint = sm->lOffset + awinsize[key_winsize];
				if (g_lSnapshotPoint >= MAXI) g_lSnapshotPoint = 1;
			}
		}

		// one final check, don't allow the snapshot point to be 
		g_lSnapshotPointOriginal = g_lSnapshotPoint; // save the original point so we can see back into the array		
        bResetArray = true;
        g_bSnapshotArrayProcessed = false;
    }
    return g_iTimePercent; //g_lSnapshotPoint;
}

long TimeWindowStop()
{
     g_bSnapshot = true;
	 if (sm && g_bSnapshot) {
	     g_lSnapshotTimeBackSeconds = 0L;
         g_lSnapshotPoint = sm->lOffset-1;  // -1 gives us some clearance we're not reading from end of array which sensor is writing
         g_lSnapshotPointOriginal = g_lSnapshotPoint; // save the original point so we can see back into the array
         if (earth.IsShown()) {
             // if in earth mode, send it back to regular seismic view, non-snapshot
             g_bSnapshot = false;
	 }
         g_bSnapshotArrayProcessed = false;
     }
	 bResetArray = true;
	 return g_lSnapshotPoint;
}

long TimeWindowStart()
{
     g_bSnapshot = false;
     g_bSnapshotArrayProcessed = false;
     g_lSnapshotTimeBackSeconds = 0;
	 g_lSnapshotPoint = 0;
     bResetArray = true;
	 return 0L;
}

bool TimeWindowIsStopped()
{
    return g_bSnapshot;
}

bool IsScaled()
{
    return bScaled;
}

void SetScaled(bool scaleit)
{
    bScaled = scaleit;
	bResetArray = true;
}

void parse_project_prefs()
{
   // this is where we can get a list of earthquakes 
   if (!sm || !strlen(sm->strProjectPreferences)) return;

   int iTotal = 0;
   int iLenQuake = 512; 
   char* quake = new char[iLenQuake];
   char strTmp[16];
   int ctr = 0; 
   bool bGo = true;

   // first do the current quakes
   vsq.clear();

/*#ifdef _DEBUG // just show one quake for now
   strcpy(quake, "8.7|1755/11/01 10:16:00|  36.0|-11.0|   0.0|near Lisbon, Portugal|http://earthquake.usgs.gov/regional/world/events/1755_11_01.php");
   parse_quake_info(quake, 1, QUAKE_WORLD85);
#else
*/
   while (bGo) {
      sprintf(strTmp, "<qu%03d>", ++ctr);
	  memset(quake, 0x00, iLenQuake);
      if (parse_str((const char*) sm->strProjectPreferences, strTmp, quake, iLenQuake)) {
        //fprintf(stdout, "%s\n", quake);
        iTotal++;
        parse_quake_info(quake, ctr, QUAKE_CURRENT);
      }
      else bGo = false;
   }

   // now do the historical (world quakes >=8.5 magnitude)
   bGo = true; // reset bGo but not ctr!
   ctr = 0;
   while (bGo) {
      sprintf(strTmp, "<wequ%03d>", ++ctr);
	  memset(quake, 0x00, iLenQuake);
      if (parse_str((const char*) sm->strProjectPreferences, strTmp, quake, iLenQuake)) {
        //fprintf(stdout, "%s\n", quake);
        iTotal++;
        parse_quake_info(quake, iTotal, QUAKE_WORLD85);
      }
      else bGo = false;
   }

   delete [] quake;

#ifndef QCNLIVE  // don't change in qcnlive mode - user selects the screen!
   // get station lat/lng (in QCNLive this is done elsewhere)
   parse_double((const char*) sm->strProjectPreferences, "<lat>", (double&) sm->dMyLatitude);  
   parse_double((const char*) sm->strProjectPreferences, "<lng>", (double&) sm->dMyLongitude);  
   parse_str((const char*) sm->strProjectPreferences, "<stn>", (char*) sm->strMyStation, SIZEOF_STATION_STRING); 
#endif

   // now get preferred view screensaver prefs if not set on cmd line
   if (!iFullScreenView) { // cmd line overrides prefs
     parse_int((const char*) sm->strProjectPreferences, "<ssp>", iFullScreenView);  
   }

#ifndef QCNLIVE  // don't change in qcnlive mode - user selects the screen!
     if (iFullScreenView) { // just maps to a number in the enum
        g_eView = (e_view) iFullScreenView;
     }
     else {
//#ifdef _DEBUG
//         g_eView = earth.ViewCombined();
//#else
	   if (sm && sm->bSensorFound) {
		  g_eView = VIEW_PLOT_3D;
	   }
	   else {
		  g_eView = earth.ViewCombined();
	   }
//#endif  // debug view
     }
#endif  // not qcnlive
}

int graphics_main(int argc, char** argv) 
{
    g_bThreadGraphics = false;
    g_bInitGraphics = false;
#if (defined(__APPLE_CC__) && defined(_DEBUG))
    // Provide a way to get error messages from system in Debug builds only. 
    // In Deployment builds, ownership violates sandbox security (unless we 
    // gave it an explicit path outside the BOINC Data directory). 
    freopen("gfx_stderr.txt", "w", stderr);
#endif
    // if started with an argument, it's probably the shared mem file
    for (int i=0; i<argc; i++) {
        if (!strcmp(argv[i], "--dump") && (i+1)<argc && (argv[i+1]))
        {
            g_strFile = argv[i+1];
        }
        if (!strcmp(argv[i], "--fullscreen"))
        {
            g_bFullScreen = true;
        }
        if (!strcmp(argv[i], "--sleep"))
        {
            boinc_sleep(1.0f);  // sleep a little if starting up in demo mode
        }
        if (!strcmp(argv[i], "--view") && (i+1)<argc && (argv[i+1]))
        {
            iFullScreenView = atoi(argv[i+1]);
        }
    }
    memset(g_strJPG, 0x00, sizeof(char) * _MAX_PATH);
    g_iJPG = 0;

    getSharedMemory();

    g_bThreadGraphics = true;

#ifndef QCNLIVE
    boinc_graphics_loop(argc, argv);
    g_bThreadGraphics = false;
#endif
    return 0;
}

//}  // namespace qcn_graphics

/*
#ifdef _WIN32
// it looks like windows gives MouseX & Y absolute to the screen size,
// so we have to further adjust
inline void WinGetCoords(int& mx, int& my)
{

	if (!g_hWnd && sm) { // need to get the hWnd for this window
		char* strTitle = new char[_MAX_PATh];
		memset(strTitle, 0x00, sizeof(char) * _MAX_PATH);
		get_window_title(strTitle, _MAX_PATH);
		g_hWnd = ::FindWindow(NULL, strTitle);
		delete [] strTitle;
    }
	POINT pt;
	pt.x = mx;
	pt.y = my;
	::ScreenToClient(g_hWnd, &pt);
	mx = pt.x;
	my = pt.y;
}
#endif 
*/

void LoadFonts()
{
	static bool bLoaded = false;
	if (bLoaded) return;
#ifdef QCNLIVE
	TTFont::ttf_load_fonts((const char*) ".", "hvtb", 300);
	bLoaded = true;
#else
	if (sm && sm->dataBOINC.project_dir) {
		TTFont::ttf_load_fonts((const char*) sm->dataBOINC.project_dir);
		bLoaded = true;
	}
#endif
}
		
void Init()
{
    static bool bFirstIn = false;
    if (bFirstIn || g_bInitGraphics) return; // return if already init
    bFirstIn = true;
	
    char path[_MAX_PATH];
	
	// init textures
	memset(&txLogo, 0x00, sizeof(TEXTURE_DESC));
	memset(&txAdd, 0x00, sizeof(TEXTURE_DESC));
	memset(&txXYZAxes, 0x00, sizeof(TEXTURE_DESC));
	memset(&txXYZAxesBlack, 0x00, sizeof(TEXTURE_DESC));

    getProjectPrefs();

    //fprintf(stderr, "QCN Graphics Init() at %f\n", sm ? sm->update_time : 0.0f);

    // setup the window widths depending on sm->dt
    // note sm->dt could possibly be 0, if so use the DT constant (.02)
    float fdt = (sm && sm->dt) ? sm->dt : g_dtOrig;
	for (int i = 0; i < MAX_KEY_WINSIZE + 1; i++) {
		awinsize[i] = (long) ceil(awinsize_seconds[i]/fdt);    // 10 seconds / dt // 500 pts @ 50Hz, but only 100 at 10Hz or 50 at 5Hz which is possible
	}

    // enable hidden-surface-removal
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glEnable (GL_LINE_SMOOTH);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);

// RIBBON GRAPH

    // setup ribbon graph stuff
	float size[3] = {xax[1] - xax[0], 10.0, 5.0};
    float pos[3];
	
    pos[0] = xax[0];
    pos[1] = yax[E_DS];
    pos[2] = 0.0;
    rgs.init(pos, size, colorsPlot[E_DS], colorsPlot[E_DS]);

    pos[0] = xax[0];
    pos[1] = yax[E_DZ];
    pos[2] = 0.0;
    rgz.init(pos, size, colorsPlot[E_DZ], colorsPlot[E_DZ]);

    pos[0] = xax[0];
    pos[1] = yax[E_DY];
    pos[2] = 0.0;
    rgy.init(pos, size, colorsPlot[E_DY], colorsPlot[E_DY]);

    pos[0] = xax[0];
    pos[1] = yax[E_DX];
    pos[2] = 0.0;
    rgx.init(pos, size, colorsPlot[E_DX], colorsPlot[E_DX]);

	LoadFonts();
    // load logo & fonts
#ifdef QCNLIVE
    strcpy(path, IMG_LOGO);
#else
    boinc_resolve_filename(IMG_LOGO, path, sizeof(path));
#endif

    if (!boinc_file_exists(path) || txLogo.load_image_file(path)) { // can use load_image_file but we know it's a JPG so just use that
       fprintf(stderr, "Error loading QCN logo file %s\n", path); 
    }

    // get the CEarth object setup...
    earth.Init();
    //cube.Init();  // doing the cube init would probably be redundant and/or conflict with the earth init

    // everything is loaded, so set flag to true to notify main QCNLive window frame
    // and so don't come into this Init routine again (which shouldn't happen anyway of course)

	// XYZ axes to show at the bottom right of 2d/3d/cube view
        #ifdef QCNLIVE
   	   strcpy(path, IMG_LOGO_XYZAXES);  // shows up on lower right
       #else
           boinc_resolve_filename(IMG_LOGO_XYZAXES, path, sizeof(path));
       #endif
	txXYZAxes.load_image_file(path);

	// XYZ axes to show at the bottom right of 2d/3d/cube view
        #ifdef QCNLIVE
           strcpy(path, IMG_LOGO_XYZAXES_BLACK);  // shows up on lower right
        #else
           boinc_resolve_filename(IMG_LOGO_XYZAXES_BLACK, path, sizeof(path));
        #endif
	txXYZAxesBlack.load_image_file(path);
	
#ifdef QCNLIVE
     // check for extra logo i.e. museum logo if any
     strcpy(path, IMG_LOGO_EXTRA);  // shows up on lower right
	 txAdd.load_image_file(path);

	earth.SetMapCombined();
#endif
    g_bInitGraphics = true;
}

void Render(int xs, int ys, double time_of_day)
{
	
    // Put this in the main loop to allow retries if the 
    // worker application has not yet created shared memory
    //
    static bool bInHere = false;
    static double dTimeCheck = sm ? sm->update_time + 3600.0f : 0.0f; // check for new prefs hourly
    static int iCounter = 0;

    if (bInHere) return; // currently rendering
    bInHere = true;
	
	if (time_of_day < 1.0f && sm && sm->current_time > 1.0f) time_of_day = sm->current_time;  // default to update time from main loop if no valid time passed in

	if (g_bFader) {
		g_faderLogo.value(time_of_day, g_alphaLogo);  // set alpha value for text
		g_alphaText = g_alphaLogo;
	}
	else {
		g_alphaText = g_alphaLogo = 1.0;
	}
	
	//g_faderText.value(time_of_day, g_alphaText);  // set alpha value for text

    if (!sm) { // try to get shared mem every few seconds, i.e. 100 frames
       if (!(++iCounter % 100)) {
            iCounter = 0;
            fprintf(stderr, "No shared memory found, trying to attach...\n");  // this should only be required in qcndemo mode
            getSharedMemory();
		    if (sm) {
               sm->update_time = dtime();  // set update time so prefs will be read below 
			   LoadFonts(); // check for font loading
		    }
       }
    }

    if (sm && (sm->update_time > dTimeCheck)) {
        //fprintf(stderr, "QCN Graphics Render() request to reread project prefs at %f\n", sm->update_time);
        dTimeCheck = sm->update_time + 3600.0f;  // check in an hour
        getProjectPrefs();   // get new prefs
    }

/*
#ifdef QCNLIVE
    if ((qcn_main::g_iStop || !sm) && qcn_graphics::g_bThreadGraphics) {
        qcn_graphics::g_bThreadGraphics = false; // mark the thread as being done
    #ifdef _WIN32
        _endthread();
    #endif
        _exit(0);
    }
#endif
*/

#ifndef QCNLIVE
    static double dTimeLast = 0.0f;
    // note don't switch views if they have a preferred view (iFullScreenView) as set on their prefs page
    if (g_bFullScreen && !iFullScreenView && sm && sm->bSensorFound)  {  //cycle through various screensaver views, not for QCNLIVE obviously
       if (sm->update_time > dTimeLast + 30.0f) {  // change at a minimum every 30 seconds
          dTimeLast = sm->update_time; // reset the timer so it will switch a view, intersperse the plot views with the quake/earth & cube
          if (bFirstShown) {
            switch (g_eView) {
               case VIEW_PLOT_3D: // if on 3d plot go to earth
                   g_eView = earth.ViewCombined(); 
                   break;
               case VIEW_EARTH_NIGHT:
               case VIEW_EARTH_COMBINED:
               case VIEW_EARTH_DAY: // if on earth go to 3d view
                   g_eView = VIEW_PLOT_2D;
                   break;
               case VIEW_PLOT_2D: // if on 2d plot go to bouncy cube
                   g_eView = VIEW_CUBE;
                   break;
               case VIEW_CUBE: // if on cube go to 3d plot
                   g_eView = VIEW_PLOT_3D;
                   break;
               default:  // should never get here
                   g_eView = earth.ViewCombined();
                   break;
            }
          }
          bFirstShown = true;             // flags that the first view hasn't been shown yet
       }
    }
#endif

#ifdef QCNLIVE
    if (g_eView == VIEW_PLOT_2D || g_eView == VIEW_PLOT_3D) { // if we're in a "plot" view pass in either our snapshot point or current offset to setup graphics memory from sensor data
#else
	if (!earth.IsShown()) { // if not qcnlive anythign but the earth view should recalc (i.e. the cube is spinning with data
#endif
           setupPlotMemory(g_bSnapshot ? g_lSnapshotPoint : sm->lOffset-1);  // note we use the next-to-last live point as current point may be being written
    }

    // from here on is the plots or earth or cube scene
    
    // draw logo first - it's in background

    switch (g_eView) {
       case VIEW_PLOT_2D:
			if (qcn_2dplot::IsWhite()) { // white background for qcnlive
				glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
				//glClearColor(0.0f, 0.33984375f, 0.62109375f, 1.0f); // blue for splash screen
			} else {  // black background for screensaver
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			}
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable (GL_LINE_SMOOTH);
			glEnable (GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
		  draw_logo(true);
		  qcn_2dplot::draw_plot();
		  draw_triggers();
		  qcn_2dplot::draw_text();
          break;
       case VIEW_PLOT_3D:
	  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          draw_logo();
          draw_plots_3d();
          draw_triggers();
          draw_text_plot();
          draw_text_user();
          break;
       case VIEW_EARTH_DAY:
       case VIEW_EARTH_NIGHT:
       case VIEW_EARTH_COMBINED:
	      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          draw_logo();
          earth.RenderScene(g_width, g_height, viewpoint_distance[g_eView], pitch_angle[g_eView], roll_angle[g_eView]);
          earth.RenderText();
          draw_text_user();
          break;
       case VIEW_CUBE:
	      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          draw_logo();
          cube.RenderScene(g_width, g_height, viewpoint_distance[g_eView], pitch_angle[g_eView], roll_angle[g_eView]);
		  cube.RenderText();
          draw_text_user();
		  break;
#ifdef QCNLIVE
		case VIEW_GAME:
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			gameMatch.RenderScene(g_width, g_height, viewpoint_distance[g_eView], pitch_angle[g_eView], roll_angle[g_eView]);
			break;
#endif
    }

    glFinish();

    bInHere = false;
}

void Resize(int w, int h)
{
    g_width = w;
    g_height = h;
    g_aspect = (float) g_width / (float) g_height;

    glViewport(0, 0, w, h);
    if (earth.IsShown()) {
        earth.Resize(g_width, g_height);
    }
    else if (g_eView==VIEW_CUBE) {
        cube.Resize(g_width, g_height);
    }
}

void MouseMove(int x, int y, int left, int middle, int right)
{
/*
#ifdef _DEBUG 
  sprintf(sm->strDisplay, "x=%d  y=%d  left=%d  mid=%d  right=%d", x, y, left, middle, right);
#endif
*/
	// swap colors on 2d view
	if (g_eView == VIEW_PLOT_2D && left && right) {
		qcn_2dplot::SetWhite(qcn_2dplot::IsWhite());
	}

	//if (g_eView != VIEW_EARTH_DAY && g_eView != VIEW_EARTH_NIGHT && g_eView != VIEW_EARTH_COMBINED) return;

    mouseSX = x;
    mouseSY = y;
	fDiff2D = 0.0f; // initialize

    if (earth.IsShown()) {
      earth.MouseMotion(mouseSX, mouseSY, left, middle, right);
    }
    else if (g_eView==VIEW_PLOT_2D) { // this is handled by the slider in qcnlive
		if (left || right) { 
			fDiff2D = mouseSX-mouseX;  // if positive it's going right (back in time), negative it's going left (forward in time), scale 1 low, 10+ high
			mouseX = mouseSX;
			mouseY = mouseSY;
#ifndef QCNLIVE
			if (fDiff2D < 0.0f) TimeWindowForward();
			else TimeWindowBack();
#endif
		}
		else {
			mouse_down = false;
		}
    }
    else if (g_eView==VIEW_CUBE) {
      cube.MouseMotion(mouseSX, mouseSY, left, middle, right);
    }
    else if (g_eView == VIEW_PLOT_3D) { // just rotate stuff on 3d plots
      if (left) {
          pitch_angle[g_eView] += (mouseSY-mouseY)*.1;
          roll_angle[g_eView] += (mouseSX-mouseX)*.1;
          mouseX = mouseSX;
          mouseY = mouseSY;
      } else if (right) {
          double d = (mouseSY-mouseY);
          viewpoint_distance[g_eView] *= exp(d/100.);
          mouseX = mouseSX;
          mouseY = mouseSY;
      } else {
          mouse_down = false;
      }
    }
}

void MouseButton(int x, int y, int which, int is_down)
{
	//if (g_eView != VIEW_EARTH_DAY && g_eView != VIEW_EARTH_NIGHT && g_eView != VIEW_EARTH_COMBINED) return;
	//if (g_eView == VIEW_PLOT_2D) return;

	//if (sm) sm->dTimeInteractive = dtime();

	mouseX = x;
    mouseY = y;

    if (earth.IsShown()) {
      earth.MouseButton(mouseX, mouseY, which, is_down);
    }
    //else if (g_eView==VIEW_CUBE) {
    //  cube.MouseButton(mouseX, mouseY, which, is_down);
    //}
    //else {
        mouse_down = is_down ? true : false;
        /*if (mouse_down) {  // save coords when mouse down
          mouseX = x;
          mouseY = y;
        }*/
    //}
}

void KeyDown(int k1, int k2)
{
   // 27=esc  81/113 = Q, 83/115 = S, 52 = left arrow, 54 = right arrow, 45 = minus, 61 = plus
   key_press = k1;
   key_press_alt = k2;

   //if (sm) sm->dTimeInteractive = dtime();
   if (key_press == 27) { // immediate quit on escape key
      qcn_graphics::Cleanup();
      _exit(0);
   }

   switch(key_press)
   { 
                case 'q':
                case 'Q':
                        // hit Q so toggle earth view/recent quake display
                        if (g_eView == VIEW_PLOT_3D || g_eView == VIEW_PLOT_2D) {
                                g_eView = earth.ViewCombined();
                        }
                        else {
                                g_eView = VIEW_PLOT_3D;
                        }
						FaderOn();
                        break;
                case 'p':
                case 'P': // screenshot
                    qcn_graphics::ScreenshotJPG();
                        break;
                case 'c':
                case 'C':
                   g_eView = VIEW_CUBE;
					FaderOn();
                   break;
                case 'l':
                case 'L':  // limit (scale) the amplitude
                        if (g_eView == VIEW_PLOT_2D) {
                           g_eView = VIEW_PLOT_3D;
                        }
                        else if (g_eView == VIEW_PLOT_3D) {
                           g_eView = VIEW_PLOT_2D;
                        }
						FaderOn();
                        bResetArray = true;
                        break;
                case 's':
                case 'S':  // hit S so toggle static display
                        g_bSnapshot = !g_bSnapshot;
                        if (sm && g_bSnapshot) {
                           g_lSnapshotPoint = sm->lOffset-1;  // -1 gives us some clearance we're not reading from end of array which sensor is writing
                           g_bSnapshotArrayProcessed = false;
                           if (earth.IsShown()) {
                                  // if in earth mode, send it back to regular seismic view, non-snapshot
                                  g_bSnapshot = false;
                           }
                        }
                        bResetArray = true;
                        break;
   }

   if (earth.IsShown()) {
       earth.KeyDown(key_press, key_press_alt);
       return;
   }
   //if (g_eView == VIEW_CUBE)  { 
   //    cube.KeyDown(key_press, key_press_alt);
   //    return;
   //}


   switch(key_press)
   {
#ifdef _WIN32
				case 188: // < on Windows, or at least on the Lenovo Thinkpad keyboard!
#endif
                case 44:  // <
                case 60:  // ,
                case 52:  // left arrow key so move snapshot point over a winsize
                        TimeWindowBack();
                        break;
#ifdef _WIN32
				case 190: // > on Windows, or at least on the Lenovo Thinkpad keyboard!
#endif
                case 46:  // >
                case 62:  // .
                case 54:  // right arrow key so move snapshot point over
                        TimeWindowForward();
                        break;
#ifdef _WIN32
				case 189: // minus on Windows, or at least on the Lenovo Thinkpad keyboard!
#endif
				case 45: // minus -- decrease the winsize 
					qcn_graphics::SetTimeWindowWidth(false);
                        break;
#ifdef _WIN32
				case 187: // plus on Windows, or at least on the Lenovo Thinkpad keyboard!
#endif
                case 61: // plus 
					qcn_graphics::SetTimeWindowWidth(true);
                        break;
   }
}

void KeyUp(int k1, int k2)
{
   key_up = k1;
   key_up_alt = k2;
   if (earth.IsShown()) earth.KeyUp(k1,k2);
   //else if (g_eView == VIEW_CUBE)  cube.KeyUp(k1, k2);
}
		
// force the alpha fader on e.g. if we switched view
void FaderOn()
{
	if (sm && sm->update_time > 1.0f) {  // default to update time from main loop if no valid time passed in	
		g_faderLogo.value(sm->update_time, g_alphaLogo, true);  // set alpha value for text
	}	
}
		
#ifdef QCNLIVE_DEMO
		const  int g_ciDemoImgMax = 7;
		static int g_iDemoImgCur  = -1;

		void demo_switch_ad()
		{
			if (txAdd.id) { // try and delete this texture
				const GLuint cgl = txAdd.id;
                glDeleteTextures(1, &cgl);
                txAdd.id = 0;
			}
			
			if (++g_iDemoImgCur > g_ciDemoImgMax)  {
                g_iDemoImgCur = -1;
                return; // leaves a blank image again
			}
			
			// setup new image texture
			char fname[32];
			sprintf(fname, "logo%02d.jpg", g_iDemoImgCur);
			// check for extra logo i.e. museum logo if any
			txAdd.load_image_file(fname);
		}
#endif

void scale_screen_qcn(int iw, int ih)
{
	float aspect_ratio = 4.0f/3.0f;
    float w=(float)iw, h=(float)ih;
    float xs, ys;
	if (h*aspect_ratio > w) {
        xs = 1.0f;
        ys = (w/aspect_ratio)/h;
    } else {
		xs = (h*aspect_ratio)/w;
        ys = 1.0f;
    }
	glScalef(xs, ys*aspect_ratio, 1.0f);
	
}
		
void mode_ortho_qcn() 
{ // try with no scale_screen as opposed to the boinc version so our graphics can "stretch" to the screen but not be out of place
		glDisable(GL_DEPTH_TEST);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, 1, 0, 1, 0, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		gluLookAt(
					  0.0, 0.0, 1.0,        // eye position
					  0, 0, 0,              // where we're looking
					  0.0, 1.0, 0.          // up is in positive Y direction
					  );
		int viewport[4];
		get_viewport(viewport);
		center_screen(viewport[2], viewport[3]);    // CMC here - this will force a pseudo 4:3 aspect ratio 
		scale_screen_qcn(viewport[2], viewport[3]);
}

		
}  // namespace qcn_graphics

