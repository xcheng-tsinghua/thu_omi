#pragma once

#include "../External/fkernel/Include/FKernelDefines.h"
#include "qwidget.h"
#include "FGuideDataType.h"
#include "qdir.h"
#include <String>
#include "FUtilities.h"
#include "GLU.h"
#include "MainDoc.h"
#include "FTHP.h"
#include "FMainWindow.h"
#include <Eigen\Dense>
#include <iostream>
#include "FPathPlan.h"

using namespace std;
using namespace Eigen;

class FPathPlan;
class PathAllEntity;

class FPathPlanAndExeDlg : public QWidget
{
	Q_OBJECT
public:
	FPathPlanAndExeDlg(QWidget* parent = NULL);
	~FPathPlanAndExeDlg();

	double GetRetreatDis() { return m_back->text().toDouble(); };
	double GetSafeDis() { return m_safe->text().toDouble(); };

	void SetRetreatDis(double Dis) { m_back->setText(QString::number(Dis)); };
	void SetSafeDis(double Dis) { m_safe->setText(QString::number(Dis)); };

	QString GetNC_Body() { return m_NC_CodeBody; };

	QProgressBar* getProgress();

private slots:
	void onCancel();
	void onPathPlan();
	void onExport();
	void onButtonClicked(QAbstractButton* button);
	void onColliButtonClicked(QAbstractButton* button);
	void onDataTransfer();

protected:
	QPushButton* m_onPathPlan;
	QPushButton* m_onCancle;
	QPushButton* m_onExport;
	QPushButton* m_DataTransfer; // 将文件传输至机床

	QLineEdit* m_back;
	QLineEdit* m_safe;
	QLineEdit* m_ProbeOnLineEdit;
	QLineEdit* m_ProbeOffLineEdit;

	QButtonGroup* m_pButtonGroup;
	QRadioButton* ShortType;
	QRadioButton* FastType;

	QButtonGroup* mp_colli_group;
	QRadioButton* mp_colli_fast_btn;
	QRadioButton* mp_colli_acc_btn;

	FPathPlan* m_pPathPlan;
	bool m_is_fast_path_plan;

	bool m_is_fast_colli_judge;

	FMainDoc* m_pDoc;
	QString m_NC_CodeBody;

	PathAllEntity* m_pInspPathShow;

	QProgressBar* m_pProgressBar;

};

class PathAllEntity : public FEntity
{
public:
	void Draw(const FT::FRenderOpt& renderOpt);//绘图

};



