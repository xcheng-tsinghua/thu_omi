#pragma once
#include <qwidget.h>
#include "AccuEvaluWnd.h"
#include "qlayout.h"
#include "qlineedit.h"
#include "qpushbutton.h"
#include "qlabel.h"
#include "qtextedit.h"

class InspReportInfoDlg :public QWidget
{
	Q_OBJECT
public:
	InspReportInfoDlg(vector<EvaluData> InspData);
	~InspReportInfoDlg();

	// [0] 零件图号
	// [1] 检测人
	// [2] 机器名
	// [3] 备注
	void JL_DataExe(vector<QString> fpInsRepInfo,int InspMonth, bool IsWS119);

public slots:
	void onConfirm();

private:
	vector<EvaluData> m_pInspData;

	QLabel* m_pPartNumberLabel;
	QLabel* m_pInspStaffLabel;
	QLabel* m_pInspEquipmentLabel;
	QLabel* m_pOtherDataLabel;

	QLineEdit* m_pPartNumberLine;
	QLineEdit* m_pInspStaffLine;
	QLineEdit* m_pInspEquipmentLine;
	QTextEdit* m_pOtherDataLine;

	QPushButton* m_pConfBnt;

	FInsReportDlg* m_pInsReport;

	QString DefaultStrPath;
};