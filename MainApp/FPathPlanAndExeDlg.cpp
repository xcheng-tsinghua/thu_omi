#include "MainDoc.h"
#include"MainWindow.h"
#include <QDebug>
#include <iostream>
#include <Eigen\Dense>
#include "../External/fkernel/Include/FKernelHeader.h"
#include "../External/fkernel/Include/DataStructure/FBody.h"
#include "FPathPlanAndExeDlg.h"
#include "occConverter.h"
#include "np.h"
#include "FCodeWnd.h"
#include <memory>

extern FMainWindow* gWindow;
extern QSettings* gDlgSetting;

static QString NC_InspectionSaveDir;

FPathPlanAndExeDlg::FPathPlanAndExeDlg(QWidget* parent)
{
	// 初始化全局静态变量
	QDir SaveDir = gAppPath;
	SaveDir.cdUp();
	SaveDir.cd("NC_Inspection");
	NC_InspectionSaveDir = SaveDir.absolutePath();

	// 初始化其它参数
	m_pPathPlan = NULL;
	IsShortestPathPlan = 1;
	m_pInspPathShow = new PathAllEntity;
	gWindow->GetMainDoc()->AddDispEnt(m_pInspPathShow);

	// 初始化控件指针
	m_onCancle = new QPushButton(QString::fromLocal8Bit("取消"));
	m_onPathPlan = new QPushButton(QString::fromLocal8Bit("执行规划"));
	m_onExport = new QPushButton(QString::fromLocal8Bit("生成检测NC"));
	m_DataTransfer = new QPushButton(QString::fromLocal8Bit("NC代码传输"));
	m_onExport->setEnabled(0);
	m_DataTransfer->setEnabled(0);
	m_pProgressBar = new QProgressBar;
	m_pProgressBar->setRange(0, 100);
	m_pProgressBar->setValue(0);

	QLabel* label7 = new QLabel(this);
	QPixmap img("../icons/PathPlanPara.png");
	label7->setPixmap(img);

	QLabel* labelG54Set = new QLabel(this);
	QPixmap imgG54("../icons/SetG54.png");
	labelG54Set->setPixmap(imgG54);

	QLabel* label8 = new QLabel(this);
	label8->setText(QString::fromLocal8Bit("回退距离/mm"));
	m_back = new QLineEdit(this);
	m_back->setText(QString::number(10));

	QLabel* label9 = new QLabel(this);
	label9->setText(QString::fromLocal8Bit("安全距离/mm"));
	m_safe = new QLineEdit(this);
	m_safe->setText(QString::number(10));

	FastType = new QRadioButton(QString::fromLocal8Bit("最短"));
	ShortType = new QRadioButton(QString::fromLocal8Bit("快速"));

	QLabel* labelProbeOn = new QLabel(QString::fromLocal8Bit("测头开启指令"));
	QLabel* labelProbeOff = new QLabel(QString::fromLocal8Bit("测头关闭指令"));

	m_ProbeOnLineEdit = new QLineEdit(this);
	m_ProbeOffLineEdit = new QLineEdit(this);
	m_ProbeOnLineEdit->setText(gDlgSetting->value("ProbeOnCmd").toString());
	m_ProbeOffLineEdit->setText(gDlgSetting->value("ProbeOffCmd").toString());

	// 窗口布局
	setWindowIcon(QIcon("../icons/CoordTrans.png"));
	this->move(1200, 55);
	this->setWindowFlags(Qt::WindowStaysOnTopHint);//永远在其他窗口之上
	this->setWindowTitle(QString::fromLocal8Bit("路径规划设置及执行"));

	QGroupBox* firstGroup = new QGroupBox(this);
	firstGroup->setTitle(QString::fromLocal8Bit("路径规划参数示意图"));
	QHBoxLayout* box1 = new QHBoxLayout();
	box1->addWidget(label7);
	firstGroup->setLayout(box1);

	QGroupBox* G54SetGroup = new QGroupBox(this);
	G54SetGroup->setTitle(QString::fromLocal8Bit("G54 坐标系设定示意图"));
	QHBoxLayout* boxG54 = new QHBoxLayout();
	boxG54->addWidget(labelG54Set);
	G54SetGroup->setLayout(boxG54);

	QVBoxLayout* mainVBoxLayout = new QVBoxLayout(this);
	//QGroupBox* thirdGroup = new QGroupBox(QString::fromLocal8Bit("坐标系变换操作"));
	//QHBoxLayout* box2 = new QHBoxLayout();
	//thirdGroup->setLayout(box2);

	QGroupBox* fourthGroup = new QGroupBox(QString::fromLocal8Bit("路径规划参数"));
	QGridLayout* mainGridLayout2 = new QGridLayout();
	mainGridLayout2->addWidget(label8, 1, 0);
	mainGridLayout2->addWidget(m_back, 1, 1);
	mainGridLayout2->addWidget(label9, 2, 0);
	mainGridLayout2->addWidget(m_safe, 2, 1);
	fourthGroup->setLayout(mainGridLayout2);

	QGroupBox* fifthGroup = new QGroupBox(QString::fromLocal8Bit("路径规划操作"));
	QHBoxLayout* box3 = new QHBoxLayout(fifthGroup);

	m_pButtonGroup = new QButtonGroup;
	QGroupBox* MTtype_Group = new QGroupBox(QString::fromLocal8Bit("路径规划方式"));
	QHBoxLayout* MT_Layout = new QHBoxLayout();
	m_pButtonGroup->addButton(FastType);
	m_pButtonGroup->addButton(ShortType);
	MT_Layout->addWidget(FastType);
	MT_Layout->addWidget(ShortType);
	MTtype_Group->setLayout(MT_Layout);
	FastType->setChecked(true);

	box3->addWidget(m_onPathPlan);
	box3->addWidget(m_pProgressBar);
	//box3->addStretch(1);
	box3->addWidget(MTtype_Group);

	QGroupBox* NC_CodeGroup = new QGroupBox(QString::fromLocal8Bit("NC代码设置"));
	QVBoxLayout* NC_CodeGroupVLayout = new QVBoxLayout(NC_CodeGroup);

	QGridLayout* NC_UpLayGLayout = new QGridLayout;
	NC_UpLayGLayout->addWidget(labelProbeOn, 0, 0);
	NC_UpLayGLayout->addWidget(m_ProbeOnLineEdit, 0, 1);
	NC_UpLayGLayout->addWidget(labelProbeOff, 1, 0);
	NC_UpLayGLayout->addWidget(m_ProbeOffLineEdit, 1, 1);

	QHBoxLayout* NC_DownHLayout = new QHBoxLayout;
	NC_DownHLayout->addWidget(m_onExport);
	NC_DownHLayout->addWidget(m_DataTransfer);
	NC_DownHLayout->addStretch(1);
	NC_DownHLayout->addWidget(m_onCancle);

	NC_CodeGroupVLayout->addLayout(NC_UpLayGLayout);
	NC_CodeGroupVLayout->addLayout(NC_DownHLayout);

	mainVBoxLayout->addWidget(fourthGroup);
	mainVBoxLayout->addWidget(firstGroup);
	mainVBoxLayout->addWidget(fifthGroup);
	mainVBoxLayout->addWidget(G54SetGroup);
	mainVBoxLayout->addWidget(NC_CodeGroup);

	connect(m_onCancle, SIGNAL(clicked()), this, SLOT(onCancel()));
	connect(m_onPathPlan, SIGNAL(clicked()), this, SLOT(onPathPlan()));
	connect(m_onExport, SIGNAL(clicked()), this, SLOT(onExport()));
	connect(m_DataTransfer, SIGNAL(clicked()), this, SLOT(onDataTransfer()));
	connect(m_pButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));

}

FPathPlanAndExeDlg::~FPathPlanAndExeDlg()
{

}

void FPathPlanAndExeDlg::onCancel()
{
	this->close();
}

QProgressBar* FPathPlanAndExeDlg::getProgress()
{
	return m_pProgressBar;
}

void FPathPlanAndExeDlg::onPathPlan()
{
	m_pProgressBar->setValue(0);

	// 设定参数
	if (!m_pPathPlan)
	{
		m_pPathPlan = new FPathPlan(gWindow->GetMainDoc());
	}

	m_pPathPlan->SetBackDist(m_back->text().toDouble());
	m_pPathPlan->SetSaftDist(m_safe->text().toDouble());

	// 路径规划
	int nsize = gWindow->GetMainDoc()->DispEntCount();
	if (nsize == 0)
	{
		QMessageBox::warning(QApplication::activeWindow(), QString::fromLocal8Bit("WARINING"), QString::fromLocal8Bit("未导入零件模型"), "OK");
		return;
	}

	m_pPathPlan->setPathPlanType(IsShortestPathPlan);

	m_pPathPlan->SetIpPntList(gWindow->GetMainDoc()->GetIpPntList());
	FIntList orders;
	gWindow->GetMainDoc()->ClearIpPathList();
	m_pPathPlan->Planning(orders, gWindow->GetMainDoc()->GetPathList());

	gWindow->UpdateModelView();

	m_onExport->setEnabled(1);
	m_DataTransfer->setEnabled(1);

	m_pProgressBar->setValue(100);

}

void FPathPlanAndExeDlg::onExport()
{
	if (gWindow->GetMainDoc()->GetPathList().isEmpty())
		return;

	FTHP* pThp = new FTHP();

	gDlgSetting->setValue("ProbeOnCmd", m_ProbeOnLineEdit->text());
	gDlgSetting->setValue("ProbeOffCmd", m_ProbeOffLineEdit->text());

	pThp->TH_NCFileWrite(gWindow->GetMainDoc()->GetPathList());
	gWindow->GetMainDoc()->ClearIpPntList();
	gWindow->GetMainDoc()->SetIpPntList(pThp->TH_GetPntsOrder());

	QString filename = NC_InspectionSaveDir + "/Inspection_Task.MPF";

	// 昌飞检测NC代码需在Meas之前暂停
	pThp->setIsPauseBeforeMeas(true);
	pThp->TH_NCFileWrite(gWindow->GetMainDoc()->GetPathList(), filename);

	m_NC_CodeBody = pThp->GetNCCodeBody();

	// 设置显示NC代码
	reinterpret_cast<FCodeWnd*>(gWindow->GetSubWnd()[2])->getTextEdit()->setPlainText(pThp->GetSaveToMachineToolLocalHead() + pThp->TH_GetHead() + m_NC_CodeBody + pThp->TH_GetTail() + pThp->GetSaveToMachineToolLocalLast());

	delete pThp;

}

void FPathPlanAndExeDlg::onButtonClicked(QAbstractButton* button)
{
	if (!m_pPathPlan)
	{
		m_pPathPlan = new FPathPlan(gWindow->GetMainDoc());

	}

	if (button->text() == QString::fromLocal8Bit("最短"))
	{
		IsShortestPathPlan = 1;
	}
	else if (button->text() == QString::fromLocal8Bit("快速"))
	{
		IsShortestPathPlan = 0;
	}
	else
	{
		QMessageBox::warning(
			QApplication::activeWindow(), QObject::tr("Caution"), QString::fromLocal8Bit("快速、最短路径规划方式设置错误，默认快速"));
	}

}

void FPathPlanAndExeDlg::onDataTransfer()
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(NC_InspectionSaveDir));

	// 创建警告消息框
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Warning);
	msgBox.setText(QString::fromLocal8Bit("请使用文件传输系统将标定用NC代码传输至机床并执行，NC执行前需要模型坐标系与机床坐标系一致。"));
	msgBox.setStandardButtons(QMessageBox::Ok);
	// 设置消息框始终位于其他窗口之上
	msgBox.setWindowFlags(msgBox.windowFlags() | Qt::WindowStaysOnTopHint);
	msgBox.exec();

}

void PathAllEntity::Draw(const FT::FRenderOpt& renderOpt)
{
	glDisable(GL_LIGHTING);

	FIpPathList	pathList = gWindow->GetMainDoc()->GetPathList();

	glColor3f(1.0f, 0.0f, 0.0f);
	for (int i = 0; i < pathList.size(); i++)
	{
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j < pathList[i].GetSize(); j++)
			glVertex3d(pathList[i].GetPathPnts()[j].x, pathList[i].GetPathPnts()[j].y, pathList[i].GetPathPnts()[j].z);
		glEnd();
	}

	glEnable(GL_LIGHTING);

}
