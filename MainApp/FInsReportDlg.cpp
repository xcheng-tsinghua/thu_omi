#include "FInsReportDlg.h"
#include "qpalette.h"
#include "qtoolbar.h"
#include "np.h"

extern QSettings* gDlgSetting;

FInsReportDlg::FInsReportDlg(vector<EvaluData> EvaluData, QString PartNumStr, QString InspStaffStr, QString InspEquipment, QString OtherDataStr, QWidget* parent):QWidget(parent)
{
    //setWindowIcon(QIcon("../icons/AccuRep.png"));//设置窗口图标
	setWindowTitle(QString::fromLocal8Bit("检测报告"));
    setFixedSize(1260, 1782);

	m_EvaluData = EvaluData;
	m_PartNumStr = PartNumStr;
	m_InspStaffStr = InspStaffStr;
	m_InspEquipment = InspEquipment;
	m_OtherDataStr = OtherDataStr;
	//string str = OtherDataStr.toStdString();
	//string str2 = PartNumStr.toStdString();
	m_pPrint = new QPushButton(QString::fromLocal8Bit("打印检测报告"));
    m_pSave = new QPushButton(QString::fromLocal8Bit("保存检测报告"));

    QGridLayout* MainLayout = new QGridLayout(this);

    m_pRepFig = new InsReportFig(EvaluData, PartNumStr, InspStaffStr, InspEquipment, OtherDataStr);//检测报告内容
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
    //自定义纸张大小
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

	/////////////////////////// 绘制顶部总览
	// 设置字体属性
	QFont font;
	font.setFamily("宋体");
	font.setBold(true);
	font.setWeight(10);
	font.setUnderline(true);
	MainPainter.setFont(font);

	// 设置画笔颜色
	QPen pen;
	pen.setColor(Qt::green);
	MainPainter.setPen(pen);

	// 开始绘制
	int FirstRowheight = 600;//检测报告黑色底纹高度

	//画第一行检测报告黑色底纹
	MainPainter.setPen(Qt::black);
	MainPainter.setBrush(Qt::black);
	MainPainter.drawRect(0, 0, 9450, FirstRowheight);

	//画第一行检测报告几个字
	QFont font1("宋体", 25, QFont::Bold, false);
	MainPainter.setFont(font1);
	MainPainter.setPen(Qt::white);
	MainPainter.drawText(150, FirstRowheight - 135, QString::fromLocal8Bit("检测报告"));

	//画第二行结果总览灰色底纹
	int SecondRowheight = 610;//结果总览灰色底纹高度

	int InvSec = 270;//结果总览每行字间距

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

	//画第一行检测报告几行个字
	//int InvSec = 270;//结果总览每行字间距
	int FirLoc_y = 890;//结果总览第一行字y坐标
	int FirLoc_x = 200;//结果总览第一行字x坐标

	QFont font2("宋体", 13, QFont::Normal, false);
	MainPainter.setFont(font2);
	MainPainter.setPen(Qt::black);
	//计算总测点数
	int NbPnts = 0;
	for (int i = 0; i < m_EvaluData.size(); i++)
	{
		for (int j = 0; j < m_EvaluData[i].ThoMesPntList.size(); j++)
		{
			NbPnts += 1;
		}
	}
	MainPainter.drawText(FirLoc_x, FirLoc_y, QString::fromLocal8Bit("总测点数：") + QString::number(NbPnts));

	MainPainter.drawText(FirLoc_x, FirLoc_y + InvSec, QString::fromLocal8Bit("检测特征数：") + QString::number(m_EvaluData.size()));

	//计算检测特征合格率
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

	MainPainter.drawText(FirLoc_x, FirLoc_y + InvSec * 2, QString::fromLocal8Bit("检测特征合格率：") + QString::number(double(NbQualify) / double(m_EvaluData.size()) * 100) + tr("%"));

	QString NoQualifyInsFea = QString::fromLocal8Bit("不合格检测特征：");
	for (int i = 0; i < NoQualifyIndex.size(); i++)
	{
		NoQualifyInsFea += (QString::fromLocal8Bit("检测特征") + QString::number(NoQualifyIndex[i]) + tr("  "));
	}
	MainPainter.drawText(FirLoc_x, FirLoc_y + InvSec * 3, NoQualifyInsFea);

	int CurrInd = 3;
	//绘制零件图号
	if (!m_PartNumStr.isEmpty())
	{
		CurrInd += 1;

		QString tmpPartNumber = QString::fromLocal8Bit("零件图号：");
		tmpPartNumber += m_PartNumStr;
		MainPainter.drawText(FirLoc_x, FirLoc_y + InvSec * CurrInd, tmpPartNumber);
	}

	//绘制检测人
	if (!m_InspStaffStr.isEmpty())
	{
		CurrInd += 1;

		QString tmpStrStaff = QString::fromLocal8Bit("检测人：");
		tmpStrStaff += m_InspStaffStr;
		MainPainter.drawText(FirLoc_x, FirLoc_y + InvSec * CurrInd, tmpStrStaff);
	}

	//绘制检测设备及编号
	if (!m_InspEquipment.isEmpty())
	{
		CurrInd += 1;

		QString tmpStrEquipment = QString::fromLocal8Bit("检测设备名及编号：");
		tmpStrEquipment += m_InspEquipment;
		MainPainter.drawText(FirLoc_x, FirLoc_y + InvSec * CurrInd, tmpStrEquipment);
	}

	//绘制其他数据
	if (!m_OtherDataStr.isEmpty())
	{
		CurrInd += 1;

		QString tmpOther = QString::fromLocal8Bit("备注：");
		tmpOther += m_OtherDataStr;
		MainPainter.drawText(FirLoc_x, FirLoc_y + InvSec * CurrInd, tmpOther);
	}

	///////////////绘制各个检测特征详细信息
	int Marginal = 0;
	int RealNameH = 1800;//决定检测特征详细信息整体高度

	RealNameH += (CurrInd - 3) * InvSec;

	int InsFeaNameH = 15;//字号，检测特征名
	int PntCompWordH = 6;//字号，逐点比较
	int EvaluResFontH = 7;//最后两行评价字号

	int OvewHigh = 45;
	int PntCompHigh = 85;
	int Width = 1260;
	int UserInfoW = 65;
	int TypeH = 25;
	int NbPntperRow = 7;//每行比较的测点数

	double TransMult = 7.5;
	OvewHigh *= TransMult;
	PntCompHigh *= TransMult;
	Width *= TransMult;
	UserInfoW *= TransMult;
	TypeH *= TransMult;

	int lastHeight = 0;
	int InsFeaNameHeight = 500;//检测特征名高度

	for (int IndFeaEva = 0; IndFeaEva < m_EvaluData.size(); IndFeaEva++)
	{
		EvaluData p_Evalu = m_EvaluData[IndFeaEva];

		int NbRow = p_Evalu.ThoMesPntList.size() / NbPntperRow;//测点比较行数
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

		//检测特征名
		QFont InsFeaNameFont("宋体", InsFeaNameH, QFont::Normal, false);
		MainPainter.setFont(InsFeaNameFont);
		MainPainter.setPen(Qt::black);
		MainPainter.drawText(5, RealNameH - 70, QString::fromLocal8Bit("检测特征") + QString::number(p_Evalu.p_InsFea->InsFeatureindex));

		//画外框
		MainPainter.setPen(QPen(Qt::black, 2));
		QPointF points1[5] = { QPointF(Marginal, Marginal + RealNameH), QPointF(Width + Marginal, Marginal + RealNameH), QPointF(Width + Marginal, Height + Marginal + RealNameH), QPointF(Marginal, Height + Marginal + RealNameH),QPointF(Marginal, Marginal + RealNameH) };
		MainPainter.drawPolyline(points1, 5);

		//画左边用户输入信息栏
		QPointF UserinfoLine1[2] = { QPointF(UserInfoW + Marginal, Marginal + RealNameH), QPointF(UserInfoW + Marginal, Height + Marginal + RealNameH) };
		MainPainter.drawPolyline(UserinfoLine1, 2);//画竖线

		//如果是检测公差，不用画第二三条横线，检测尺寸才需要
		MainPainter.setPen(Qt::black);
		QPointF UserinfoLine2[2] = { QPointF(Marginal, Marginal + RealNameH + TypeH), QPointF(Marginal + UserInfoW, Marginal + RealNameH + TypeH) };
		MainPainter.drawPolyline(UserinfoLine2, 2);

		int EachH = (NbRow * PntCompHigh + OvewHigh - TypeH) / 3;//左侧横线除第一行外每行行高
		if (p_Evalu.p_InsFea->type == DISTANCE || p_Evalu.p_InsFea->type == DIAMETER)
		{
			QPointF UserinfoLine3[2] = { QPointF(Marginal, Marginal + RealNameH + TypeH + EachH), QPointF(Marginal + UserInfoW, Marginal + RealNameH + TypeH + EachH) };
			MainPainter.drawPolyline(UserinfoLine3, 2);

			QPointF UserinfoLine4[2] = { QPointF(Marginal, Marginal + RealNameH + TypeH + 2 * EachH), QPointF(Marginal + UserInfoW, Marginal + RealNameH + TypeH + 2 * EachH) };
			MainPainter.drawPolyline(UserinfoLine4, 2);
		}

		//画最下方总览栏横线
		QPointF OvewLine[2] = { QPointF(Marginal + UserInfoW, Height + Marginal + RealNameH - OvewHigh), QPointF(Width + Marginal, Height + Marginal + RealNameH - OvewHigh) };
		MainPainter.drawPolyline(OvewLine, 2);

		//写左侧用户输入信息
		//写第一行检测类型
		QFont InsTypeFont("Microsoft YaHei", PntCompWordH * 1.3, QFont::Normal, false);
		MainPainter.setFont(InsTypeFont);
		MainPainter.setPen(Qt::black);
		QString insType;
		if (p_Evalu.p_InsFea->type == PARALLELISM)
		{
			insType = QString::fromLocal8Bit("平行度");
		}
		else if (p_Evalu.p_InsFea->type == ROUNDNESS)
		{
			insType = QString::fromLocal8Bit("圆度");
		}
		else if (p_Evalu.p_InsFea->type == DISTANCE)
		{
			if (p_Evalu.p_InsFea->DirState == 1)
			{
				insType = QString::fromLocal8Bit("X向距离");
			}
			else if (p_Evalu.p_InsFea->DirState == 2)
			{
				insType = QString::fromLocal8Bit("Y向距离");
			}
			else if (p_Evalu.p_InsFea->DirState == 3)
			{
				insType = QString::fromLocal8Bit("Z向距离");
			}
			else
			{
				insType = QString::fromLocal8Bit("距离");
			}
		}
		else if (p_Evalu.p_InsFea->type == VERTICALITY)
		{
			insType = QString::fromLocal8Bit("垂直度");
		}
		else if (p_Evalu.p_InsFea->type == DIAMETER)
		{
			insType = QString::fromLocal8Bit("直径");
		}
		else if (p_Evalu.p_InsFea->type == AUXIALITY)
		{
			insType = QString::fromLocal8Bit("同轴度");
		}
		else if (p_Evalu.p_InsFea->type == FLATNESS)
		{
			insType = QString::fromLocal8Bit("平面度");
		}
		else
		{
			insType = QString::fromLocal8Bit("未知");
		}
		MainPainter.drawText(Marginal + 3 * TransMult, Marginal + RealNameH + TypeH - 6 * TransMult, insType);

		//写第二行用户输入
		QFont InsTypeFontUser("Microsoft YaHei", PntCompWordH * 1.1, QFont::Normal, false);
		MainPainter.setFont(InsTypeFontUser);
		MainPainter.setPen(Qt::black);

		int DevName_x = 4;//左侧用户输入信息汉字左右偏移距离
		int DevValue_x = 2;//左侧用户输入信息数字左右偏移距离
		int DevValue_y = -3;//左侧用户输入信息数字上下偏移距离
		int UpMv = 20;//左侧用户输入栏上下两排字间距17
		int Dev_Sig = 10;//左侧只有一栏时，内容上下偏移量
		int Dev_Tri = -2;

		DevName_x *= TransMult;
		DevValue_x *= TransMult;
		DevValue_y *= TransMult;
		UpMv *= TransMult;
		Dev_Sig *= TransMult;
		Dev_Tri *= TransMult;

		if (p_Evalu.p_InsFea->type == DISTANCE)
		{
			MainPainter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + EachH - UpMv + Dev_Tri, insType + QString::fromLocal8Bit("："));
			MainPainter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + EachH + Dev_Tri, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));

			MainPainter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + 2 * EachH - UpMv + Dev_Tri, QString::fromLocal8Bit("上偏差："));
			MainPainter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + 2 * EachH + Dev_Tri, tr("+") + QString::number(p_Evalu.p_InsFea->FInsUpDeviationValue) + tr("mm"));

			MainPainter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + 3 * EachH - UpMv + Dev_Tri, QString::fromLocal8Bit("下偏差："));
			MainPainter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + 3 * EachH + Dev_Tri, tr("-") + QString::number(p_Evalu.p_InsFea->FInsDownDeviationValue) + tr("mm"));
		}
		else if (p_Evalu.p_InsFea->type == DIAMETER)
		{
			MainPainter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + EachH - UpMv + Dev_Tri, QString::fromLocal8Bit("直径："));
			MainPainter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + EachH + Dev_Tri, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));

			MainPainter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + 2 * EachH - UpMv + Dev_Tri, QString::fromLocal8Bit("上偏差："));
			MainPainter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + 2 * EachH + Dev_Tri, tr("+") + QString::number(p_Evalu.p_InsFea->FInsUpDeviationValue) + tr("mm"));

			MainPainter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + 3 * EachH - UpMv + Dev_Tri, QString::fromLocal8Bit("下偏差："));
			MainPainter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + 3 * EachH + Dev_Tri, tr("-") + QString::number(p_Evalu.p_InsFea->FInsDownDeviationValue) + tr("mm"));
		}
		else if (p_Evalu.p_InsFea->type == PARALLELISM)
		{
			MainPainter.drawText(DevName_x + Marginal, Dev_Sig + Marginal + RealNameH + TypeH + 1.5 * EachH - UpMv, QString::fromLocal8Bit("平行度："));
			MainPainter.drawText(DevValue_x + Marginal, Dev_Sig + DevValue_y + Marginal + RealNameH + TypeH + 1.5 * EachH, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));
		}
		else if (p_Evalu.p_InsFea->type == ROUNDNESS)
		{
			MainPainter.drawText(DevName_x + Marginal, Dev_Sig + Marginal + RealNameH + TypeH + 1.5 * EachH - UpMv, QString::fromLocal8Bit("圆度："));
			MainPainter.drawText(DevValue_x + Marginal, Dev_Sig + DevValue_y + Marginal + RealNameH + TypeH + 1.5 * EachH, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));
		}
		else if (p_Evalu.p_InsFea->type == VERTICALITY)
		{
			MainPainter.drawText(DevName_x + Marginal, Dev_Sig + Marginal + RealNameH + TypeH + 1.5 * EachH - UpMv, QString::fromLocal8Bit("垂直度："));
			MainPainter.drawText(DevValue_x + Marginal, Dev_Sig + DevValue_y + Marginal + RealNameH + TypeH + 1.5 * EachH, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));
		}
		else if (p_Evalu.p_InsFea->type == AUXIALITY)
		{
			MainPainter.drawText(DevName_x + Marginal, Dev_Sig + Marginal + RealNameH + TypeH + 1.5 * EachH - UpMv, QString::fromLocal8Bit("同轴度："));
			MainPainter.drawText(DevValue_x + Marginal, Dev_Sig + DevValue_y + Marginal + RealNameH + TypeH + 1.5 * EachH, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));
		}
		else if (p_Evalu.p_InsFea->type == FLATNESS)
		{
			MainPainter.drawText(DevName_x + Marginal, Dev_Sig + Marginal + RealNameH + TypeH + 1.5 * EachH - UpMv, QString::fromLocal8Bit("平面度："));
			MainPainter.drawText(DevValue_x + Marginal, Dev_Sig + DevValue_y + Marginal + RealNameH + TypeH + 1.5 * EachH, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));
		}
		else
		{
			cout << "检测特征类型出错，位于InsFeaUnitPainter_Rep.cpp,165" << endl;
		}

		//画逐点误差比较框
		int PntCompW = (Width - UserInfoW) / NbPntperRow;//小框宽度
		int perRowH = PntCompHigh / 5;//逐点比较每行高度
		int DevAll_x = 2;//逐点比较信息相对于所在局部小格区域原点左右向偏移距离
		int DevAll_y = -3;//逐点比较信息相对于所在局部小格区域原点上下向偏移距离

		DevAll_x *= TransMult;
		DevAll_y *= TransMult;

		for (int i = 0; i < p_Evalu.ThoMesPntList.size(); i++)
		{
			//判断该框在第几排第几列，一排NbPntperRow个,行列从零开始
			int nRow = i / NbPntperRow;
			int nCol = i % NbPntperRow;

			//画最外框
			MainPainter.setPen(QPen(Qt::black, 2));
			QPointF OutLines[5] = { QPointF(Marginal + UserInfoW + nCol * PntCompW, Marginal + RealNameH + nRow * PntCompHigh), QPointF(Marginal + UserInfoW + PntCompW + nCol * PntCompW, Marginal + RealNameH + nRow * PntCompHigh), QPointF(Marginal + UserInfoW + PntCompW + nCol * PntCompW, Marginal + RealNameH + PntCompHigh + nRow * PntCompHigh), QPointF(Marginal + UserInfoW + nCol * PntCompW, Marginal + RealNameH + PntCompHigh + nRow * PntCompHigh),QPointF(Marginal + UserInfoW + nCol * PntCompW, Marginal + RealNameH + nRow * PntCompHigh) };
			MainPainter.drawPolyline(OutLines, 5);

			//画小框中横线
			MainPainter.setPen(Qt::black);
			for (int j = 1; j <= 4; j++)
			{
				QPointF pRowLine[2] = { QPointF(Marginal + UserInfoW + nCol * PntCompW, Marginal + RealNameH + nRow * PntCompHigh + j * perRowH), QPointF(Marginal + UserInfoW + PntCompW + nCol * PntCompW, Marginal + RealNameH + nRow * PntCompHigh + j * perRowH) };
				MainPainter.drawPolyline(pRowLine, 2);
			}

			//画小框中竖线，这里的0.3表示对于某一点的信息比较框，点名那一列占30%，剩余两列65%
			QPointF pColLine[2] = { QPointF(Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, Marginal + RealNameH + nRow * PntCompHigh), QPointF(Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, Marginal + RealNameH + PntCompHigh + nRow * PntCompHigh) };
			MainPainter.drawPolyline(pColLine, 2);

			QPointF pColLine2[2] = { QPointF(Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, Marginal + RealNameH + nRow * PntCompHigh), QPointF(Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, Marginal + RealNameH + PntCompHigh + nRow * PntCompHigh - perRowH) };
			MainPainter.drawPolyline(pColLine2, 2);

			//逐点比较框第一列字
			QFont PntCopFont("Microsoft YaHei", PntCompWordH, QFont::Normal, false);
			MainPainter.setFont(PntCopFont);
			MainPainter.setPen(Qt::black);
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + perRowH, QString::fromLocal8Bit("点") + QString::number(i));
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 2 * perRowH, QString::fromLocal8Bit("X"));
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 3 * perRowH, QString::fromLocal8Bit("Y"));
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 4 * perRowH, QString::fromLocal8Bit("Z"));
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 5 * perRowH, QString::fromLocal8Bit("误差"));

			//第二列，这里的0.3表示对于某一点的信息比较框，点名那一列占30%，剩余两列65%
			//MainPainter.setPen(Qt::blue);
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + perRowH, QString::fromLocal8Bit("理论"));
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 2 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].first.X()));
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 3 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].first.Y()));
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 4 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].first.Z()));

			MainPainter.setPen(Qt::black);//第二列最后一排，与第二列其它排不一样，15表示单独为这一列设置的向左偏移量
			MainPainter.drawText(30 + Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 5 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].first.Distance(p_Evalu.ThoMesPntList[i].second)));

			//第三列
			//MainPainter.setPen(Qt::red);
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + perRowH, QString::fromLocal8Bit("实际"));
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 2 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].second.X()));
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 3 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].second.Y()));
			MainPainter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 4 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].second.Z()));

		}

		//写最下面两行评价
		//第一行
		QFont EvaluFont("Microsoft YaHei", EvaluResFontH, QFont::Normal, false);
		MainPainter.setFont(EvaluFont);
		MainPainter.setPen(Qt::black);
		QString EvaluCont;
		if (p_Evalu.p_InsFea->type == DISTANCE)
		{
			EvaluCont = insType + QString::fromLocal8Bit("：") + QString::number(p_Evalu.DimensionValue) + QString::fromLocal8Bit("mm ，上偏差：+") + QString::number(p_Evalu.upDeviation) + QString::fromLocal8Bit("mm ，下偏差：") + QString::number(p_Evalu.downDeviation) + tr("mm");

		}
		else if (p_Evalu.p_InsFea->type == DIAMETER)
		{
			EvaluCont = QString::fromLocal8Bit("直径：") + QString::number(p_Evalu.DimensionValue) + QString::fromLocal8Bit("mm ，上偏差：+") + QString::number(p_Evalu.upDeviation) + QString::fromLocal8Bit("mm ，下偏差：") + QString::number(p_Evalu.downDeviation) + tr("mm");
		}
		else if (p_Evalu.p_InsFea->type == PARALLELISM)
		{
			EvaluCont = QString::fromLocal8Bit("平行度：") + QString::number(p_Evalu.ToleranceValue) + tr("mm");
		}
		else if (p_Evalu.p_InsFea->type == ROUNDNESS)
		{
			EvaluCont = QString::fromLocal8Bit("圆度：") + QString::number(p_Evalu.ToleranceValue) + tr("mm");
		}
		else if (p_Evalu.p_InsFea->type == VERTICALITY)
		{
			EvaluCont = QString::fromLocal8Bit("垂直度：") + QString::number(p_Evalu.ToleranceValue) + tr("mm");
		}
		else if (p_Evalu.p_InsFea->type == AUXIALITY)
		{
			EvaluCont = QString::fromLocal8Bit("同轴度：") + QString::number(p_Evalu.ToleranceValue) + tr("mm");
		}
		else if (p_Evalu.p_InsFea->type == FLATNESS)
		{
			EvaluCont = QString::fromLocal8Bit("平面度：") + QString::number(p_Evalu.ToleranceValue) + tr("mm");
		}
		else
		{
			cout << "检测特征类型出错，位于InsFeaUnitPainter_Rep.cpp,258" << endl;
		}

		int DevRes_x = 5;//检测结果合格不合格相对于局部区域原点x轴偏移量
		int DevRes_y = -6;//检测结果合格不合格相对于局部区域原点y轴偏移量
		int Inval_Res = 25;//检测结果两排之间间距
		int InspResDevX = 75;//检测结果合格不合格向右偏移量

		DevRes_x *= TransMult;
		DevRes_y *= TransMult;
		Inval_Res *= TransMult;
		InspResDevX *= TransMult;

		MainPainter.drawText(DevRes_x + UserInfoW + Marginal, Height + Marginal + RealNameH - Inval_Res, EvaluCont);

		//第二行
		MainPainter.drawText(DevRes_x + UserInfoW + Marginal, Height + Marginal + RealNameH + DevRes_y, QString::fromLocal8Bit("检测结果："));

		if (p_Evalu.qualify)
		{
			MainPainter.setPen(Qt::green);
			MainPainter.drawText(DevRes_x + UserInfoW + Marginal + InspResDevX, Height + Marginal + RealNameH + DevRes_y, QString::fromLocal8Bit("合格"));
		}
		else
		{
			MainPainter.setPen(Qt::red);
			MainPainter.drawText(DevRes_x + UserInfoW + Marginal + InspResDevX, Height + Marginal + RealNameH + DevRes_y, QString::fromLocal8Bit("不合格"));
		}
	}

	MainPainter.end();
}

void FInsReportDlg::SaveTxtInsReport(QString SavedFilepath)
{
	QFile file(SavedFilepath);
	if (!file.open(QIODevice::WriteOnly)) 
	{
		QMessageBox::information(QApplication::activeWindow(), QString::fromLocal8Bit("写入错误,ERROR!"),
			QString::fromLocal8Bit("文件不可写入！"), "OK");
		return;
	}
	else {
		QTextStream stream(&file);

		//写入检测报告识别码
		stream << "Inspection report M3699512395" << endl;

		//写入总测点数
		int NbPnts = 0;
		for (int i = 0; i < m_EvaluData.size(); i++)
		{
			for (int j = 0; j < m_EvaluData[i].ThoMesPntList.size(); j++)
			{
				NbPnts += 1;
			}
		}
		stream << "Number of measure points: " + QString::number(NbPnts) << endl;

		//写入检测特征数
		stream << "Number of inspection features: " + QString::number(m_EvaluData.size()) << endl;

		//写入检测特征检测合格率
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

		//写入不合格检测特征
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

		//写入零件图号
		if (!m_PartNumStr.isEmpty())
		{
			QString tmpPartNumber = "Part Number: ";
			tmpPartNumber += m_PartNumStr;

			stream << tmpPartNumber << endl;
		}

		//写入检测人
		if (!m_InspStaffStr.isEmpty())
		{
			QString tmpStrStaff = "Inspection staff: ";
			tmpStrStaff += m_InspStaffStr;

			stream << tmpStrStaff << endl;
		}

		//写入检测设备及编号
		if (!m_InspEquipment.isEmpty())
		{
			QString tmpStrEquipment = "Inspection equipment and number: ";
			tmpStrEquipment += m_InspEquipment;

			stream << tmpStrEquipment << endl;
		}

		//绘制其他数据
		if (!m_OtherDataStr.isEmpty())
		{
			QString tmpOther = "Remarks: ";
			tmpOther += m_OtherDataStr;

			stream << tmpOther << endl;
		}

		//写入检测特征
		//写入检测类型
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

			//写入用户定义公差数值
			stream << "User tolerance value: " + QString::number(m_EvaluData[i].p_InsFea->FInsToleranceValue) << endl;

			//写入用户定义尺寸公差上下偏差
			
			bool IsScaleTolerance = (tmpType == DISTANCE) || (tmpType == DIAMETER);
			if (IsScaleTolerance)
			{
				stream << "User up deviation: " + QString::number(m_EvaluData[i].p_InsFea->FInsUpDeviationValue) << endl;
				stream << "User Down deviation: " + QString::number(m_EvaluData[i].p_InsFea->FInsDownDeviationValue) << endl;
			}

			//写入实测公差数值
			stream << "Inspected tolerance value: " + QString::number(m_EvaluData[i].ToleranceValue) << endl;

			//写入实测上下偏差
			if (IsScaleTolerance)
			{
				stream << "Inspected up deviation: " + QString::number(m_EvaluData[i].upDeviation) << endl;
				stream << "Inspected down deviation: " + QString::number(m_EvaluData[i].downDeviation) << endl;
			}

			//写入是否合格
			if (m_EvaluData[i].qualify)
			{
				stream << "Is qualify: true"<< endl;
			}
			else
			{
				stream << "Is qualify: false" << endl;
			}

			//写入测点信息
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

	// 打开PDF
	QDesktopServices::openUrl(QUrl(pdfPath));

}