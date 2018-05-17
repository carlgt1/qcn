#ifndef _QCN_2DPLOT_H_
#define _QCN_2DPLOT_H_

namespace qcn_2dplot {	
	extern short g_iShowSig;
	
	extern void TimeZoomOut();
	extern void TimeZoomIn();
	extern void SensorDataZoomAuto();
	extern void SensorDataZoomIn();
	extern void SensorDataZoomOut();
	extern void ShowSigPlot(bool bShow = true);
	extern bool IsSigPlot();

	extern int GetTimerTick();
	extern void SetTimerTick(const int iTT);
	extern bool IsWhite();
	extern void SetWhite(const bool bValue = true);

	extern void draw_text();
	extern void draw_tick_marks();
	extern void draw_plot() ;
	extern void draw_scrollbar();

#ifdef QCNLIVE
  extern void draw_makequake_message(); // just an overlay for the countdown etc
#endif

}  // end namespace

#endif // _QCN_2DPLOT_H_
