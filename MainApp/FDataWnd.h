/*	
\brief	A window class to define inspection configuration window.
\author: Jacky
\date    2017-03
*/
#ifndef F_INSPECTCONFIG_WND_H
#define F_INSPECTCONFIG_WND_H
#pragma once
#include "../External/fkernel/Include/FKernelDefines.h"
#include <Qtgui>
#include "MainDoc.h"


class QSqlTableModel;
//class FMainWindow;

class FDataWnd : public QDockWidget
{
	Q_OBJECT
public:
	//FDataWnd(FMainWindow* parent=NULL);
	FDataWnd(QWidget* parent,FMainDoc* pDoc);
	~FDataWnd(void);

	//accessing functions
	QTableView* GetTableView();
	void	clearData();
	//FMainWindow* gWindow;

public slots:
	void	onResetPointData(bool dspTheo,int mesNum,bool dspError);
	void	onUpdateMesPointData(int num);
	void	onSetHighlightPnt(const QModelIndex & current, const QModelIndex & previous);
	void    onUpdateError(int num);

protected:
	void InitializeModel(QSqlTableModel* model);

	//data members
	QTableView*		m_pTableView;
	QSqlTableModel*	m_pDbModel;		//model of database
	QWidget*		m_pDataPage;		//data page
	FMainDoc*		m_pDoc;	
};

#endif