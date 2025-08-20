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

	setFixedSize(1260, 1782);//A4��6
	MainHeight = 170;

	InvSec = 27;//�������ÿ���ּ��

	//�����Ÿ߶�
	if (!m_PartNumStr.isEmpty())
	{
		MainHeight += InvSec;
	}

	//����˸߶�
	if (!m_InspStaffStr.isEmpty())
	{
		MainHeight += InvSec;
	}

	//����豸����Ÿ߶�
	if (!m_InspEquipmentStr.isEmpty())
	{
		MainHeight += InvSec;
	}

	//�������ݸ߶�
	if (!m_OtherDataStr.isEmpty())
	{
		MainHeight += InvSec;
	}

	//����ÿ�����������ϸ��Ϣ
	for (int i = 0; i < EvaluData.size(); i++)
	{
		InsFeaUnitPainter_Rep* figunit = new InsFeaUnitPainter_Rep(EvaluData[i],this);
		figunit->move(20, MainHeight);//���ƾ���
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
	int FirstRowheight = 55;//��ⱨ���ɫ���Ƹ߶�

	//����һ�м�ⱨ���ɫ����
	QPainter painter(this);
	painter.setPen(Qt::black);
	painter.setBrush(Qt::black);
	painter.drawRect(0, 0, 1782, FirstRowheight);

	//����һ�м�ⱨ�漸����
	QFont font1("����", 25, QFont::Bold, false);
	painter.setFont(font1);
	painter.setPen(Qt::white);
	painter.drawText(15, FirstRowheight - 12, QString::fromLocal8Bit("��ⱨ��"));

	//���ڶ��н��������ɫ����
	int SecondRowheight = 60;//���������ɫ���Ƹ߶�

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

	//����һ�м�ⱨ�漸�и���
	//int InvSec = 27;//�������ÿ���ּ��
	int FirLoc_y = 80;//���������һ����y����
	int FirLoc_x = 20;//���������һ����x����

	QFont font2("����", 13, QFont::Normal, false);
	painter.setFont(font2);
	painter.setPen(Qt::black);
	//�����ܲ����
	int NbPnts = 0;
	for (int i = 0; i < p_EvaluData.size(); i++)
	{
		for (int j = 0; j < p_EvaluData[i].ThoMesPntList.size(); j++)
		{
			NbPnts += 1;
		}
	}
	painter.drawText(FirLoc_x, FirLoc_y, QString::fromLocal8Bit("�ܲ������") + QString::number(NbPnts));

	painter.drawText(FirLoc_x, FirLoc_y + InvSec, QString::fromLocal8Bit("�����������") + QString::number(p_EvaluData.size()));

	//�����������ϸ���
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

	painter.drawText(FirLoc_x, FirLoc_y + InvSec * 2, QString::fromLocal8Bit("��������ϸ��ʣ�") + QString::number(double(NbQualify) / double(p_EvaluData.size()) * 100) + tr("%"));

	QString NoQualifyInsFea = QString::fromLocal8Bit("���ϸ���������");
	for (int i = 0; i < NoQualifyIndex.size(); i++)
	{
		NoQualifyInsFea += (QString::fromLocal8Bit("�������") + QString::number(NoQualifyIndex[i]) + tr("  "));
	}
	painter.drawText(FirLoc_x, FirLoc_y + InvSec * 3, NoQualifyInsFea);

	int CurrInd = 3;
	//�������ͼ��
	if (!m_PartNumStr.isEmpty())
	{
		CurrInd += 1;

		QString tmpPartNumber = QString::fromLocal8Bit("���ͼ�ţ�");
		tmpPartNumber += m_PartNumStr;
		painter.drawText(FirLoc_x, FirLoc_y + InvSec * CurrInd, tmpPartNumber);
	}

	//���Ƽ����
	if (!m_InspStaffStr.isEmpty())
	{
		CurrInd += 1;

		QString tmpStrStaff = QString::fromLocal8Bit("����ˣ�");
		tmpStrStaff += m_InspStaffStr;
		painter.drawText(FirLoc_x, FirLoc_y + InvSec * CurrInd, tmpStrStaff);
	}

	//���Ƽ���豸�����
	if (!m_InspEquipmentStr.isEmpty())
	{
		CurrInd += 1;

		QString tmpStrEquipment = QString::fromLocal8Bit("����豸����ţ�");
		tmpStrEquipment += m_InspEquipmentStr;
		painter.drawText(FirLoc_x, FirLoc_y + InvSec * CurrInd, tmpStrEquipment);
	}

	//������������
	if (!m_OtherDataStr.isEmpty())
	{
		CurrInd += 1;

		QString tmpOther = QString::fromLocal8Bit("��ע��");
		tmpOther += m_OtherDataStr;
		painter.drawText(FirLoc_x, FirLoc_y + InvSec * CurrInd, tmpOther);
	}

	//painter.drawRect(0, FirstRowheight, 1782, FirstRowheight + SecondRowheight);
}


