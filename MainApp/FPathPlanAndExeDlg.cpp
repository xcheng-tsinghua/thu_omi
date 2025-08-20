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
	// ��ʼ��ȫ�־�̬����
	QDir SaveDir = gAppPath;
	SaveDir.cdUp();
	SaveDir.cd("NC_Inspection");
	NC_InspectionSaveDir = SaveDir.absolutePath();

	// ��ʼ����������
	m_pPathPlan = NULL;
	IsShortestPathPlan = 1;
	m_pInspPathShow = new PathAllEntity;
	gWindow->GetMainDoc()->AddDispEnt(m_pInspPathShow);

	// ��ʼ���ؼ�ָ��
	m_onCancle = new QPushButton(QString::fromLocal8Bit("ȡ��"));
	m_onPathPlan = new QPushButton(QString::fromLocal8Bit("ִ�й滮"));
	m_onExport = new QPushButton(QString::fromLocal8Bit("���ɼ��NC"));
	m_DataTransfer = new QPushButton(QString::fromLocal8Bit("NC���봫��"));
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
	label8->setText(QString::fromLocal8Bit("���˾���/mm"));
	m_back = new QLineEdit(this);
	m_back->setText(QString::number(10));

	QLabel* label9 = new QLabel(this);
	label9->setText(QString::fromLocal8Bit("��ȫ����/mm"));
	m_safe = new QLineEdit(this);
	m_safe->setText(QString::number(10));

	FastType = new QRadioButton(QString::fromLocal8Bit("���"));
	ShortType = new QRadioButton(QString::fromLocal8Bit("����"));

	QLabel* labelProbeOn = new QLabel(QString::fromLocal8Bit("��ͷ����ָ��"));
	QLabel* labelProbeOff = new QLabel(QString::fromLocal8Bit("��ͷ�ر�ָ��"));

	m_ProbeOnLineEdit = new QLineEdit(this);
	m_ProbeOffLineEdit = new QLineEdit(this);
	m_ProbeOnLineEdit->setText(gDlgSetting->value("ProbeOnCmd").toString());
	m_ProbeOffLineEdit->setText(gDlgSetting->value("ProbeOffCmd").toString());

	// ���ڲ���
	setWindowIcon(QIcon("../icons/CoordTrans.png"));
	this->move(1200, 55);
	this->setWindowFlags(Qt::WindowStaysOnTopHint);//��Զ����������֮��
	this->setWindowTitle(QString::fromLocal8Bit("·���滮���ü�ִ��"));

	QGroupBox* firstGroup = new QGroupBox(this);
	firstGroup->setTitle(QString::fromLocal8Bit("·���滮����ʾ��ͼ"));
	QHBoxLayout* box1 = new QHBoxLayout();
	box1->addWidget(label7);
	firstGroup->setLayout(box1);

	QGroupBox* G54SetGroup = new QGroupBox(this);
	G54SetGroup->setTitle(QString::fromLocal8Bit("G54 ����ϵ�趨ʾ��ͼ"));
	QHBoxLayout* boxG54 = new QHBoxLayout();
	boxG54->addWidget(labelG54Set);
	G54SetGroup->setLayout(boxG54);

	QVBoxLayout* mainVBoxLayout = new QVBoxLayout(this);
	//QGroupBox* thirdGroup = new QGroupBox(QString::fromLocal8Bit("����ϵ�任����"));
	//QHBoxLayout* box2 = new QHBoxLayout();
	//thirdGroup->setLayout(box2);

	QGroupBox* fourthGroup = new QGroupBox(QString::fromLocal8Bit("·���滮����"));
	QGridLayout* mainGridLayout2 = new QGridLayout();
	mainGridLayout2->addWidget(label8, 1, 0);
	mainGridLayout2->addWidget(m_back, 1, 1);
	mainGridLayout2->addWidget(label9, 2, 0);
	mainGridLayout2->addWidget(m_safe, 2, 1);
	fourthGroup->setLayout(mainGridLayout2);

	QGroupBox* fifthGroup = new QGroupBox(QString::fromLocal8Bit("·���滮����"));
	QHBoxLayout* box3 = new QHBoxLayout(fifthGroup);

	m_pButtonGroup = new QButtonGroup;
	QGroupBox* MTtype_Group = new QGroupBox(QString::fromLocal8Bit("·���滮��ʽ"));
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

	QGroupBox* NC_CodeGroup = new QGroupBox(QString::fromLocal8Bit("NC��������"));
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

	// �趨����
	if (!m_pPathPlan)
	{
		m_pPathPlan = new FPathPlan(gWindow->GetMainDoc());
	}

	m_pPathPlan->SetBackDist(m_back->text().toDouble());
	m_pPathPlan->SetSaftDist(m_safe->text().toDouble());

	// ·���滮
	int nsize = gWindow->GetMainDoc()->DispEntCount();
	if (nsize == 0)
	{
		QMessageBox::warning(QApplication::activeWindow(), QString::fromLocal8Bit("WARINING"), QString::fromLocal8Bit("δ�������ģ��"), "OK");
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

	// ���ɼ��NC��������Meas֮ǰ��ͣ
	pThp->setIsPauseBeforeMeas(true);
	pThp->TH_NCFileWrite(gWindow->GetMainDoc()->GetPathList(), filename);

	m_NC_CodeBody = pThp->GetNCCodeBody();

	// ������ʾNC����
	reinterpret_cast<FCodeWnd*>(gWindow->GetSubWnd()[2])->getTextEdit()->setPlainText(pThp->GetSaveToMachineToolLocalHead() + pThp->TH_GetHead() + m_NC_CodeBody + pThp->TH_GetTail() + pThp->GetSaveToMachineToolLocalLast());

	delete pThp;

}

void FPathPlanAndExeDlg::onButtonClicked(QAbstractButton* button)
{
	if (!m_pPathPlan)
	{
		m_pPathPlan = new FPathPlan(gWindow->GetMainDoc());

	}

	if (button->text() == QString::fromLocal8Bit("���"))
	{
		IsShortestPathPlan = 1;
	}
	else if (button->text() == QString::fromLocal8Bit("����"))
	{
		IsShortestPathPlan = 0;
	}
	else
	{
		QMessageBox::warning(
			QApplication::activeWindow(), QObject::tr("Caution"), QString::fromLocal8Bit("���١����·���滮��ʽ���ô���Ĭ�Ͽ���"));
	}

}

void FPathPlanAndExeDlg::onDataTransfer()
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(NC_InspectionSaveDir));

	// ����������Ϣ��
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Warning);
	msgBox.setText(QString::fromLocal8Bit("��ʹ���ļ�����ϵͳ���궨��NC���봫����������ִ�У�NCִ��ǰ��Ҫģ������ϵ���������ϵһ�¡�"));
	msgBox.setStandardButtons(QMessageBox::Ok);
	// ������Ϣ��ʼ��λ����������֮��
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
