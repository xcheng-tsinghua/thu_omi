#pragma once

#include "../External/fkernel/Include/FKernelDefines.h"
#include <Qtgui>
#include "QSharedPointer"
#include "MainDoc.h"
#include "FTreeWnd.h"
#include "FInsFeaGraph.h"

class FInsTreeWidget;

class FInsFeaTreeWnd : public QDockWidget
{
	Q_OBJECT
public:
	FInsFeaTreeWnd(QWidget* parent, FMainDoc* pDoc);
	~FInsFeaTreeWnd(void);
	void	initTree(QList<QTreeWidgetItem*> treeList);
	void    clearTree();
	void    initTree_assembly(QList<QTreeWidgetItem*> treeList);
	void	contextMenuEvent(QContextMenuEvent* event);

	FInsTreeWidget*	GetTree() { return m_pTree; };
	vector<QTreeWidgetItem*> m_pPntItemList;

public slots:
	void onToPointPlan();
	void onToAccuEvalu();
	void onFInsFeaDel();
	void onExpandAll();
	void onFoldAll();
	void onEvaluAll();
	//void onUpdateMeasPnts();
	void onComputeSurfPntNum();

protected:
	FMainDoc* m_pDoc;
	FInsTreeWidget* m_pTree;
	FInsTreeWidget* m_pInsFeaGraphTree;

	QDockWidget* dockWidget;

	QSharedPointer<QMenu>	m_pPopMenu;
	QTreeWidgetItem* m_pTmpItem;	//记录右击跳出菜单处的item，用于传递给action使用
	//QMenu*	m_pPopMenu;
	QAction* m_pToPointPlan;
	QAction* m_pAccuEvalu;
	QAction* m_pFInsFeaDel;
	QAction* m_pFoldAll;
	QAction* m_pExpandAll;
	QAction* m_pEvaluAll;
	QAction* m_pFacePntNumCal;

};


class FInsTreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	FInsTreeWidget(QWidget* parent = 0);

	void	mousePressEvent(QMouseEvent* event);
};