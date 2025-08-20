/*	
\brief	A window with tree structure to display and edit inspection features.
\author: Jacky
\date    2017-12
*/
#ifndef F_TREE_WND_H
#define F_TREE_WND_H
#pragma once
#include "../External/fkernel/Include/FKernelDefines.h"
#include <Qtgui>
#include <qtreewidget.h>
#include "QSharedPointer"
#include "MainDoc.h"
#include "FInsFeatureDlg.h"

class QMenu;
class QAction;
class FTreeWidget;

class FTreeWnd : public QDockWidget
{
	Q_OBJECT
public:
	//FDataWnd(FMainWindow* parent=NULL);
	FTreeWnd(QWidget* parent,FMainDoc* pDoc);
	~FTreeWnd(void);
	void	initTree();
	void    initTree_YU();
	void    initTree_assembly(QList<QTreeWidgetItem*> treeList);
	void	clearTree();
	FTreeWidget* getTreeWidget() { return m_pTree; }
	//QTreeWidgetItem*	GetItem(int idx, FIpFeature::FIpFtType type);
	void getNumFromName(const char* str, int* num);

	void	contextMenuEvent(QContextMenuEvent * event);

	//accessing functions

	//FMainWindow* gWindow;

public slots:
	void	onRenameItem();
	void	onTest();
	void	onConfig();
	void    onNewFeat();
	void    onAddEle();
	//void    onLeftPressShow(QTreeWidgetItem* pressedItem, int column);
	void onExpandAll();
	void onFoldAll();
	void onShowNearSurf();
	void onAutoInsFea();

signals:
	//void	signalRenameItem(QTreeWidgetItem*);
	//void    itemClicked(QTreeWidgetItem*, int);


protected:
	//data members
	FMainDoc*		m_pDoc;	
	FTreeWidget*	m_pTree;
	//FInsFeatureDlg* m_pFeatDlg;
	QDockWidget *dockWidget;

	QSharedPointer<QMenu>	m_pPopMenu;
	//QMenu*	m_pPopMenu;
	QAction*	m_pActionRename;
	QAction*	m_pActionConfig;
	QAction*	m_pActionTest;
	QAction*    m_pActionNewFeat;
	QAction*    m_pActionAddEle;
	QAction*	m_pShowNearbySurf;
	QAction*	m_pActionAutoInsfea;

	QAction* m_pFoldAll;
	QAction* m_pExpandAll;

	QTreeWidgetItem* m_pTmpItem;	//记录右击跳出菜单处的item，用于传递给action使用

	//QWidget *dockWidgetContents;
	//QWidget *widget;
	//QVBoxLayout *verticalLayout_2;
	
};

class FTreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	//FTreeWidget(QWidget* parent = 0) : QTreeWidget(parent) { LastSelItem = NULL; IsCtrlPressed = 0;}
	FTreeWidget(QWidget* parent = 0);
	void	setDoc(FMainDoc* doc) {m_pDoc = doc;}
	void	mousePressEvent(QMouseEvent * event);
	void	keyPressEvent(QKeyEvent* event);
	void	keyReleaseEvent(QKeyEvent* event);
protected:
	FMainDoc* m_pDoc;
	QTreeWidgetItem* LastSelItem;
	int IsCtrlPressed;
};

#endif