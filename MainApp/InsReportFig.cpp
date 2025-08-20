#include "InsReportFig.h"
#include "qpushbutton.h"
#include "qgridlayout.h"
#include "InsFeaUnitPainter_Rep.h"
#include "FMainWindow.h"

extern FMainWindow* gWindow;

InsReportFig::InsReportFig(vector<EvaluData> EvaluData, QString PartNumStr, QString InspStaffStr, QString InspEquipmentStr, QString OtherDataStr, QWidget* parent):QWidget(parent)
{
	p_EvaluData = EvaluData;

	m_PartNumStr = PartNumStr;
	m_InspStaffStr = InspStaffStr;
	m_InspEquipmentStr = InspEquipmentStr;
	m_OtherDataStr = OtherDataStr;

	setFixedSize(1260, 1782);//A4×6
	MainHeight = 170;

	InvSec = 27;//结果总览每行字间距

	//零件编号高度
	if (!m_PartNumStr.isEmpty())
	{
		MainHeight += InvSec;
	}

	//检测人高度
	if (!m_InspStaffStr.isEmpty())
	{
		MainHeight += InvSec;
	}

	//检测设备及编号高度
	if (!m_InspEquipmentStr.isEmpty())
	{
		MainHeight += InvSec;
	}

	//其他数据高度
	if (!m_OtherDataStr.isEmpty())
	{
		MainHeight += InvSec;
	}

	//设置每个检测特征详细信息
	for (int i = 0; i < EvaluData.size(); i++)
	{
		InsFeaUnitPainter_Rep* figunit = new InsFeaUnitPainter_Rep(EvaluData[i],this);
		figunit->move(20, MainHeight);//下移距离
		MainHeight += (figunit->GetHeight() + 40);
	}

	if (MainHeight > 1722)
	{
		setFixedSize(1260, MainHeight + 60);
	}
}

InsReportFig::~InsReportFig()
{

}

void InsReportFig::paintEvent(QPaintEvent*)
{
	int FirstRowheight = 55;//检测报告黑色底纹高度

	//画第一行检测报告黑色底纹
	QPainter painter(this);
	painter.setPen(Qt::black);
	painter.setBrush(Qt::black);
	painter.drawRect(0, 0, 1782, FirstRowheight);

	//画第一行检测报告几个字
	QFont font1("宋体", 25, QFont::Bold, false);
	painter.setFont(font1);
	painter.setPen(Qt::white);
	painter.drawText(15, FirstRowheight - 12, QString::fromLocal8Bit("检测报告"));

	//画第二行结果总览灰色底纹
	int SecondRowheight = 60;//结果总览灰色底纹高度

	painter.setPen(QColor(147, 149, 152));
	painter.setBrush(QColor(147, 149, 152));

	if (!m_PartNumStr.isEmpty())
	{
		SecondRowheight += InvSec;
	}

	if (!m_InspStaffStr.isEmpty())
	{
		SecondRowheight += InvSec;
	}

	if (!m_InspEquipmentStr.isEmpty())
	{
		SecondRowheight += InvSec;
	}

	if (!m_OtherDataStr.isEmpty())
	{
		SecondRowheight += InvSec;
	}

	painter.drawRect(0, FirstRowheight, 1782, FirstRowheight + SecondRowheight);

	//画第一行检测报告几行个字
	//int InvSec = 27;//结果总览每行字间距
	int FirLoc_y = 80;//结果总览第一行字y坐标
	int FirLoc_x = 20;//结果总览第一行字x坐标

	QFont font2("宋体", 13, QFont::Normal, false);
	painter.setFont(font2);
	painter.setPen(Qt::black);
	//计算总测点数
	int NbPnts = 0;
	for (int i = 0; i < p_EvaluData.size(); i++)
	{
		for (int j = 0; j < p_EvaluData[i].ThoMesPntList.size(); j++)
		{
			NbPnts += 1;
		}
	}
	painter.drawText(FirLoc_x, FirLoc_y, QString::fromLocal8Bit("总测点数：") + QString::number(NbPnts));

	painter.drawText(FirLoc_x, FirLoc_y + InvSec, QString::fromLocal8Bit("检测特征数：") + QString::number(p_EvaluData.size()));

	//计算检测特征合格率
	int NbQualify = 0;
	vector<int> NoQualifyIndex;
	for (int i = 0; i < p_EvaluData.size(); i++)
	{
		if (p_EvaluData[i].qualify)
		{
			NbQualify += 1;
		}
		else
		{
			NoQualifyIndex.push_back(p_EvaluData[i].p_InsFea->InsFeatureindex);
		}
	}

	painter.drawText(FirLoc_x, FirLoc_y + InvSec * 2, QString::fromLocal8Bit("检测特征合格率：") + QString::number(double(NbQualify) / double(p_EvaluData.size()) * 100) + tr("%"));

	QString NoQualifyInsFea = QString::fromLocal8Bit("不合格检测特征：");
	for (int i = 0; i < NoQualifyIndex.size(); i++)
	{
		NoQualifyInsFea += (QString::fromLocal8Bit("检测特征") + QString::number(NoQualifyIndex[i]) + tr("  "));
	}
	painter.drawText(FirLoc_x, FirLoc_y + InvSec * 3, NoQualifyInsFea);

	int CurrInd = 3;
	//绘制零件图号
	if (!m_PartNumStr.isEmpty())
	{
		CurrInd += 1;

		QString tmpPartNumber = QString::fromLocal8Bit("零件图号：");
		tmpPartNumber += m_PartNumStr;
		painter.drawText(FirLoc_x, FirLoc_y + InvSec * CurrInd, tmpPartNumber);
	}

	//绘制检测人
	if (!m_InspStaffStr.isEmpty())
	{
		CurrInd += 1;

		QString tmpStrStaff = QString::fromLocal8Bit("检测人：");
		tmpStrStaff += m_InspStaffStr;
		painter.drawText(FirLoc_x, FirLoc_y + InvSec * CurrInd, tmpStrStaff);
	}

	//绘制检测设备及编号
	if (!m_InspEquipmentStr.isEmpty())
	{
		CurrInd += 1;

		QString tmpStrEquipment = QString::fromLocal8Bit("检测设备及编号：");
		tmpStrEquipment += m_InspEquipmentStr;
		painter.drawText(FirLoc_x, FirLoc_y + InvSec * CurrInd, tmpStrEquipment);
	}

	//绘制其他数据
	if (!m_OtherDataStr.isEmpty())
	{
		CurrInd += 1;

		QString tmpOther = QString::fromLocal8Bit("备注：");
		tmpOther += m_OtherDataStr;
		painter.drawText(FirLoc_x, FirLoc_y + InvSec * CurrInd, tmpOther);
	}

	//painter.drawRect(0, FirstRowheight, 1782, FirstRowheight + SecondRowheight);
}


