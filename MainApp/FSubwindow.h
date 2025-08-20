#ifndef JCP3SUBWINDOW_H
#define JCP3SUBWINDOW_H

#include <QMdiSubWindow>
#include "TitleBar.h"

class FMainDoc;
class FMainView;

class FSubWindow : public QMdiSubWindow
{
	Q_OBJECT
public:
	FSubWindow(QWidget* parent = NULL, FMainDoc* pDoc = NULL);
	~FSubWindow();
public:
	////////////////////////////// Accessing methods ////////////////////////////////////////////
	FMainView* GetView() { return m_pView; }
	void		SetView(FMainView* pview);
	virtual void resizeEvent(QResizeEvent* resizeEvent);
	//////////////////////////////////////////////////////////////////////////
	void Update();
	TitleBar* m_pTitleBar;

private:
	FMainDoc* m_pDoc;
	FMainView* m_pView;
};

#endif // JCP3SUBWINDOW_H


//以下为原始代码
//#ifndef JCP3SUBWINDOW_H
//#define JCP3SUBWINDOW_H
//
//#include <QMdiSubWindow>
//
//class FMainDoc;
//class FMainView;
//
//class FSubWindow : public QMdiSubWindow
//{
//	Q_OBJECT
//public:
//	FSubWindow(QWidget* parent = NULL, FMainDoc* pDoc = NULL);
//	~FSubWindow();
//public:
//	////////////////////////////// Accessing methods ////////////////////////////////////////////
//	FMainView* GetView() { return m_pView; }
//	void		SetView(FMainView* pview);
//	virtual void resizeEvent(QResizeEvent* resizeEvent);
//	//////////////////////////////////////////////////////////////////////////
//	void Update();
//
//private:
//	FMainDoc* m_pDoc;
//	FMainView* m_pView;
//};
//
//#endif // JCP3SUBWINDOW_H