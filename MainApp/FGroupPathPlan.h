// ��ǰ���ڵ�������λ�Ĳ�����ݣ��Ե�һ����λ����ϵΪ׼���ڶ�����λ����ϵ���뵽��һ������ϵ�����������
// ʹ��ǰ����Ͳ�����ȷ�� G75Z0 ָ���ǽ�Z��ԭ����������λ
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

	// ת���Ƕȡ�������
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

	// �������з�λ����Ĳ��
	void onLoadAllPnts();

	void onPathPlan();

	void onSaveOrientationItem();

	// �������۲�㲢��������ʱ��㻹�ھֲ�����ϵ
	void onImportPnts();

	void onExNCCode();
	void onFrameAlt();

	// ��������ת����ͬһ����ϵ��������ϵΪ��һ��λ��������ϵ
	void onPntsTransForm();

	void onRotateDirClicked(QAbstractButton* button);
	void onResetModel();

	void onSaveTheoPnts();



private:
	// �����㣬ÿ����λ�ϵĲ����һ��| FIpPntList |��
	std::vector<FIpPntList> m_AllPntList;

	// ����ÿ������ϵ�����ԭʼ����ϵ�ı任ϵ��
	std::vector<TransInfo> m_TransInfoList;

	QTreeWidget* m_pAllInspItemsTree;

	// �����
	QLineEdit* m_pRotateAngleLine;
	QLineEdit* m_pCAxisAngleLine;
	QLineEdit* m_pCoorInMainFrameX;
	QLineEdit* m_pCoorInMainFrameY;
	QLineEdit* m_pCoorInMainFrameZ;
	QLineEdit* m_pCurrFrameNameLine;

	// ��ť
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

