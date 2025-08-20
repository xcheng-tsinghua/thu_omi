/*
数据可视化的曲线窗口
*/
#ifndef F_STATVISUAL_WND_H
#define F_STATVISUAL_WND_H
#pragma once
#include "../External/fkernel/Include/FKernelDefines.h"
#include <Qtgui>
#include "MainDoc.h"
#include "QLayout"
#include "QTabWidget"

class	QCustomPlot;

class FStatVisualWnd : public QDockWidget
{
	Q_OBJECT
public:
	//FDataWnd(FMainWindow* parent=NULL);
	FStatVisualWnd(QWidget* parent,FMainDoc* pDoc);
	~FStatVisualWnd(void);
	//FMainWindow* gWindow;

public slots:
	void			PlotPic1();

protected:
	QWidget*		m_pCentralWgt;		// widget in center
	QVBoxLayout*	m_pMainVLayout;		// main layout(Vertical)
	QTabWidget*		m_pPlotTabWgt;		//	tabwidget used to hold plots
	QCustomPlot*	m_pCustomPlot1;
	QCustomPlot*	m_pCustomPlot2;

	


	//data members
	FMainDoc*		m_pDoc;	
};

#endif