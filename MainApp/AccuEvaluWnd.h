/*
��������ʵ�����ϽǾ�����������
����-2021/3/26
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

	//�Ƿ�ϸ�
	bool qualify;

	//������ֵ
	double ToleranceValue;

	//�ߴ���ֵ
	double DimensionValue;

	double upDeviation;
	double downDeviation;

	//��Ӧ�������
	FInsFeature* p_InsFea;//��Ӧ�������

	//first:���ۣ�second:����
	vector<pair<gp_Pnt, gp_Pnt>> ThoMesPntList;//first:���ۣ�second:����

	// 2024.7.17 �����롢ֱ�����������ʾΪ����ֵ + ƫ�����ֵ-����ֵ��
	// �ߴ�ƫ��
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
	QStandardItemModel* GetModel() { return m_pEvaResModel; };//���
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
	QTreeWidgetItem* m_pTmpItem;	//��¼�һ������˵�����item�����ڴ��ݸ�actionʹ��
	QAction* m_pAccuEvalu;
	QAction* m_pEvaluReport;
	QAction* m_pFeaDel;
	QAction* m_pAccuEvaDel;
	QAction* m_pEvaluAll;
	QAction* m_pRemoveAllEvaluItem;

	QWidget* m_pParent;
	QWidget* widget;

	QTableView* m_pEvaResView;//���
	QStandardItemModel* m_pEvaResModel;
	vector<FInsFeature*> m_InsfratureList;
	int m_TableCurrRow;
	vector<EvaluData> m_EvaluDataList;

	FInsReportDlg* m_pInsReport;
};

