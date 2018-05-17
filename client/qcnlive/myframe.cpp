/*
 *  myframe.cpp
 *  qcn
 *
 *  Created by Carl Christensen on 2/15/08.
 *
 */

#include "qcnqt.h"

// these are our toolbar icons in C-array-style XPM format, var names are the same as file name with a suffix _xpm
#include "icons.h"   // 32x32 icons made from "cat icons/*.xpm > icons.h"

#include "dlgsettings.h"
#include "dlgmakequake.h"
#include "qcn_earth.h"
#include "qcn_2dplot.h"

/* canonical list of all icons used for toolbar in qcn/client/qcnlive/icons directory:

 // view
 icon_earth.xpm
 icon_twod.xpm
 icon_threed.xpm
 icon_cube.xpm

 
 // options
 icon_absolute.xpm
 icon_ff.xpm
 icon_horiz_zoom_in.xpm
 icon_horiz_zoom_out.xpm
 icon_moon.xpm
 icon_nospin.xpm
 icon_pause.xpm
 icon_play.xpm
 icon_quakelist.xpm
 icon_record.xpm
 icon_rw.xpm
 icon_scaled.xpm
 icon_spin.xpm
 icon_stop.xpm
 icon_sun.xpm
 icon_usgs.xpm
 icon_vert_zoom_auto.xpm
 icon_vert_zoom_in.xpm
 icon_vert_zoom_out.xpm

 icon_sig.xpm
 icon_camera.xpm


*/

const short ICON_SIZE = 32;

/*
MyAboutBox::MyAboutBox(QMainWindow *pmw)  
   : QMessageBox(QMessageBox::Information, 
				 tr("About QCNLive"), 
				 tr("<b>QCNLive</b> is provided by the <BR> Quake-Catcher Network Project <BR><BR>http://quakecatcher.net<BR><BR>(c) 2008-2016 IRIS"),
				 QMessageBox::Ok,
				 pmw
				 )
{
	 
	 wxAboutDialogInfo myAboutBox;
	 //myAboutBox.SetIcon(wxIcon("qcnwin.ico", wxBITMAP_TYPE_ICO));
	 myAboutBox.SetVersion(wxString(QCN_VERSION_STRING));
	 myAboutBox.SetName(wxT("QCNLive"));
	 myAboutBox.SetWebSite(wxT("http://quakecatcher.net"), wxT("Quake-Catcher Network Website"));
	 myAboutBox.SetCopyright(wxT("(c) 2008-2016 IRIS")); 
	 //myAboutBox.AddDeveloper(wxT("Carl Christensen  (carlgt1@hotmail.com"));
	 myAboutBox.SetDescription(wxT("This software is provided free of charge for educational purposes.\n\nPlease visit us on the web:\n"));
	 
	 wxAboutBox(myAboutBox);
	 
	 QDialog* dlgAbout = new QDialog(this);
	 dlgAbout->setModal(true);
	 dlgAbout->exec();
}

void MyAboutBox::mousePressEvent(QMouseEvent *event)
{
	QString strText;
	strText.sprintf("x=%d  y=%d", event->x(), event->y());
	information (this, "clicked", strText);
}
 */	
	
	
MyFrame::MyFrame(MyApp* papp) 
   : QMainWindow(),
		m_dockWidgetView(NULL),
		m_dockWidgetOption(NULL),
		m_toolBarView(NULL),
		m_toolBarOption(NULL),
		m_menuBar(NULL),
		m_actionCurrent(NULL),
		m_pMyApp(papp)
{
}

bool MyFrame::Init()
{	
    m_bEarthDay = true;
    m_bEarthRotate = true;
    m_iSensorAction = 0;
	
    m_bSensorAbsolute2D = false;
    m_bSensorAbsolute3D = false;
	
	// setup all the actions, menus, and toolbars before defining the central widget (our OpenGL GLWidget)
    createActions();
    createMenus();
	createToolbar();

    m_centralWidget = new QWidget;
    setCentralWidget(m_centralWidget);
	
    m_glWidget = new QCNGLWidget(this);
    //pixmapLabel = new QLabel;
    m_glWidgetArea = new QScrollArea;
    m_glWidgetArea->setWidget(m_glWidget);
    m_glWidgetArea->setWidgetResizable(true);
    m_glWidgetArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_glWidgetArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_glWidgetArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_glWidgetArea->setMinimumSize(50, 50);
	
	// time slider for sensor views
    m_sliderTime = new QSlider(Qt::Horizontal);
    m_sliderTime->setRange(0, 100);
    m_sliderTime->setSingleStep(1);
    m_sliderTime->setPageStep(20);
    m_sliderTime->setTickInterval(10);
	setTimeSliderValue(100);
    m_sliderTime->setTickPosition(QSlider::TicksRight);
	// connect the slider value changed signal to SetTimePosition
    connect(m_sliderTime, SIGNAL(valueChanged(int)), this, SLOT(slotTimePosition(int)));
	// connect the slider setvalue slot to the widget timePosChnged
    connect(this, SIGNAL(signalTimePosition(int)), m_sliderTime, SLOT(setValue(int)));
	m_sliderTime->hide();
	
	m_ptbBase = NULL; // no toolbar base yet
	
    QGridLayout *centralLayout = new QGridLayout;
    centralLayout->addWidget(m_glWidgetArea, 0, 0, 1, 2);
    centralLayout->addWidget(m_sliderTime, 1, 0, 1, 2);
    m_centralWidget->setLayout(centralLayout);
			
#ifdef QCN_RAW_DATA
    setWindowTitle(tr("QCNLive (Raw Data)"));
#else
    setWindowTitle(tr("QCNLive"));
#endif
	statusBar()->showMessage(tr("Ready"), 0);
		
	QSettings settings(SET_COMPANY, SET_APP);
	if (settings.contains(QT_WINDOW_SETTINGS_GEOMETRY))  {
		restoreGeometry(settings.value(QT_WINDOW_SETTINGS_GEOMETRY).toByteArray());
	}
	else { // use a default 640 x 480?
		setGeometry(100, 100, 640, 480);
	}
	// windows state/toolbar layout etc
	if (settings.contains(QT_WINDOW_SETTINGS_STATE))  {
		restoreState(settings.value(QT_WINDOW_SETTINGS_STATE).toByteArray());
	}
	
	qcn_graphics::g_bFullScreen = this->isFullScreen(); // get full screen state
	if (qcn_graphics::g_bFullScreen) { // hide the toolbars in full-screen mode
		m_toolBarOption->hide();
		m_toolBarView->hide();
	}

	return true;
}

void MyFrame::updateGLWidget()  // force a screen redraw, i.e. for the "Make-A-Quake" screenshot
{
	if (m_glWidget && qcn_graphics::g_bInitGraphics) {
		m_glWidget->updateGL();
	}
}

int MyFrame::getTimeSliderValue()
{
	return m_sliderTime->value();
}

void MyFrame::setTimeSliderValue(const int iPos)
{
	qcn_graphics::setTimePercent(iPos);
	m_sliderTime->setValue(iPos);
	if (iPos == 100) {
		ToggleStartStop(true); // toggled start if near 100
	}
	else {
		ToggleStartStop(false); 
		qcn_graphics::TimeWindowPercent(iPos); 
	}
}

void MyFrame::slotTimePosition(int iPos)
{
	emit signalTimePosition(iPos);
	setTimeSliderValue(iPos);
}

void MyFrame::setStartupView(int iView)
{
#ifdef QCN_RAW_DATA  // default to 2d view
    m_actionCurrent = m_actionViewEarth;
    m_actionViewSensor2D->activate(QAction::Trigger);
#else
        m_actionViewEarth->activate(QAction::Trigger);
#endif
    updateGLWidget();
}

void MyFrame::createActions()
{
	// setup the actions of the various menu bar and toggle buttons
	
	// File actions
    m_actionFileExit = new QAction(tr("E&xit"), this);
	m_actionFileExit->setToolTip(tr("Quit QCNLive"));
    m_actionFileExit->setShortcuts(QKeySequence::Quit);
    connect(m_actionFileExit, SIGNAL(triggered()), this, SLOT(close()));	
	
	m_actionFileDialogSettings = new QAction(tr("Local &Settings"), this);
	m_actionFileDialogSettings->setShortcut(tr("Ctrl+S"));
	m_actionFileDialogSettings->setToolTip(tr("Enter local settings such as station name, latutide, longitude, elevation"));
	connect(m_actionFileDialogSettings, SIGNAL(triggered()), this, SLOT(fileDialogSettings()));

	m_actionFileMakeQuake = new QAction(tr("&Make Earthquake"), this);
	m_actionFileMakeQuake->setToolTip(tr("Make and Print Your Own Earthquake"));
	m_actionFileMakeQuake->setShortcut(tr("Ctrl+M")); 
	connect(m_actionFileMakeQuake, SIGNAL(triggered()), this, SLOT(fileMakeQuake()));
	
	// View actions
	m_actionViewEarth = new QAction(tr("&Earthquakes"), this);
	m_actionViewEarth->setToolTip(tr("Select this view to see the latest and historical earthquakes worldwide"));
	m_actionViewEarth->setCheckable(true);
	m_actionViewEarth->setIcon(QIcon(QPixmap(icon_earth_xpm)));
	m_actionViewEarth->setShortcut(tr("Ctrl+E"));
    connect(m_actionViewEarth, SIGNAL(triggered()), this, SLOT(actionView()));

	m_actionViewSensor2D = new QAction(tr("Sensor &2-dimensional"), this);
	m_actionViewSensor2D->setToolTip(tr("Select this view to see your accelerometer output as a 2-dimensional plot"));
	m_actionViewSensor2D->setCheckable(true);
	m_actionViewSensor2D->setIcon(QIcon(QPixmap(icon_twod_xpm)));
	m_actionViewSensor2D->setShortcut(tr("Ctrl+2"));
    connect(m_actionViewSensor2D, SIGNAL(triggered()), this, SLOT(actionView()));

	m_actionViewSensor3D = new QAction(tr("Sensor &3-dimensional"), this);
	m_actionViewSensor3D->setToolTip(tr("Select this to see your accelerometer output as a 3-dimensional plot"));
	m_actionViewSensor3D->setCheckable(true);
	m_actionViewSensor3D->setIcon(QIcon(QPixmap(icon_threed_xpm)));
	m_actionViewSensor3D->setShortcut(tr("Ctrl+3"));
    connect(m_actionViewSensor3D, SIGNAL(triggered()), this, SLOT(actionView()));

	m_actionViewCube = new QAction(tr("&Cube"), this);
	m_actionViewCube->setToolTip(tr("Select this view to see a bouncing cube that responds to your accelerometer"));
	m_actionViewCube->setCheckable(true);
	m_actionViewCube->setIcon(QIcon(QPixmap(icon_cube_xpm)));
	m_actionViewCube->setShortcut(tr("Ctrl+C"));
    connect(m_actionViewCube, SIGNAL(triggered()), this, SLOT(actionView()));

	/*
	m_actionViewGame = new QAction(tr("&Game"), this);
	m_actionViewGame->setToolTip(tr("Select this view to play a game with your accelerometer"));
	m_actionViewGame->setCheckable(true);
	//m_actionViewGame->setIcon(QIcon(icon_cube_xpm));
	m_actionViewGame->setShortcut(tr("Ctrl+G"));
    connect(m_actionViewGame, SIGNAL(triggered()), this, SLOT(actionView()));
	*/

	m_actionViewFullScreen = new QAction(tr("Toggle &Full Screen Mode"), this);
	m_actionViewFullScreen->setToolTip(tr("Go into full-screen mode (ESC key to exit)"));
	m_actionViewFullScreen->setShortcut(tr("Ctrl+F"));
    connect(m_actionViewFullScreen, SIGNAL(triggered()), this, SLOT(actionView()));
	
	
	// Option - Earth
	m_actionOptionEarthDay = new QAction(tr("&Day"), this);
	m_actionOptionEarthDay->setToolTip(tr("Show day view of global earthquake map"));
	m_actionOptionEarthDay->setCheckable(true);
	m_actionOptionEarthDay->setIcon(QIcon(QPixmap(icon_sun_xpm)));
	connect(m_actionOptionEarthDay, SIGNAL(triggered()), this, SLOT(actionOptionEarth()));

	m_actionOptionEarthNight = new QAction(tr("&Night"), this);
	m_actionOptionEarthNight->setToolTip(tr("Show night view of global earthquake map"));
	m_actionOptionEarthNight->setCheckable(true);
	m_actionOptionEarthNight->setIcon(QIcon(QPixmap(icon_moon_xpm)));
	connect(m_actionOptionEarthNight, SIGNAL(triggered()), this, SLOT(actionOptionEarth()));
	
	m_actionOptionEarthRotateOn = new QAction(tr("&Auto-rotate"), this);
	m_actionOptionEarthRotateOn->setToolTip(tr("Auto-rotate the globe"));
	m_actionOptionEarthRotateOn->setCheckable(true);
	m_actionOptionEarthRotateOn->setIcon(QIcon(QPixmap(icon_spin_xpm)));
	connect(m_actionOptionEarthRotateOn, SIGNAL(triggered()), this, SLOT(actionOptionEarth()));
	
	m_actionOptionEarthRotateOff = new QAction(tr("&Stop rotation"), this);
	m_actionOptionEarthRotateOff->setToolTip(tr("Stop rotation of the globe"));
	m_actionOptionEarthRotateOff->setCheckable(true);
	m_actionOptionEarthRotateOff->setIcon(QIcon(QPixmap(icon_nospin_xpm)));
	connect(m_actionOptionEarthRotateOff, SIGNAL(triggered()), this, SLOT(actionOptionEarth()));
	
	m_actionOptionEarthQuakelist = new QAction(tr("&Get latest earthquakes"), this);
	m_actionOptionEarthQuakelist->setToolTip(tr("Get the latest earthquake list from the USGS"));
	//m_actionOptionEarthQuakelist->setCheckable(true);
	m_actionOptionEarthQuakelist->setIcon(QIcon(QPixmap(icon_quakelist_xpm)));
	connect(m_actionOptionEarthQuakelist, SIGNAL(triggered()), this, SLOT(actionOptionEarth()));
	
	m_actionOptionEarthUSGS = new QAction(tr("&USGS Website"), this);
	m_actionOptionEarthUSGS->setToolTip(tr("Go to the USGS website for the currently selected earthquake"));
	//m_actionOptionEarthUSGS->setCheckable(true);
	m_actionOptionEarthUSGS->setIcon(QIcon(QPixmap(icon_usgs_xpm)));
	connect(m_actionOptionEarthUSGS, SIGNAL(triggered()), this, SLOT(actionOptionEarth()));
	
	
	// Option - Sensor (2D & 3D)
	m_actionOptionSensorVerticalZoomAuto = new QAction(tr("Auto-Zoom Vertical Scale"), this);
	m_actionOptionSensorVerticalZoomAuto->setToolTip(tr("Auto-Zoom Vertical Scale"));
	m_actionOptionSensorVerticalZoomAuto->setIcon(QIcon(QPixmap(icon_vert_zoom_auto_xpm)));
	//m_actionOptionSensorVerticalZoomAuto->setCheckable(true);
	connect(m_actionOptionSensorVerticalZoomAuto, SIGNAL(triggered()), this, SLOT(actionOptionSensor()));
	
	m_actionOptionSensorVerticalZoomIn = new QAction(tr("Zoom In Vertical Scale"), this);
	m_actionOptionSensorVerticalZoomIn->setToolTip(tr("Zoom In Vertical Scale"));
	m_actionOptionSensorVerticalZoomIn->setIcon(QIcon(QPixmap(icon_vert_zoom_in_xpm)));
	//m_actionOptionSensorVerticalZoomIn->setCheckable(true);
	connect(m_actionOptionSensorVerticalZoomIn, SIGNAL(triggered()), this, SLOT(actionOptionSensor()));
	
	m_actionOptionSensorVerticalZoomOut = new QAction(tr("Zoom Out Vertical Scale"), this);
	m_actionOptionSensorVerticalZoomOut->setToolTip(tr("Zoom Out Vertical Scale"));
	m_actionOptionSensorVerticalZoomOut->setIcon(QIcon(QPixmap(icon_vert_zoom_out_xpm)));
	//m_actionOptionSensorVerticalZoomOut->setCheckable(true);
	connect(m_actionOptionSensorVerticalZoomOut, SIGNAL(triggered()), this, SLOT(actionOptionSensor()));
	
	m_actionOptionSensorHorizontalZoomIn = new QAction(tr("Zoom In Time Scale"), this);
	m_actionOptionSensorHorizontalZoomIn->setToolTip(tr("Zoom In Time Scale"));
	m_actionOptionSensorHorizontalZoomIn->setIcon(QIcon(QPixmap(icon_horiz_zoom_in_xpm)));
	//m_actionOptionSensorHorizontalZoomIn->setCheckable(true);
	connect(m_actionOptionSensorHorizontalZoomIn, SIGNAL(triggered()), this, SLOT(actionOptionSensor()));
	
	m_actionOptionSensorHorizontalZoomOut = new QAction(tr("Zoom Out Time Scale"), this);
	m_actionOptionSensorHorizontalZoomOut->setToolTip(tr("Zoom Out Time Scale"));
	m_actionOptionSensorHorizontalZoomOut->setIcon(QIcon(QPixmap(icon_horiz_zoom_out_xpm)));
	//m_actionOptionSensorHorizontalZoomOut->setCheckable(true);
	connect(m_actionOptionSensorHorizontalZoomOut, SIGNAL(triggered()), this, SLOT(actionOptionSensor()));
	
	m_actionOptionSensorBack = new QAction(tr("Move Back"), this);
	m_actionOptionSensorBack->setToolTip(tr("Move Back In Time"));
	m_actionOptionSensorBack->setIcon(QIcon(QPixmap(icon_rw_xpm)));
	//m_actionOptionSensorBack->setCheckable(true);
	connect(m_actionOptionSensorBack, SIGNAL(triggered()), this, SLOT(actionOptionSensor()));
	
	m_actionOptionSensorPause = new QAction(tr("Pause Display"), this);
	m_actionOptionSensorPause->setToolTip(tr("Pause Sensor Display"));
	m_actionOptionSensorPause->setIcon(QIcon(QPixmap(icon_pause_xpm)));
	//m_actionOptionSensorPause->setCheckable(true);
	connect(m_actionOptionSensorPause, SIGNAL(triggered()), this, SLOT(actionOptionSensor()));
	
	m_actionOptionSensorResume = new QAction(tr("Start Display"), this);
	m_actionOptionSensorResume->setToolTip(tr("Start Sensor Display"));
	m_actionOptionSensorResume->setIcon(QIcon(QPixmap(icon_play_xpm)));
	//m_actionOptionSensorResume->setCheckable(true);
	connect(m_actionOptionSensorResume, SIGNAL(triggered()), this, SLOT(actionOptionSensor()));
	
	m_actionOptionSensorRecordStart = new QAction(tr("Start Recording"), this);
	m_actionOptionSensorRecordStart->setToolTip(tr("Start Recording Sensor Time Series"));
	m_actionOptionSensorRecordStart->setIcon(QIcon(QPixmap(icon_record_xpm)));
	m_actionOptionSensorRecordStart->setCheckable(true);
	connect(m_actionOptionSensorRecordStart, SIGNAL(triggered()), this, SLOT(actionOptionSensor()));
	
	m_actionOptionSensorRecordStop = new QAction(tr("Stop Recording"), this);
	m_actionOptionSensorRecordStop->setToolTip(tr("Stop Recording Sensor Time Series"));
	m_actionOptionSensorRecordStop->setIcon(QIcon(QPixmap(icon_stop_xpm)));
	m_actionOptionSensorRecordStop->setCheckable(true);
	connect(m_actionOptionSensorRecordStop, SIGNAL(triggered()), this, SLOT(actionOptionSensor()));
	
	m_actionOptionSensorForward = new QAction(tr("Move Forward"), this);
	m_actionOptionSensorForward->setToolTip(tr("Move Forward In Time"));
	m_actionOptionSensorForward->setIcon(QIcon(QPixmap(icon_ff_xpm)));
	//m_actionOptionSensorForward->setCheckable(true);
	connect(m_actionOptionSensorForward, SIGNAL(triggered()), this, SLOT(actionOptionSensor()));
	
	m_actionOptionSensorAbsolute = new QAction(tr("&Absolute sensor values"), this);
	m_actionOptionSensorAbsolute->setToolTip(tr("Absolute sensor values"));
	m_actionOptionSensorAbsolute->setIcon(QIcon(QPixmap(icon_absolute_xpm)));
	m_actionOptionSensorAbsolute->setCheckable(true);
	connect(m_actionOptionSensorAbsolute, SIGNAL(triggered()), this, SLOT(actionOptionSensor()));
	
	m_actionOptionSensorScaled = new QAction(tr("S&caled sensor values"), this);
	m_actionOptionSensorScaled->setToolTip(tr("Scaled sensor values"));
	m_actionOptionSensorScaled->setIcon(QIcon(QPixmap(icon_scaled_xpm)));
	m_actionOptionSensorScaled->setCheckable(true);
	connect(m_actionOptionSensorScaled, SIGNAL(triggered()), this, SLOT(actionOptionSensor()));
	
	m_actionOptionSensorSignificance = new QAction(tr("Show Significance Values"), this);
	m_actionOptionSensorSignificance->setToolTip(tr("Show the computed significance values"));
	m_actionOptionSensorSignificance->setIcon(QIcon(QPixmap(icon_sig_xpm)));
	m_actionOptionSensorSignificance->setCheckable(true);
	connect(m_actionOptionSensorSignificance, SIGNAL(triggered()), this, SLOT(actionOptionSensor()));
	
	

	// Option action for all (Screenshot / Logo)
	m_actionOptionScreenshot = new QAction(tr("Screenshot"), this);
	m_actionOptionScreenshot->setToolTip(tr("Make a screenshot and save as a JPG file in data directory"));
	m_actionOptionScreenshot->setIcon(QIcon(QPixmap(icon_camera_xpm)));
	connect(m_actionOptionScreenshot, SIGNAL(triggered()), this, SLOT(actionOptionScreenshot()));
	
	m_actionOptionLogo = new QAction(tr(""), this);
	m_actionOptionLogo->setToolTip(tr(""));
	//m_actionOptionLogo->setIcon(QIcon(QPixmap(icon_scaled_xpm)));
	connect(m_actionOptionLogo, SIGNAL(triggered()), this, SLOT(actionOptionLogo()));
		
	
	// Help actions
    m_actionHelpAbout = new QAction(tr("&About QCNLive"), this);
	m_actionHelpAbout->setToolTip(tr("About the QCNLive"));
    connect(m_actionHelpAbout, SIGNAL(triggered()), this, SLOT(actionHelp()));

    m_actionHelpAboutQt = new QAction(tr("About &Qt"), this);
	m_actionHelpAboutQt->setToolTip(tr("About the Qt library"));
    connect(m_actionHelpAboutQt, SIGNAL(triggered()), this, SLOT(actionHelp()));
	
	m_actionHelpManual = new QAction(tr("&Manual (PDF) for QCNLive"), this);
	m_actionHelpManual->setToolTip(tr("Download/View Manual (PDF) for QCNLive"));
    connect(m_actionHelpManual, SIGNAL(triggered()), this, SLOT(actionHelp()));

	m_actionHelpWebQCN = new QAction(tr("&QCN Website"), this);
	m_actionHelpWebQCN->setToolTip(tr("Visit the main QCN website"));
    connect(m_actionHelpWebQCN, SIGNAL(triggered()), this, SLOT(actionHelp()));
	
	m_actionHelpWebQCNLive = new QAction(tr("QCN&Live Website"), this);
	m_actionHelpWebQCNLive->setToolTip(tr("Visit the QCNLive website"));
    connect(m_actionHelpWebQCNLive, SIGNAL(triggered()), this, SLOT(actionHelp()));
	
	m_actionHelpWebEarthquakes = new QAction(tr("&Earthquake Information"), this);
	m_actionHelpWebEarthquakes->setToolTip(tr("Visit QCN's website for earthquakes"));
    connect(m_actionHelpWebEarthquakes, SIGNAL(triggered()), this, SLOT(actionHelp()));
	
	m_actionHelpWebLessons = new QAction(tr("Lessons and &Activities"), this);
	m_actionHelpWebLessons->setToolTip(tr("Lessons and Activities website"));
    connect(m_actionHelpWebLessons, SIGNAL(triggered()), this, SLOT(actionHelp()));
	
	m_actionHelpWebRequestSensor = new QAction(tr("&Request a Sensor"), this);
	m_actionHelpWebRequestSensor->setToolTip(tr("Request/Purchase a sensor to use with QCN"));
    connect(m_actionHelpWebRequestSensor, SIGNAL(triggered()), this, SLOT(actionHelp()));
	
	m_actionHelpWebGlossary = new QAction(tr("&Glossary"), this);
	m_actionHelpWebGlossary->setToolTip(tr("Online Glossary"));
    connect(m_actionHelpWebGlossary, SIGNAL(triggered()), this, SLOT(actionHelp()));
}

void MyFrame::createMenus()
{
    // Make a menubar
	
	// File
    m_menuFile = menuBar()->addMenu(tr("&File"));
    m_menuFile->addAction(m_actionFileDialogSettings);
    m_menuFile->addAction(m_actionFileMakeQuake);
    m_menuFile->addAction(m_actionFileExit);
	
	// View
	m_menuView = menuBar()->addMenu(tr("&View"));
	m_menuView->addAction(m_actionViewEarth);
	m_menuView->addAction(m_actionViewSensor2D);
	m_menuView->addAction(m_actionViewSensor3D);
	m_menuView->addAction(m_actionViewCube);
	//m_menuView->addAction(m_actionViewGame);
	
	m_menuView->addSeparator();
	m_menuView->addAction(m_actionViewFullScreen);
	
	// Options - these change based on the View
	m_menuOptions = menuBar()->addMenu(tr("&Options"));
	

	// Help
	m_menuHelp = menuBar()->addMenu(tr("&Help"));
	m_menuHelp->addAction(m_actionHelpManual);
	m_menuHelp->addAction(m_actionHelpWebQCN);
	m_menuHelp->addAction(m_actionHelpWebQCNLive);
	m_menuHelp->addAction(m_actionHelpWebEarthquakes);
	m_menuHelp->addAction(m_actionHelpWebLessons);
	m_menuHelp->addAction(m_actionHelpWebRequestSensor);
	m_menuHelp->addAction(m_actionHelpWebGlossary);
#ifndef __APPLE_CC__
	m_menuHelp->addSeparator();  // on Mac the Help/About goes on the left-most system menu, so don't need a separator
#endif
	m_menuHelp->addAction(m_actionHelpAbout);
	m_menuHelp->addAction(m_actionHelpAboutQt);	
}

QSize MyFrame::getSize()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("QCNLive"),
                                         tr("Enter pixmap size:"),
                                         QLineEdit::Normal,
                                         tr("%1 x %2").arg(m_glWidget->width())
										 .arg(m_glWidget->height()),
                                         &ok);
    if (!ok)
        return QSize();
	
    QRegExp regExp(tr("([0-9]+) *x *([0-9]+)"));
    if (regExp.exactMatch(text)) {
        int width = regExp.cap(1).toInt();
        int height = regExp.cap(2).toInt();
        if (width > 0 && width < 2048 && height > 0 && height < 2048)
            return QSize(width, height);
    }
	
    return m_glWidget->size();
}

void MyFrame::closeEvent(QCloseEvent* pqc)
{
	QSettings settings(SET_COMPANY, SET_APP);
	settings.setValue(QT_WINDOW_SETTINGS_GEOMETRY, saveGeometry());
	settings.setValue(QT_WINDOW_SETTINGS_STATE, saveState());
	QWidget::closeEvent(pqc);
}

/*
void MyFrame::moveEvent (QMoveEvent* pme)
{
}

void MyFrame::resizeEvent(QResizeEvent* prs)
{
}
*/

void MyFrame::EarthRotate(bool bAuto)
{
	if (!qcn_graphics::earth.IsShown()) return;  // only matters if we're on the earth view!
	// see if it's rotating and we want to stop, or it's not rotating and we want it to start
	if ( (!bAuto && qcn_graphics::earth.IsAutoRotate())
		|| (bAuto && ! qcn_graphics::earth.IsAutoRotate()))  {
		m_bEarthRotate = bAuto;
		Toggle(m_actionOptionEarthRotateOff, !bAuto);
		Toggle(m_actionOptionEarthRotateOn, bAuto);
		qcn_graphics::earth.AutoRotate(bAuto);
	}
}

void MyFrame::ToggleStartStop(bool bStart)
{
	Toggle(m_actionOptionSensorResume, bStart);
	Toggle(m_actionOptionSensorPause, !bStart);
	if (bStart && qcn_graphics::TimeWindowIsStopped()) {
		qcn_graphics::TimeWindowStart();
	}
	else if (!bStart && !qcn_graphics::TimeWindowIsStopped()) {
		qcn_graphics::TimeWindowStop();
	}
}

void MyFrame::createToolbar()
{	
	m_toolBarView = new QToolBar(tr("View"), this);	
	if (m_toolBarView) { // && m_dockWidgetView) {
		m_toolBarView->setObjectName("QCN_TB_VIEW");
		m_toolBarView->setIconSize(QSize(ICON_SIZE, ICON_SIZE));		
		ToolBarView();
		addToolBar(Qt::TopToolBarArea, m_toolBarView);
	}
	
	m_toolBarOption = new QToolBar(tr("Options"), this);	
	if (m_toolBarOption) {
		m_toolBarOption->setObjectName("QCN_TB_OPTION");
		m_toolBarOption->setIconSize(QSize(ICON_SIZE, ICON_SIZE));		
		addToolBar(Qt::TopToolBarArea, m_toolBarOption);
		ToolBarEarth(true);
	} 
	
#ifdef __APPLE_CC__
	setUnifiedTitleAndToolBarOnMac(false);
#endif
}

void MyFrame::fullScreenToggle(bool bFull)
{
	if (qcn_graphics::g_bFullScreen && !bFull) {
		qcn_graphics::g_bFullScreen = false;
		m_toolBarOption->show();
		m_toolBarView->show();
		this->showNormal();
	}
	else {
		qcn_graphics::g_bFullScreen = true;
		m_toolBarOption->hide();
		m_toolBarView->hide();
		this->showFullScreen();
	}
}


void MyFrame::actionView()
{
	// get item from event do appropriate action (boinc_key_press!)
	// todo: hook up the other toolbars
	if (m_actionCurrent) Toggle(m_actionCurrent, false, true);
	
	// figure out who called this, i.e. get the pointer of the QAction* from trigger and compare
	QAction *pAction = qobject_cast<QAction*>(QObject::sender());
	bool bChanged = false;
	if (pAction == m_actionViewSensor2D)
	{
		// note only redraw sensor toolbar if not coming from a sensor view already
		//if (qcn_graphics::g_eView != VIEW_PLOT_2D && cn_graphics::g_eView != VIEW_PLOT_3D) ToolBarSensor(evt.GetId());
		ToolBarSensor2D();
		setTimeSliderValue(100);
		m_sliderTime->show();
		qcn_graphics::g_eView = VIEW_PLOT_2D;
		bChanged = true;
	}
	else if (pAction == m_actionViewSensor3D)
	{
		// note only redraw sensor toolbar if not coming from a sensor view already
		//if (qcn_graphics::g_eView != VIEW_PLOT_2D && cn_graphics::g_eView != VIEW_PLOT_3D) ToolBarSensor(evt.GetId());
		ToolBarSensor3D();
		setTimeSliderValue(100);
		m_sliderTime->show();
		qcn_graphics::g_eView = VIEW_PLOT_3D;
		bChanged = true;
	}
	else if (pAction == m_actionViewCube)
	{
		ToolBarCube();
		m_sliderTime->hide();
		qcn_graphics::g_eView = VIEW_CUBE;
		bChanged = true;
	}
	else if (pAction == m_actionViewGame)
	{
		/*
		//ToolBarCube();
		m_sliderTime->hide();
		qcn_graphics::g_eView = VIEW_GAME;
		bChanged = true;
		fullScreenToggle(true);
		*/
	}
	else if (pAction == m_actionViewEarth) 
	{
		m_sliderTime->hide();
		ToolBarEarth();
		if (m_bEarthDay) {
			qcn_graphics::g_eView = VIEW_EARTH_DAY;
			qcn_graphics::earth.SetMapCombined();
		}
		else {
			qcn_graphics::g_eView = VIEW_EARTH_NIGHT;
			qcn_graphics::earth.SetMapNight();
		}
		bChanged = true;
	} 
	else if (pAction == m_actionViewFullScreen) {
		fullScreenToggle();
	}
	
	m_actionCurrent = pAction;
    qcn_graphics::ResetPlotArray();
}


void MyFrame::actionOptionScreenshot()
{
	m_pMyApp->processEvents();
	const char* strSS = qcn_graphics::ScreenshotJPG();
	if (strSS && strSS[0] != 0x00) {
		// we have a valid screenshot filename
		char* statmsg = new char[_MAX_PATH];
		sprintf(statmsg, "Screenshot file saved to %s", strSS);
		statusBar()->showMessage(tr(statmsg), 5000);
		delete [] statmsg;
	}
}

void MyFrame::actionOptionLogo()
{
#ifdef QCNLIVE_DEMO
	qcn_graphics::demo_switch_ad();
#endif
}

void MyFrame::fileMakeQuake()
{
	// if we're in a make-quake session and hit this menu, prompt to cancel
	if (qcn_graphics::g_MakeQuake.bActive) {
		if (QMessageBox::question(this, tr("Cancel Quake?"), tr("Cancel the current 'quake' and start a new session?"), 
				QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes) {
			m_pMyApp->stopQuakeTimer(); // stop quake timer
			qcn_graphics::g_MakeQuake.clear(); // clear this session and proceed to the dialog below
			statusBar()->showMessage(tr("Earthquake monitoring cancelled"), 5000);
		}
		else {
			return; // they don't want to quit
		}
	}
	
	CDialogMakeQuake* pcmq = new CDialogMakeQuake(m_pMyApp->getMakeQuakeTime(), 
		m_pMyApp->getMakeQuakeCountdown(), 
		m_pMyApp->getMakeQuakePrinterString(), this, Qt::Dialog);
	if (pcmq) {
		pcmq->exec();
	    if (pcmq->start()) { // data was validated and set in shared memory (sm)
			m_pMyApp->setMakeQuakeTime(pcmq->getMakeQuakeTime());
			m_pMyApp->setMakeQuakeCountdown(pcmq->getMakeQuakeCountdown());
			qcn_graphics::g_MakeQuake.iTime = pcmq->getMakeQuakeTime();
			qcn_graphics::g_MakeQuake.iCountdown = pcmq->getMakeQuakeCountdown();
			
			m_pMyApp->setMakeQuakePrinterInfo(pcmq->getMakeQuakePrinterInfo());
			m_pMyApp->setMakeQuakePrinterString(pcmq->getMakeQuakePrinterString());
			
			pcmq->getUserString(qcn_graphics::g_MakeQuake.strName);
			
			// setup proper view for 2d sensor, i.e. 10sec or 60 sec window width
			m_actionViewSensor2D->activate(QAction::Trigger);
			if (qcn_graphics::g_MakeQuake.iTime <= 10)
				qcn_graphics::SetTimeWindowWidthInt(10);
			else 
				qcn_graphics::SetTimeWindowWidthInt(60);
			// set to absolute
#ifndef QCN_RAW_DATA
			m_actionOptionSensorAbsolute->activate(QAction::Trigger); // set to absolute scale
#endif
			setTimeSliderValue(100);
			
			SetToggleSensor(false);
			
			statusBar()->showMessage(tr("Monitoring for earthquakes..."));
			m_pMyApp->startQuakeTimer(); // starts up quake timer
			qcn_graphics::g_MakeQuake.bActive = true; // this is a flag as well as mutex if we're in other threads, but graphics thread is not separate from qcnlive
			qcn_graphics::g_MakeQuake.bDisplay = true; // don't for get the flag for the display -- so countdown/monitor msg won't show on our screenshot
			qcn_graphics::g_MakeQuake.bReceived = false; // this gets set to true in qcn_graphics thread (qcn_2dplot.cpp) when bActive && !bDisplay
		}
	    delete pcmq;
	}
}

void MyFrame::fileDialogSettings()
{
	CDialogSettings* pcds = new CDialogSettings(this, Qt::Dialog);
	if (pcds) {
		int myOldSensor = sm->iMySensor;
		pcds->exec();
	    if (pcds->saved()) { // data was validated and set in shared memory (sm)
			m_pMyApp->set_qcnlive_prefs(); // saved in KillMainThread 
			// probably have to kill & restart the main thread too?
			if (m_pMyApp && myOldSensor != sm->iMySensor) {  // we changed sensors, have to restart main thread?
				// put up a message box to quit and restart
				if (QMessageBox::information(this, tr("Restart QCNLive?"),
						tr("You have changed your preferred sensor.\n\nPlease restart to use your new preferred USB sensor choice.\n\nClick 'OK' to quit now.\nClick 'Cancel' to continue QCNLive."), 
							QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok) == QMessageBox::Ok)
					close();
			}
			// enable/disable significance button
			m_actionOptionSensorSignificance->setEnabled(sm->iMyAxisSingle == 0);
			if (sm->iMyAxisSingle) {
				qcn_2dplot::g_iShowSig = 0;
				m_actionOptionSensorSignificance->setChecked(false);
			}
		}
	    delete pcds;
	}
}

void MyFrame::actionOptionEarth()
{
	// figure out who called this, i.e. get the pointer of the QAction* from trigger and compare
	QAction *pAction = qobject_cast<QAction*>(QObject::sender());
	if (pAction == m_actionOptionEarthDay) {
		m_bEarthDay = true;
		qcn_graphics::earth.SetMapCombined(); 	}
	else if (pAction == m_actionOptionEarthNight) {
		m_bEarthDay = false;
		qcn_graphics::earth.SetMapNight();	}
	else if (pAction == m_actionOptionEarthRotateOn) {
			EarthRotate(true);	
	}
	else if (pAction == m_actionOptionEarthRotateOff) {
			EarthRotate(false);	
	}
	else if (pAction == m_actionOptionEarthUSGS) {
		statusBar()->showMessage(tr("Opening USGS website for selected earthquake"), 5000);
		qcn_graphics::earth.checkURLClick(true);	}
	else if (pAction == m_actionOptionEarthQuakelist) {
			if (m_pMyApp) m_pMyApp->GetLatestQuakeList();
	}
	SetToggleEarth();
}

void MyFrame::actionHelp()
{
	QAction *pAction = qobject_cast<QAction*>(QObject::sender());
	std::string strURL("");
	if (pAction == m_actionHelpManual) {
		strURL = "http://quakecatcher.net/downloads/QCNLive_User_Manual.pdf";
	}
	else if (pAction == m_actionHelpWebQCN) {
		strURL = "http://quakecatcher.net";
	}
	else if (pAction == m_actionHelpWebQCNLive) {
		strURL = "http://quakecatcher.net/learning/software.php";
	}
	else if (pAction == m_actionHelpWebEarthquakes) {
		strURL = "http://quakecatcher.net/learning/earthquakes.php";
	}
	else if (pAction == m_actionHelpWebLessons) {
		strURL = "http://quakecatcher.net/learning/lessons.php";
	}
	else if (pAction == m_actionHelpWebRequestSensor) {
		strURL = "http://quakecatcher.net/learning/requests.php";
	}
	else if (pAction == m_actionHelpWebGlossary) {
		strURL = "http://quakecatcher.net/learning/glossary.php";
	}
	else if (pAction == m_actionHelpAbout) {
		//MyAboutBox myabout(this);
		//myabout.exec();	
#ifdef QCN_RAW_DATA
		QString strVer(tr("About QCNLive (Raw Data) version "));
#else
		QString strVer(tr("About QCNLive version "));
#endif
		QString strMsg(tr("<b>QCNLive</b> is provided free of charge, "
			"<BR>no guarantees or warrantees or liabilities,<BR>"
			"(i.e. use at your own risk),<BR>all rights reserved, by the "
			"<BR> Quake-Catcher Network Project <BR><BR>"
			"<A HREF='http://quakecatcher.net'>http://quakecatcher.net</A>"
		    "<BR><BR><A HREF='http://www.iris.edu/'>Incorporated Research Institutions for Seismology (IRIS)</A>"
                        "<BR><BR>"
			"<A HREF='http://www.scec.org/'>Southern California Earthquake Center (SCEC)</A>"
			"<BR><BR>(c) 2008-2016 IRIS")
		);
		strVer += QCN_VERSION_STRING;
		QMessageBox::about(this, strVer, 
#ifdef __APPLE_CC__  // title string on window is missing on Mac
			strVer + "<BR><BR>" + strMsg
#else   // title string on window is OK, so just need the regular message
			strMsg
#endif
			);
	}
	else if (pAction == m_actionHelpAboutQt) {
		QMessageBox::aboutQt(this, tr("QCNLive Is Built With Qt"));
	}
	if (!strURL.empty())  qcn_util::launchURL(strURL.c_str());
}

void MyFrame::actionOptionSensor() 
{
	// get item from event do appropriate action (boinc_key_press!)
	QAction *pAction = qobject_cast<QAction*>(QObject::sender());
	if (pAction == m_actionOptionSensorBack) {
		if (! qcn_graphics::TimeWindowIsStopped()) {
			m_iSensorAction = 1;
			qcn_graphics::TimeWindowStop(); 
		}
		setTimeSliderValue(qcn_graphics::TimeWindowBack());
	}
	else if (pAction == m_actionOptionSensorPause) {
		if (! qcn_graphics::TimeWindowIsStopped()) {
			m_iSensorAction = 1;
			qcn_graphics::TimeWindowStop(); 
		}
	}
	else if (pAction == m_actionOptionSensorResume) {
		if (qcn_graphics::TimeWindowIsStopped()) {
			m_iSensorAction = 0;
			qcn_graphics::TimeWindowStart();
			setTimeSliderValue(100);
		}
	}
	else if (pAction == m_actionOptionSensorRecordStart || pAction == m_actionOptionSensorRecordStop) {
		// trying to change recording state -- handled by the Toggle i.e. the appropriate button is enabled and the other disabled
		if (qcn_graphics::TimeWindowIsStopped()) {
			m_iSensorAction = 0;
			qcn_graphics::TimeWindowStart();
		}
		if (sm->bSensorFound) {
			if (sm->bRecording) { // we're turning off recording
				statusBar()->showMessage(tr("Recording stopped"), 1000);
			}
			else { // we're starting recording
				statusBar()->showMessage(tr("Recording..."), 0);
			}
			
			// flip the state
			sm->bRecording = !sm->bRecording;
		}
	}
	else if (pAction == m_actionOptionSensorForward) {
		if (! qcn_graphics::TimeWindowIsStopped()) {
			m_iSensorAction = 1;
			qcn_graphics::TimeWindowStop(); 
		}
		setTimeSliderValue(qcn_graphics::TimeWindowForward());
	}
	else if (pAction == m_actionOptionSensorAbsolute || pAction == m_actionOptionSensorScaled) {
		if (qcn_graphics::g_eView == VIEW_PLOT_2D) {
			m_bSensorAbsolute2D = (bool)(pAction == m_actionOptionSensorAbsolute);
		}
		else {
			m_bSensorAbsolute3D = (bool)(pAction == m_actionOptionSensorAbsolute);
		}
	}
	else if (pAction == m_actionOptionSensorHorizontalZoomOut) {
		qcn_graphics::SetTimeWindowWidth(true);
		setTimeSliderValue(100);
	}
	else if (pAction == m_actionOptionSensorHorizontalZoomIn) {
		qcn_graphics::SetTimeWindowWidth(false);
		setTimeSliderValue(100);
	}
	else if (pAction == m_actionOptionSensorVerticalZoomOut) {
		qcn_2dplot::SensorDataZoomOut();
	}
	else if (pAction == m_actionOptionSensorVerticalZoomIn) {
		qcn_2dplot::SensorDataZoomIn();
	}
	else if (pAction == m_actionOptionSensorVerticalZoomAuto) {
		qcn_2dplot::SensorDataZoomAuto();
	}
	else if (pAction == m_actionOptionSensorSignificance) { // toggle significance plot
		if (qcn_2dplot::IsSigPlot()) qcn_2dplot::ShowSigPlot(false);
		else qcn_2dplot::ShowSigPlot(true);
	}
	
	SetToggleSensor((bool)(qcn_graphics::g_eView == VIEW_PLOT_3D));
}


void MyFrame::ToolBarView()
{
	if (!m_toolBarView) return; // null toolbar?  should have already been created in Init()
	// just set the actions which were already created and map to the menu
	m_toolBarView->addAction(m_actionViewEarth); 
	m_toolBarView->addAction(m_actionViewSensor2D); 
	m_toolBarView->addAction(m_actionViewSensor3D); 
	m_toolBarView->addAction(m_actionViewCube); 
}

void MyFrame::RemoveCurrentTools()
{   
    m_toolBarOption->clear();
	m_menuOptions->clear();
	return;   // simpler!!!
	
	// remove the current "Option" tools if any 
	// it depends on what the current view is so don't switch the view before calling this function!
	m_menuOptions->clear(); // can just clear the options menu
	
	// first get rid of any separators
	/*
	vector<QAction*>::iterator itAction = m_vqaSeparator.begin();
	while (itAction != m_vqaSeparator.end()) {
		if (!*itAction) m_toolBarOption->removeAction(*itAction); // check for null pointer
		itAction++;
	}
	m_vqaSeparator.clear(); // clear the separator vector
	*/
	
	switch (qcn_graphics::g_eView)
	{
		case VIEW_EARTH_DAY:
		case VIEW_EARTH_NIGHT:
			m_toolBarOption->removeAction(m_actionOptionEarthDay);
			m_toolBarOption->removeAction(m_actionOptionEarthNight);
			m_toolBarOption->removeAction(m_actionOptionEarthRotateOn);
			m_toolBarOption->removeAction(m_actionOptionEarthRotateOff);
			m_toolBarOption->removeAction(m_actionOptionEarthUSGS);
			m_toolBarOption->removeAction(m_actionOptionEarthQuakelist);
			break;
		case VIEW_PLOT_2D:
			m_toolBarOption->removeAction(m_actionOptionSensorVerticalZoomAuto);
			m_toolBarOption->removeAction(m_actionOptionSensorVerticalZoomIn);
			m_toolBarOption->removeAction(m_actionOptionSensorVerticalZoomOut);
			m_toolBarOption->removeAction(m_actionOptionSensorHorizontalZoomIn);
			m_toolBarOption->removeAction(m_actionOptionSensorHorizontalZoomOut);
			m_toolBarOption->removeAction(m_actionOptionSensorBack);
			m_toolBarOption->removeAction(m_actionOptionSensorPause);
			m_toolBarOption->removeAction(m_actionOptionSensorResume);
			m_toolBarOption->removeAction(m_actionOptionSensorRecordStart);
			m_toolBarOption->removeAction(m_actionOptionSensorRecordStop);
			m_toolBarOption->removeAction(m_actionOptionSensorForward);
			m_toolBarOption->removeAction(m_actionOptionSensorAbsolute);
			m_toolBarOption->removeAction(m_actionOptionSensorScaled);
			m_toolBarOption->removeAction(m_actionOptionSensorSignificance);
			break;
		case VIEW_PLOT_3D:
			m_toolBarOption->removeAction(m_actionOptionSensorHorizontalZoomIn);
			m_toolBarOption->removeAction(m_actionOptionSensorHorizontalZoomOut);
			m_toolBarOption->removeAction(m_actionOptionSensorBack);
			m_toolBarOption->removeAction(m_actionOptionSensorPause);
			m_toolBarOption->removeAction(m_actionOptionSensorResume);
			m_toolBarOption->removeAction(m_actionOptionSensorRecordStart);
			m_toolBarOption->removeAction(m_actionOptionSensorRecordStop);
			m_toolBarOption->removeAction(m_actionOptionSensorForward);
			m_toolBarOption->removeAction(m_actionOptionSensorAbsolute);
			m_toolBarOption->removeAction(m_actionOptionSensorScaled);
			break;
		default:
			break;
	}
	
	// we always need to get rid of the screenshot icon
	m_toolBarOption->removeAction(m_actionOptionScreenshot);	
}

void MyFrame::AddToolBarSeparator()
{
	//QAction* pqa = 
	m_toolBarOption->addSeparator();
	m_menuOptions->addSeparator();
	/*
	 if (pqa) {
		m_vqaSeparator.push_back(pqa);
	}
	*/
}

// toggle on off both the menu & the toolbar
void MyFrame::Toggle(QAction* pqa, const bool bCheck, const bool bEnable)
{
	pqa->setChecked(bCheck);
	pqa->setEnabled(bEnable);
}

void MyFrame::SetToggleSensor(const bool b3D)
{
	// m_iSensorAction of 0 means we're playing/recording; if 1 it means we're moving back or forward or paused	
	Toggle(m_actionOptionSensorResume, (bool)(sm->bRecording && m_iSensorAction == 0));
	Toggle(m_actionOptionSensorPause, (bool)(sm->bRecording && m_iSensorAction == 1));

	if (b3D) { // force to be scaled in the 3D view
		m_bSensorAbsolute3D = false;
		if (!qcn_graphics::IsScaled()) qcn_graphics::SetScaled(true);
		Toggle(m_actionOptionSensorAbsolute, m_bSensorAbsolute3D, m_bSensorAbsolute3D);
		Toggle(m_actionOptionSensorScaled, !m_bSensorAbsolute3D, !m_bSensorAbsolute3D);
	}
	else { 
#ifdef QCN_RAW_DATA
		m_bSensorAbsolute2D = false;
		if (!qcn_graphics::IsScaled()) qcn_graphics::SetScaled(true);
		Toggle(m_actionOptionSensorAbsolute, m_bSensorAbsolute2D, m_bSensorAbsolute2D);
		Toggle(m_actionOptionSensorScaled, !m_bSensorAbsolute2D, !m_bSensorAbsolute2D);
#else
		if (m_bSensorAbsolute2D) {
			if (qcn_graphics::IsScaled()) qcn_graphics::SetScaled(false);
		}
		else {
			if (!qcn_graphics::IsScaled()) qcn_graphics::SetScaled(true);
		}
		Toggle(m_actionOptionSensorAbsolute, m_bSensorAbsolute2D);
		Toggle(m_actionOptionSensorScaled, !m_bSensorAbsolute2D);		
#endif
	}
	
	// recording state
	Toggle(m_actionOptionSensorRecordStart, sm->bRecording, !sm->bRecording);
	Toggle(m_actionOptionSensorRecordStop, !sm->bRecording, sm->bRecording);
}

void MyFrame::SetToggleEarth()
{
	Toggle(m_actionOptionEarthDay, m_bEarthDay);
	Toggle(m_actionOptionEarthNight, !m_bEarthDay);
	Toggle(m_actionOptionEarthRotateOff, !m_bEarthRotate);
	Toggle(m_actionOptionEarthRotateOn, m_bEarthRotate);
}


void MyFrame::SensorNavButtons()
{  // common buttons/menu choices for 2 or 3-d sensor plot view
	
	m_toolBarOption->addAction(m_actionOptionSensorHorizontalZoomIn);
	m_menuOptions->addAction(m_actionOptionSensorHorizontalZoomIn);

	m_toolBarOption->addAction(m_actionOptionSensorHorizontalZoomOut);
	m_menuOptions->addAction(m_actionOptionSensorHorizontalZoomOut);
	
	AddToolBarSeparator(); // use our function to keep track of separators so they can be easily removed
	
	m_toolBarOption->addAction(m_actionOptionSensorBack);
	m_menuOptions->addAction(m_actionOptionSensorBack);
	
	m_toolBarOption->addAction(m_actionOptionSensorPause);
	m_menuOptions->addAction(m_actionOptionSensorPause);
	
	m_toolBarOption->addAction(m_actionOptionSensorResume);
	m_menuOptions->addAction(m_actionOptionSensorResume);
	
	m_toolBarOption->addAction(m_actionOptionSensorRecordStart);
	m_menuOptions->addAction(m_actionOptionSensorRecordStart);
	
	m_toolBarOption->addAction(m_actionOptionSensorRecordStop);
	m_menuOptions->addAction(m_actionOptionSensorRecordStop);
	
	m_toolBarOption->addAction(m_actionOptionSensorForward);
	m_menuOptions->addAction(m_actionOptionSensorForward);
	
}

void MyFrame::AddScreenshotItem()
{
	AddToolBarSeparator(); // use our function to keep track of separators so they can be easily removed

	m_toolBarOption->addAction(m_actionOptionScreenshot);
	m_menuOptions->addAction(m_actionOptionScreenshot);
	
#ifdef QCNLIVE_DEMO  
	// add a function to cycle through ad images i.e. science museum logos
	m_menuOptions->addAction(m_actionOptionLogo);
#endif
	
}

void MyFrame::ToolBarEarth(bool bFirst)
{
    if (!m_toolBarOption) return; // null toolbar?
		
    if (!bFirst)
		RemoveCurrentTools();
	
	//m_toolBarOption->addSeparator();	 // don't need to use our function as we want to keep this separator permanently
	
	// menu & toolbar options were cleared, so have to add actions again
	
	// day & night view
	m_toolBarOption->addAction(m_actionOptionEarthDay); 
	m_menuOptions->addAction(m_actionOptionEarthDay);
	
	m_toolBarOption->addAction(m_actionOptionEarthNight); 
	m_menuOptions->addAction(m_actionOptionEarthNight);
	
	AddToolBarSeparator(); // use our function to keep track of separators so they can be easily removed	
	
	// rotate on/off
	m_toolBarOption->addAction(m_actionOptionEarthRotateOn); 
	m_menuOptions->addAction(m_actionOptionEarthRotateOn);
	
	m_toolBarOption->addAction(m_actionOptionEarthRotateOff); 
	m_menuOptions->addAction(m_actionOptionEarthRotateOff);
	
	AddToolBarSeparator(); // use our function to keep track of separators so they can be easily removed
	
	m_toolBarOption->addAction(m_actionOptionEarthQuakelist); 
	m_menuOptions->addAction(m_actionOptionEarthQuakelist);
	
	m_toolBarOption->addAction(m_actionOptionEarthUSGS); 
	m_menuOptions->addAction(m_actionOptionEarthUSGS);
	
    AddScreenshotItem();

	if (bFirst) 
		Toggle(m_actionViewEarth, true, true);
	
    SetToggleEarth();
}

void MyFrame::ToolBarSensor2D()
{
    if (!m_toolBarOption) return; // null toolbar?
    RemoveCurrentTools();

	//m_toolBarOption->addSeparator();	 // don't need to use our function as we want to keep this separator permanently

	m_toolBarOption->addAction(m_actionOptionSensorVerticalZoomAuto);
	m_menuOptions->addAction(m_actionOptionSensorVerticalZoomAuto);

	m_toolBarOption->addAction(m_actionOptionSensorVerticalZoomIn);
	m_menuOptions->addAction(m_actionOptionSensorVerticalZoomIn);

	m_toolBarOption->addAction(m_actionOptionSensorVerticalZoomOut);
	m_menuOptions->addAction(m_actionOptionSensorVerticalZoomOut);

		
    // scrollbar for back & forth time
	/*
	if (scrollBar2D) {
    	 AddToolBarSeparator(); // use our function to keep track of separators so they can be easily removed
		toolBar->AddControl(scrollBar2D);
	}
	*/
	
	SensorNavButtons();
	
	AddToolBarSeparator(); // use our function to keep track of separators so they can be easily removed
	
	m_toolBarOption->addAction(m_actionOptionSensorAbsolute);
	m_menuOptions->addAction(m_actionOptionSensorAbsolute);
	
	m_toolBarOption->addAction(m_actionOptionSensorScaled);
	m_menuOptions->addAction(m_actionOptionSensorScaled);

	AddToolBarSeparator(); // use our function to keep track of separators so they can be easily removed
	
	m_toolBarOption->addAction(m_actionOptionSensorSignificance);
	m_menuOptions->addAction(m_actionOptionSensorSignificance);
	m_actionOptionSensorSignificance->setEnabled(sm->iMyAxisSingle == 0); // set enabled based on single axis shown

    AddScreenshotItem();
	
    SetToggleSensor(false);  // put this after realize() because we may enable/disable tools
}

void MyFrame::ToolBarSensor3D()
{
    if (!m_toolBarOption) return; // null toolbar?
    RemoveCurrentTools();

	//m_toolBarOption->addSeparator();	 // don't need to use our function as we want to keep this separator permanently

	SensorNavButtons();

	AddToolBarSeparator(); // use our function to keep track of separators so they can be easily removed
	
	m_toolBarOption->addAction(m_actionOptionSensorAbsolute);
	m_menuOptions->addAction(m_actionOptionSensorAbsolute);
	
	m_toolBarOption->addAction(m_actionOptionSensorScaled);
	m_menuOptions->addAction(m_actionOptionSensorScaled);
	
    AddScreenshotItem();
	
	//	qcn_graphics::g_eView = iView;
    SetToggleSensor(true);  // put this after realize() because we may enable/disable tools
}

void MyFrame::ToolBarCube()
{
    if (!m_toolBarOption) return; // null toolbar?
    RemoveCurrentTools();
    AddScreenshotItem();
}

