#include "SetCoorPrecisely.h"
#include <qcombobox.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qfiledialog.h>
#include "np.h"
#include "ToleranceFunction.h"
#include <qvector.h>
#include "FMainWindow.h"

extern FMainWindow* gWindow;

SetCoorPrecisely::SetCoorPrecisely()
{
	InitWIdgets();

	m_CalMode = 0;


}

SetCoorPrecisely::~SetCoorPrecisely()
{



}

void SetCoorPrecisely::onSelDataFile1()
{
	FIpPntList appPnts = gWindow->GetMainDoc()->GetMesPntList();

	for (int i = 0; i < 10; i++)
	{
		m_PlanePntList.push_back(gp_Pnt(appPnts[i].x, appPnts[i].y, appPnts[i].z));
	}
	for (int i = 10; i < appPnts.size(); i++)
	{
		m_CylinderPntList.push_back(gp_Pnt(appPnts[i].x, appPnts[i].y, appPnts[i].z));
	}


	//QString filename = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择测点文件"), QString(QString::fromLocal8Bit("未命名")), tr("Text files (*.txt)"));
	//m_PlanePntList = IpPntToGpPnt(readPoinsFromTXT(filename));
}

void SetCoorPrecisely::onSelDataFile2()
{
	QString filename = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择测点文件"), QString(QString::fromLocal8Bit("未命名")), tr("Text files (*.txt)"));
	m_CylinderPntList = IpPntToGpPnt(readPoinsFromTXT(filename));
}

void SetCoorPrecisely::onCalOrign()
{
	// 获取检测数据
	m_CylinderPntList = IpPntToGpPnt(gWindow->GetMainDoc()->GetMesPntList());

	if (m_CylinderPntList.size() < 6)
	{
		MesgBox(QString::fromLocal8Bit("点数不符合要求"));

		return;
	}

	// 找到圆柱面的轴心点
	vector<gp_Pnt> CenPnt2;
	gp_Pln projPln = gp_Pln(0, 0, 1, -1);

	ToleranceFunction::CylinToCenr(m_CylinderPntList, projPln, CenPnt2);

	if (CenPnt2.size() < 2)
	{
		MesgBox(QString::fromLocal8Bit("点数不符合要求"));

		return;
	}

	double AllX = 0;
	double AllY = 0;
	double AllZ = 0;

	int PntSize = CenPnt2.size();

	for (int i = 0; i < PntSize; i++)
	{
		AllX += CenPnt2[i].X();
		AllY += CenPnt2[i].Y();
	}

	AllX /= PntSize;
	AllY /= PntSize;

	m_pOrignX->setText(QString::number(AllX));
	m_pOrignY->setText(QString::number(AllY));
	m_pOrignZ->setText("----------------");


	//// 拟合平面，假设平面法向量为（0,0,1）
	//int PlaneSize = m_PlanePntList.size();
	//double Allz = 0;
	//for (int i = 0; i < PlaneSize; i++)
	//{
	//	Allz += m_PlanePntList[i].Z();
	//}

	//Allz /= PlaneSize;
	//gp_Pln fitPlane = gp_Pln(0, 0, 1, -Allz);

	//gp_Pnt CenPntCal = gp_Pnt((CenPnt2[0].X() + CenPnt2[1].X()) / 2, (CenPnt2[0].Y() + CenPnt2[1].Y()) / 2, (CenPnt2[0].Z() + CenPnt2[1].Z()) / 2);

	//m_pOrignX->setText(QString::number(CenPntCal.X()));
	//m_pOrignY->setText(QString::number(CenPntCal.Y()));
	//m_pOrignZ->setText(QString::number(Allz));

}

void SetCoorPrecisely::onGetModel(int i)
{
	// 第0行i就是0
	m_CalMode = i;
}

void SetCoorPrecisely::InitWIdgets()
{
	QVBoxLayout* MainLayout = new QVBoxLayout(this);

	QGridLayout* UpLayout1 = new QGridLayout;
	m_pModeSel = new QComboBox;
	m_pModeSel->addItem(QString::fromLocal8Bit("圆柱轴线 - 平面定原点"));
	//m_pModeSel->addItem(QString::fromLocal8Bit("三平面定原点"));
	QPushButton* SelCoorFileBnt1 = new QPushButton(QString::fromLocal8Bit("选择平面测点"));
	QPushButton* SelCoorFileBnt2 = new QPushButton(QString::fromLocal8Bit("选择圆柱测点"));
	QPushButton* CalBnt = new QPushButton(QString::fromLocal8Bit("计算"));
	UpLayout1->addWidget(m_pModeSel, 0, 0);
	//UpLayout1->addWidget(SelCoorFileBnt1, 1, 0);
	//UpLayout1->addWidget(SelCoorFileBnt2, 1, 1);
	UpLayout1->addWidget(CalBnt, 2, 1);
	QGroupBox* Group1 = new QGroupBox(QString::fromLocal8Bit("操作"));
	Group1->setLayout(UpLayout1);
	MainLayout->addWidget(Group1);

	QGridLayout* UpLauout2 = new QGridLayout;
	m_pOrignX = new QLineEdit;
	m_pOrignY = new QLineEdit;
	m_pOrignZ = new QLineEdit;
	UpLauout2->addWidget(new QLabel("X"), 0, 0);
	UpLauout2->addWidget(m_pOrignX, 0, 1);
	UpLauout2->addWidget(new QLabel("Y"), 1, 0);
	UpLauout2->addWidget(m_pOrignY, 1, 1);
	UpLauout2->addWidget(new QLabel("Z"), 2, 0);
	UpLauout2->addWidget(m_pOrignZ, 2, 1);
	QGroupBox* Group2 = new QGroupBox(QString::fromLocal8Bit("原点移动至"));
	Group2->setLayout(UpLauout2);
	MainLayout->addWidget(Group2);

	connect(m_pModeSel, SIGNAL(currentIndexChanged(int)), this, SLOT(onGetModel(int)));
	connect(SelCoorFileBnt1, SIGNAL(clicked()), this, SLOT(onSelDataFile1()));
	connect(SelCoorFileBnt2, SIGNAL(clicked()), this, SLOT(onSelDataFile2()));
	connect(CalBnt, SIGNAL(clicked()), this, SLOT(onCalOrign()));










}