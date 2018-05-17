// the main OpenGL widget for Qt

#include <QtGui>
#include <QtOpenGL>
#include <QTimer>

#include <math.h>

#include "qcnopenglwidget.h"
#include "qcn_earth.h"

QCNGLWidget::QCNGLWidget(QWidget *parent)
    : QGLWidget(parent)
{
	m_pframe = (MyFrame*) parent;
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(animate()));
    m_timer->start(40);  // 40 milliseconds = 25 frames per second, a decent animation rate
	
	m_mouseDown[GLUT_LEFT_BUTTON] = false;
	m_mouseDown[GLUT_MIDDLE_BUTTON] = false;
	m_mouseDown[GLUT_RIGHT_BUTTON] = false;
	
	// init the OpenGL graphics vars from the qcn_graphics namespace
	qcn_graphics::graphics_main(0, NULL);
	setAutoBufferSwap(true);
}

QCNGLWidget::~QCNGLWidget()
{
	m_timer->stop();
    makeCurrent();
}

void QCNGLWidget::setTimePosition(const double& dTime)
{
	emit TimePositionChanged(dTime);
	updateGL();
}

void QCNGLWidget::initializeGL()
{
    if (!qcn_graphics::g_bInitGraphics) {  // first time in, need to init OpenGL settings & load bitmaps etc
        qcn_graphics::Init(); 
    }	
}

void QCNGLWidget::paintGL()
{
   qcn_graphics::Render(0,0,0);
}

void QCNGLWidget::resizeGL(int width, int height)
{
    // call the qcn_graphics.cpp resize	
	qcn_graphics::Resize(width, height);	
	// tell the earth to regen earthquakes coords
	qcn_graphics::earth.RecalculateEarthquakePositions();
	
}

/*

void QCNGLWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	int which = whichGLUTButton(event, true);
	switch(qcn_graphics::g_eView) {
		case VIEW_EARTH_DAY:
		case VIEW_EARTH_NIGHT:
			m_pframe->EarthRotate(true);
			break;
		case VIEW_PLOT_2D:
			m_pframe->ToggleStartStop(true);
			break;
		case VIEW_PLOT_3D:
		case VIEW_EARTH_COMBINED:
		case VIEW_CUBE:
			break;
	}
	
	// force to start
	qcn_graphics::MouseButton(event->x(), event->y(), which, 1);
	
	QCursor cursorNormal(Qt::ArrowCursor);
}
*/

int QCNGLWidget::whichGLUTButton(const QMouseEvent* event, const bool bDown)
{
	int which = GLUT_NO_BUTTON;
	switch(event->button()) {
		case Qt::LeftButton:  
			which = GLUT_LEFT_BUTTON;
			break;
		case Qt::MidButton:  
			which = GLUT_MIDDLE_BUTTON;
			break;
		case Qt::RightButton:  
			which = GLUT_RIGHT_BUTTON;
			break;
		default:
			which = GLUT_NO_BUTTON;
	}
	if (which != GLUT_NO_BUTTON) {
		m_mouseDown[which] = bDown;  
	}
	return which;
}

void QCNGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
	int which = whichGLUTButton(event, false);
	qcn_graphics::MouseButton(event->x(), event->y(), which, 0);
	QCursor cursorNormal(Qt::ArrowCursor);
	setCursor(cursorNormal);
}

void QCNGLWidget::mousePressEvent(QMouseEvent *event)
{
	int which = whichGLUTButton(event, true);
    m_lastPos = event->pos();
	
	//if (which >= GLUT_LEFT_BUTTON && which <= GLUT_RIGHT_BUTTON) m_mouseDown[which] = true;  // the wxwidgets getbutton is one off from our left/mid/right array
	
	switch(qcn_graphics::g_eView) {
		case VIEW_EARTH_DAY:
		case VIEW_EARTH_NIGHT:
			m_pframe->EarthRotate(false);
			break;
		case VIEW_PLOT_2D:
			//m_pframe->ToggleStartStop(false);
			break;
		case VIEW_PLOT_3D:
		case VIEW_EARTH_COMBINED:
		case VIEW_CUBE:		
			break;
		case VIEW_GAME:		
			break;
	}

	QCursor cursorHand(Qt::OpenHandCursor);
	setCursor(cursorHand);
	
	qcn_graphics::MouseButton(event->x(), event->y(), which, 1);
	
}

void QCNGLWidget::mouseMoveEvent(QMouseEvent *event)
{
   int dx = event->x() - m_lastPos.x();
	const int iFactor = 10;
	if (dx > iFactor) dx = iFactor;
	else if (dx < -iFactor) dx = -iFactor;  // avoid huge jumps
   //int dy = event->y() - m_lastPos.y();
   m_lastPos = event->pos();
	if (qcn_graphics::g_eView == VIEW_PLOT_2D) { // set the slider as a dx
		int iVal = m_pframe->getTimeSliderValue() + dx;
		if (iVal > 100) iVal = 100;
		else if (iVal < 1) iVal = 0;
		m_pframe->setTimeSliderValue(iVal);
	}
	else {
		qcn_graphics::MouseMove(event->x(), event->y(), 
			m_mouseDown[GLUT_LEFT_BUTTON]    ? 1 : 0, 
			m_mouseDown[GLUT_MIDDLE_BUTTON]  ? 1 : 0, 
			m_mouseDown[GLUT_RIGHT_BUTTON]   ? 1 : 0
		);
	}
}

void QCNGLWidget::animate()
{
	if (sm && m_pframe && sm->strDisplay[0]) { // little trick to display a status message to the GUI from elsewhere in the qcn system i.e. after writing a sac file
		m_pframe->statusBar()->showMessage(sm->strDisplay, 5000);
		memset(sm->strDisplay, 0x00, sizeof(sm->strDisplay));
	}
	updateGL();
}
