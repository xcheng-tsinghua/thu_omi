#pragma once

#include"MainWindow.h"
#include"qwidget.h"
#include"AccuEvaluWnd.h"

class InsReportFig :public QWidget
{
	Q_OBJECT
public:
	InsReportFig(vector<EvaluData> EvaluData, QString PartNumStr, QString InspStaffStr, QString InspEquipmentStr, QString OtherDataStr, QWidget* parent = 0);
	~InsReportFig();

	void paintEvent(QPaintEvent*);
	int GetMainHeight() { return MainHeight; };

private:
	int MainHeight;//决定每个检测特征结果的位置
	vector<EvaluData> p_EvaluData;

	QString m_PartNumStr;
	QString m_InspStaffStr;
	QString m_InspEquipmentStr;
	QString m_OtherDataStr;

	int InvSec;//结果总览每行字间距
};