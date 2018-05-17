/*
 *  qcnqt.h
 *  qcnqt
 *
 *  Created by Carl Christensen on 2/9/08.
 *
 */
 
#ifndef _QCNLIVE_H_
#define _QCNLIVE_H_

#include <QApplication>
#include <QtGui>
#include <QtOpenGL>

 #include <QApplication>
 #include <QMainWindow>
 #include <QSplashScreen>
 #include <QTimer>

#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QPrinterInfo>
 
 #ifndef _WIN32
 #include <unistd.h>
 #endif

#include "main.h"
#include "qcn_graphics.h"
#include "qcn_curl.h"
#include "myframe.h"
#include "qcnlive_define.h"

class GLWidget;
class MyFrame;


// definition for the main app
class MyApp: public QApplication
{	
  private:
	Q_OBJECT

	QTimer* m_timerQuakeList;  // slow timer for getting quakes every half hour
	QTimer* m_timerMakeQuake;  // a short run timer to handle the quake shake stuff
	
	MyFrame* m_frame;
    QRect m_rect;            // apps screen coordinates
    QSplashScreen* m_psplash;  // the apps splash screen
	bool m_bInit;  // flag to see if we are initialized
	int m_iMakeQuakeTime; // "shake" time 
	int m_iMakeQuakeCountdown; // countdown time
	QPrinterInfo m_MakeQuakePrinter;
	QString m_strMakeQuakePrinter;
	
	QString m_strQuakeJPG;
	
  private slots:
    void slotGetLatestQuakeList();
    void slotMakeQuake();
	void slotPrintPreview(QPrinter* qpr);

  public:
    MyApp(int& argc, char** argv);
	
	bool Init();
    int Exit();

    //void SetRect(const wxSize& newsize, const wxPoint& newposition);
    void SetRect(const QRect& rect);
    void GetLatestQuakeList();

    bool get_qcnlive_prefs();
    bool set_qcnlive_prefs();

    void SetPath(const char* strArgv = NULL);
    bool CreateBOINCInitFile();
    bool MainInit();
    void KillSplashScreen(); 
	bool KillMainThread();
	bool StartMainThread();
	
	int getWidth() {return m_rect.width(); }
	int getHeight() {return m_rect.height(); }
	int getX() {return m_rect.x(); }
	int getY() {return m_rect.y(); }
	void setRect(const QRect& rect) {  m_rect = rect; }
        const QRect& getRect() { return m_rect; }
	
        int getMakeQuakeTime() { return m_iMakeQuakeTime; }
	void setMakeQuakeTime(const int iTime) { m_iMakeQuakeTime = iTime; }
	
        int getMakeQuakeCountdown() { return m_iMakeQuakeCountdown; }
	void setMakeQuakeCountdown(const int iTime) { m_iMakeQuakeCountdown = iTime; }

	const QPrinterInfo& getMakeQuakePrinterInfo() { return m_MakeQuakePrinter; }
	void setMakeQuakePrinterInfo(const QPrinterInfo& prn) { m_MakeQuakePrinter = prn; }

	const QString& getMakeQuakePrinterString() { return m_strMakeQuakePrinter; }
	void setMakeQuakePrinterString(const QString& prn) { m_strMakeQuakePrinter = prn; }

	void startQuakeTimer()
	{
		m_timerMakeQuake->start(1000L);  // in milliseconds, so check every second
	}
	
	void stopQuakeTimer()
	{
		m_timerMakeQuake->stop();  // stops the timer
	}
	
};

#endif // ifndef _QCNLIVE_H_
