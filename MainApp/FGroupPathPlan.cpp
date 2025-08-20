#include "FGroupPathPlan.h"
#include "qboxlayout.h"
#include "FMainWindow.h"
#include <functional>
#include "FAdjustMachine.h"
#include "FAdjustUpSphere.h"
#include "FPathPlanAndExeDlg.h"
#include "occConverter.h"
#include "np.h"
#include "CoorTrans.h"
#include "PointPlanDlg.h"
#include "MeasPt.h"

extern FMainWindow* gWindow;

FGroupPathPlan::FGroupPathPlan()
{
	move(1250, 150);
	setWindowIcon(QIcon("../icons/GroupPathPlan.png"));
	setWindowTitle(QString::fromLocal8Bit("������滮"));
	setWindowFlags(Qt::WindowStaysOnTopHint);//��Զ����������֮��

	/// <����NC����>
	// ������Z����ԭ����������λ
	m_SubHeadLine1 = "G75Z0\n";

	// ʾ��Ϊ G0C90 ��90��ʾC��Ƕȣ���Ҫ�ں��������û��������ȥ90\n
	m_SubHeadLine2 = "G0C";

	// ʾ��Ϊ G56 G17 G40 G01 G90��G56Ϊ����ϵ������Ҫ�ں��������û��������G56
	m_SubHeadLine3 = " G17 G40 G01 G90\n\n";
	/// </����NC����>

	m_RotateDir = gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));

	QHBoxLayout* MainHLayout = new QHBoxLayout(this);

	m_pAllInspItemsTree = new QTreeWidget;
	m_pAllInspItemsTree->setHeaderHidden(true);

	QVBoxLayout* LeftVLayout = new QVBoxLayout;

	QGridLayout* LeftUpLayout = new QGridLayout;

	QLabel* pRotateAngleLabel = new QLabel(QString::fromLocal8Bit("��ת�Ƕ�"));
	m_pRotateAngleLine = new QLineEdit;
	m_pRotateAngleLine->setText("0");

	LeftUpLayout->addWidget(pRotateAngleLabel, 0, 0);
	LeftUpLayout->addWidget(m_pRotateAngleLine, 0, 1);

	QLabel* pOrignCoorInMainFrame = new QLabel(QString::fromLocal8Bit("ԭ������"));
	pOrignCoorInMainFrame->setToolTip(QString::fromLocal8Bit("����Ҫ����ϵ�µ�����"));

	QLabel* pOrignCoorXLabel = new QLabel(QString::fromLocal8Bit("X"));
	QLabel* pOrignCoorYLabel = new QLabel(QString::fromLocal8Bit("Y"));
	QLabel* pOrignCoorZLabel = new QLabel(QString::fromLocal8Bit("Z"));

	m_pCoorInMainFrameX = new QLineEdit;
	m_pCoorInMainFrameX->setText(QString::number(0));
	m_pCoorInMainFrameY = new QLineEdit;
	m_pCoorInMainFrameY->setText(QString::number(0));
	m_pCoorInMainFrameZ = new QLineEdit;
	m_pCoorInMainFrameZ->setText(QString::number(0));

	LeftUpLayout->addWidget(pOrignCoorInMainFrame, 1, 0);
	LeftUpLayout->addWidget(pOrignCoorXLabel, 2, 0);
	LeftUpLayout->addWidget(m_pCoorInMainFrameX, 2, 1);
	LeftUpLayout->addWidget(pOrignCoorYLabel, 3, 0);
	LeftUpLayout->addWidget(m_pCoorInMainFrameY, 3, 1);
	LeftUpLayout->addWidget(pOrignCoorZLabel, 4, 0);
	LeftUpLayout->addWidget(m_pCoorInMainFrameZ, 4, 1);

	QLabel* pRotateAxisDir = new QLabel(QString::fromLocal8Bit("��ת�᷽��"));
	m_pRotateX = new QRadioButton("X");
	m_pRotateX->setChecked(true);
	m_pRotateY = new QRadioButton("Y");
	m_pRotateZ = new QRadioButton("Z");

	LeftUpLayout->addWidget(pRotateAxisDir, 5, 0);
	LeftUpLayout->addWidget(m_pRotateX, 6, 0);
	LeftUpLayout->addWidget(m_pRotateY, 7, 0);
	LeftUpLayout->addWidget(m_pRotateZ, 8, 0);

	QButtonGroup* pRotateDirGroup = new QButtonGroup;
	pRotateDirGroup->addButton(m_pRotateX);
	pRotateDirGroup->addButton(m_pRotateY);
	pRotateDirGroup->addButton(m_pRotateZ);

	QLabel* pFrameNameLabel = new QLabel(QString::fromLocal8Bit("����ϵ��"));
	m_pCurrFrameNameLine = new QLineEdit;
	m_pCurrFrameNameLine->setText("G54");
	LeftUpLayout->addWidget(pFrameNameLabel, 9, 0);
	LeftUpLayout->addWidget(m_pCurrFrameNameLine, 9, 1);

	QLabel* pCAxisAngleLabel = new QLabel(QString::fromLocal8Bit("C��Ƕ�"));
	m_pCAxisAngleLine = new QLineEdit;
	m_pCAxisAngleLine->setText("0");

	LeftUpLayout->addWidget(pCAxisAngleLabel, 10, 0);
	LeftUpLayout->addWidget(m_pCAxisAngleLine, 10, 1);

	LeftVLayout->addLayout(LeftUpLayout);

	QGridLayout* LeftDownLayout = new QGridLayout;

	pPntPlan = new QPushButton(QString::fromLocal8Bit("���滮"));
	LeftDownLayout->addWidget(pPntPlan, 0, 0);

	pPathPlan = new QPushButton(QString::fromLocal8Bit("·���滮"));
	LeftDownLayout->addWidget(pPathPlan, 1, 0);

	pSaveOrientationItem = new QPushButton(QString::fromLocal8Bit("�����ⷽλ"));
	LeftDownLayout->addWidget(pSaveOrientationItem, 2, 0);

	pFrameAlt = new QPushButton(QString::fromLocal8Bit("�޸�����ϵ"));
	LeftDownLayout->addWidget(pFrameAlt, 2, 1);

	pExNCCode = new QPushButton(QString::fromLocal8Bit("����NC����"));
	LeftDownLayout->addWidget(pExNCCode, 3, 0);

	pLoadPnts = new QPushButton(QString::fromLocal8Bit("������"));
	LeftDownLayout->addWidget(pLoadPnts, 4, 0);

	pExportTheoPnts = new QPushButton(QString::fromLocal8Bit("�������"));
	LeftDownLayout->addWidget(pExportTheoPnts, 4, 1);

	pImportPnts = new QPushButton(QString::fromLocal8Bit("������"));
	LeftDownLayout->addWidget(pImportPnts, 5, 0);

	pPntsTransForm = new QPushButton(QString::fromLocal8Bit("ת�Ʋ��"));
	LeftDownLayout->addWidget(pPntsTransForm, 6, 0);

	pResetModel = new QPushButton(QString::fromLocal8Bit("�ָ�ģ��"));
	LeftDownLayout->addWidget(pResetModel, 6, 1);

	LeftVLayout->addLayout(LeftDownLayout);

	MainHLayout->addLayout(LeftVLayout);
	MainHLayout->addWidget(m_pAllInspItemsTree);


	connect(pPntPlan, SIGNAL(clicked()), this, SLOT(onPntPlan()));
	connect(pLoadPnts, SIGNAL(clicked()), this, SLOT(onLoadAllPnts()));
	connect(pPathPlan, SIGNAL(clicked()), this, SLOT(onPathPlan()));
	connect(pSaveOrientationItem, SIGNAL(clicked()), this, SLOT(onSaveOrientationItem()));
	connect(pImportPnts, SIGNAL(clicked()), this, SLOT(onImportPnts()));
	connect(pExNCCode, SIGNAL(clicked()), this, SLOT(onExNCCode()));
	connect(pFrameAlt, SIGNAL(clicked()), this, SLOT(onFrameAlt()));
	connect(pPntsTransForm, SIGNAL(clicked()), this, SLOT(onPntsTransForm()));
	connect(pRotateDirGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onRotateDirClicked(QAbstractButton*)));
	connect(pResetModel, SIGNAL(clicked()), this, SLOT(onResetModel()));
	connect(pExportTheoPnts, SIGNAL(clicked()), this, SLOT(onSaveTheoPnts()));

	//style
	{
		m_pAllInspItemsTree->setStyleSheet
		(
			"QTreeView::branch:has-children:!has-siblings:closed,\
		QTreeView::branch:closed:has-children:has-siblings{border-image: none; image: url(../icons/TreeCollapse.png);}\
		QTreeView::branch:open:has-children:!has-siblings,\
		QTreeView::branch:open:has-children:has-siblings{border-image: none; image: url(../icons/TreeExpand.png);}"
			"QWidget"
			"{"
			"border: 1px solid rgb(232,232,236);"
			"background:rgb(247,249,254);"
			"}"
			"QScrollBar:vertical"
			"{"
			"width:22px;"
			"background:rgba(0,0,0,0%);"
			"margin:0px,0px,0px,0px;"
			"padding-top:18px;"
			"padding-bottom:18px;"
			"padding-right:5px;"
			"padding-left:5px;"
			"background:rgba(232,232,236,100%);"
			"}"
			// ���������˱����Բ
			"QScrollBar::handle:vertical"
			"{"
			"width:8px;"
			"background:rgba(0,0,0,25%);"
			"min-height:20;"
			"max-width:10;"
			"}"
			// ���ŵ��������ϵ�ʱ����ɫ����
			"QScrollBar::handle:vertical:hover"
			"{"
			"width:8px;"
			"background:rgba(0,0,0,50%);"
			"min-height:20;"
			"}"
			// ���Ӧ���������¼�ͷ�ģ�3.png���Ǽ�ͷ
			"QScrollBar::add-line:vertical"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollDown.png);"
			"subcontrol-position:bottom;"
			"}"
			// �����ϼ�ͷ
			"QScrollBar::sub-line:vertical"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollUp.png);"
			"subcontrol-position:top;"
			"}"
			// �����ŵ��¼�ͷ�ϵ�ʱ��
			"QScrollBar::add-line:vertical:hover"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollDown2.png);"
			"subcontrol-position:bottom;"
			"}"
			// �����ŵ��¼�ͷ�ϵ�ʱ��
			"QScrollBar::sub-line:vertical:hover"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollUp2.png);"
			"subcontrol-position:top;"
			"}"
			// ��������������ʱ������Ĳ��ֺ�����Ĳ���
			"QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical"
			"{"
			"background:rgba(232,232,236,100%);"
			"}"

			//����
			"QScrollBar:horizontal"
			"{"
			"height:22px;"
			"background:rgba(0,0,0,0%);"
			"margin:0px,0px,0px,0px;"
			"padding-top:5px;"
			"padding-bottom:5px;"
			"padding-right:18px;"
			"padding-left:18px;"
			"background:rgba(232,232,236,100%);"
			"}"
			// ���������˱����Բ
			"QScrollBar::handle:horizontal"
			"{"
			"height:8px;"
			"background:rgba(0,0,0,25%);"
			"min-width:10;"
			"max-height:20;"
			"}"
			// ���ŵ��������ϵ�ʱ����ɫ����
			"QScrollBar::handle:horizontal:hover"
			"{"
			"height:8px;"
			"background:rgba(0,0,0,50%);"
			"min-width:20;"
			"}"
			// �������ͷ�ģ�3.png���Ǽ�ͷ
			"QScrollBar::sub-line:horizontal"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollLeft.png);"
			"subcontrol-position:left;"
			"}"
			// �����Ҽ�ͷ
			"QScrollBar::add-line:horizontal"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollRight.png);"
			"subcontrol-position:right;"
			"}"
			// �����ŵ����ͷ�ϵ�ʱ��
			"QScrollBar::sub-line:horizontal:hover"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollLeft2.png);"
			"subcontrol-position:left;"
			"}"
			// �����ŵ��Ҽ�ͷ�ϵ�ʱ��
			"QScrollBar::add-line:horizontal:hover"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollRight2.png);"
			"subcontrol-position:right;"
			"}"
			// ��������������ʱ������Ĳ��ֺ�����Ĳ���
			"QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal"
			"{"
			"background:rgba(232,232,236,100%);"
			"}"
		);

	}

}

void FGroupPathPlan::onPntPlan()
{
	gWindow->onPointPlan();
	gWindow->GetPointPlanDlg()->move(1100, 550);
}

void FGroupPathPlan::onLoadAllPnts()
{
	FIpPntList AllInspPnts;

	for (auto ita = m_AllPntList.begin(); ita != m_AllPntList.end(); ++ita)
	{
		for (auto ita2 = (*ita).begin(); ita2 != (*ita).end(); ++ita2)
		{
			AllInspPnts.push_back(*ita2);
		}
	}

	gWindow->GetMainDoc()->ClearIpPntList();
	gWindow->GetMainDoc()->SetIpPntList(AllInspPnts);

}

void FGroupPathPlan::onPathPlan()
{
	gWindow->onPathPlanAndExe();
}

void FGroupPathPlan::onFrameAlt()
{
	gWindow->onCoordAlt();

	gWindow->GetCoorTransDlg()->SetActivateFromGroupPathPlan(this);
	gWindow->GetCoorTransDlg()->move(1500, 500);
}

void FGroupPathPlan::onSaveOrientationItem()
{
	QString tmpFrameName = m_pCurrFrameNameLine->text();
	tmpFrameName.remove('G');
	m_pCurrFrameNameLine->setText('G' + QString::number(tmpFrameName.toInt() + 1));

	// ������
	m_AllPntList.push_back(gWindow->GetMainDoc()->GetIpPntList());

	// ����NC
	QString tmpSubHead;

	// ÿ���㿪ͷ��ע�Ͳ���
	tmpSubHead = "\n;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;" + QString::number(m_pAllInspItemsTree->topLevelItemCount()) + QString::fromLocal8Bit("��λ��㣬C��Ƕȣ�") + m_pCAxisAngleLine->text() + "\n";

	tmpSubHead += m_SubHeadLine1;
	tmpSubHead += m_SubHeadLine2 + m_pCAxisAngleLine->text() + "\n";
	tmpSubHead += m_pCurrFrameNameLine->text() + m_SubHeadLine3;

	m_NC_CodeBody += tmpSubHead;
	m_NC_CodeBody += gWindow->GetPathPlanAndExeDlg()->GetNC_Body();

	gWindow->GetPathPlanAndExeDlg()->close();
	delete gWindow->GetPathPlanAndExeDlg();
	gWindow->SetPathPlanAndExeDlg(NULL);


	// ��������Ŀ
	QTreeWidgetItem* TopTreeItem = new QTreeWidgetItem;
	TopTreeItem->setText(0, QString::fromLocal8Bit("��ⷽλ��") + QString::number(m_pAllInspItemsTree->topLevelItemCount()));

	QTreeWidgetItem* FrameNameItem = new QTreeWidgetItem;
	FrameNameItem->setText(0, QString::fromLocal8Bit("����ϵ����") + m_pCurrFrameNameLine->text());

	QTreeWidgetItem* CAxisAngleTreeItem = new QTreeWidgetItem;
	CAxisAngleTreeItem->setText(0, QString::fromLocal8Bit("C��Ƕȣ�") + m_pCAxisAngleLine->text());

	QTreeWidgetItem* AngleTreeItem = new QTreeWidgetItem;
	AngleTreeItem->setText(0, QString::fromLocal8Bit("ת���Ƕȣ�") + m_pRotateAngleLine->text());

	QTreeWidgetItem* XTreeItem = new QTreeWidgetItem;
	XTreeItem->setText(0, QString::fromLocal8Bit("ԭ��X���꣺") + m_pCoorInMainFrameX->text());

	QTreeWidgetItem* YTreeItem = new QTreeWidgetItem;
	YTreeItem->setText(0, QString::fromLocal8Bit("ԭ��Y���꣺") + m_pCoorInMainFrameY->text());

	QTreeWidgetItem* ZTreeItem = new QTreeWidgetItem;
	ZTreeItem->setText(0, QString::fromLocal8Bit("ԭ��Z���꣺") + m_pCoorInMainFrameZ->text());

	QTreeWidgetItem* PntNumItem = new QTreeWidgetItem;
	PntNumItem->setText(0, QString::fromLocal8Bit("�������") + QString::number(gWindow->GetMainDoc()->GetIpPntList().size()));


	TopTreeItem->addChild(FrameNameItem);
	TopTreeItem->addChild(CAxisAngleTreeItem);
	TopTreeItem->addChild(AngleTreeItem);
	TopTreeItem->addChild(XTreeItem);
	TopTreeItem->addChild(YTreeItem);
	TopTreeItem->addChild(ZTreeItem);
	TopTreeItem->addChild(PntNumItem);

	m_pAllInspItemsTree->addTopLevelItem(TopTreeItem);

	// ��������ϵת������
	double RotateAngle = m_pRotateAngleLine->text().toDouble();
	RotateAngle = (RotateAngle / 180.0) * PI;
	double FrameX = m_pCoorInMainFrameX->text().toDouble();
	double FrameY = m_pCoorInMainFrameY->text().toDouble();
	double FrameZ = m_pCoorInMainFrameZ->text().toDouble();

	m_TransInfoList.push_back(TransInfo(RotateAngle, FrameX, FrameY, FrameZ));

	// ɾ����ʾ�����۲�㼰MainDoc�е����۲��
	// ɾ����ʾ��·��
	int nSize = gWindow->GetMainDoc()->getPntsShowList().size();
	for (int i = 0; i < nSize; i++)
	{
		gWindow->GetMainDoc()->RemoveDispEnts(gWindow->GetMainDoc()->getPntsShowList()[nSize - 1 - i]);
		delete gWindow->GetMainDoc()->getPntsShowList()[nSize - 1 - i];
	}

	gWindow->GetMainDoc()->ClearIpPntList();
	gWindow->GetMainDoc()->SetIpPntList(FIpPntList());
	gWindow->GetMainDoc()->ClearIpPathList();

	gWindow->GetMainDoc()->getPntsShowList().clear();

	gWindow->UpdateModelView();

	onResetModel();
}

void FGroupPathPlan::onImportPnts()
{
	// ��������
	//onLoadAllPnts();

	// �����������
	gWindow->onImportMeaData();

	// ת�Ʋ��
	//onPntsTransForm();

	// �ָ�ģ��
	//onResetModel();

}

void FGroupPathPlan::onExNCCode()
{
	if (gWindow->GetPathPlanAndExeDlg() == NULL)
	{
		gWindow->SetPathPlanAndExeDlg(new FPathPlanAndExeDlg());
	}

	FTHP* pThp = new FTHP();
	//pThp->SetProbeOnCommand(gWindow->GetPathPlanAndExeDlg()->GetProbeOnCommand());
	//pThp->SetProbeOffCommand(gWindow->GetPathPlanAndExeDlg()->GetProbeOffCommand());

	QString AllNCCode = pThp->TH_GetHead() + m_NC_CodeBody + pThp->TH_GetTail();

	QString fileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("����NC����"), tr("NC Code"),
		tr("Numerical Control Files (*.MPF)"));

	if (fileName.isEmpty())
	{
		return;
	}

	QFile	file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text))
	{
		QMessageBox::warning(0, QString::fromLocal8Bit("ʧ��"),
			QString::fromLocal8Bit("�ļ�����д��"));
		return;
	}
	QTextStream	out(&file);
	out.setRealNumberNotation(QTextStream::FixedNotation); //ScientificNotation��Scientific notation,FixedNotation���������,SmartNotation����ѧ�򶨵����
	out << AllNCCode;
	file.close();

	delete pThp;

}

void FGroupPathPlan::onPntsTransForm()
{
	// ��ת�����ۼ��������ݣ���Ȼ��ȡ�������ã����������һ��������������Խ���������в�������Ӱ�� gWindow->GetMainDoc()->GetIpPntList() ������
	// ��ȡ���Ǹ����ھֲ�����ϵ�µ�����
	FIpPntList TheoPnts = gWindow->GetMainDoc()->GetIpPntList();
	FIpPntList MeasPnts = gWindow->GetMainDoc()->GetMesPntList();

	if (TheoPnts.size() != MeasPnts.size())
	{
		QMessageBox::warning(
			QApplication::activeWindow(), QObject::tr("Caution"), QString::fromLocal8Bit("�뵼��ʵ�����ݺ����ԣ�"));

		return;
	}

	// ���ձ����˳�򣬽�������·���
	vector<FIpPntList> ReGroupTheoPnts;
	vector<FIpPntList> ReGroupMeasPnts;

	int NbIterations = 0;
	for (int i = 0; i < m_AllPntList.size(); i++)
	{
		FIpPntList tmpTheoPnts;
		FIpPntList tmpMeasPnts;

		for (int j = 0; j < m_AllPntList[i].size(); j++)
		{
			tmpTheoPnts.push_back(TheoPnts[NbIterations]);
			tmpMeasPnts.push_back(MeasPnts[NbIterations]);

			NbIterations += 1;
		}

		ReGroupTheoPnts.push_back(tmpTheoPnts);
		ReGroupMeasPnts.push_back(tmpMeasPnts);

	}

	// ����Ӧ����������ת��
	FIpPntList TransformedTheoPnts;
	FIpPntList TransformedMeasPnts;

	function<FIpPoint(gp_Pnt)> GpToFip = [=](gp_Pnt aPnt)->FIpPoint {return FIpPoint(aPnt.X(), aPnt.Y(), aPnt.Z()); };

	for (int i = 0; i < ReGroupTheoPnts.size(); i++)
	{
		for (int j = 0; j < ReGroupTheoPnts[i].size(); j++)
		{
			gp_Pnt tmpTheoPnt = GP_PNT(ReGroupTheoPnts[i][j]);
			gp_Pnt tmpMeasPnt = GP_PNT(ReGroupMeasPnts[i][j]);

			// �ȼ�����ת�ýǶ�
			gp_Trsf OcctRotate;
			OcctRotate.SetRotation(m_RotateDir, -m_TransInfoList[i].Angle);

			tmpTheoPnt.Transform(OcctRotate);
			tmpMeasPnt.Transform(OcctRotate);

			// ��ƽ�ƻ�ȥ
			gp_Trsf OcctMove;
			OcctMove.SetValues(1, 0, 0, -m_TransInfoList[i].X, 0, 1, 0, -m_TransInfoList[i].Y, 0, 0, 1, -m_TransInfoList[i].Z);

			tmpTheoPnt.Transform(OcctMove);
			tmpMeasPnt.Transform(OcctMove);

			TransformedTheoPnts.push_back(GpToFip(tmpTheoPnt));
			TransformedMeasPnts.push_back(GpToFip(tmpMeasPnt));
		}
	}

	gWindow->GetMainDoc()->ClearIpPntList();
	gWindow->GetMainDoc()->SetIpPntList(TransformedTheoPnts);

	gWindow->GetMainDoc()->clearMesPointList();
	gWindow->GetMainDoc()->SetMesPntList(TransformedMeasPnts);




}

void FGroupPathPlan::onRotateDirClicked(QAbstractButton* button)
{
	if (button->text() == "X")
	{
		m_RotateDir = gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
	}
	else if (button->text() == "Y")
	{
		m_RotateDir = gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0));
	}
	else if (button->text() == "Z")
	{
		m_RotateDir = gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
	}
	else
	{
		_DEBUG_ERROR("�оݴ���");
	}
	cout << "������滮��ת��״̬��������" << endl;
}

void FGroupPathPlan::onResetModel()
{
	if (gWindow->GetCoorTransDlg())
	{
		gWindow->GetCoorTransDlg()->ResetModel();
	}
}

void FGroupPathPlan::onSaveTheoPnts()
{
	gWindow->onExportTheoData();

}

void FGroupPathPlan::SetXYZ_Checked(int XYZ)
{
	switch (XYZ)
	{
	case 0:
	{
		m_pRotateX->setChecked(true);
		m_RotateDir = gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
		break;
	}
	case 1:
	{
		m_pRotateY->setChecked(true);
		m_RotateDir = gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0));
		break;
	}
	case 2:
	{
		m_pRotateZ->setChecked(true);
		m_RotateDir = gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
		break;
	}
	default:
		break;
	}

}
