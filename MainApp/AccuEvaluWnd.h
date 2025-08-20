/*
该类用于实现右上角精度评定窗口
程曦-2021/3/26
*/

#pragma once
#include "qwidget.h"
#include "MainDoc.h"
#include "qmainwindow.h"
#include "qsqltablemodel.h"

class FInsReportDlg;

struct EvaluData
{
	EvaluData();

	//是否合格
	bool qualify;

	//公差数值
	double ToleranceValue;

	//尺寸数值
	double DimensionValue;

	double upDeviation;
	double downDeviation;

	//对应检测特征
	FInsFeature* p_InsFea;//对应检测特征

	//first:理论，second:测量
	vector<pair<gp_Pnt, gp_Pnt>> ThoMesPntList;//first:理论，second:测量

	// 2024.7.17 将距离、直径评估结果显示为测量值 + 偏差（理论值-测量值）
	// 尺寸偏差
	double Deviation;

};

class AccuEvaluWnd :public QDockWidget
{
	Q_OBJECT
public:
	AccuEvaluWnd(QWidget* parent, FMainDoc* pDoc);
	~AccuEvaluWnd();
	void	initTree(QList<QTreeWidgetItem*> treeList);
	void    clearTree();
	void    initTree_assembly(FInsFeature* FinsFea);
	void	contextMenuEvent(QContextMenuEvent* event);
	void	AcEvaluate(FInsFeature* pInsFeature);
	void	InitializeModel(QStandardItemModel* model);
	EvaluData	RealValueCal(FInsFeature* pInsFea);

	QTreeWidget* GetTree() { return m_pTree; };
	QStandardItemModel* GetModel() { return m_pEvaResModel; };//表格
	vector<FInsFeature*>& GetEvaInsList() { return m_InsfratureList; };
	vector<EvaluData>& GetEvaDataList() { return m_EvaluDataList; };

public slots:
	void	onToAccuEvalu();
	void	onToEvaluReport();
	void	onFInsFeaDel();
	void	onChartRowDel();
	void	onEvaluAll();
	void	onRemoveAllEvaluItem();
	//
	void	printPreviewSlot(QPrinter* printerPixmap);

private:
	QTreeWidget* m_pTree;
	FMainDoc* m_pDoc;
	QSharedPointer<QMenu>	m_pPopMenu;
	QTreeWidgetItem* m_pTmpItem;	//记录右击跳出菜单处的item，用于传递给action使用
	QAction* m_pAccuEvalu;
	QAction* m_pEvaluReport;
	QAction* m_pFeaDel;
	QAction* m_pAccuEvaDel;
	QAction* m_pEvaluAll;
	QAction* m_pRemoveAllEvaluItem;

	QWidget* m_pParent;
	QWidget* widget;

	QTableView* m_pEvaResView;//表格
	QStandardItemModel* m_pEvaResModel;
	vector<FInsFeature*> m_InsfratureList;
	int m_TableCurrRow;
	vector<EvaluData> m_EvaluDataList;

	FInsReportDlg* m_pInsReport;
};

