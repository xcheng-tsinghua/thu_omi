#pragma once
#include"MainWindow.h"
#include"qwidget.h"
#include"qpushbutton.h"

#include"InsReportFig.h"

class EvaluData;

class FInsReportDlg :public QWidget
{
	Q_OBJECT
public:
	FInsReportDlg(vector<EvaluData> EvaluData, QString PartNumStr, QString InspStaffStr, QString InspEquipment, QString OtherDataStr, QWidget* parent = 0);
	FInsReportDlg();
	~FInsReportDlg();

	void JL_SaveInsReport(int InspMonth, bool IsWS119);

private slots:
	void onSaveInsReport();
	void onPrintInsReport();

private:
	void DrawinspReportForPdf(QString SavedFilepath);
	void SaveTxtInsReport(QString SavedFilepath);

	QPushButton* m_pPrint;
	QPushButton* m_pSave;

	InsReportFig* m_pRepFig;
	vector<EvaluData> m_EvaluData;

	QString m_PartNumStr;
	QString m_InspStaffStr;
	QString m_InspEquipment;
	QString m_OtherDataStr;
};