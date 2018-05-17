#ifndef _MYFRAME_H_
#define _MYFRAME_H_

#include "qcnqt.h"
#include "qcnopenglwidget.h"

#include <QApplication>
#include <QMainWindow>
#include <QSplashScreen>
#include <QTimer>
#include <QSlider>
#include <QScrollArea>
#include <QLabel>
#include <QDockWidget>

class QCNGLWidget;
class MyApp;

/*
// simple about box class so I can trap clicks on the QCN URL
class MyAboutBox : public QMessageBox
{
private:
	Q_OBJECT 
	
public:
	MyAboutBox(QMainWindow *pmw);
	
protected:
	void mousePressEvent(QMouseEvent *event);
	
};
*/

class MyFrame : public QMainWindow
{
private:
    Q_OBJECT

    QWidget* m_centralWidget;
    QScrollArea* m_glWidgetArea;
    //QScrollArea* pixmapLabelArea;
    QCNGLWidget* m_glWidget;
	//QStatusBar* m_statusbar;
	QDockWidget* m_dockWidgetView;
	QDockWidget* m_dockWidgetOption;
	QToolBar* m_toolBarView;
	QToolBar* m_toolBarOption;
	QMenuBar* m_menuBar;
	QSlider* m_sliderTime;

	// menu objects
	QMenu* m_menuFile;
	QMenu* m_menuView;
	QMenu* m_menuOptions;
	QMenu* m_menuHelp;
	
	// actions for menu & toggle buttons
    QAction* m_actionFileExit;
	QAction* m_actionFileDialogSettings;	
	QAction* m_actionFileMakeQuake;
	
	QAction* m_actionViewEarth;
	QAction* m_actionViewSensor2D;
	QAction* m_actionViewSensor3D;
	QAction* m_actionViewCube;
	QAction* m_actionViewGame;
	QAction* m_actionViewFullScreen;
	
	QAction* m_actionOptionEarthDay;
	QAction* m_actionOptionEarthNight;
	QAction* m_actionOptionEarthRotateOn;
	QAction* m_actionOptionEarthRotateOff;
	QAction* m_actionOptionEarthUSGS;
	QAction* m_actionOptionEarthQuakelist;

	//QAction* m_actionOptionSensor01;
	//QAction* m_actionOptionSensor10;
	//QAction* m_actionOptionSensor60;
	QAction* m_actionOptionSensorVerticalZoomAuto;
	QAction* m_actionOptionSensorVerticalZoomIn;
	QAction* m_actionOptionSensorVerticalZoomOut;
	QAction* m_actionOptionSensorHorizontalZoomIn;
	QAction* m_actionOptionSensorHorizontalZoomOut;
	QAction* m_actionOptionSensorBack;
	QAction* m_actionOptionSensorPause;
	QAction* m_actionOptionSensorResume;
	QAction* m_actionOptionSensorRecordStart;
	QAction* m_actionOptionSensorRecordStop;
	QAction* m_actionOptionSensorForward;
	QAction* m_actionOptionSensorAbsolute;
	QAction* m_actionOptionSensorScaled;
	QAction* m_actionOptionSensorSignificance;
	//QAction* m_actionOptionSensorScrollbar;

	
    QAction* m_actionHelpAbout;
    QAction* m_actionHelpAboutQt;
	QAction* m_actionHelpManual;
	QAction* m_actionHelpWebQCN;
	QAction* m_actionHelpWebQCNLive;
	QAction* m_actionHelpWebEarthquakes;
	QAction* m_actionHelpWebLessons;
	QAction* m_actionHelpWebRequestSensor;
	QAction* m_actionHelpWebGlossary;
	
	QAction* m_actionOptionScreenshot;
	QAction* m_actionOptionLogo;
	
	QAction* m_actionCurrent; // save the current action
	
	// pointer to the base app instance
    MyApp* m_pMyApp;
	
    bool m_bEarthDay;
    bool m_bEarthRotate;
    long m_iSensorAction;
	bool m_bSensorAbsolute2D;
	bool m_bSensorAbsolute3D;

	QToolBar* m_ptbBase;
	
	//std::vector<QAction*> m_vqaSeparator; // a vector of separators so we can remove them as needed i.e. when redrawing toolbars
	void AddToolBarSeparator();  // function to keep track of separators for easy removal
	
public:
	MyFrame(MyApp* papp);

	bool Init();
    void setStartupView(int iView = 0);
	void EarthRotate(bool bAuto = true);
	void ToggleStartStop(bool bStart);

        int getTimeSliderValue();
        void setTimeSliderValue(const int iPos);

		void updateGLWidget();
	
	void fullScreenToggle(bool bFull = false);

	
private slots:
	void fileDialogSettings();
	void fileMakeQuake();
	void actionView();
	
	void actionOptionEarth();
	void actionOptionSensor();	
	void actionOptionScreenshot();
	void actionOptionLogo();

        void actionHelp();

        void slotTimePosition(int iPos);  // time slider slot
			
signals:
        void signalTimePosition(int iPos); // time slider signal
    
private:
	// inherited events
    void closeEvent(QCloseEvent* pqc);
	//void resizeEvent(QResizeEvent* prs);
	//void moveEvent (QMoveEvent* pme);

	// utility functions for toolbars menus etc
    void createActions();
    void createMenus();
	void createToolbar();

	//void setPixmap(const QPixmap &pixmap);
    QSize getSize();

    void ToolBarView();
	void Toggle(QAction* pqa, const bool bCheck = true, const bool bEnable = true);
    void ToolBarEarth(bool bFirst = false);
    void ToolBarSensor2D();
    void ToolBarSensor3D();
    void ToolBarCube();
    void AddScreenshotItem();
    void SetToggleEarth();
    void SetToggleSensor(const bool b3D);
	void SensorNavButtons();
    void RemoveCurrentTools();

};

#endif // _MYFRAME_H_
