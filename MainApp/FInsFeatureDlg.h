#pragma once
#include "qwidget.h"
#include "qgridlayout.h"
#include "QGroupBox.h"
#include "QCheckBox.h"
#include "QListWidget.h"
#include "QLayout.h"
#include "QtGui/QAbstractItemView"
#include "qlabel.h"
#include "qlineedit.h"
#include "qpushbutton.h"
#include <vector>
#include "qtreewidget.h"
#include "FInsFeature.h"
#include <iostream>
#include <unordered_set>

class FInsFeatureDlg :public QWidget
{
	Q_OBJECT
public:
	FInsFeatureDlg(QWidget* parent = NULL);
	~FInsFeatureDlg();

	void contextMenuEvent(QContextMenuEvent* event);

	void addSurf(FSurface* _surf);

	void setListWidgetBcgDef();

	void faceSelG1();
	void faceSelG2();

private slots:
	void ChooseTargetFace(int i);
	void GenerateInsFeature();
	void ExitGenerateInsFeature();
	void onDeleteFace();
	void onAIS_FaceSel();
	void onRemoveAllSurf();
	void onButtonClicked(QAbstractButton* button);
	void onDirSelButtonClicked(QAbstractButton* button);

private:

	class IFDListWidget :public QListWidget
	{
	public:
		IFDListWidget(FInsFeatureDlg* _dlg, const int& _ind, const double& _r, const double& _g, const double& _b);
		~IFDListWidget();

		void setBcgDef();

	protected:
		void mousePressEvent(QMouseEvent* event) override;

	private:
		FInsFeatureDlg* m_pInsFeaDlg;
		QString getStyleSheet(double _r, double _g, double _b);

		// = 1: 第一组
		// = 2: 第二组
		int m_nInd;

		// [0, 1]
		double m_bcgR;
		double m_bcgG;
		double m_bcgB;

		// [0, 1]
		double m_bcgDefR;
		double m_bcgDefG;
		double m_bcgDefB;

	};

	void showInsParameterList();
	void closeEvent(QCloseEvent* event);

	void hideXYZSel();
	void showXYZSel();

	void hideValueInputLabelLine();
	void showDimensionValueInputLabelLine();
	void showToleranceValueInputLabelLine();

	// 存放两组曲面的哈希表
	unordered_set<FSurface*> m_HashSurfList1;
	unordered_set<FSurface*> m_HashSurfList2;

	QGridLayout* FaceListGridLayout;/////////////两个ListWidget的布局
	QGridLayout* InsRequestGridLayout;/////////////检测要求布局

	QRadioButton* pDis_Stra;
	QRadioButton* pDis_X;
	QRadioButton* pDis_Y;
	QRadioButton* pDis_Z;

	IFDListWidget* FaceList1;
	IFDListWidget* FaceList2;

	QLineEdit* pToleranceValueEditLine;
	QLineEdit* pUpDeviationEditLine;
	QLineEdit* pDownDeviationEditLine;

	QLabel* pToleranceValueLabel;
	QLabel* pUpDeviationLabel;
	QLabel* pDownDeviationLabel;

	QGridLayout* InsParameterGridLayout;/////////////检测参数列表布局
	QGroupBox* InsParameterGroup;/////////////检测参数

	// 鼠标右键点击时所在的FaceListWidget
	// =1：在第一个
	// =2：在第二个
	int ClickedFaceListWidget;

	//0：直线距离，1：X向距离，2：Y向距离，3：Z向距离
	int DirSelState;

	///////////////输入的检测参数数值
	double ToleranceValue;
	double UpDeviationValue;
	double DownDeviationValue;

	InsType SelType;// 选定的检测类型

	// 右键菜单
	QAction* m_pActionDelete;
	//QAction* m_pAIS_FaceSel;
	QAction* m_pDelAllSurf;
	QSharedPointer<QMenu>	m_pPopMenu;
	QListWidgetItem* m_pTmpItem;

	FFeatureTool* m_pCurTool;		///select tool

};