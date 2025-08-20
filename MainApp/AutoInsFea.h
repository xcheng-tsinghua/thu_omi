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

	// �ڱ��ص���ţ���Ӽ������ʱ����ȫ�ֱ����������
	int m_LocalInsFeaInd;

	QTreeWidgetItem* m_pTmpItem;	//��¼�һ������˵�����item�����ڴ��ݸ�actionʹ��

	QAction* m_pFoldAll;
	QAction* m_pExpandAll;
	QAction* m_pAddInsFea;

	QAction* m_pFiltParaSetting;

	FilterParasDlg* m_pFilterParasDlg;

	// m_PlaneAreaRate ��ʾ������ƽ�����Ϊ A ,ֻ��������� m_PlaneAreaRate * A ��ƽ��Żᱻ����
	double m_PlaneAreaRate;

	// m_PlaneAreaRank ��ʾ���һ���� N ��ƽ��, ����С��������, ֻ���������� m_PlaneAreaRank * N ��ƽ��Żᱻ����
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
