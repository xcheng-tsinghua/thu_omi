#pragma once
#include <qdockwidget.h>
#include <qboxlayout.h>
#include <qpushbutton.h>
#include <qtreewidget.h>
#include <qlineedit.h>

extern QString gDockTitleColor;

class QMenu;
class AutoInsFeaTreeWidget;
class FilterParasDlg;

class AutoInsFea :public QDockWidget
{
	Q_OBJECT
public:
	AutoInsFea();

	void SetFilterVals(double paraPlaneAreaRate, double paraPlaneAreaRank);

private slots:
	void onClose();
	void onSearchingInsFea();
	void onCollapseAll();
	void onExpandAll();
	void onAddInsFea();
	void onFilterParaSetting();

protected:
	void contextMenuEvent(QContextMenuEvent* event);
	void closeEvent(QCloseEvent* event)override;

private:
	void ClearTreeItems();

	QVBoxLayout* MainVLayput;

	QSharedPointer<QMenu>	m_pPopMenu;

	AutoInsFeaTreeWidget* m_pTree;

	QLineEdit* m_pTolValueLine;
	QLineEdit* m_pUpDevLine;
	QLineEdit* m_pDownDevLine;

	QTreeWidgetItem* m_pPARALLELISM_Item;
	QTreeWidgetItem* m_pROUNDNESS_Item;
	QTreeWidgetItem* m_pDISTANCE_Item;
	QTreeWidgetItem* m_pVERTICALITY_Item;
	QTreeWidgetItem* m_pDIAMETER_Item;
	QTreeWidgetItem* m_pAUXIALITY_Item;
	QTreeWidgetItem* m_pFLATNESS_Item;

	// 在本地的序号，添加检测特征时根据全局变量分配序号
	int m_LocalInsFeaInd;

	QTreeWidgetItem* m_pTmpItem;	//记录右击跳出菜单处的item，用于传递给action使用

	QAction* m_pFoldAll;
	QAction* m_pExpandAll;
	QAction* m_pAddInsFea;

	QAction* m_pFiltParaSetting;

	FilterParasDlg* m_pFilterParasDlg;

	// m_PlaneAreaRate 表示如果最大平面面积为 A ,只有面积大于 m_PlaneAreaRate * A 的平面才会被考虑
	double m_PlaneAreaRate;

	// m_PlaneAreaRank 表示如果一共有 N 个平面, 按从小到大排序, 只有排名大于 m_PlaneAreaRank * N 的平面才会被考虑
	double m_PlaneAreaRank;


};

class AutoInsFeaTreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	AutoInsFeaTreeWidget(QWidget* parent = 0);

	void	mousePressEvent(QMouseEvent* event);
};

class QCheckBox;

class FilterParasDlg :public QWidget
{
	Q_OBJECT
public:
	FilterParasDlg(AutoInsFea* paraMainDlg);
	bool GetIsCylinderOnly() { return m_IsCylinderOnly; }
	bool GetIsPlaneOnly() { return m_IsPlaneOnly; }


protected:
	void paintEvent(QPaintEvent* event) override;
	//virtual bool winEvent(MSG* message, long* result)override;

private slots:
	void onConfirm();
	void onOnlyCylinderCheckBox();
	void onOnlyPlaneCheckBox();


private:
	QLineEdit* m_pPlaneAreaRate;
	QLineEdit* m_pPlaneAreaRank;

	AutoInsFea* m_pAutoInsFea;

	QCheckBox* m_pIsCylinderOnlyFilt;
	QCheckBox* m_pIsPlaneOnlyFilt;

	bool m_IsCylinderOnly;
	bool m_IsPlaneOnly;

};
