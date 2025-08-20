// 当前用于导入多个方位的测点数据，以第一个方位坐标系为准，第二个方位坐标系对齐到第一个坐标系，并补偿误差
// 使用前必须和操作工确定 G75Z0 指令是将Z轴原地升到上限位
#pragma once

#include "qwidget.h"
#include "qlabel.h"
#include "qcombobox.h"
#include "qlistwidget.h"
#include "qpushbutton.h"
#include "qtreewidget.h"
#include "qlineedit.h"
#include "FGuideDataType.h"

//class FIpPntList;

struct TransInfo
{
	TransInfo(double theAngle, double theX, double theY, double theZ): 
		Angle(theAngle), 
		X(theX),
		Y(theY),
		Z(theZ){};

	// 转动角度——弧度
	double Angle;

	double X;
	double Y;
	double Z;
};

class FGroupPathPlan :public QWidget
{
	Q_OBJECT
public:
	FGroupPathPlan();
	~FGroupPathPlan() {};

	void SetTransXLine(QString paraMovX) { m_pCoorInMainFrameX->setText(paraMovX); };
	void SetTransYLine(QString paraMovY) { m_pCoorInMainFrameY->setText(paraMovY); };
	void SetTransZLine(QString paraMovZ) { m_pCoorInMainFrameZ->setText(paraMovZ); };

	void SetRotateLine(QString paraRotX) { m_pRotateAngleLine->setText(paraRotX); m_pCAxisAngleLine->setText(QString::number(-paraRotX.toDouble())); };

	// XYZ = 0:X
	// XYZ = 1:Y
	// XYZ = 2:Z
	void SetXYZ_Checked(int XYZ);

private slots:
	void onPntPlan();

	// 载入所有方位保存的测点
	void onLoadAllPnts();

	void onPathPlan();

	void onSaveOrientationItem();

	// 导入理论测点并补偿，此时测点还在局部坐标系
	void onImportPnts();

	void onExNCCode();
	void onFrameAlt();

	// 将各组测点转移至同一坐标系，该坐标系为第一方位所在坐标系
	void onPntsTransForm();

	void onRotateDirClicked(QAbstractButton* button);
	void onResetModel();

	void onSaveTheoPnts();



private:
	// 保存测点，每个方位上的测点在一个| FIpPntList |里
	std::vector<FIpPntList> m_AllPntList;

	// 保存每组坐标系相对于原始坐标系的变换系数
	std::vector<TransInfo> m_TransInfoList;

	QTreeWidget* m_pAllInspItemsTree;

	// 输入框
	QLineEdit* m_pRotateAngleLine;
	QLineEdit* m_pCAxisAngleLine;
	QLineEdit* m_pCoorInMainFrameX;
	QLineEdit* m_pCoorInMainFrameY;
	QLineEdit* m_pCoorInMainFrameZ;
	QLineEdit* m_pCurrFrameNameLine;

	// 按钮
	QPushButton* pPntPlan;
	QPushButton* pPathPlan;
	QPushButton* pSaveOrientationItem;
	QPushButton* pFrameAlt;
	QPushButton* pExNCCode;
	QPushButton* pLoadPnts;
	QPushButton* pImportPnts;
	QPushButton* pPntsTransForm;
	QPushButton* pResetModel;
	QPushButton* pExportTheoPnts;

	QRadioButton* m_pRotateX;
	QRadioButton* m_pRotateY;
	QRadioButton* m_pRotateZ;

	QString m_NC_CodeBody;
	QString m_SubHeadLine1;
	QString m_SubHeadLine2;
	QString m_SubHeadLine3;

	gp_Ax1 m_RotateDir;
};

