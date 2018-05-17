/* qcnqt -- main QCN GUI (using Qt cross-platform windowing toolkit) */

#include "qcnqt.h"
#include "qcn_graphics.h"
#include "qcn_curl.h"
#include "qcnopenglwidget.h"

// main program for Qt window
int main(int argc, char *argv[])
{
	int iReturn = 1; // default to error status exit
    MyApp myApp(argc, argv); // the constructor of MyApp does all the necessary initialization, splash screens etc (destructor does the cleanup of course)
    if (myApp.Init()) { // if this is false then an error on startup
		iReturn = myApp.exec();
	}
	myApp.Exit();
    return iReturn;
}

MyApp::MyApp(int& argc, char** argv)  
  : QApplication(argc, argv), 
    m_timerQuakeList(NULL), m_timerMakeQuake(NULL), m_frame(NULL), 
     m_iMakeQuakeTime(10), m_iMakeQuakeCountdown(10), m_MakeQuakePrinter()
{
}

#ifdef _WIN32  // Win fn ptr
  DWORD WINAPI QCNThreadMain(LPVOID /* lpParameter */)
#else  // Mac & Linux thread function ptr
  void* QCNThreadMain(void*)
#endif
{ 
    return 
#ifdef _WIN32
	(DWORD)
#else 
	(void*)
#endif 
	     qcn_main::qcn_main(0, NULL);
}

// the next two will be used in the main thread, but declare here (outside the thread)
CQCNShMem* volatile sm = NULL;

void MyApp::SetPath(const char* strArgv)
{
  // use the Qt function to get the path as Mac can have weird paths depending on if you click from Finder etc
    
    QString qstrPath = QCoreApplication::applicationDirPath();
    QByteArray qbaPath = qstrPath.toLocal8Bit();
    const char* strPath = qbaPath.data();
  	
    _chdir(strPath);
    
#ifdef __APPLE_CC__
    // it seems we now need to check we are not within the app ie .app/Contents/MacOS
    if (strcasestr(qbaPath.data(), "Contents/MacOS")) {
        // we need to chdir three levels back
        _chdir("../../../");
    }
#endif // apple special case

    _chdir(QCNGUI_INIT_DIR); // now move to the init subdir
        
      // set icon - FILENAME_LOGO is set in qcnlive_define.h for the appropriate Mac/Win version
	if (boinc_file_exists(FILENAME_LOGO)) 
		setWindowIcon(QIcon(FILENAME_LOGO));
	
/* for debugging...
    
    QMessageBox msgBox;
    msgBox.setInformativeText(qbaPath.data());
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
 */
    
}

bool MyApp::CreateBOINCInitFile()
{
    qcn_curl::g_curlBytes = 0;
	bool bInit = true;
	
	// create the init_data.xml file including the curl lookup to the website

	char* strQuake = new char[qcn_curl::BYTESIZE_CURL];
	memset(strQuake, 0x00, sizeof(char) * qcn_curl::BYTESIZE_CURL);	

    // OK, first off let's try to do a boinc curl connection to get strQuake
	if (!qcn_curl::execute_curl(QCNGUI_URL_QUAKE, strQuake, qcn_curl::BYTESIZE_CURL)) {
	    // Our curl lookup of recent quake data failed, so 
		// copy over historical quake data so we have something
	    memset(strQuake, 0x00, sizeof(char) * qcn_curl::BYTESIZE_CURL);
	    strcpy(strQuake, QCNGUI_QUAKE_HISTORICAL);
		bInit = false;	
	}

    // write a new file if we got the curl data above or file doesn't exist already
    if (bInit || !boinc_file_exists(QCNGUI_BOINC_INIT_FILE)) {
       FILE* fInitFile = fopen(QCNGUI_BOINC_INIT_FILE, "w");
  	   if (fInitFile) {
	      fprintf(fInitFile, QCNGUI_INIT_1, (int)(atof(QCN_VERSION_STRING)*100.0f), "qcnlive");
	      fprintf(fInitFile, "%s", strQuake);
	      fprintf(fInitFile, QCNGUI_INIT_2, "qcnlive");
	      fclose(fInitFile);
	      fInitFile = NULL;
	   }
	   else {
	      bInit = false;
	   }
	}
	delete [] strQuake;
	
	return bInit;
}

// this inits a lot of things and gets ready & launches the main thread etc
bool MyApp::MainInit()
{
	// delete old junk files
	//boinc_delete_file(XML_PREFS_FILE);
	boinc_delete_file(STDERR_FILE);
	boinc_delete_file("boinc_finished");
	boinc_delete_file("boinc_lockfile");
/*
	// freopen stdout to stdout.txt
	if (!freopen("stdout.txt", "w", stdout)) {
           fprintf(stderr, "Can't redirect stdout for qcnqt!\n");
	}
 */
	
	processEvents(); // give the app time to process mouse events since we're before the event loop

    // start init QCN/BOINC stuff -- this gets the latest quake data and creates a boinc-style init_data.xml file
	if (m_psplash) m_psplash->showMessage(tr("Retrieving latest earthquakes..."), Qt::AlignRight | Qt::AlignBottom, Qt::black);
	
	CreateBOINCInitFile();

    qcn_main::g_bDemo = false;
	qcn_graphics::g_bFader = false; // no fader required, just in screensaver mode
    qcn_util::ResetCounter(WHERE_MAIN_STARTUP);  // this is the one and only place ResetCounter is called outside of the sensor thread, so it's safe
    qcn_main::parseArgs(0, NULL); // parse args has to be done early in startup, right after the first ResetCounter usually

	processEvents(); // give the app time to process mouse events since we're before the event loop
	if (m_psplash) m_psplash->showMessage(tr("Getting initial settings..."), Qt::AlignRight | Qt::AlignBottom, Qt::black);
    get_qcnlive_prefs();  // this sets the m_rect among other things

    return StartMainThread();  
}

bool MyApp::get_qcnlive_prefs()
{
    // read in the saved trigger count
    FILE *fp; 
    char* strRead = new char[1024];
	
    memset(strRead, 0x00, 1024 * sizeof(char));
	
    // basic defaults
    m_rect.setX(MY_RECT_DEFAULT_POS_X);
    m_rect.setY(MY_RECT_DEFAULT_POS_Y);
    m_rect.setWidth(MY_RECT_DEFAULT_WIDTH);
    m_rect.setHeight(MY_RECT_DEFAULT_HEIGHT);
	
	m_iMakeQuakeTime = 10; // default time for make-quake countdown i.e. 10 seconds
	m_iMakeQuakeCountdown = 10; 
	m_strMakeQuakePrinter = PRINTER_PDF;

    sm->dMyLatitude = NO_LAT;
    sm->dMyLongitude = NO_LNG; 
    memset((char*) sm->strMyStation, 0x00, SIZEOF_STATION_STRING);

    sm->dMyElevationMeter = 0.0f; 
    sm->iMyElevationFloor = 0; 
	sm->iMySensor = -1;
	sm->iMyAxisSingle = 0x00;
	sm->bMyContinual = false;  // default to no continual recording (i.e. user has to start/stop recording via the button)
	sm->bMyOutputSAC = false;  // default to csv text output i.e. not sac

    if (!boinc_file_exists(QCNGUI_XML_PREFS_FILE)) {
		delete [] strRead;
		return false; // don't bother if doesn't exist!
	}

    if ( (fp = fopen(QCNGUI_XML_PREFS_FILE, "r")) == NULL) {
       fprintf(stdout, "Error opening file %s\n", QCNGUI_XML_PREFS_FILE);
		delete [] strRead;
       return false;
    }
    fread(strRead, sizeof(char), _MAX_PATH, fp);
    fclose(fp);

    // get the current screen dimensions so we can reset to a sensible size
    // in case they are at a lower resolution now than when they saved in the past etc
    QSize qsize(this->desktop()->size());

    // parse the settings, but default to sensible sizes in case they don't make sense
    char strParse[16];  // make the tag from the define which don't have the <>
	int iTemp = -1;
    sprintf(strParse, "<%s>", XML_X);
    if (parse_int(strRead, strParse, iTemp) && iTemp >= 0)
		m_rect.setX(iTemp);
	else
        m_rect.setX(MY_RECT_DEFAULT_POS_X);
	
	iTemp = -1;
    sprintf(strParse, "<%s>", XML_Y);
    if (parse_int(strRead, strParse, iTemp) && iTemp >= 0)
		m_rect.setY(iTemp);
	else
        m_rect.setY(MY_RECT_DEFAULT_POS_Y);

	iTemp = -1;
    sprintf(strParse, "<%s>", XML_WIDTH);
    if (parse_int(strRead, strParse, iTemp) && iTemp >= 100 && iTemp <= qsize.width())
		m_rect.setWidth(iTemp);
	else
        m_rect.setWidth(MY_RECT_DEFAULT_WIDTH);
    
	iTemp = -1;
	sprintf(strParse, "<%s>", XML_HEIGHT);
    if (parse_int(strRead, strParse, iTemp) && iTemp >= 100 && iTemp <= qsize.height())
		m_rect.setHeight(iTemp);
	else
        m_rect.setHeight(MY_RECT_DEFAULT_HEIGHT);

    // get preferred sensor if any
    sprintf(strParse, "<%s>", XML_SENSOR);
    if (!parse_int(strRead, strParse, sm->iMySensor) || sm->iMySensor <= 0)
        sm->iMySensor = -1;

    // get preferred single axis if any
    sprintf(strParse, "<%s>", XML_AXIS_SINGLE);
    if (!parse_int(strRead, strParse, sm->iMyAxisSingle) || sm->iMyAxisSingle <= 0)
        sm->iMyAxisSingle = 0x00;
	
    // check for valid lat/lng range
    sprintf(strParse, "<%s>", XML_LATITUDE);
    if (parse_double(strRead, strParse, (double&) sm->dMyLatitude)) {
      if (sm->dMyLatitude < -90.0f || sm->dMyLatitude > 90.0f) {
        sm->dMyLatitude = NO_LAT;
      }
    }
    else {
        sm->dMyLatitude = NO_LAT;
    }

    sprintf(strParse, "<%s>", XML_LONGITUDE);
    if (parse_double(strRead, strParse, (double&) sm->dMyLongitude)) {
      if (sm->dMyLongitude < -180.0f || sm->dMyLongitude > 180.0f) {
        sm->dMyLongitude = NO_LNG;
      }
    }
    else {
        sm->dMyLongitude = NO_LNG;
    }

    sprintf(strParse, "<%s>", XML_STATION);
    if (!parse_str(strRead, strParse, (char*) sm->strMyStation, SIZEOF_STATION_STRING))
       memset((char*) sm->strMyStation, 0x00, SIZEOF_STATION_STRING);

    if (strlen(sm->strMyStation)>0) {
		strcpy(sm->dataBOINC.wu_name, sm->strMyStation);
    }

    // elevation data
    sprintf(strParse, "<%s>", XML_ELEVATION);
    parse_double(strRead, strParse, (double&) sm->dMyElevationMeter);
    sprintf(strParse, "<%s>", XML_FLOOR);
    parse_int(strRead, strParse, (int&) sm->iMyElevationFloor);

    // continual
	int iTmp = 0;
    sprintf(strParse, "<%s>", XML_CONTINUAL);
    parse_int(strRead, strParse, iTmp);
	sm->bMyContinual = (bool)(iTmp > 0);
		
    // sac format
	iTmp = 0;
    sprintf(strParse, "<%s>", XML_SACFORMAT);
    parse_int(strRead, strParse, iTmp);
	sm->bMyOutputSAC = (bool)(iTmp > 0);

    // show vertical time lines
	iTmp = 0;
    sprintf(strParse, "<%s>", XML_VERTICAL_TIME);
    parse_int(strRead, strParse, iTmp);
	sm->bMyVerticalTime = (bool)(iTmp > 0);

	// show vertical trigger lines
	iTmp = 0;
    sprintf(strParse, "<%s>", XML_VERTICAL_TRIGGER);
    parse_int(strRead, strParse, iTmp);
	sm->bMyVerticalTrigger = (bool)(iTmp > 0);
	
	// makequake time length (default 10 seconds)
    sprintf(strParse, "<%s>", XML_MAKEQUAKE_TIME);
	if (!parse_int(strRead, strParse, m_iMakeQuakeTime) || m_iMakeQuakeTime < 1 || m_iMakeQuakeTime > 60)
		m_iMakeQuakeTime = 10; // default time for make-quake countdown i.e. 10 seconds
		
    sprintf(strParse, "<%s>", XML_MAKEQUAKE_COUNTDOWN);
	if (!parse_int(strRead, strParse, m_iMakeQuakeCountdown) || m_iMakeQuakeCountdown < 1 || m_iMakeQuakeCountdown > 60)
		m_iMakeQuakeCountdown = 10; // default time for make-quake countdown i.e. 10 seconds

	char* strPrn = new char[_MAX_PATH];
	memset(strPrn, 0x00, sizeof(char) * _MAX_PATH);
    sprintf(strParse, "<%s>", XML_MAKEQUAKE_PRINTER);
	if (parse_str(strRead, strParse, strPrn, _MAX_PATH)) {
		m_strMakeQuakePrinter = strPrn;
	}
	else {
		m_strMakeQuakePrinter = PRINTER_PDF; // default time for make-quake countdown i.e. 10 seconds
	}

	delete [] strPrn;
	delete [] strRead;

    return true;
}

bool MyApp::set_qcnlive_prefs()
{   
	if (!sm) return false;
	
    FILE *fp; 
	if (strlen(sm->strMyStation)>0) strcpy(sm->dataBOINC.wu_name, sm->strMyStation); // copy station name to workunit name
    if ( (fp = fopen(QCNGUI_XML_PREFS_FILE, "w")) == NULL) {
       fprintf(stdout, "Error opening file %s\n", QCNGUI_XML_PREFS_FILE);
       return false;
    }
	
    // just save a string of pseudo-XML	
    fprintf(fp, "<%s>%d</%s>\n"
                "<%s>%d</%s>\n"
                "<%s>%d</%s>\n"
                "<%s>%d</%s>\n"
                "<%s>%f</%s>\n"
                "<%s>%f</%s>\n"
                "<%s>%s</%s>\n"
                "<%s>%f</%s>\n"
                "<%s>%d</%s>\n"
   			    "<%s>%d</%s>\n"
				"<%s>%d</%s>\n"
				"<%s>%d</%s>\n"
				"<%s>%d</%s>\n"
				"<%s>%d</%s>\n"
				"<%s>%d</%s>\n"
				"<%s>%d</%s>\n"
				"<%s>%d</%s>\n"
				"<%s>%s</%s>\n"
                        ,
                    XML_X, m_rect.x(), XML_X,
                    XML_Y, m_rect.y(), XML_Y, 
                    XML_WIDTH, m_rect.width(), XML_WIDTH, 
                    XML_HEIGHT, m_rect.height(), XML_HEIGHT,
                    XML_LATITUDE, sm->dMyLatitude, XML_LATITUDE,
                    XML_LONGITUDE, sm->dMyLongitude, XML_LONGITUDE,
                    XML_STATION, sm->strMyStation, XML_STATION,
                    XML_ELEVATION, sm->dMyElevationMeter, XML_ELEVATION,
                    XML_FLOOR, sm->iMyElevationFloor, XML_FLOOR,
					XML_SENSOR, sm->iMySensor, XML_SENSOR,
					XML_AXIS_SINGLE, sm->iMyAxisSingle, XML_AXIS_SINGLE,
					XML_CONTINUAL, (sm->bMyContinual ? 1 : 0), XML_CONTINUAL,
					XML_SACFORMAT, (sm->bMyOutputSAC ? 1 : 0), XML_SACFORMAT,
					XML_VERTICAL_TIME, (sm->bMyVerticalTime ? 1 : 0), XML_VERTICAL_TIME,
					XML_VERTICAL_TRIGGER, (sm->bMyVerticalTrigger ? 1 : 0), XML_VERTICAL_TRIGGER,
			        XML_MAKEQUAKE_TIME, m_iMakeQuakeTime, XML_MAKEQUAKE_TIME,
					XML_MAKEQUAKE_COUNTDOWN, m_iMakeQuakeCountdown, XML_MAKEQUAKE_COUNTDOWN,
					XML_MAKEQUAKE_PRINTER, (const char*) m_strMakeQuakePrinter.toLatin1(), XML_MAKEQUAKE_PRINTER	
    );

    fclose(fp);
    return true;
}

void MyApp::KillSplashScreen()
{  // used in a wx.CallAfter from the initial myglpane Render() call so that we can get rid of the splash screen just as soon 
   // as the final init (the graphics files loaded etc) is completed!
   if (m_psplash) {
	   m_psplash->close();
	   delete m_psplash;
	   m_psplash = NULL;
   }
}
	
void MyApp::SetRect(const QRect& rect)
{
   m_rect = rect;
}

bool MyApp::Init()
{
    
	m_bInit = false;
		
	// do splash screen until the mainwin show	
    m_psplash = NULL; // init the splash screen
	
    SetPath();  // go to the init/ directory
	
	// splash screen
	if (boinc_file_exists(FILENAME_SPLASH)) {
		m_psplash = new QSplashScreen(QPixmap(FILENAME_SPLASH));

	}

	// we're hosed if can't make splash screen?
	if (!m_psplash) return false;
	
	m_psplash->show();
	m_psplash->showMessage(tr("Starting up..."), Qt::AlignRight | Qt::AlignBottom, Qt::black);
	
	
	// first check makequake dir exists
	if (!is_dir(DIR_MAKEQUAKE)) {
		boinc_mkdir(DIR_MAKEQUAKE);
	}
	
	// init make-quake stuff
	m_iMakeQuakeTime = 10;
	qcn_graphics::g_MakeQuake.clear();
	
    // note that since we're all in one process (yet with multiple threads, we can just build our shmem struct on the heap with new
	// it will get deleted in MyApp::Exit, so we don't need the boinc call below (which I think never destroys the shared mem segment!)
    //	sm = (CQCNShMem*) boinc_graphics_make_shmem(QCNGUI_SHMEM, sizeof(CQCNShMem));
	// init the graphics stuff, i.e. memory pointer
    // clear memory and setup important vars below

	processEvents(); // give the app time to process mouse events since we're before the event loop
	
    sm = new CQCNShMem();
    if (!sm) {
        fprintf(stderr, "failed to create shared mem segment %s, exiting\n", QCNGUI_SHMEM);
        return false;
    }
    strcpy(sm->dataBOINC.wu_name, "qcnlive");

	/*
    m_rect.setX(MY_RECT_DEFAULT_POS_X);
    m_rect.setY(MY_RECT_DEFAULT_POS_Y);
    m_rect.setWidth(MY_RECT_DEFAULT_WIDTH);
    m_rect.setHeight(MY_RECT_DEFAULT_HEIGHT);
	*/

    if (!MainInit()) return false;  // this does a lot of init stuff such as get the latest quake list via curl etc
	
    // if here then the main thread was launched & init
	processEvents(); // give the app time to process mouse events since we're before the event loop

    // setup the toolbar controls for the 2D Plot, i.e. a horiz scrollbar, buttons for scaling etc
    // CMC frame->SetupToolbars();

	// setup & start the timer for getting the next earthquake list from the qcn server
    m_timerQuakeList = new QTimer(this);
    connect(m_timerQuakeList, SIGNAL(timeout()), this, SLOT(slotGetLatestQuakeList()));
    m_timerQuakeList->start(1800000L);  // in milliseconds, so a half hour is a lot!  can start now
	
	// setup the timer for making quakes - it's actually triggered from a menu choice in MyFrame
	// the MyFrame menu will start this timer ever second, which will call pMyApp->slotMakeQuake
    m_timerMakeQuake = new QTimer(this);
    connect(m_timerMakeQuake, SIGNAL(timeout()), this, SLOT(slotMakeQuake()));
	
	processEvents(); // give the app time to process mouse events since we're before the event loop
	if (m_psplash) m_psplash->showMessage(tr("Preparing graphics engine..."), Qt::AlignRight | Qt::AlignBottom, Qt::black);
	m_frame = new MyFrame(this);  // construct the window frame
	if (m_frame) {
		m_frame->Init();
		processEvents(); // give the app time to process mouse events since we're before the event loop
		m_frame->show();  // show the main window frame
		if (m_psplash) m_psplash->finish(m_frame);  // this will sto the m_psplash after the main window is shown
		KillSplashScreen();
		m_bInit = true;
	}
	KillSplashScreen(); // just in case the myframe construction failed

    if (m_bInit) {
        m_frame->setStartupView();
    }
    return m_bInit;
} 

// a private slot to launch the quakelist every hour
void MyApp::slotGetLatestQuakeList()
{
	GetLatestQuakeList();
}

// a private slot to monitor the "MakeQuake" stuff
void MyApp::slotMakeQuake()
{  // this gets triggered every second, so decrement countdown, then start after 10 seconds make a snapshot
	// when done should probably issue a timer->stop?  hope you can do that from within a slot!
	static QPrintPreviewDialog* qppr = NULL;
	static QPrinter* qpr = NULL;
	static char strName[64];
	static QString strPDF;

	if (!qcn_graphics::g_MakeQuake.bActive || qcn_graphics::g_eView != VIEW_PLOT_2D) { // quit if they changed view
		// odd if not active, stop timer & return
		m_timerMakeQuake->stop();
		qcn_graphics::g_MakeQuake.clear(); // can reset/reuse
		m_frame->statusBar()->showMessage(tr("Error in making quake - did you change the view from 2D Plot?"), 10000);
	}
	
	// a second has gone by, decrement countdown, or else
	if (--qcn_graphics::g_MakeQuake.iCountdown > 0) return; // still in countdown

	// if made it here we're monitoring, so decrement the iTime
	if (--qcn_graphics::g_MakeQuake.iTime > 0) return; // still shakin'
	
	// if here we can take a screenshot, so first stop this timer slot from firing again
	m_timerMakeQuake->stop();
	
	// i.e. when bActive & !bDisplay received in qcn_graphics thread, set bReceived = true and wait here
	int iCtr = 0;
	qcn_graphics::g_MakeQuake.bDisplay = false; // turn it off so any screen messages for make-quake disappear
	// when this is true our screen should not have countdown/montor msgs
	while (!qcn_graphics::g_MakeQuake.bReceived && ++iCtr <= 2000) { // wait up to 2 seconds for message to get received by graphics thread
		processEvents();
		usleep(1000);
	}

	if (!qcn_graphics::g_MakeQuake.bReceived) { // hmm, never got the received message from graphics thread, they may have switched 2D plot view 
		qcn_graphics::g_MakeQuake.clear(); // can reset/reuse
		m_frame->statusBar()->showMessage(tr("Graphics error in making quake - please try again"), 10000);
	}
	
	m_frame->updateGLWidget(); // force a screen refresh before the frame grab i.e. may be a lagging 'Monitoring...1s' message
	m_frame->statusBar()->showMessage(tr("Processing image..."));
	
	m_strQuakeJPG = qcn_graphics::ScreenshotJPG(); // this actually gets the screen grab and returns the filename
	if (m_strQuakeJPG.isEmpty() || !boinc_file_exists(m_strQuakeJPG.toLatin1()))  { //error - must not have saved pic 
		m_frame->statusBar()->showMessage(tr("Error in this session - sorry - please try later!"), 5000);
		m_strQuakeJPG.clear();
		goto done;
	}
	memset(strName, 0x00, sizeof(char) * 64);
	strlcpy(strName, qcn_graphics::g_MakeQuake.strName, 63);
	qcn_util::strAlNum(strName);
	strPDF.sprintf("%s/%s_%ld.pdf", DIR_MAKEQUAKE, strName, (long) dtime());
	
	// setup virtual printer device if they want PDF output or the QPrinterInfo struct is null (invalid)
	if (m_strMakeQuakePrinter == PRINTER_PDF || m_MakeQuakePrinter.isNull()) {
		qpr = new QPrinter(QPrinter::HighResolution);
		if (!qpr) goto done; // error
		qpr->setOutputFileName(strPDF);
		qpr->setOutputFormat(QPrinter::PdfFormat);
		qpr->setPaperSize(QPrinter::Letter);
	}
	else { // they chose a real printer i.e. makequake must be valid
		qpr = new QPrinter(m_MakeQuakePrinter, QPrinter::HighResolution);
		if (!qpr) goto done; // error
		qpr->setOutputFormat(QPrinter::NativeFormat);
	}

	// we always want landscape & letter size
	qpr->setOrientation(QPrinter::Landscape);

	// now do a printpreview which will "paint" the PDF
	qppr = new QPrintPreviewDialog(qpr, m_frame);
	qppr->setWindowFlags(Qt::Window);
	connect(qppr, SIGNAL(paintRequested(QPrinter*)), SLOT(slotPrintPreview(QPrinter*)));
	qppr->exec(); // bring up the window and "paint"
	
	m_frame->statusBar()->showMessage("Finished!", 5000);

	
done:
	if (qppr) delete qppr;
	if (qpr) delete qpr;
	qpr = NULL;
	qppr = NULL;
	if (boinc_file_exists(m_strQuakeJPG.toLatin1())) boinc_delete_file(m_strQuakeJPG.toLatin1()); // get rid of original file if exists
	m_strQuakeJPG.clear();
	qcn_graphics::g_MakeQuake.clear(); // can reset/reuse now
}

void MyApp::slotPrintPreview(QPrinter* qpr)
{
	static QPixmap* qpjpg = NULL;
	static QPainter* paint = NULL;
	
	if (!qpr) return;
	
	QString strTitle;
	QString strOut, statmsg;
        int iOldY;
	QRect rect;
	QSize size;

	// setup font
	int iPoint = 24;
	const int iLen = strlen(qcn_graphics::g_MakeQuake.strName);
	if (iLen > 50) {
		iPoint = 16;
	}
	else if (iLen > 20) {
		iPoint = 20;        //handle long names/point size of font
	}
	QFont qf("Arial", iPoint, QFont::Bold | QFont::Black); // Arial 24px bold should be a nice big title?

	// add 's or ' to end if name ends with s
	char strApos[3];	
	memset(strApos, 0x00, sizeof(char) * 3);	
	
	// get the right suffix i.e. apostrophe if name ends in s else 's
	if (iLen > 0 && qcn_graphics::g_MakeQuake.strName[iLen-1] == 's') strcpy(strApos, "'");
	else strcpy(strApos, "'s");
	
	strTitle = qcn_graphics::g_MakeQuake.strName;
	strTitle += strApos;
	strTitle += " Earthquake";
		
	// setup for printing
	qpjpg = new QPixmap(m_strQuakeJPG);
	if (!qpjpg || qpjpg->width() < 20) goto done; // our image didn't load
			
	paint = new QPainter(qpr);
    if (!paint) {
		statmsg = tr("Print error - Do you have a printer or PDF support?");
		m_frame->statusBar()->showMessage(statmsg);
		goto done; // error a la paint->begin(qpr) below?
	}
	
	rect = paint->viewport();
	size = qpjpg->size();
	size.scale(rect.size(), Qt::KeepAspectRatio);
        paint->setViewport(rect.x(), rect.y(), size.width(), size.height());
	paint->setWindow(rect);
	
        iOldY = rect.y();
	rect.setY(iOldY + ((float) rect.height() * .1));  // do some juggling around to ensure space at the top for the name
	rect.setHeight(rect.height() * .90);
	paint->drawPixmap(rect, *qpjpg);
	rect.setY(iOldY + ((float) rect.height() * .05));   // draw text not quite at the top edge
	
	// CMC HERE - add custom logo to bottom of printout
/*
	if (txAdd.id) {
        float pos[3] = {0.0f, 0.27f, 0.0f};
		if (bExtraOnly && ! g_MakeQuake.bActive) {
			pos[0] = 0.50f; 
			pos[1] = -0.20f;
		}
		else { // move additional logo to the top
			pos[0] = -0.015f;
			pos[1] = 0.545f;
		}
	}
*/		
	
	paint->setFont(qf);
	paint->drawText(rect, Qt::AlignHCenter | Qt::AlignTop, strTitle);
	paint->end();
	
	//statmsg = "Quake PDF saved to " + strPDF;
	//statmsg = "Quake PDF saved to " + strPDF;
	//m_frame->statusBar()->showMessage(statmsg);
	
	
done:
	if (qpjpg) delete qpjpg;
	if (paint) delete paint;
	qpjpg = NULL;
	paint = NULL;
	
}

void MyApp::GetLatestQuakeList()
{
	m_frame->statusBar()->showMessage(tr("Getting recent earthquake list..."), 5000);

    // this sequence will get the latest earthquake list
    if (! CreateBOINCInitFile()) {
		m_frame->statusBar()->showMessage(tr("Failed to get the latest earthquake list, try again later!"), 5000);
        return; // may as well split
    }
    qcn_graphics::getProjectPrefs(); // we have the strProjectPrefs so can get earthquake data now, this is in qcn_graphics.cpp
	m_frame->statusBar()->showMessage(tr("Earthquake list updated"), 5000);
}

int MyApp::Exit()
{
	if (m_timerQuakeList) {
		m_timerQuakeList->stop();
		delete m_timerQuakeList;
		m_timerQuakeList = NULL;
	}
	if (m_timerMakeQuake) {
		m_timerMakeQuake->stop();
		delete m_timerMakeQuake;
		m_timerMakeQuake = NULL;
	}
	if (m_frame) {
		delete m_frame; // necessary?
		m_frame = NULL;
	}

    KillMainThread();
	
	qcn_graphics::Cleanup();
	
	if (sm) { // try to remove the global shared mem?  that would be nice...
	   fprintf(stdout, "Freeing shared memory segment\n");
	   delete sm;
	   sm = NULL; // paranoid!  but who knows maybe the last microsecond the graphics will try to access sm...
	}
	
	fflush(stdout);

    // just in case we're exiting early and the splash screen still up!	
	KillSplashScreen();

    return 0;
}

bool MyApp::StartMainThread()
{
	qcn_main::g_threadMain = new CQCNThread(QCNThreadMain);
	if (qcn_main::g_threadMain) qcn_main::g_threadMain->Start();  // note returns whether main thread was created & started OK
	return (bool) (qcn_main::g_threadMain != NULL);
}

bool MyApp::KillMainThread()
{
	qcn_main::doMainQuit(); // qcn_main::g_iStop = TRUE; // try and sleep a little to give the threads a chance to stop, a second should suffice
	set_qcnlive_prefs();  // save graphics prefs
	
	int iCtr = 0;
	if (qcn_main::g_threadMain && qcn_main::g_threadMain->IsRunning())  { // the main thread is running, so kill it
		fprintf(stdout, "qcnqt: stopping main monitoring thread\n");
		while (qcn_main::g_threadMain->IsRunning() && ++iCtr <= 3000) {
			processEvents();
			usleep(1000);
		}
		fprintf(stdout, "qcnqt: main thread quit within %f seconds...\n", (float) iCtr * .001f);
	}
	else {
		fprintf(stdout, "qcnqt: main thread stopped\n");
	}
	
	if (qcn_main::g_threadMain) { // free main thread resources
		qcn_main::g_threadMain->Stop();
		delete qcn_main::g_threadMain;
		qcn_main::g_threadMain = NULL;
	}
	return true;
}
