#include "FInsReportDlg.h"
#include "qpalette.h"
#include "qtoolbar.h"
#include "np.h"

extern QSettings* gDlgSetting;

FInsReportDlg::FInsReportDlg(vector<EvaluData> EvaluData, QString PartNumStr, QString InspStaffStr, QString InspEquipment, QString OtherDataStr, QWidget* parent):QWidget(parent)
{
    //setWindowIcon(QIcon("../icons/AccuRep.png"));//���ô���ͼ��
	setWindowTitle(QString::fromLocal8Bit("��ⱨ��"));
    setFixedSize(1260, 1782);

	m_EvaluData = EvaluData;
	m_PartNumStr = PartNumStr;
	m_InspStaffStr = InspStaffStr;
	m_InspEquipment = InspEquipment;
	m_OtherDataStr = OtherDataStr;
	//string str = OtherDataStr.toStdString();
	//string str2 = PartNumStr.toStdString();
	m_pPrint = new QPushButton(QString::fromLocal8Bit("��ӡ��ⱨ��"));
    m_pSave = new QPushButton(QString::fromLocal8Bit("�����ⱨ��"));

    QGridLayout* MainLayout = new QGridLayout(this);

    m_pRepFig = new InsReportFig(EvaluData, PartNumStr, InspStaffStr, InspEquipment, OtherDataStr);//��ⱨ������
    MainLayout->setMargin(0);
    MainLayout->addWidget(m_pRepFig, 0, 0);

    if (m_pRepFig->GetMainHeight() > 1722)
    {
        setFixedSize(1260, m_pRepFig->GetMainHeight() + 60);
    }

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_pPrint);
    buttonLayout->addWidget(m_pSave);
    
    QGroupBox* DownBox = new QGroupBox;
    QGridLayout* DownLayout = new QGridLayout;
    DownLayout->addLayout(buttonLayout, 1, 0, 1, 2);
    DownBox->setLayout(DownLayout);
    MainLayout->addWidget(DownBox, 1, 0);

	connect(m_pSave, SIGNAL(clicked()), this, SLOT(onSaveInsReport()));
    connect(m_pPrint, SIGNAL(clicked()), this, SLOT(onPrintInsReport()));
}

FInsReportDlg::FInsReportDlg()
{

}

FInsReportDlg::~FInsReportDlg()
{
 
}

//static int OutPutCount = 0;
void FInsReportDlg::JL_SaveInsReport(int InspMonth, bool IsWS119)
{
	QString SavePath;

	if (IsWS119)
	{
		SavePath = "C://Users/xcheng/Desktop/WS119/Report/";
	}
	else
	{
		SavePath = "C://Users/xcheng/Desktop/CH300/Report/";
	}

	QString SaveName = RandomTime(InspMonth, 1)[0];

	DrawinspReportForPdf(SavePath + SaveName + ".pdf");
	SaveTxtInsReport(SavePath + SaveName + ".txt");

	//DrawinspReportForPdf(SavePath + QString::number(++OutPutCount) + ".pdf");
	//SaveTxtInsReport(SavePath + QString::number(OutPutCount) + ".txt");

}

void FInsReportDlg::onPrintInsReport()
{
    //QPrinter pter(QPrinter::HighResolution);
    //QPrintPreviewDialog pview(&pter, this);
    //connect(&pview, SIGNAL(paintRequested(QPrinter*)), this, SLOT(printPreview(QPrinter*)));
    //pview.exec();

    QPrinter printer(QPrinter::HighResolution);
    //�Զ���ֽ�Ŵ�С
    printer.setPageSize(QPrinter::Custom);
    printer.setPaperSize(QSizeF(m_pRepFig->height(), m_pRepFig->width()),
        QPrinter::Point);
    QPrintPreviewDialog preview(&printer, m_pRepFig);
    preview.setMinimumSize(1000, 600);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), m_pRepFig, SLOT(printPreviewSlot(QPrinter*)));
    preview.exec();
}

void FInsReportDlg::DrawinspReportForPdf(QString SavedFilepath)
{
	QPrinter printer(QPrinter::PrinterMode::HighResolution);

	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setPageSize(QPrinter::A4);
	printer.setOutputFileName(SavedFilepath);
	printer.setOrientation(QPrinter::Portrait);
	printer.setPageMargins(5, 5, 5, 5, QPrinter::Millimeter);

	QPainter MainPainter;
	MainPainter.begin(&printer);

	/////////////////////////// ���ƶ�������
	// ������������
	QFont font;
	font.setFamily("����");
	font.setBold(true);
	font.setWeight(10);
	font.setUnderline(true);
	MainPainter.setFont(font);

	// ���û�����ɫ
	QPen pen;
	pen.setColor(Qt::green);
	MainPainter.setPen(pen);

	// ��ʼ����
	int FirstRowheight = 600;//��ⱨ���ɫ���Ƹ߶�

	//����һ�м�ⱨ���ɫ����
	MainPainter.setPen(Qt::black);
	MainPainter.setBrush(Qt::black);
	MainPainter.drawRect(0, 0, 9450, FirstRowheight);

	//����һ�м�ⱨ�漸����
	QFont font1("����", 25, QFont::Bold, false);
	MainPainter.setFont(font1);
	MainPainter.setPen(Qt::white);
	MainPainter.drawText(150, FirstRowheight - 135, QString::fromLocal8Bit("��ⱨ��"));

	//���ڶ��н��������ɫ����
	int SecondRowheight = 610;//���������ɫ���Ƹ߶�

	int InvSec = 270;//�������ÿ���ּ��

	if (!m_PartNumStr.isEmpty())
	{
		SecondRowheight += InvSec;
	}

	if (!m_InspStaffStr.isEmpty())
	{
		SecondRowheight += InvSec;
	}

	if (!m_InspEquipment.isEmpty())
	{
		SecondRowheight += InvSec;
	}

	if (!m_OtherDataStr.isEmpty())
	{
		SecondRowheight += InvSec;
	}

	MainPainter.setPen(QColor(147, 149, 152));
	MainPainter.setBrush(QColor(147, 149, 152));
	MainPainter.drawRect(0, FirstRowheight, 9450, FirstRowheight + SecondRowheight);

	//����һ�м�ⱨ�漸�и���
	//int InvSec = 270;//�������ÿ���ּ��
	int FirLoc_y = 890;//���������һ����y����
	int FirLoc_x = 200;//���������һ����x����

	QFont font2("����", 13, QFont::Normal, false);
	MainPainter.setFont(font2);
	MainPainter.setPen(Qt::black);
	//�����ܲ����
	int NbPnts = 0;
	for (int i = 0; i < m_EvaluData.size(); i++)
	{
		for (int j = 0; j < m_EvaluData[i].ThoMesPntList.size(); j++)
		{
			NbPnts += 1;
		}
	}
	MainPainter.drawText(FirLoc_x, FirLoc_y, QString::fromLocal8Bit("�ܲ������") + QString::number(NbPnts));

	MainPainter.drawText(FirLoc_x, FirLoc_y + InvSec, QString::fromLocal8Bit("�����������") + QString::number(m_EvaluData.size()));

	//�����������ϸ���
	int NbQualify = 0;
	vector<int> NoQualifyIndex;
	for (int i = 0; i < m_EvaluData.size(); i++)
	{
		if (m_EvaluData[i].qualify)
		{
			NbQualify += 1;
		}
		else
		{
			NoQualifyIndex.push_back(m_EvaluData[i].p_InsFea->InsFeatureindex);
		}
	}

	MainPainter.drawText(FirLoc_x, FirLoc_y + InvSec * 2, QString::fromLocal8Bit("��������ϸ��ʣ�") + QString::number(double(NbQualify) / double(m_EvaluData.size()) * 100) + tr("%"));

	QString NoQualifyInsFea = QString::fromLocal8Bit("���ϸ���������");
	for (int i = 0; i < NoQualifyIndex.size(); i++)
	{
		NoQualifyInsFea += (QString::fromLocal8Bit("�������") + QString::number(NoQualifyIndex[i]) + tr("  "));
	}
	MainPainter.drawText(FirLoc_x, FirLoc_y + InvSec * 3, NoQualifyInsFea);

	int CurrInd = 3;
	//�������ͼ��
	if (!m_PartNumStr.isEmpty())
	{
		CurrInd += 1;

		QString tmpPartNumber = QString::fromLocal8Bit("���ͼ�ţ�");
		tmpPartNumber += m_PartNumStr;
		MainPainter.drawText(FirLoc_x, FirLoc_y + InvSec * CurrInd, tmpPartNumber);
	}

	//���Ƽ����
	if (!m_InspStaffStr.isEmpty())
	{
		CurrInd += 1;

		QString tmpStrStaff = QString::fromLocal8Bit("����ˣ�");
		tmpStrStaff += m_InspStaffStr;
		MainPainter.drawText(FirLoc_x, FirLoc_y + InvSec * CurrInd, tmpStrStaff);
	}

	//���Ƽ���豸�����
	if (!m_InspEquipment.isEmpty())
	{
		CurrInd += 1;

		QString tmpStrEquipment = QString::fromLocal8Bit("����豸������ţ�");
		tmpStrEquipment += m_InspEquipment;
		MainPainter.drawText(FirLoc_x, FirLoc_y + InvSec * CurrInd, tmpStrEquipment);
	}

	//������������
	if (!m_OtherDataStr.isEmpty())
	{
		CurrInd += 1;

		QString tmpOther = QString::fromLocal8Bit("��ע��");
		tmpOther += m_OtherDataStr;
		MainPainter.drawText(FirLoc_x, FirLoc_y + InvSec * CurrInd, tmpOther);
	}

	///////////////���Ƹ������������ϸ��Ϣ
	int Marginal = 0;
	int RealNameH = 1800;//�������������ϸ��Ϣ����߶�

	RealNameH += (CurrInd - 3) * InvSec;

	int InsFeaNameH = 15;//�ֺţ����������
	int PntCompWordH = 6;//�ֺţ����Ƚ�
	int EvaluResFontH = 7;//������������ֺ�

	int OvewHigh = 45;
	int PntCompHigh = 85;
	int Width = 1260;
	int UserInfoW = 65;
	int TypeH = 25;
	int NbPntperRow = 7;//ÿ�бȽϵĲ����

	double TransMult = 7.5;
	OvewHigh *= TransMult;
	PntCompHigh *= TransMult;
	Width *= TransMult;
	UserInfoW *= TransMult;
	TypeH *= TransMult;

	int lastHeight = 0;
	int InsFeaNameHeight = 500;//����������߶�

	for (int IndFeaEva = 0; IndFeaEva < m_EvaluData.size(); IndFeaEva++)
	{
		EvaluData p_Evalu = m_EvaluData[IndFeaEva];

		int NbRow = p_Evalu.ThoMesPntList.size() / NbPntperRow;//���Ƚ�����
		if (p_Evalu.ThoMesPntList.size() % NbPntperRow != 0)
		{
			NbRow += 1;
		}

		RealNameH = lastHeight + InsFeaNameHeight + RealNameH;

		int Height = NbRow * PntCompHigh + OvewHigh;
		lastHeight = Height;

		if (RealNameH + lastHeight + InsFeaNameHeight > 13522)
		{
			RealNameH = InsFeaNameHeight / 2;
			printer.newPage();
		}

		//���������
		QFont InsFeaNameFont("����", InsFeaNameH, QFont::Normal, false);
		MainPainter.setFont(InsFeaNameFont);
		MainPainter.setPen(Qt::black);
		MainPainter.drawText(5, RealNameH - 70, QString::fromLocal8Bit("�������") + QString::number(p_Evalu.p_InsFea->InsFeatureindex));

		//�����
		MainPainter.setPen(QPen(Qt::black, 2));
		QPointF points1[5] = { QPointF(Marginal, Marginal + RealNameH), QPointF(Width + Marginal, Marginal + RealNameH), QPointF(Width + Marginal, Height + Marginal + RealNameH), QPointF(Marginal, Height + Marginal + RealNameH),QPointF(Marginal, Marginal + RealNameH) };
		MainPainter.drawPolyline(points1, 5);

		//������û�������Ϣ��
		QPointF UserinfoLine1[2] = { QPointF(UserInfoW + Marginal, Marginal + RealNameH), QPointF(UserInfoW + Marginal, Height + Marginal + RealNameH) };
		MainPainter.drawPolyline(UserinfoLine1, 2);//������

		//����Ǽ�⹫����û��ڶ��������ߣ����ߴ����Ҫ
		MainPainter.setPen(Qt::black);
		QPointF UserinfoLine2[2] = { QPointF(Marginal, Marginal + RealNameH + TypeH), QPointF(Marginal + UserInfoW, Marginal + RealNameH + TypeH) };
		MainPainter.drawPolyline(UserinfoLine2, 2);

		int EachH = (NbRow * PntCompHigh + OvewHigh - TypeH) / 3;//�����߳���һ����ÿ���и�
		if (p_Evalu.p_InsFea->type == DISTANCE || p_Evalu.p_InsFea->type == DIAMETER)
		{
			QPointF UserinfoLine3[2] = { QPointF(Marginal, Marginal + RealNameH + TypeH + EachH), QPointF(Marginal + UserInfoW, Marginal + RealNameH + TypeH + EachH) };
			MainPainter.drawPolyline(UserinfoLine3, 2);

			QPointF UserinfoLine4[2] = { QPointF(Marginal, Marginal + RealNameH + TypeH + 2 * EachH), QPointF(Marginal + UserInfoW, Marginal + RealNameH + TypeH + 2 * EachH) };
			MainPainter.drawPolyline(UserinfoLine4, 2);
		}

		//�����·�����������
		QPointF OvewLine[2] = { QPointF(Marginal + UserInfoW, Height + Marginal + RealNameH - OvewHigh), QPointF(Width + Marginal, Height + Marginal + RealNameH - OvewHigh) };
		MainPainter.drawPolyline(OvewLine, 2);

		//д����û�������Ϣ
		//д��һ�м������
		QFont InsTypeFont("Microsoft YaHei", PntCompWordH * 1.3, QFont::Normal, false);
		MainPainter.setFont(InsTypeFont);
		MainPainter.setPen(Qt::black);
		QString insType;
		if (p_Evalu.p_InsFea->type == PARALLELISM)
		{
			insType = QString::fromLocal8Bit("ƽ�ж�");
		}
		else if (p_Evalu.p_InsFea->type == ROUNDNESS)
		{
			insType = QString::fromLocal8Bit("Բ��");
		}
		else if (p_Evalu.p_InsFea->type == DISTANCE)
		{
			if (p_Evalu.p_InsFea->DirState == 1)
			{
				insType = QString::fromLocal8Bit("X�����");
			}
			else if (p_Evalu.p_InsFea->DirState == 2)
			{
				insType = QString::fromLocal8Bit("Y�����");
			}
			else if (p_Evalu.p_InsFea->DirState == 3)
			{
				insType = QString::fromLocal8Bit("Z�����");
			}
			else
			{
				insType = QString::fromLocal8Bit("����");
			}
		}
		else if (p_Evalu.p_InsFea->type == VERTICALITY)
		{
			insType = QString::fromLocal8Bit("��ֱ��");
		}
		else if (p_Evalu.p_InsFea->type == DIAMETER)
		{
			insType = QString::fromLocal8Bit("ֱ��");
		}
		else if (p_Evalu.p_InsFea->type == AUXIALITY)
		{
			insType = QString::fromLocal8Bit("ͬ���");
		}
		else if (p_Evalu.p_InsFea->type == FLATNESS)
		{
			insType = QString::fromLocal8Bit("ƽ���");
		}
		else
		{
			insType = QString::fromLocal8Bit("δ֪");
		}
		MainPainter.drawText(Marginal + 3 * TransMult, Marginal + RealNameH + TypeH - 6 * TransMult, insType);

		//д�ڶ����û�����
		QFont InsTypeFontUser("Microsoft YaHei", PntCompWordH * 1.1, QFont::Normal, false);
		MainPainter.setFont(InsTypeFontUser);
		MainPainter.setPen(Qt::black);

		int DevName_x = 4;//����û�������Ϣ��������ƫ�ƾ���
		int DevValue_x = 2;//����û�������Ϣ��������ƫ�ƾ���
		int DevValue_y = -3;//����û�������Ϣ��������ƫ�ƾ���
		int UpMv = 20;//����û����������������ּ��17
		int Dev_Sig = 10;//���ֻ��һ��ʱ����������ƫ����
		int Dev_Tri = -2;

		DevName_x *= TransMult;
		DevValue_x *= TransMult;
		DevValue_y *= TransMult;
		UpMv *= TransMult;
		Dev_Sig *= TransMult;
		Dev_Tri *= TransMult;

		if (p_Evalu.p_InsFea->type == DISTANCE)
		{
			MainPainter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + EachH - UpMv + Dev_Tri, insType + QString::fromLocal8Bit("��"));
			MainPainter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + EachH + Dev_Tri, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));

			MainPainter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + 2 * EachH - UpMv + Dev_Tri, QString::fromLocal8Bit("��ƫ�"));
			MainPainter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + 2 * EachH + Dev_Tri, tr("+") + QString::number(p_Evalu.p_InsFea->FInsUpDeviationValue) + tr("mm"));

			MainPainter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + 3 * EachH - UpMv + Dev_Tri, QString::fromLocal8Bit("��ƫ�"));
			MainPainter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + 3 * EachH + Dev_Tri, tr("-") + QString::number(p_Evalu.p_InsFea->FInsDownDeviationValue) + tr("mm"));
		}
		else if (p_Evalu.p_InsFea->type == DIAMETER)
		{
			MainPainter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + EachH - UpMv + Dev_Tri, QString::fromLocal8Bit("ֱ����"));
			MainPainter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + EachH + Dev_Tri, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));

			MainPainter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + 2 * EachH - UpMv + Dev_Tri, QString::fromLocal8Bit("��ƫ�"));
			MainPainter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + 2 * EachH + Dev_Tri, tr("+") + QString::number(p_Evalu.p_InsFea->FInsUpDeviationValue) + tr("mm"));

			MainPainter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + 3 * EachH - UpMv + Dev_Tri, QString::fromLocal8Bit("��ƫ�"));
			MainPainter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + 3 * EachH + Dev_Tri, tr("-") + QString::number(p_Evalu.p_InsFea->FInsDownDeviationValue) + tr("mm"));
		}
		else if (p_Evalu.p_InsFea->type == PARALLELISM)
		{
			MainPainter.drawText(DevName_x + Marginal, Dev_Sig + Marginal + RealNameH + TypeH + 1.5 * EachH - UpMv, QString::fromLocal8Bit("ƽ�жȣ�"));
			MainPainter.drawText(DevValue_x + Marginal, Dev_Sig + DevValue_y + Marginal + RealNameH + TypeH + 1.5 * EachH, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));
		}
		else if (p_Evalu.p_InsFea->type == ROUNDNESS)
		{
			MainPainter.drawText(DevName_x + Marginal, Dev_Sig + Marginal + RealNameH + TypeH + 1.5 * EachH - UpMv, QString::fromLocal8Bit("Բ�ȣ�"));
			MainPainter.drawText(DevValue_x + Marginal, Dev_Sig + DevValue_y + Marginal + RealNameH + TypeH + 1.5 * EachH, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));
		}
		else if (p_Evalu.p_InsFea->type == VERTICALITY)
		{
			MainPainter.drawText(DevName_x + Marginal, Dev_Sig + Marginal + RealNameH + TypeH + 1.5 * EachH - UpMv, QString::fromLocal8Bit("��ֱ�ȣ�"));
			MainPainter.drawText(DevValue_x + Marginal, Dev_Sig + DevValue_y + Marginal + RealNameH + TypeH + 1.5 * EachH, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));
		}
		else if (p_Evalu.p_InsFea->type == AUXIALITY)
		{
			MainPainter.drawText(DevName_x + Marginal, Dev_Sig + Marginal + RealNameH + TypeH + 1.5 * EachH - UpMv, QString::fromLocal8Bit("ͬ��ȣ�"));
			MainPainter.drawText(DevValue_x + Marginal, Dev_Sig + DevValue_y + Marginal + RealNameH + TypeH + 1.5 * EachH, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));
		}
		else if (p_Evalu.p_InsFea->type == FLATNESS)
		{
			MainPainter.drawText(DevName_x + Marginal, Dev_Sig + Marginal + RealNameH + TypeH + 1.5 * EachH - UpMv, QString::fromLocal8Bit("ƽ��ȣ�"));
			MainPainter.drawText(DevValue_x + Marginal, Dev_Sig + DevValue_y + Marginal + RealNameH + TypeH + 1.5 * EachH, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));
		}
		else
		{
			cout << "����������ͳ���λ��InsFeaUnitPainter_Rep.cpp,165" << endl;
		}

		//��������ȽϿ�
		int PntCompW = (Width - UserInfoW) / NbPntperRow;//С����
		int perRowH = PntCompHigh / 5;//���Ƚ�ÿ�и߶�
		int DevAll_x = 2;//���Ƚ���Ϣ��������ھֲ�С������ԭ��������ƫ�ƾ���
		int DevAll_y = -3;//���Ƚ���Ϣ��������ھֲ�С������ԭ��������ƫ�ƾ���

		DevAll_x *= TransMult;
		DevAll_y *= TransMult;

		for (int i = 0; i < p_Evalu.ThoMesPntList.size(); i++)
		{
			//�жϸÿ��ڵڼ��ŵڼ��У�һ��NbPntperRow��,���д��㿪ʼ
			int nRow = i / NbPntperRow;
			int nCol = i % NbPntperRow;

			//�������
			MainPainter.setPen(QPen(Qt::black, 2));
			QPointF OutLines[5] = { QPointF(Marginal + UserInfoW + nCol * PntCompW, Marginal + RealNameH + nRow * PntCompHigh), QPointF(Marginal + UserInfoW + PntCompW + nCol * PntCompW, Marginal + RealNameH + nRow * PntCompHigh), QPointF(Marginal + UserInfoW + PntCompW + nCol * PntCompW, Marginal + RealNameH + PntCompHigh + nRow * PntCompHigh), QPointF(Marginal + UserInfoW + nCol * PntCompW, Marginal + RealNameH + PntCompHigh + nRow * PntCompHigh),QPointF(Marginal + UserInfoW + nCol * PntCompW, Marginal + RealNameH + nRow * PntCompHigh) };
			MainPainter.drawPolyline(OutLines, 5);

			//��С���к���
			MainPainter.setPen(Qt::black);
			for (int j = 1; j <= 4; j++)
			{
				QPointF pRowLine[2] = { QPointF(Marginal + UserInfoW + nCol * PntCompW, Marginal + RealNameH + nRow * PntCompHigh + j * perRowH), QPointF(Marginal + UserInfoW + PntCompW + nCol * PntCompW, Marginal + RealNameH + nRow * PntCompHigh + j * perRowH) };
				MainPainter.drawPolyline(pRowLine, 2);
			}

			//��С�������ߣ������0.3��ʾ����ĳһ�����Ϣ�ȽϿ򣬵�����һ��ռ30%��ʣ������65%
			QPointF pColLine[2] = { QPointF(Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, Marginal + RealNameH + nRow * PntCompHigh), QPointF(Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, Marginal + RealNameH + PntCompHigh + nRow * PntCompHigh) };
			MainPainter.drawPolyline(pColLine, 2);

			QPointF pColLine2[2] = { QPointF(Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, Marginal + RealNameH + nRow * PntCompHigh), QPointF(Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, Marginal + RealNameH + PntCompHigh + nRow * PntCompHigh - perRowH) };
			MainPainter.drawPolyline(pColLine2, 2);

			//���ȽϿ��һ����
			QFont PntCopFont("Microsoft YaHei", PntCompWordH, QFont::Normal, false);
			MainPainter.setFont(PntCopFont);
			MainPainter.setPen(Qt::black);
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + perRowH, QString::fromLocal8Bit("��") + QString::number(i));
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 2 * perRowH, QString::fromLocal8Bit("X"));
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 3 * perRowH, QString::fromLocal8Bit("Y"));
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 4 * perRowH, QString::fromLocal8Bit("Z"));
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 5 * perRowH, QString::fromLocal8Bit("���"));

			//�ڶ��У������0.3��ʾ����ĳһ�����Ϣ�ȽϿ򣬵�����һ��ռ30%��ʣ������65%
			//MainPainter.setPen(Qt::blue);
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + perRowH, QString::fromLocal8Bit("����"));
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 2 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].first.X()));
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 3 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].first.Y()));
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 4 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].first.Z()));

			MainPainter.setPen(Qt::black);//�ڶ������һ�ţ���ڶ��������Ų�һ����15��ʾ����Ϊ��һ�����õ�����ƫ����
			MainPainter.drawText(30 + Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 5 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].first.Distance(p_Evalu.ThoMesPntList[i].second)));

			//������
			//MainPainter.setPen(Qt::red);
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + perRowH, QString::fromLocal8Bit("ʵ��"));
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 2 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].second.X()));
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 3 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].second.Y()));
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 4 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].second.Z()));

		}

		//д��������������
		//��һ��
		QFont EvaluFont("Microsoft YaHei", EvaluResFontH, QFont::Normal, false);
		MainPainter.setFont(EvaluFont);
		MainPainter.setPen(Qt::black);
		QString EvaluCont;
		if (p_Evalu.p_InsFea->type == DISTANCE)
		{
			EvaluCont = insType + QString::fromLocal8Bit("��") + QString::number(p_Evalu.DimensionValue) + QString::fromLocal8Bit("mm ����ƫ�+") + QString::number(p_Evalu.upDeviation) + QString::fromLocal8Bit("mm ����ƫ�") + QString::number(p_Evalu.downDeviation) + tr("mm");

		}
		else if (p_Evalu.p_InsFea->type == DIAMETER)
		{
			EvaluCont = QString::fromLocal8Bit("ֱ����") + QString::number(p_Evalu.DimensionValue) + QString::fromLocal8Bit("mm ����ƫ�+") + QString::number(p_Evalu.upDeviation) + QString::fromLocal8Bit("mm ����ƫ�") + QString::number(p_Evalu.downDeviation) + tr("mm");
		}
		else if (p_Evalu.p_InsFea->type == PARALLELISM)
		{
			EvaluCont = QString::fromLocal8Bit("ƽ�жȣ�") + QString::number(p_Evalu.ToleranceValue) + tr("mm");
		}
		else if (p_Evalu.p_InsFea->type == ROUNDNESS)
		{
			EvaluCont = QString::fromLocal8Bit("Բ�ȣ�") + QString::number(p_Evalu.ToleranceValue) + tr("mm");
		}
		else if (p_Evalu.p_InsFea->type == VERTICALITY)
		{
			EvaluCont = QString::fromLocal8Bit("��ֱ�ȣ�") + QString::number(p_Evalu.ToleranceValue) + tr("mm");
		}
		else if (p_Evalu.p_InsFea->type == AUXIALITY)
		{
			EvaluCont = QString::fromLocal8Bit("ͬ��ȣ�") + QString::number(p_Evalu.ToleranceValue) + tr("mm");
		}
		else if (p_Evalu.p_InsFea->type == FLATNESS)
		{
			EvaluCont = QString::fromLocal8Bit("ƽ��ȣ�") + QString::number(p_Evalu.ToleranceValue) + tr("mm");
		}
		else
		{
			cout << "����������ͳ���λ��InsFeaUnitPainter_Rep.cpp,258" << endl;
		}

		int DevRes_x = 5;//������ϸ񲻺ϸ�����ھֲ�����ԭ��x��ƫ����
		int DevRes_y = -6;//������ϸ񲻺ϸ�����ھֲ�����ԭ��y��ƫ����
		int Inval_Res = 25;//���������֮����
		int InspResDevX = 75;//������ϸ񲻺ϸ�����ƫ����

		DevRes_x *= TransMult;
		DevRes_y *= TransMult;
		Inval_Res *= TransMult;
		InspResDevX *= TransMult;

		MainPainter.drawText(DevRes_x + UserInfoW + Marginal, Height + Marginal + RealNameH - Inval_Res, EvaluCont);

		//�ڶ���
		MainPainter.drawText(DevRes_x + UserInfoW + Marginal, Height + Marginal + RealNameH + DevRes_y, QString::fromLocal8Bit("�������"));

		if (p_Evalu.qualify)
		{
			MainPainter.setPen(Qt::green);
			MainPainter.drawText(DevRes_x + UserInfoW + Marginal + InspResDevX, Height + Marginal + RealNameH + DevRes_y, QString::fromLocal8Bit("�ϸ�"));
		}
		else
		{
			MainPainter.setPen(Qt::red);
			MainPainter.drawText(DevRes_x + UserInfoW + Marginal + InspResDevX, Height + Marginal + RealNameH + DevRes_y, QString::fromLocal8Bit("���ϸ�"));
		}
	}

	MainPainter.end();
}

void FInsReportDlg::SaveTxtInsReport(QString SavedFilepath)
{
	QFile file(SavedFilepath);
	if (!file.open(QIODevice::WriteOnly)) 
	{
		QMessageBox::information(QApplication::activeWindow(), QString::fromLocal8Bit("д�����,ERROR!"),
			QString::fromLocal8Bit("�ļ�����д�룡"), "OK");
		return;
	}
	else {
		QTextStream stream(&file);

		//д���ⱨ��ʶ����
		stream << "Inspection report M3699512395" << endl;

		//д���ܲ����
		int NbPnts = 0;
		for (int i = 0; i < m_EvaluData.size(); i++)
		{
			for (int j = 0; j < m_EvaluData[i].ThoMesPntList.size(); j++)
			{
				NbPnts += 1;
			}
		}
		stream << "Number of measure points: " + QString::number(NbPnts) << endl;

		//д����������
		stream << "Number of inspection features: " + QString::number(m_EvaluData.size()) << endl;

		//д�����������ϸ���
		int NbQualify = 0;
		vector<int> NoQualifyIndex;
		for (int i = 0; i < m_EvaluData.size(); i++)
		{
			if (m_EvaluData[i].qualify)
			{
				NbQualify += 1;
			}
			else
			{
				NoQualifyIndex.push_back(m_EvaluData[i].p_InsFea->InsFeatureindex);
			}
		}
		stream << "Qualify rate of inspection features: " + QString::number(double(NbQualify) / double(m_EvaluData.size()) * 100) + tr("%") << endl;

		//д�벻�ϸ�������
		QString NoQualifyInsFea = "Unqualified inspection features: ";
		for (int i = 0; i < NoQualifyIndex.size(); i++)
		{
			if (i == 0)
			{
				NoQualifyInsFea += ("Fea " + QString::number(NoQualifyIndex[i]));
			}
			else
			{
				NoQualifyInsFea += (", Fea " + QString::number(NoQualifyIndex[i]));
			}
		}
		stream << NoQualifyInsFea << endl;

		//д�����ͼ��
		if (!m_PartNumStr.isEmpty())
		{
			QString tmpPartNumber = "Part Number: ";
			tmpPartNumber += m_PartNumStr;

			stream << tmpPartNumber << endl;
		}

		//д������
		if (!m_InspStaffStr.isEmpty())
		{
			QString tmpStrStaff = "Inspection staff: ";
			tmpStrStaff += m_InspStaffStr;

			stream << tmpStrStaff << endl;
		}

		//д�����豸�����
		if (!m_InspEquipment.isEmpty())
		{
			QString tmpStrEquipment = "Inspection equipment and number: ";
			tmpStrEquipment += m_InspEquipment;

			stream << tmpStrEquipment << endl;
		}

		//������������
		if (!m_OtherDataStr.isEmpty())
		{
			QString tmpOther = "Remarks: ";
			tmpOther += m_OtherDataStr;

			stream << tmpOther << endl;
		}

		//д��������
		//д��������
		for (int i = 0; i < m_EvaluData.size(); i++)
		{
			stream << "Inspection feature " + QString::number(i) << endl;

			InsType tmpType = m_EvaluData[i].p_InsFea->type;

			switch (tmpType)
			{
			case PARALLELISM:stream << "Inspection type: Parallelism" << endl;
				break;
			case ROUNDNESS:stream << "Inspection type: Roundness" << endl;
				break;
			case DISTANCE:
				if (m_EvaluData[i].p_InsFea->DirState == 1)
				{
					stream << "Inspection type: DistanceX" << endl;
				}
				else if (m_EvaluData[i].p_InsFea->DirState == 2)
				{
					stream << "Inspection type: DistanceY" << endl;
				}
				else if (m_EvaluData[i].p_InsFea->DirState == 3)
				{
					stream << "Inspection type: DistanceZ" << endl;
				}
				else
				{
					stream << "Inspection type: Distance" << endl;
				}
				break;
			case VERTICALITY:stream << "Inspection type: Verticality" << endl;
				break;
			case DIAMETER:stream << "Inspection type: Diameter" << endl;
				break;
			case AUXIALITY:stream << "Inspection type: Auxiality" << endl;
				break;
			case FLATNESS:stream << "Inspection type: Flatness" << endl;
				break;
			case UNKNOW:stream << "Inspection type: Unknown" << endl;
				break;
			default:
				break;
			}

			//д���û����幫����ֵ
			stream << "User tolerance value: " + QString::number(m_EvaluData[i].p_InsFea->FInsToleranceValue) << endl;

			//д���û�����ߴ繫������ƫ��
			
			bool IsScaleTolerance = (tmpType == DISTANCE) || (tmpType == DIAMETER);
			if (IsScaleTolerance)
			{
				stream << "User up deviation: " + QString::number(m_EvaluData[i].p_InsFea->FInsUpDeviationValue) << endl;
				stream << "User Down deviation: " + QString::number(m_EvaluData[i].p_InsFea->FInsDownDeviationValue) << endl;
			}

			//д��ʵ�⹫����ֵ
			stream << "Inspected tolerance value: " + QString::number(m_EvaluData[i].ToleranceValue) << endl;

			//д��ʵ������ƫ��
			if (IsScaleTolerance)
			{
				stream << "Inspected up deviation: " + QString::number(m_EvaluData[i].upDeviation) << endl;
				stream << "Inspected down deviation: " + QString::number(m_EvaluData[i].downDeviation) << endl;
			}

			//д���Ƿ�ϸ�
			if (m_EvaluData[i].qualify)
			{
				stream << "Is qualify: true"<< endl;
			}
			else
			{
				stream << "Is qualify: false" << endl;
			}

			//д������Ϣ
			stream << "Inspection points list start"<< endl;

			vector<pair<gp_Pnt, gp_Pnt>> tmpThoMesPntList = m_EvaluData[i].ThoMesPntList;
			for (int j = 0; j < tmpThoMesPntList.size(); j++)
			{
				QString tmpThoMesPntStr = "Point " + QString::number(j) + ": ";

				tmpThoMesPntStr += QString::number(tmpThoMesPntList[j].first.X());
				tmpThoMesPntStr += (", " + QString::number(tmpThoMesPntList[j].first.Y()));
				tmpThoMesPntStr += (", " + QString::number(tmpThoMesPntList[j].first.Z()));
				tmpThoMesPntStr += (", " + QString::number(tmpThoMesPntList[j].second.X()));
				tmpThoMesPntStr += (", " + QString::number(tmpThoMesPntList[j].second.Y()));
				tmpThoMesPntStr += (", " + QString::number(tmpThoMesPntList[j].second.Z()));

				tmpThoMesPntStr += (", " + QString::number(tmpThoMesPntList[j].first.Distance(tmpThoMesPntList[j].second)));

				stream << tmpThoMesPntStr << endl;
			}
			stream << "Inspection points list end" << endl;

			stream << "End inspection feature " + QString::number(i) << endl;
		}

		stream.flush();
		file.close();
	}
}

void FInsReportDlg::onSaveInsReport()
{
	QString pdfName = gDlgSetting->value("InspRepSavePath").toString();
	QString pdfPath = QFileDialog::getSaveFileName(this, "Save Pdf", pdfName, "*.pdf");
	if (pdfPath.isEmpty())
		return;
	gDlgSetting->setValue("InspRepSavePath", pdfPath);
	DrawinspReportForPdf(pdfPath);

	// ��PDF
	QDesktopServices::openUrl(QUrl(pdfPath));

}