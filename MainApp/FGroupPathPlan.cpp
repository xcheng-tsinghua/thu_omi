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
	setWindowTitle(QString::fromLocal8Bit("分组检测规划"));
	setWindowFlags(Qt::WindowStaysOnTopHint);//永远在其他窗口之上

	/// <设置NC代码>
	// 将主轴Z坐标原地升到上限位
	m_SubHeadLine1 = "G75Z0\n";

	// 示例为 G0C90 ，90表示C轴角度，需要在后续根据用户输入加上去90\n
	m_SubHeadLine2 = "G0C";

	// 示例为 G56 G17 G40 G01 G90，G56为坐标系名，需要在后续根据用户输入加上G56
	m_SubHeadLine3 = " G17 G40 G01 G90\n\n";
	/// </设置NC代码>

	m_RotateDir = gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));

	QHBoxLayout* MainHLayout = new QHBoxLayout(this);

	m_pAllInspItemsTree = new QTreeWidget;
	m_pAllInspItemsTree->setHeaderHidden(true);

	QVBoxLayout* LeftVLayout = new QVBoxLayout;

	QGridLayout* LeftUpLayout = new QGridLayout;

	QLabel* pRotateAngleLabel = new QLabel(QString::fromLocal8Bit("旋转角度"));
	m_pRotateAngleLine = new QLineEdit;
	m_pRotateAngleLine->setText("0");

	LeftUpLayout->addWidget(pRotateAngleLabel, 0, 0);
	LeftUpLayout->addWidget(m_pRotateAngleLine, 0, 1);

	QLabel* pOrignCoorInMainFrame = new QLabel(QString::fromLocal8Bit("原点坐标"));
	pOrignCoorInMainFrame->setToolTip(QString::fromLocal8Bit("在主要坐标系下的坐标"));

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

	QLabel* pRotateAxisDir = new QLabel(QString::fromLocal8Bit("旋转轴方向"));
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

	QLabel* pFrameNameLabel = new QLabel(QString::fromLocal8Bit("坐标系名"));
	m_pCurrFrameNameLine = new QLineEdit;
	m_pCurrFrameNameLine->setText("G54");
	LeftUpLayout->addWidget(pFrameNameLabel, 9, 0);
	LeftUpLayout->addWidget(m_pCurrFrameNameLine, 9, 1);

	QLabel* pCAxisAngleLabel = new QLabel(QString::fromLocal8Bit("C轴角度"));
	m_pCAxisAngleLine = new QLineEdit;
	m_pCAxisAngleLine->setText("0");

	LeftUpLayout->addWidget(pCAxisAngleLabel, 10, 0);
	LeftUpLayout->addWidget(m_pCAxisAngleLine, 10, 1);

	LeftVLayout->addLayout(LeftUpLayout);

	QGridLayout* LeftDownLayout = new QGridLayout;

	pPntPlan = new QPushButton(QString::fromLocal8Bit("测点规划"));
	LeftDownLayout->addWidget(pPntPlan, 0, 0);

	pPathPlan = new QPushButton(QString::fromLocal8Bit("路径规划"));
	LeftDownLayout->addWidget(pPathPlan, 1, 0);

	pSaveOrientationItem = new QPushButton(QString::fromLocal8Bit("保存检测方位"));
	LeftDownLayout->addWidget(pSaveOrientationItem, 2, 0);

	pFrameAlt = new QPushButton(QString::fromLocal8Bit("修改坐标系"));
	LeftDownLayout->addWidget(pFrameAlt, 2, 1);

	pExNCCode = new QPushButton(QString::fromLocal8Bit("导出NC代码"));
	LeftDownLayout->addWidget(pExNCCode, 3, 0);

	pLoadPnts = new QPushButton(QString::fromLocal8Bit("载入测点"));
	LeftDownLayout->addWidget(pLoadPnts, 4, 0);

	pExportTheoPnts = new QPushButton(QString::fromLocal8Bit("导出测点"));
	LeftDownLayout->addWidget(pExportTheoPnts, 4, 1);

	pImportPnts = new QPushButton(QString::fromLocal8Bit("导入测点"));
	LeftDownLayout->addWidget(pImportPnts, 5, 0);

	pPntsTransForm = new QPushButton(QString::fromLocal8Bit("转移测点"));
	LeftDownLayout->addWidget(pPntsTransForm, 6, 0);

	pResetModel = new QPushButton(QString::fromLocal8Bit("恢复模型"));
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
			// 滚动条两端变成椭圆
			"QScrollBar::handle:vertical"
			"{"
			"width:8px;"
			"background:rgba(0,0,0,25%);"
			"min-height:20;"
			"max-width:10;"
			"}"
			// 鼠标放到滚动条上的时候，颜色变深
			"QScrollBar::handle:vertical:hover"
			"{"
			"width:8px;"
			"background:rgba(0,0,0,50%);"
			"min-height:20;"
			"}"
			// 这个应该是设置下箭头的，3.png就是箭头
			"QScrollBar::add-line:vertical"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollDown.png);"
			"subcontrol-position:bottom;"
			"}"
			// 设置上箭头
			"QScrollBar::sub-line:vertical"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollUp.png);"
			"subcontrol-position:top;"
			"}"
			// 当鼠标放到下箭头上的时候
			"QScrollBar::add-line:vertical:hover"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollDown2.png);"
			"subcontrol-position:bottom;"
			"}"
			// 当鼠标放到下箭头上的时候
			"QScrollBar::sub-line:vertical:hover"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollUp2.png);"
			"subcontrol-position:top;"
			"}"
			// 当滚动条滚动的时候，上面的部分和下面的部分
			"QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical"
			"{"
			"background:rgba(232,232,236,100%);"
			"}"

			//横向
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
			// 滚动条两端变成椭圆
			"QScrollBar::handle:horizontal"
			"{"
			"height:8px;"
			"background:rgba(0,0,0,25%);"
			"min-width:10;"
			"max-height:20;"
			"}"
			// 鼠标放到滚动条上的时候，颜色变深
			"QScrollBar::handle:horizontal:hover"
			"{"
			"height:8px;"
			"background:rgba(0,0,0,50%);"
			"min-width:20;"
			"}"
			// 设置左箭头的，3.png就是箭头
			"QScrollBar::sub-line:horizontal"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollLeft.png);"
			"subcontrol-position:left;"
			"}"
			// 设置右箭头
			"QScrollBar::add-line:horizontal"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollRight.png);"
			"subcontrol-position:right;"
			"}"
			// 当鼠标放到左箭头上的时候
			"QScrollBar::sub-line:horizontal:hover"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollLeft2.png);"
			"subcontrol-position:left;"
			"}"
			// 当鼠标放到右箭头上的时候
			"QScrollBar::add-line:horizontal:hover"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollRight2.png);"
			"subcontrol-position:right;"
			"}"
			// 当滚动条滚动的时候，上面的部分和下面的部分
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

	// 保存测点
	m_AllPntList.push_back(gWindow->GetMainDoc()->GetIpPntList());

	// 保存NC
	QString tmpSubHead;

	// 每组测点开头的注释部分
	tmpSubHead = "\n;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;" + QString::number(m_pAllInspItemsTree->topLevelItemCount()) + QString::fromLocal8Bit("方位测点，C轴角度：") + m_pCAxisAngleLine->text() + "\n";

	tmpSubHead += m_SubHeadLine1;
	tmpSubHead += m_SubHeadLine2 + m_pCAxisAngleLine->text() + "\n";
	tmpSubHead += m_pCurrFrameNameLine->text() + m_SubHeadLine3;

	m_NC_CodeBody += tmpSubHead;
	m_NC_CodeBody += gWindow->GetPathPlanAndExeDlg()->GetNC_Body();

	gWindow->GetPathPlanAndExeDlg()->close();
	delete gWindow->GetPathPlanAndExeDlg();
	gWindow->SetPathPlanAndExeDlg(NULL);


	// 保存树项目
	QTreeWidgetItem* TopTreeItem = new QTreeWidgetItem;
	TopTreeItem->setText(0, QString::fromLocal8Bit("检测方位：") + QString::number(m_pAllInspItemsTree->topLevelItemCount()));

	QTreeWidgetItem* FrameNameItem = new QTreeWidgetItem;
	FrameNameItem->setText(0, QString::fromLocal8Bit("坐标系名：") + m_pCurrFrameNameLine->text());

	QTreeWidgetItem* CAxisAngleTreeItem = new QTreeWidgetItem;
	CAxisAngleTreeItem->setText(0, QString::fromLocal8Bit("C轴角度：") + m_pCAxisAngleLine->text());

	QTreeWidgetItem* AngleTreeItem = new QTreeWidgetItem;
	AngleTreeItem->setText(0, QString::fromLocal8Bit("转动角度：") + m_pRotateAngleLine->text());

	QTreeWidgetItem* XTreeItem = new QTreeWidgetItem;
	XTreeItem->setText(0, QString::fromLocal8Bit("原点X坐标：") + m_pCoorInMainFrameX->text());

	QTreeWidgetItem* YTreeItem = new QTreeWidgetItem;
	YTreeItem->setText(0, QString::fromLocal8Bit("原点Y坐标：") + m_pCoorInMainFrameY->text());

	QTreeWidgetItem* ZTreeItem = new QTreeWidgetItem;
	ZTreeItem->setText(0, QString::fromLocal8Bit("原点Z坐标：") + m_pCoorInMainFrameZ->text());

	QTreeWidgetItem* PntNumItem = new QTreeWidgetItem;
	PntNumItem->setText(0, QString::fromLocal8Bit("测点数：") + QString::number(gWindow->GetMainDoc()->GetIpPntList().size()));


	TopTreeItem->addChild(FrameNameItem);
	TopTreeItem->addChild(CAxisAngleTreeItem);
	TopTreeItem->addChild(AngleTreeItem);
	TopTreeItem->addChild(XTreeItem);
	TopTreeItem->addChild(YTreeItem);
	TopTreeItem->addChild(ZTreeItem);
	TopTreeItem->addChild(PntNumItem);

	m_pAllInspItemsTree->addTopLevelItem(TopTreeItem);

	// 保存坐标系转换数据
	double RotateAngle = m_pRotateAngleLine->text().toDouble();
	RotateAngle = (RotateAngle / 180.0) * PI;
	double FrameX = m_pCoorInMainFrameX->text().toDouble();
	double FrameY = m_pCoorInMainFrameY->text().toDouble();
	double FrameZ = m_pCoorInMainFrameZ->text().toDouble();

	m_TransInfoList.push_back(TransInfo(RotateAngle, FrameX, FrameY, FrameZ));

	// 删除显示的理论测点及MainDoc中的理论测点
	// 删除显示的路径
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
	// 先载入测点
	//onLoadAllPnts();

	// 导入测量数据
	gWindow->onImportMeaData();

	// 转移测点
	//onPntsTransForm();

	// 恢复模型
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

	QString fileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("保存NC代码"), tr("NC Code"),
		tr("Numerical Control Files (*.MPF)"));

	if (fileName.isEmpty())
	{
		return;
	}

	QFile	file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text))
	{
		QMessageBox::warning(0, QString::fromLocal8Bit("失败"),
			QString::fromLocal8Bit("文件不可写入"));
		return;
	}
	QTextStream	out(&file);
	out.setRealNumberNotation(QTextStream::FixedNotation); //ScientificNotation＃Scientific notation,FixedNotation＃定点符号,SmartNotation＃科学或定点符号
	out << AllNCCode;
	file.close();

	delete pThp;

}

void FGroupPathPlan::onPntsTransForm()
{
	// 需转移理论及测量数据，虽然获取的是引用，但是如果用一个数组接收它，对接收数组进行操作不会影响 gWindow->GetMainDoc()->GetIpPntList() 的内容
	// 获取的是各自在局部坐标系下的坐标
	FIpPntList TheoPnts = gWindow->GetMainDoc()->GetIpPntList();
	FIpPntList MeasPnts = gWindow->GetMainDoc()->GetMesPntList();

	if (TheoPnts.size() != MeasPnts.size())
	{
		QMessageBox::warning(
			QApplication::activeWindow(), QObject::tr("Caution"), QString::fromLocal8Bit("请导入实测数据后再试！"));

		return;
	}

	// 按照保存的顺序，将测点重新分组
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

	// 将对应组的坐标进行转换
	FIpPntList TransformedTheoPnts;
	FIpPntList TransformedMeasPnts;

	function<FIpPoint(gp_Pnt)> GpToFip = [=](gp_Pnt aPnt)->FIpPoint {return FIpPoint(aPnt.X(), aPnt.Y(), aPnt.Z()); };

	for (int i = 0; i < ReGroupTheoPnts.size(); i++)
	{
		for (int j = 0; j < ReGroupTheoPnts[i].size(); j++)
		{
			gp_Pnt tmpTheoPnt = GP_PNT(ReGroupTheoPnts[i][j]);
			gp_Pnt tmpMeasPnt = GP_PNT(ReGroupMeasPnts[i][j]);

			// 先继续旋转该角度
			gp_Trsf OcctRotate;
			OcctRotate.SetRotation(m_RotateDir, -m_TransInfoList[i].Angle);

			tmpTheoPnt.Transform(OcctRotate);
			tmpMeasPnt.Transform(OcctRotate);

			// 再平移回去
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
		_DEBUG_ERROR("判据错误");
	}
	cout << "分组检测规划旋转轴状态发生更改" << endl;
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
