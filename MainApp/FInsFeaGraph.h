#pragma once
#include <qdockwidget.h>
#include <qtreewidget.h>

class QVBoxLayout;
class FInsFeaGraphTreeWidget;

class FInsFeaGraph :public QDockWidget
{
	Q_OBJECT
public:
	FInsFeaGraph();


private slots:
	void onClose();
	void onComputeInsFeaGraph();
	void onSampling();
	void onCollapseAll();
	void onExpandAll();

protected:
	void contextMenuEvent(QContextMenuEvent* event);
	void closeEvent(QCloseEvent* event)override;


private:
	void ClearTreeItems();

	QVBoxLayout* MainVLayput;

	FInsFeaGraphTreeWidget* m_pTree;

	QSharedPointer<QMenu> m_pPopMenu;
	QAction* m_pSampling;
	QAction* m_pFoldAll;
	QAction* m_pExpandAll;

	QTreeWidgetItem* m_pTmpItem;	//记录右击跳出菜单处的item，用于传递给action使用

};


class FInsFeaGraphTreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	FInsFeaGraphTreeWidget(QWidget* parent = 0);

	void	mousePressEvent(QMouseEvent* event);
};