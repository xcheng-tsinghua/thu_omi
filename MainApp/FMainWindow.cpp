#include <iostream>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <QTextStream>
#include <QMessageBox>
#include <QLayout>
#include <QLineEdit>
#include <QDomDocument>
#include "GeneratedFiles/ui_mainwindow.h"
#include "../External/fkernel/Include/FKernelHeader.h"
#include "../External/fkernel/Include/DataStructure/FBody.h"
#include "../External/fkernel/Include/DataStructure/FCurve.h"
#include "../External/fkernel/Include//Features/FFreeCurve.h"
#include "../External/fkernel/Include/Features/FExtend.h"
#include "FUtilities.h"
#include "MainDoc.h"
#include "MainView.h"
#include "FSubWindow.h"
#include "FCodeWnd.h"
#include "FDataWnd.h"
#include "FTreeWnd.h"
#include "FXMLReader.h"
#include "FTHP.h"
#include "FAdjustUpSphere.h"
#include "FAdjustRingProbe.h"
#include "FAdjustAngleProbe.h"
#include "FAdjustMachine.h"
#include "FStatVisualWnd.h"
#include "FGuideDataType.h"
#include "FTest.h"
#include "Tools/FSelectTool.h"
#include "Tools/DataTransferTool.h"
#include "PointPlanDlg.h"
#include "FMainWindow.h"
#include "FAccuracyEvaluation.h"
#include "occConverter.h"
#include "FInsFeaTreeWnd.h"
#include "FPathPlanAndExeDlg.h"
#include "AccuEvaluWnd.h"
#include "FInsReportDlg.h"
#include "EntityHighlight.h"
#include "np.h"
#include "InspVis_thread.h"
#include "CoorTrans.h"
#include "FToolPathDlg.h"
#include "FGroupPathPlan.h"
#include "InspReportInfoDlg.h"
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <Geom_Curve.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <ProjLib.hxx>
#include "AutoInsFea.h"
#include "SetCoorPrecisely.h"
#include "ErrCompsteDlg.h"
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>

extern QSettings* gDlgSetting;

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#endif
#define new DBG_NEW
#endif  // _DEBUG

FMainWindow::FMainWindow(QWidget* parent)
	:QMainWindow(parent)
{
	// 子窗口
	m_pPointPlan = NULL;
	m_pPathAndExe = NULL;
	m_pInsFeaDlg = NULL;
	m_pInsReport = NULL;
	m_pCoorAltDlg = NULL;
	m_pUpSphereDlg = NULL;
	m_pGroupPathPlanDlg = NULL;
	m_pMachineErrCompDlg = NULL;
	m_pAutoInsFeaDlg = NULL;
	m_pSetCoorPrecisely = NULL;
	m_pErrorMapDlg = NULL;
	m_pToolPathDlg = NULL;

	// 其它指针变量
	m_pCurTool = NULL;
	m_pConverter = NULL;
	m_pFreeBody = NULL;
	m_pProbeVis = new ProbeVis(m_pDoc);
	m_pVisThread = new InspVis_thread;
	
	//////////////////////////////////////////////
	//cout	<<	"Main thread:"	<<	QThread::currentThreadId()	<<	endl;
	//1. Create the main window UI
	CreateUI();	
	CreateStatusBar();

	//2. Create DOC instance
	m_pDoc = new FMainDoc(this);
	FT::gPDoc = m_pDoc;
	m_pDoc->setProbeDia(99999);
	m_pDoc->setRefDia(99999);
	m_pDoc->setNbLongitudePnt(99999);
	m_pDoc->setNbLatitudePnt(99999);

	onInspectConfig();		//create subwindows
	//m_pDoc->ImportProbe();	
	//3. Create the single-slots in main window
	CreateSignalSlots();

	//将doc中设置pointlist时发出的信号与datawnd中重新显示的槽函数连接
	connect(m_pDoc, SIGNAL(resetPointDataSignal(bool, int, bool)), m_wndlist[1], SLOT(onResetPointData(bool, int, bool)));
	connect(m_pDoc,SIGNAL(updateMesPointDataSignal(int)),m_wndlist[1],SLOT(onUpdateMesPointData(int)));
	connect(m_pDoc,SIGNAL(onUpdateErrorSignal(int)),m_wndlist[1],SLOT(onUpdateError(int)));
	connect(m_pDoc, SIGNAL(resetPointDataSignal(bool, int, bool)), m_wndlist[6], SLOT(onUpdateMeasPnts()));

	//start a new thread to visualize the measuring process
	m_qthread.start();

	//style
	{
		QString SeparatorColor = "rgb(93,107,153);";

		this->setStyleSheet(
			"QMainWindow::separator"
			"{"
			"width: 2px;"
			"height: 2px;"
			"border: 3px solid rgb(93,107,153);"
			"background:" + SeparatorColor +
			//"background:rgb(133,147,193);"
			"}"
			"QMainWindow"
			"{"
			//"background:rgb(204,213,240);"
			//"background:rgb(93,107,153);"
			"background:rgb(255,255,255);"
			//"border: 10px solid rgb(204,213,240);"
			"}"
		);
		m_pUI->menubar->setStyleSheet(
			"QMenu"
			"{"
			"background-color:rgb(233,238,255);"
			"border:1px solid rgb(93,107,153);"
			"}"
			"QMenu::item:selected"
			"{"
			"color:black;"
			"background-color:rgb(177,197,255);"
			"}"
			"QMenu::item:pressed"
			"{"
			"color:black;"
			"background-color:rgb(177,197,255);"
			"}"

			"QMenuBar"
			"{"
			"background-color: rgb(204,213,240);"
			//"border:1px dotted red;"
			"}"
			"QMenuBar::item"
			"{"
			"background:transparent;"
			"}"
			"QMenuBar::item:selected"
			"{"
			"color:black;"
			"background-color:rgb(236,244,255);"
			"border:1px solid rgb(93,107,153);"
			"}"
			"QMenuBar::item:pressed"
			"{"
			"color:black;"
			"background-color:rgb(233,238,255);"
			"border:1px solid rgb(93,107,153);"
			"}"
		);

		m_pUI->toolBar->setIconSize(QSize(26, 26));
		//m_pUI->toolBar->setMinimumHeight(40);
		m_pUI->toolBar->setStyleSheet(
			"QToolBar"
			"{"
			"border:0px solid rgb(232,232,236);"
			"background:rgb(204,213,240);"
			//"border: 2px"
			"}"
			"QToolBar::handle"
			"{"
			//"background:green;"
			"image: url(../icons/ToolBarHandle.png);"
			"}"
		);

		int MarginWidth = 8;
		m_pUI->TopMargin->setFixedHeight(MarginWidth);
		m_pUI->TopMargin->setStyleSheet(
			"QToolBar"
			"{"
			"border:0px solid rgb(232,232,236);"
			"background:" + SeparatorColor +
			//"border: 2px"
			"}"
		);
		m_pUI->TopMargin->setMovable(false);

		m_pUI->LeftMargin->setFixedWidth(MarginWidth);
		m_pUI->LeftMargin->setStyleSheet(
			"QToolBar"
			"{"
			"border:0px solid rgb(232,232,236);"
			"background:" + SeparatorColor +
			//"border: 2px"
			"}"
		);
		m_pUI->LeftMargin->setMovable(false);

		m_pUI->RightMargin->setFixedWidth(MarginWidth);
		m_pUI->RightMargin->setStyleSheet(
			"QToolBar"
			"{"
			"border:0px solid rgb(232,232,236);"
			"background:" + SeparatorColor +
			//"border: 2px"
			"}"
		);
		m_pUI->RightMargin->setMovable(false);

		m_pUI->BottomMargin->setFixedHeight(MarginWidth);
		m_pUI->BottomMargin->setStyleSheet(
			"QToolBar"
			"{"
			"border:0px solid rgb(232,232,236);"
			"background:" + SeparatorColor +
			//"border: 2px"
			"}"
		);
		m_pUI->BottomMargin->setMovable(false);

		m_pUI->statusbar->setStyleSheet(//底部状态栏
			"QStatusBar"
			"{"
			"background:rgb(64,80,141);"
			"}"
		);

		//自定义主界面标题栏
		selMainWidget = new FramelessWidget; //创建一个QWidget容器
		this->setParent(selMainWidget);//重新设置这个UI界面的父对象为QWidget
		pTitleBar = new WindowTitle(selMainWidget); //定义一个标题栏类
		this->installEventFilter(pTitleBar);//安装事件过滤器
		QGridLayout* pLayout = new QGridLayout();//创建一个整体布局器
		pLayout->addWidget(pTitleBar);  //添加标题栏
		pLayout->addWidget(this);       //添加UI界面
		pLayout->setSpacing(0);         //布局之间的距离
		pLayout->setContentsMargins(0, 0, 0, 0); //布局器的四周边距
		selMainWidget->setLayout(pLayout);  //将这个布局器设置在QWidget上
		//selMainWidget->setAttribute(Qt::WA_TranslucentBackground, true);//设置空白区域透明
		selMainWidget->setSizePolicy(this->sizePolicy());
		selMainWidget->setMaximumSize(this->maximumSize());
		//selMainWidget->setMaximumSize(this->maximumSize());
		this->setWindowTitle(QString::fromLocal8Bit(" Thu-OMI"));
		//selMainWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
		//Qt::FramelessWindowHint
		//Qt::CustomizeWindowHint
		//selMainWidget->FormBorderStyle = Windows.Forms.FormBorderStyle.None
	}
}

FMainWindow::~FMainWindow()
{

}

void FMainWindow::AllocateSurfNode()
{
	// 判断Edge是否相同
	function<bool(TopoDS_Edge, TopoDS_Edge)> IsEdgeTheSame = [=](TopoDS_Edge Edge1, TopoDS_Edge Edge2)->bool
	{
		double CurveStart1 = 0, CurveEnd1 = 0;

		BRep_Tool::Range(Edge1, CurveStart1, CurveEnd1);
		
		Handle(Geom_Curve) Curve1= BRep_Tool::Curve(Edge1, CurveStart1, CurveEnd1);

		double CurveStart2 = 0, CurveEnd2 = 0;

		BRep_Tool::Range(Edge2, CurveStart2, CurveEnd2);

		Handle(Geom_Curve) Curve2 = BRep_Tool::Curve(Edge2, CurveStart2, CurveEnd2);

		if (Curve1.IsNull() || Curve2.IsNull())
		{
			return false;
		}

		gp_Pnt PtStartCurve1 = Curve1->Value(CurveStart1);
		gp_Pnt PtEndCurve1 = Curve1->Value(CurveEnd1);
		gp_Pnt PtMidCurve1 = Curve1->Value((CurveStart1 + CurveEnd1) / 2.0);

		gp_Pnt PtStartCurve2 = Curve2->Value(CurveStart2);
		gp_Pnt PtEndCurve2 = Curve2->Value(CurveEnd2);
		gp_Pnt PtMidCurve2 = Curve2->Value((CurveStart2 + CurveEnd2) / 2.0);

		bool IsStartOverlap = false;
		bool IsEndOverlap = false;
		bool IsMidOverlap = false;

		if (abs(PtStartCurve1.Distance(PtStartCurve2)) < 1e-6 || abs(PtStartCurve1.Distance(PtEndCurve2)) < 1e-6)
		{
			IsStartOverlap = true;
		}
		
		if (abs(PtEndCurve1.Distance(PtStartCurve2)) < 1e-6 || abs(PtEndCurve1.Distance(PtEndCurve2)) < 1e-6)
		{
			IsEndOverlap = true;
		}

		if (abs(PtMidCurve1.Distance(PtMidCurve2)) < 1e-6)
		{
			IsMidOverlap = true;
		}

		return IsStartOverlap && IsEndOverlap && IsMidOverlap;

	};

	int nSize = m_pDoc->GetSurfNearbyGraph().size();

	m_pConverter->GetProgressBar()->setLabelText(QString::fromLocal8Bit("计算型面邻接图"));
	m_pConverter->GetProgressBar()->setValue(90);

	vector<TopoDS_Face> tmpTopoDS_FaceList = m_pConverter->GetTopoDS_FaceList();

	for (int i = 0; i < nSize - 1; i++)
	{
		// 将第i个节点和第i+1→N个相比，对比有没有相同的Edge，若有，则将其互相加入对方的邻接节点数组
		for (int j = i + 1; j < nSize; j++)
		{
			vector<TopoDS_Edge> EdgeList1;
			vector<TopoDS_Edge> EdgeList2;

			TopoDS_Face Face1 = tmpTopoDS_FaceList[m_pDoc->GetSurfNearbyGraph()[i]->GetGeomIndex()];

			for (TopExp_Explorer aExpEdge(Face1, TopAbs_EDGE); aExpEdge.More(); aExpEdge.Next())
			{
				EdgeList1.push_back(TopoDS::Edge(aExpEdge.Current()));
			}

			TopoDS_Face Face2 = tmpTopoDS_FaceList[m_pDoc->GetSurfNearbyGraph()[j]->GetGeomIndex()];

			for (TopExp_Explorer aExpEdge(Face2, TopAbs_EDGE); aExpEdge.More(); aExpEdge.Next())
			{
				EdgeList2.push_back(TopoDS::Edge(aExpEdge.Current()));
			}

			for (auto ita1 = EdgeList1.begin(); ita1 != EdgeList1.end(); ++ita1)
			{
				for (auto ita2 = EdgeList2.begin(); ita2 != EdgeList2.end(); ++ita2)
				{
					if (IsEdgeTheSame(*ita1, *ita2))
					{
						m_pDoc->GetSurfNearbyGraph()[i]->GetNearbyNodeList().push_back(m_pDoc->GetSurfNearbyGraph()[j]);
						m_pDoc->GetSurfNearbyGraph()[j]->GetNearbyNodeList().push_back(m_pDoc->GetSurfNearbyGraph()[i]);

						goto ExitEdgeJudgement;
					}

				}

			}

		ExitEdgeJudgement:

			int pass = 1;

		}

	}

	m_pConverter->GetProgressBar()->setLabelText(QString::fromLocal8Bit("完成"));
	m_pConverter->GetProgressBar()->setValue(100);
	m_pConverter->GetProgressBar()->autoClose();
}

void FMainWindow::CreateStatusBar()
{
	//status bar
	m_hintLabel=new QLabel("",this);
	m_hintLabel->setAlignment(Qt::AlignLeft);
	m_hintLabel->setMinimumSize(m_hintLabel->sizeHint());
	this->statusBar()->addPermanentWidget(m_hintLabel,1);
}

void FMainWindow::CreateUI()
{
	m_pUI = new Ui::UiMainWindow;
	m_pUI->setupUi(this);
	this->setAcceptDrops(true);	//allow to drop a file to open
	m_pMdiArea = new QMdiArea;
	setCentralWidget(m_pMdiArea);
	QApplication::setActiveWindow(this);

	//设置图标
	{
		m_pUI->action_exportRep->setIcon(QIcon("../icons/AccuRep.png"));
		m_pUI->action_importMeaData->setIcon(QIcon("../icons/importMeaData.png"));
		m_pUI->action_importTheoData->setIcon(QIcon("../icons/importTheoData.png"));
		m_pUI->action_exportTheoData->setIcon(QIcon("../icons/exportTheoData.png"));
		m_pUI->action_exportResult->setIcon(QIcon("../icons/exportResult.png"));
		m_pUI->action_ImportInspConfig->setIcon(QIcon("../icons/ConfigImport.png"));
		m_pUI->actionPointPlan_2->setIcon(QIcon("../icons/PntPlan.png"));
		m_pUI->actionOpcUaTHU->setIcon(QIcon("../icons/OpcUaTHU.png"));
		m_pUI->actionCoordTrans->setIcon(QIcon("../icons/Execute.png"));
		m_pUI->actionFiveAxisPathPlan->setIcon(QIcon("../icons/FiveAxisPathPlan.png"));
		m_pUI->actionGroupPathPlan->setIcon(QIcon("../icons/GroupPathPlan.png"));
		m_pUI->action_ProbeMachine->setIcon(QIcon("../icons/ProbeMachine.png"));
		m_pUI->action_FProbe0->setIcon(QIcon("../icons/AdjustRingProbe.png"));
		m_pUI->action_FProbe1->setIcon(QIcon("../icons/AdjustAngleProbe.png"));
		m_pUI->action_FProbe2->setIcon(QIcon("../icons/AdjustUpSphere.png"));
		m_pUI->action_FProbe3->setIcon(QIcon("../icons/AdjustLowerSphere.png"));
		m_pUI->actionRecogHole->setIcon(QIcon("../icons/RecogHole.png"));
		m_pUI->actionInspectConfig->setIcon(QIcon("../icons/Config.png"));
		m_pUI->actionInspectMode->setIcon(QIcon("../icons/InspectMode.png"));
		m_pUI->actionInspectAnalysis->setIcon(QIcon("../icons/InspectAnalysis.png"));
		m_pUI->action_TCPConfig->setIcon(QIcon("../icons/TCPConfig.png"));
		m_pUI->actionToolResetLayout->setIcon(QIcon("../icons/ToolResetLayout.png"));
		m_pUI->actionToolComputeError->setIcon(QIcon("../icons/ToolComputeError.png"));
		m_pUI->actionToolSetLocalFrame->setIcon(QIcon("../icons/ToolSetLocalFrame.png"));
		m_pUI->actionSurfaceEval->setIcon(QIcon("../icons/SurfaceEval.png"));
		m_pUI->actionTestDialog->setIcon(QIcon("../icons/TestDialog.png"));
		m_pUI->actionTest_Others->setIcon(QIcon("../icons/Test_Others.png"));
		m_pUI->actionMeasureStart->setIcon(QIcon("../icons/MeasureStart.png"));
		m_pUI->actionMeasurePause->setIcon(QIcon("../icons/MeasurePause.png"));
		m_pUI->actionMeasureReset->setIcon(QIcon("../icons/MeasureReset.png"));
		m_pUI->actionOPC_ON->setIcon(QIcon("../icons/MeasureStart.png"));
		m_pUI->action_save->setIcon(QIcon("../icons/save.png"));

		m_pUI->action_new->setIcon(QIcon("../icons/new.png"));
		m_pUI->action_open->setIcon(QIcon("../icons/open.png"));
		m_pUI->action_saveas->setIcon(QIcon("../icons/saveas.png"));

		m_pUI->action_pan->setIcon(QIcon("../icons/pan.png"));
		m_pUI->action_zoomall->setIcon(QIcon("../icons/zoomall.png"));
		m_pUI->action_smoothdisp->setIcon(QIcon("../icons/smooth.png"));
		m_pUI->actionCoorAlt->setIcon(QIcon("../icons/CoordTrans.png"));
		m_pUI->actionToolPathDlg->setIcon(QIcon("../icons/ToolPath.png"));
		m_pUI->actionErrorComp->setIcon(QIcon("../icons/ErrorComp.png"));
		m_pUI->actionSetCoorPrecisely->setIcon(QIcon("../icons/SetCoorPrecisely.png"));
	}
}

void FMainWindow::CreateSignalSlots()
{
	//menu File
	connect(m_pUI->action_new, SIGNAL(triggered()), this, SLOT(onFileNew()));
	connect(m_pUI->action_open, SIGNAL(triggered()), this, SLOT(onFileOpen()));
	connect(m_pUI->action_saveas, SIGNAL(triggered()), this, SLOT(onFileSave()));
	connect(m_pUI->action_quit, SIGNAL(triggered()), this, SLOT(onFileClose()));
	connect(m_pUI->action_importGuide, SIGNAL(triggered()), this, SLOT(onImportGuideFile()));
	connect(m_pUI->action_importMeaData, SIGNAL(triggered()), this, SLOT(onImportMeaData()));
	connect(m_pUI->action_importTheoData, SIGNAL(triggered()), this, SLOT(onImportTheoData()));
	connect(m_pUI->action_exportTheoData, SIGNAL(triggered()), this, SLOT(onExportTheoData()));
	connect(m_pUI->action_exportResult, SIGNAL(triggered()), this, SLOT(onExportResult()));
	connect(m_pUI->action_exportRep, SIGNAL(triggered()), this, SLOT(onExportReport()));
	connect(m_pUI->action_ImportInspConfig, SIGNAL(triggered()), this, SLOT(onImportInspConfig()));
	

	//menu View
	connect(m_pUI->action_topview, SIGNAL(triggered()), this, SLOT(onViewTop()));
	connect(m_pUI->action_bottomview, SIGNAL(triggered()), this, SLOT(onViewBottom()));
	connect(m_pUI->action_frontview, SIGNAL(triggered()), this, SLOT(onViewFront()));
	connect(m_pUI->action_backview, SIGNAL(triggered()), this, SLOT(onViewBack()));
	connect(m_pUI->action_leftview, SIGNAL(triggered()), this, SLOT(onViewLeft()));
	connect(m_pUI->action_rightview, SIGNAL(triggered()), this, SLOT(onViewRight()));
	connect(m_pUI->action_zoomin, SIGNAL(triggered()), this, SLOT(onViewZoomIn()));
	connect(m_pUI->action_zoomout, SIGNAL(triggered()), this, SLOT(onViewZoomOut()));
	connect(m_pUI->action_zoomall, SIGNAL(triggered()), this, SLOT(onViewZoomAll()));
	connect(m_pUI->action_orbit, SIGNAL(triggered()), this, SLOT(onViewOrbit()));
	connect(m_pUI->action_pan, SIGNAL(triggered()), this, SLOT(onViewPan()));
	connect(m_pUI->action_smoothdisp, SIGNAL(triggered()), this, SLOT(onViewSmooth()));
	connect(m_pUI->action_meshdisp, SIGNAL(triggered()), this, SLOT(onViewMesh()));
	connect(m_pUI->action_wiredisp, SIGNAL(triggered()), this, SLOT(onViewWire()));

	//menu inspection
	connect(m_pUI->actionInspectConfig, SIGNAL(triggered()), this, SLOT(onInspectConfig()));
	connect(m_pUI->actionInspectMode, SIGNAL(triggered()), this, SLOT(onInspectMode()));
	connect(m_pUI->actionInspectAnalysis, SIGNAL(triggered()), this, SLOT(onInspectAnalysis()));

	//menu processing
	connect(m_pUI->actionToolPathDlg, SIGNAL(triggered()), this, SLOT(onToolPathDlg()));
	connect(m_pUI->actionErrorComp, SIGNAL(triggered()), this, SLOT(onErrorComp()));

	//menu measure
	connect(m_pUI->actionMeasureStart, SIGNAL(triggered()), this, SLOT(onMeasureStart()));
	connect(m_pUI->actionMeasurePause, SIGNAL(triggered()), this, SLOT(onMeasurePause()));
	connect(m_pUI->actionMeasureReset, SIGNAL(triggered()), this, SLOT(onMeasureReset()));
	connect(m_pUI->actionSetCoorPrecisely, SIGNAL(triggered()), this, SLOT(onSetCoorPrecisely()));
	
	connect(m_pUI->actionOutNCFile, SIGNAL(triggered()), this, SLOT(onMeasureOutNCFile()));
	connect(m_pUI->actionOutDimFile, SIGNAL(triggered()), this, SLOT(onMeasureOutDimFile()));
	connect(m_pUI->actionMesPathPlanning, SIGNAL(triggered()), this, SLOT(onMeasurePathPlanning()));
	connect(m_pUI->actionGroupPathPlan, SIGNAL(triggered()), this, SLOT(onGroupPathPlan()));
	
	connect(m_pUI->actionMesRePath, SIGNAL(triggered()), this, SLOT(onMeasureRePath()));
	connect(m_pUI->actionFiveAxisPathPlan, SIGNAL(triggered()), this, SLOT(onFiveAxisPathPlan()));
	//connect(m_pUI->actionInsFeature, SIGNAL(triggered()), this, SLOT(onInsFeature()));
	connect(m_pUI->actionPointPlan_2, SIGNAL(triggered()), this, SLOT(onPointPlan()));
	connect(m_pUI->actionOpcUaTHU, SIGNAL(triggered()), this, SLOT(onOpc()));
	connect(m_pUI->actionOPC_ON, SIGNAL(triggered()), this, SLOT(onOpcSimu()));
	connect(m_pUI->actionCoordTrans, SIGNAL(triggered()), this, SLOT(onPathPlanAndExe()));
	connect(m_pUI->actionCoorAlt, SIGNAL(triggered()), this, SLOT(onCoordAlt()));
	

	//menu tool
	connect(m_pUI->actionToolResetLayout, SIGNAL(triggered()), this, SLOT(onToolResetWindowLayout()));
	connect(m_pUI->actionDefaultSetting, SIGNAL(triggered()), this, SLOT(onResetToDefaultSetting()));
	connect(m_pUI->action_ProbeMachine, SIGNAL(triggered()), this, SLOT(onToolFAdjustMachine()));
	connect(m_pUI->action_FProbe0, SIGNAL(triggered()), this, SLOT(onToolAdjustRingProbe()));
	connect(m_pUI->action_FProbe1, SIGNAL(triggered()), this, SLOT(onToolAdjustAngleProbe()));
	connect(m_pUI->action_FProbe2, SIGNAL(triggered()), this, SLOT(onToolAdjustUpSphere()));
	connect(m_pUI->action_FProbe3, SIGNAL(triggered()), this, SLOT(onToolAdjustLowerSphere()));
	connect(m_pUI->actionToolSelPnt, SIGNAL(triggered()), this, SLOT(onToolSelectIpPoint()));
	connect(m_pUI->actionToolSelSurf, SIGNAL(triggered()), this, SLOT(onToolSelectSurface()));
	connect(m_pUI->actionToolSetLocalFrame, SIGNAL(triggered()), this, SLOT(onToolSetLocalFrame()));
	connect(m_pUI->actionSurfaceEval, SIGNAL(triggered()), this, SLOT(onSurfEval()));
	connect(m_pUI->action_TCPConfig, SIGNAL(triggered()), this, SLOT(onTCPConfig()));

	//menu Test
	connect(m_pUI->actionTestDialog, SIGNAL(triggered()), this, SLOT(onTestDialog()));
	connect(m_pUI->actionTest_Others, SIGNAL(triggered()), this, SLOT(onTestOthers()));
	//recognition
	connect(m_pUI->actionRecogRib, SIGNAL(triggered()), this, SLOT(onRecogRib()));
	connect(m_pUI->actionRecogSlot, SIGNAL(triggered()), this, SLOT(onRecogSlot()));
	connect(m_pUI->actionRecogProfile, SIGNAL(triggered()), this, SLOT(onRecogProfile()));
	connect(m_pUI->actionRecogHole, SIGNAL(triggered()), this, SLOT(onRecogHole()));


}

void FMainWindow::dragEnterEvent(QDragEnterEvent* e)
{
	QString jcdMimeType = "application/x-qt-windows-mime;value=\"FileNameW\"";
	if ( e->mimeData()->hasFormat(jcdMimeType) )
	{
		e->acceptProposedAction();
	}
	QMainWindow::dragEnterEvent(e);
}

void FMainWindow::dropEvent(QDropEvent* e)
{
	QString jcdMimeType = "application/x-qt-windows-mime;value=\"FileNameW\"";
	QByteArray filenameBytes;
	QString filename;

	if (e->mimeData()->hasFormat(jcdMimeType))
	{
		filenameBytes=e->mimeData()->data(jcdMimeType);
		filename=QTextCodec::codecForName("UTF-16")->toUnicode(filenameBytes);
		OpenFile(filename);
	}
	QMainWindow::dropEvent(e);
}

void FMainWindow::PointerInit()
{
	if (m_pConverter)
	{
		delete m_pConverter;
		m_pConverter = NULL;
	}

	if (m_pPointPlan)
	{
		delete m_pPointPlan;
		m_pPointPlan = NULL;
	}

	if (m_pPathAndExe)
	{
		delete m_pPathAndExe;
		m_pPathAndExe = NULL;
	}

	if (m_pInsFeaDlg)
	{
		delete m_pInsFeaDlg;
		m_pInsFeaDlg = NULL;
	}

	if (m_pInsReport)
	{
		delete m_pInsReport;
		m_pInsReport = NULL;
	}

	if (m_pCoorAltDlg)
	{
		delete m_pCoorAltDlg;
		m_pCoorAltDlg = NULL;
	}

	if (m_pUpSphereDlg)
	{
		delete m_pUpSphereDlg;
		m_pUpSphereDlg = NULL;
	}

	if (m_pToolPathDlg)
	{
		delete m_pToolPathDlg;
		m_pToolPathDlg = NULL;
	}

	if (m_pGroupPathPlanDlg)
	{
		delete m_pGroupPathPlanDlg;
		m_pGroupPathPlanDlg = NULL;
	}

	if (m_pMachineErrCompDlg)
	{
		delete m_pMachineErrCompDlg;
		m_pMachineErrCompDlg = NULL;
	}

	if (m_pSetCoorPrecisely)
	{
		delete m_pSetCoorPrecisely;
		m_pSetCoorPrecisely = NULL;
	}

	if (m_pErrorMapDlg)
	{
		delete m_pErrorMapDlg;
		m_pErrorMapDlg = NULL;
	}

	//m_pDoc->setProbeDia(99999);
	//m_pDoc->setRefDia(99999);
	//m_pDoc->setNbLongitudePnt(99999);
	//m_pDoc->setNbLatitudePnt(99999);
}

void FMainWindow::MinimizeAll()
{
	if (m_pPointPlan && m_pPointPlan->isVisible())
	{
		m_pPointPlan->showMinimized();
	}
	if (m_pPathAndExe && m_pPathAndExe->isVisible())
	{
		m_pPathAndExe->showMinimized();
	}
	if (m_pInsFeaDlg && m_pInsFeaDlg->isVisible())
	{
		m_pInsFeaDlg->showMinimized();
	}
	if (m_pInsReport && m_pInsReport->isVisible())
	{
		m_pInsReport->showMinimized();
	}
	if (m_pCoorAltDlg && m_pCoorAltDlg->isVisible())
	{
		m_pCoorAltDlg->showMinimized();
	}
	if (m_pUpSphereDlg && m_pUpSphereDlg->isVisible())
	{
		m_pUpSphereDlg->showMinimized();
	}
	//if (m_pFiveAxisPath && m_pFiveAxisPath->isVisible())
	//{
	//	m_pFiveAxisPath->showMinimized();
	//}
	if (m_pToolPathDlg && m_pToolPathDlg->isVisible())
	{
		m_pToolPathDlg->showMinimized();
	}
	if (m_pGroupPathPlanDlg && m_pGroupPathPlanDlg->isVisible())
	{
		m_pGroupPathPlanDlg->showMinimized();
	}
	if (m_pMachineErrCompDlg && m_pMachineErrCompDlg->isVisible())
	{
		m_pMachineErrCompDlg->showMinimized();
	}
	if (m_pSetCoorPrecisely && m_pSetCoorPrecisely->isVisible())
	{
		m_pSetCoorPrecisely->showMinimized();
	}
	if (m_pErrorMapDlg && m_pErrorMapDlg->isVisible())
	{
		m_pErrorMapDlg->showMinimized();
	}

	
}

void FMainWindow::CloseAll()
{
	if (m_pPointPlan)
	{
		m_pPointPlan->close();
	}
	if (m_pPathAndExe)
	{
		m_pPathAndExe->close();
	}
	if (m_pInsFeaDlg)
	{
		m_pInsFeaDlg->close();
	}
	if (m_pInsReport)
	{
		m_pInsReport->close();
	}
	if (m_pCoorAltDlg)
	{
		m_pCoorAltDlg->close();
	}
	if (m_pUpSphereDlg)
	{
		m_pUpSphereDlg->close();
	}
	//if (m_pFiveAxisPath)
	//{
	//	m_pFiveAxisPath->close();
	//}
	if (m_pToolPathDlg)
	{
		m_pToolPathDlg->close();
	}
	if (m_pGroupPathPlanDlg)
	{
		m_pGroupPathPlanDlg->close();
	}
	if (m_pMachineErrCompDlg)
	{
		m_pMachineErrCompDlg->close();
	}
	if (m_pSetCoorPrecisely)
	{
		m_pSetCoorPrecisely->close();
	}
	if (m_pErrorMapDlg)
	{
		m_pErrorMapDlg->close();
	}

	
}

void FMainWindow::AssignTheoPnts(FInsFeature* pInsFea, vector<int>& FacePntIndexList, const FIpPntList& pointsAll)
{
	FacePntIndexList.clear();

	// 寻找该检测特征的全部曲面
	vector<TopoDS_Face> TDSFaceAll;
	for (int i = 0; i < pInsFea->m_FSurfList1.size(); i++)
	{
		FSurface* cFSurf = pInsFea->m_FSurfList1[i];
		TopoDS_Face cTDSFace = m_pConverter->FSurf2TDSFace(cFSurf);

		TDSFaceAll.push_back(cTDSFace);
	}

	for (int i = 0; i < pInsFea->m_FSurfList2.size(); i++)
	{
		FSurface* cFSurf = pInsFea->m_FSurfList2[i];
		TopoDS_Face cTDSFace = m_pConverter->FSurf2TDSFace(cFSurf);

		TDSFaceAll.push_back(cTDSFace);
	}

	int nPnts = pointsAll.size();

	vector<int> PntIndex;

	// 寻找每个面上的测点及测点索引
	for (auto ita = TDSFaceAll.begin(); ita != TDSFaceAll.end(); ++ita)
	{
		TopoDS_Face cFace = *ita;

		for (int i = 0; i < nPnts; i++)
		{
			if (isPointInShape(GP_PNT(pointsAll[i]), cFace))
			{
				PntIndex.push_back(i);
			}

		}

	}

}

void FMainWindow::ResetToDefaultCountenance()
{
	if (!m_pConverter)
	{
		return;
	}

	cout << "转到默认颜色" << endl;

	m_pConverter->setAllSurfColorDefault();

}

QWidget* FMainWindow::GetCurrentWnd()
{
	int nSize = m_wndlist.count();
	for (int i=0; i<nSize; i++)
	{
		if (m_wndlist[i]->isActiveWindow())
			return m_wndlist[i];
	}
	return 0;
}

bool FMainWindow::OpenFile(QString fname)
{
	//FT::FFreeBody* pFBody(NULL);
	QString fExt = FT::gGetExtFromName(fname).toUpper();
	if (fExt=="OBJ")
	{
		m_pFreeBody = gImportOBJFile(fname.toLocal8Bit().data());
	}
	else if (fExt=="STL")
		m_pFreeBody = gImportSTLFile(fname.toLocal8Bit().data());
	else if (fExt=="STEP" || fExt == "STP")
	{
		m_pConverter = new occConverter();
		m_pConverter->importStep(fname);
		static_cast<FTreeWnd*>(m_wndlist[0])->initTree_assembly(m_pConverter->getTreeList());

		// 分配节点
		//AllocateSurfNode();

	}
	//else if (fExt == "MDA")
	//{
	//	FIpPntList tPntlist;
	//	gOpenMDAFile(fname.toLocal8Bit().data(), tPntlist);
	//	m_pDoc->SetIpPntList(tPntlist);
	//}
	else 
	{
		QMessageBox::warning(
			QApplication::activeWindow(),QObject::tr("Caution"),QObject::tr(" File extension is not supported in current version!"));
	}
	if (fname.contains(QString("A320-D521-11414-200-02-LS")) && fExt=="OBJ")
	{
		FForm trans, rotT;
		trans.Translation(F3dVector(-264.764, -894.977, 0));
		rotT.Rotation(-141.121/180*PI, F3dVector(0, 0, 1), F3dPoint(0, 0, 0));
		m_pFreeBody->DoTransform(rotT*trans);
		m_pFreeBody->Update();
	}
	if(!m_pFreeBody)	return false;
	m_pDoc->AddDispEnt((FEntity*)m_pFreeBody);
	return true;
}

bool FMainWindow::onFileNew()
{
	if (!m_pDoc) return false;

	if (m_pDoc->isAniRunning())
	{
		return false;
	}

	//因为测头与检测轨迹的Entity存储在线程器中而不是文档中
	//因此重置时不能将其从显示列表删除,清除前把它取出，后再放入
	//FPathVisualization*	tmpEnt = nullptr;
	//PFEntityList* pDispList = m_pDoc->GetDispEntList();
	//for (int i = 0; i < pDispList->size(); i++)
	//{
	//	if (dynamic_cast<FPathVisualization*>((*pDispList)[i]))
	//	{
	//		tmpEnt = dynamic_cast<FPathVisualization*>((*pDispList)[i]);
	//		break;
	//	}
	//}

	m_pDoc->OnNewDocument();

	//if (tmpEnt)
	//{
	//	m_pDoc->AddDispEnt(tmpEnt);
	//}

	static_cast<FTreeWnd*>(m_wndlist[0])->clearTree();
	static_cast<FDataWnd*>(m_wndlist[1])->clearData();
	static_cast<FCodeWnd*>(m_wndlist[2])->clearTextEdit();
	static_cast<FInsFeaTreeWnd*>(m_wndlist[6])->clearTree();
	static_cast<AccuEvaluWnd*>(m_wndlist[7])->onRemoveAllEvaluItem();
	static_cast<AccuEvaluWnd*>(m_wndlist[7])->clearTree();

	m_pDoc->UpdateAllViews();
	return true;

}

bool FMainWindow::onFileClose()
{
	if (m_pMdiArea)
	{
		m_pMdiArea->closeAllSubWindows();
	}
	return true;
}

bool FMainWindow::onFileOpen()
{
	if (!m_pDoc)	return false;

	QString defaultModelDir = gDlgSetting->value("ModelReadPath").toString();
	QString filename = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择模型文件"), defaultModelDir, tr("Mesh/Point files (*.obj *.stl *.step *.mda *.stp)"));

	if (!filename.isEmpty())
	{
		gDlgSetting->setValue("ModelReadPath", filename);

		if (OpenFile(filename) && m_pModelWnd)
		{
			m_pDoc->UpdateAllViews();
			m_pModelWnd->GetView()->GetKernelView()->ZoomAll();
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

bool FMainWindow::onFileSave()
{
	function<QString(FSurface*)> theFSurfName = [](FSurface* aFSurf)->QString
	{
		SURFACE_TYPE theSurfType;
		int ind_in_all = 0;
		int ind_in_type = 0;
		QString tmpFSurfName;

		gWindow->GetSTEPModelConverter()->FSurfInfo(aFSurf, theSurfType, ind_in_all, ind_in_type, tmpFSurfName);

		return tmpFSurfName;
	};

	//保存用户检测配置数据
	QString defaultConfigSaveDir = gDlgSetting->value("ConfigSavePath").toString();
	QString fileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("保存检测配置"), defaultConfigSaveDir, tr("Config Files (*.cfg)"));

	if (!fileName.isEmpty()) {
		gDlgSetting->setValue("ConfigSavePath", fileName);

		QFile file(fileName);
		if (!file.open(QIODevice::WriteOnly)) {
			QMessageBox::information(gWindow, QString::fromLocal8Bit("写入错误,ERROR!"),
				QString::fromLocal8Bit("文件不可写入！"), "OK");
			return 0;
		}
		else {
			QTextStream stream(&file);

			//写入配置文件标识符
			stream << "Thu-OMI InspConfig File" << endl;

			//写入文件名
			stream << "STEP file name: " << m_pConverter->GetPartName() << endl;

			//写入红宝石球半径
			stream << "Dameter of ruby ball: " << m_pDoc->getProbeDia() << endl;

			//写入标准球直径
			stream << "Dameter of reference sphere: " << m_pDoc->getRefDia() << endl;

			//写入经向点数
			stream << "Longitude points number: " << m_pDoc->getNbLongitudePnt() << endl;

			//写入纬向点数
			stream << "Latitude points number: " << m_pDoc->getNbLatitudePnt() << endl;

			//写入检测特征
			for (int i = 0; i < m_pDoc->GetInsFeaList().size(); i++)
			{
				QString tmpInsType;
				FInsFeature* CurrFea = m_pDoc->GetInsFeaList()[i];
				switch (m_pDoc->GetInsFeaList()[i]->type)
				{
				case PARALLELISM:
					tmpInsType = "PARALLELISM";
					break;

				case ROUNDNESS:
					tmpInsType = "ROUNDNESS";
					break;

				case DISTANCE:
					if (m_pDoc->GetInsFeaList()[i]->DirState == 1)
					{
						tmpInsType = "DISTANCEX";
					}
					else if (m_pDoc->GetInsFeaList()[i]->DirState == 2)
					{
						tmpInsType = "DISTANCEY";
					}
					else if (m_pDoc->GetInsFeaList()[i]->DirState == 3)
					{
						tmpInsType = "DISTANCEZ";
					}
					else
					{
						tmpInsType = "DISTANCEL";
					}
					break;

				case VERTICALITY:
					tmpInsType = "VERTICALITY";
					break;

				case DIAMETER:
					tmpInsType = "DIAMETER";
					break;

				case AUXIALITY:
					tmpInsType = "AUXIALITY";
					break;

				case FLATNESS:
					tmpInsType = "FLATNESS";
					break;

				default:
					tmpInsType = "OTHER";
					break;
				}

				stream << "InsFeature " << i << endl;
				stream << "InsType: " << tmpInsType << endl;
				stream << "Tolerance Value: " << m_pDoc->GetInsFeaList()[i]->FInsToleranceValue << endl << "UP " << m_pDoc->GetInsFeaList()[i]->FInsUpDeviationValue << endl << "DOWN " << m_pDoc->GetInsFeaList()[i]->FInsDownDeviationValue << endl;
				
				for (int j = 0; j < m_pDoc->GetInsFeaList()[i]->m_FSurfList1.size(); j++)
				{
					stream << "Geom1" << ":" << theFSurfName(m_pDoc->GetInsFeaList()[i]->m_FSurfList1[j]) << endl;
				}

				for (int j = 0; j < m_pDoc->GetInsFeaList()[i]->m_FSurfList2.size(); j++)
				{
					stream << "Geom2" << ":" << theFSurfName(m_pDoc->GetInsFeaList()[i]->m_FSurfList2[j]) << endl;
				}

				stream << "End Fea " << i << endl;
			}

			//写入对模型的变换
			if (m_pCoorAltDlg)
			{
				for (int i = 0; i < m_pCoorAltDlg->GetTransRec().size(); i++)
				{
					stream << "Transform " << i << endl;

					stream << "Move X: " << m_pCoorAltDlg->GetTransRec()[i].Move_X << endl;
					stream << "Move Y: " << m_pCoorAltDlg->GetTransRec()[i].Move_Y << endl;
					stream << "Move Z: " << m_pCoorAltDlg->GetTransRec()[i].Move_Z << endl;

					stream << "Rot X: " << m_pCoorAltDlg->GetTransRec()[i].Rotate_X << endl;
					stream << "Rot Y: " << m_pCoorAltDlg->GetTransRec()[i].Rotate_Y << endl;
					stream << "Rot Z: " << m_pCoorAltDlg->GetTransRec()[i].Rotate_Z << endl;

					stream << "End Trans " << i << endl;
				}
			}

			if (m_pPathAndExe)
			{
				stream << "Safe distance: " << m_pPathAndExe->GetSafeDis() << endl;
				stream << "Retreat distance: " << m_pPathAndExe->GetRetreatDis() << endl;
			}

			stream.flush();
			file.close();
		}
	}

	return true;
}

void FMainWindow::onImportMeaData()
{
	QString defaultIMeasDir = gDlgSetting->value("IMeasPath").toString();
	QString filename = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择测量数据"), defaultIMeasDir, tr("*.txt *.mpf *.MPF"));

	if (!filename.isEmpty())
	{
		gDlgSetting->setValue("IMeasPath", filename);

		QMessageBox msgBox;
		msgBox.setText(QString::fromLocal8Bit("是否进行误差补偿？"));
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);

		int ret = msgBox.exec();
		if (ret == QMessageBox::Yes) 
		{
			CSDataTrans* pDt = new CSDataTrans();
			pDt->pMc = new FProbeComp(m_pDoc);
			pDt->txtFileReader(filename);

		}
		else if(ret == QMessageBox::No)
		{
			CSDataTrans::importMesData(filename);
		}
		else
		{
			MesgBox(QString::fromLocal8Bit("未执行数据导入"));
		}

	}

}

void FMainWindow::onImportTheoData()
{
	QString defaultITheoDir = gDlgSetting->value("ITheoPath").toString();

	QString filename = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择理论测点文件"), defaultITheoDir, tr("*.txt"));

	if (!filename.isEmpty())
	{
		gDlgSetting->setValue("ITheoPath", filename);

		CSDataTrans* pDt = new CSDataTrans();
		pDt->importTheoData(filename);
	}
}

void FMainWindow::onExportTheoData()
{
	QString defaultETheoDir = gDlgSetting->value("ETheoPath").toString();
	QString fileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("导出理论测点"), defaultETheoDir,
		tr("Text Files (*.txt)"));

	if (!fileName.isEmpty()) {
		gDlgSetting->setValue("ETheoPath", fileName);

		QFile file(fileName);
		if (!file.open(QIODevice::WriteOnly)) {
			QMessageBox::information(gWindow, QString::fromLocal8Bit("写入错误,ERROR!"),
				QString::fromLocal8Bit("文件不可写入！"), "OK");
			return;
		}
		else {
			QTextStream stream(&file);

			int num = this->GetMainDoc()->GetIpPntList().size();

			if (num == 0)
			{
				return;
			}

			for (int i = 0; i < num; i++)
			{
				FIpPoint IpPnt = GetMainDoc()->GetIpPntList().at(i);

				double cx = IpPnt.x;
				double cy = IpPnt.y;
				double cz = IpPnt.z;
				double ci = IpPnt.i;
				double cj = IpPnt.j;
				double ck = IpPnt.k;

				cx = zeroLim(cx);
				cy = zeroLim(cy);
				cz = zeroLim(cz);
				ci = zeroLim(ci);
				cj = zeroLim(cj);
				ck = zeroLim(ck);
				
				stream << i + 1 << "	" << cx << "	" << cy << "	" << cz << "	" << ci << "	" << cj << "	" << ck << endl;
				//stream << cx << "	" << cy << "	" << cz << "	" << ci << "	" << cj << "	" << ck << endl;
				//stream << i + 1 << " " << IpPnt.x << " " << IpPnt.y << " " << IpPnt.z << " " << IpPnt.i << " " << IpPnt.j << " " << IpPnt.k << endl;
			}
			//stream << ui->textEdit->toPlainText();
			stream.flush();
			file.close();
		}
	}
}

void FMainWindow::onExportResult()
{
	QString defaultEMeasDir = gDlgSetting->value("EMeasPath").toString();
	QString fileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("导出实际测点"), defaultEMeasDir, tr("Text Files (*.txt)"));

	if (!fileName.isEmpty()) {
		gDlgSetting->setValue("EMeasPath", fileName);

		QFile file(fileName);
		if (!file.open(QIODevice::WriteOnly)) {
			QMessageBox::information(gWindow, QString::fromLocal8Bit("写入错误,ERROR!"),
				QString::fromLocal8Bit("文件不可写入！"), "OK");
			return;
		}
		else {
			QTextStream stream(&file);

			int num = this->GetMainDoc()->GetMesPntList().size();

			if (num == 0)
			{
				return;
			}

			for (int i = 0; i < num; i++)
			{
				string sx, sy, sz, si, sj, sk;
				FIpPoint MesPnt = GetMainDoc()->GetMesPntList().at(i);

				stream << i + 1 << "	" << MesPnt.x << "	" << MesPnt.y << "	" << MesPnt.z << "	" << MesPnt.i << "	" << MesPnt.j << "	" << MesPnt.k << endl;

			}
			//stream << ui->textEdit->toPlainText();
			stream.flush();
			file.close();
		}
	}
}

void FMainWindow::onExportReport()
{
	InspReportInfoDlg* theInfoDlg = new InspReportInfoDlg(static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->GetEvaDataList());
	theInfoDlg->show();


	//FInsReportDlg* m_pInsReport = new FInsReportDlg(static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->GetEvaDataList());
	//QScrollArea* s = new QScrollArea();
	//s->setWindowIcon(QIcon("../icons/AccuRep.png"));
	//s->setWindowTitle(QString::fromLocal8Bit("检测报告"));
	//s->setGeometry(100, 80, 1260, 900);
	//s->setFixedSize(1285, 900);
	//s->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	//s->setWidget(m_pInsReport);
	//s->show();
}

void FMainWindow::onImportInspConfig()
{
	if (!m_pConverter)
	{
		QMessageBox::warning(
			QApplication::activeWindow(), QObject::tr("Caution"), QString::fromLocal8Bit("未打开STEP文件"));
		return;
	}

	QString defaultSelConfigDir = gDlgSetting->value("SelConfigPath").toString();
	QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择检测配置文件"), defaultSelConfigDir,
		tr("Config Files (*.cfg)"));

	if (fileName.isEmpty()) return;
	gDlgSetting->setValue("SelConfigPath", fileName);

	QFile file(fileName);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug() << "Can't open the file!" << endl;
		return;
	}

	int ReadLineIndex = 0;
	int ReadInsFeaIndex = 0;
	int TransformIndex = 0;

	vector<FInsFeature*> TheFileInsFeas;
	//bool IsInsFea = false;
	bool IsTrans = false;
	vector<FInsFeature*> FileInsFeas;
	vector<TransPara> FileTransList;

	while (!file.atEnd())
	{
		QByteArray line = file.readLine();
		QString CurrLineStr(line);

		if ((ReadLineIndex == 0) && (!CurrLineStr.contains("Thu-OMI InspConfig File")))
		{
			QMessageBox::warning(
				QApplication::activeWindow(), QObject::tr("Caution"), QString::fromLocal8Bit("无效的配置文件"));
			return;
		}

		ReadLineIndex += 1;

		//读取文件名
		if (CurrLineStr[0].toAscii() == 'S' && CurrLineStr[1].toAscii() == 'T' && CurrLineStr[2].toAscii() == 'E' && CurrLineStr[3].toAscii() == 'P')
		{
			string theSubNameRed = CurrLineStr.mid(16, CurrLineStr.size() - 2).toStdString();
			string theFileNameSave = m_pConverter->GetPartName().toStdString();

			if (m_pConverter->GetPartName() + "\n" != CurrLineStr.mid(16))
			{
				QMessageBox::warning(
					QApplication::activeWindow(), QObject::tr("Caution"), QString::fromLocal8Bit("该配置与模型不匹配"));
				return;
			}
		}

		//读取红宝石球直径
		if (CurrLineStr[11].toAscii() == 'r' && CurrLineStr[12].toAscii() == 'u' && CurrLineStr[13].toAscii() == 'b' && CurrLineStr[14].toAscii() == 'y')
		{
			m_pDoc->setProbeDia(ExDouFrQsg(CurrLineStr));
		}

		//读取标准球半直径
		if (CurrLineStr[11].toAscii() == 'r' && CurrLineStr[12].toAscii() == 'e' && CurrLineStr[13].toAscii() == 'f' && CurrLineStr[14].toAscii() == 'e')
		{
			m_pDoc->setRefDia(ExDouFrQsg(CurrLineStr));
		}

		//读取经向点数
		if (CurrLineStr[0].toAscii() == 'L' && CurrLineStr[1].toAscii() == 'o' && CurrLineStr[2].toAscii() == 'n' && CurrLineStr[3].toAscii() == 'g')
		{
			m_pDoc->setNbLongitudePnt(ExDouFrQsg(CurrLineStr));
		}

		//读取纬向点数
		if (CurrLineStr[0].toAscii() == 'L' && CurrLineStr[1].toAscii() == 'a' && CurrLineStr[2].toAscii() == 't' && CurrLineStr[3].toAscii() == 'i')
		{
			m_pDoc->setNbLatitudePnt(ExDouFrQsg(CurrLineStr));
		}

		//读取检测特征
		if (CurrLineStr[0].toAscii() == 'I' && CurrLineStr[1].toAscii() == 'n' && CurrLineStr[2].toAscii() == 's' && CurrLineStr[3].toAscii() == 'F')
		{
			//IsInsFea = true;
			cout << "读取到字符Insfea:" << CurrLineStr[11].toAscii() <<endl;
			FileInsFeas.push_back(new FInsFeature);
			QString FeaIndexAscii = CurrLineStr[11].toAscii();
			FileInsFeas[ReadInsFeaIndex]->InsFeatureindex = ExNumFrQsg(CurrLineStr);
		}

		//读取检测类型
		if (CurrLineStr[0].toAscii() == 'I' && CurrLineStr[1].toAscii() == 'n' && CurrLineStr[2].toAscii() == 's' && CurrLineStr[3].toAscii() == 'T')
		{
			if (CurrLineStr[9].toAscii() == 'P' && CurrLineStr[10].toAscii() == 'A' && CurrLineStr[11].toAscii() == 'R' && CurrLineStr[12].toAscii() == 'A')
			{
				FileInsFeas[ReadInsFeaIndex]->type = PARALLELISM;
			}
			else if (CurrLineStr[9].toAscii() == 'R' && CurrLineStr[10].toAscii() == 'O' && CurrLineStr[11].toAscii() == 'U' && CurrLineStr[12].toAscii() == 'N')
			{
				FileInsFeas[ReadInsFeaIndex]->type = ROUNDNESS;
			}
			else if (CurrLineStr[9].toAscii() == 'D' && CurrLineStr[10].toAscii() == 'I' && CurrLineStr[11].toAscii() == 'S' && CurrLineStr[12].toAscii() == 'T')
			{
				FileInsFeas[ReadInsFeaIndex]->type = DISTANCE;
				if (CurrLineStr[17].toAscii() == 'X')
				{
					FileInsFeas[ReadInsFeaIndex]->DirState = 1;
				}
				else if (CurrLineStr[17].toAscii() == 'Y')
				{
					FileInsFeas[ReadInsFeaIndex]->DirState = 2;
				}
				else if (CurrLineStr[17].toAscii() == 'Z')
				{
					FileInsFeas[ReadInsFeaIndex]->DirState = 3;
				}
				else
				{
					FileInsFeas[ReadInsFeaIndex]->DirState = 0;
				}
			}
			else if (CurrLineStr[9].toAscii() == 'V' && CurrLineStr[10].toAscii() == 'E' && CurrLineStr[11].toAscii() == 'R' && CurrLineStr[12].toAscii() == 'T')
			{
				FileInsFeas[ReadInsFeaIndex]->type = VERTICALITY;
			}
			else if (CurrLineStr[9].toAscii() == 'D' && CurrLineStr[10].toAscii() == 'I' && CurrLineStr[11].toAscii() == 'A' && CurrLineStr[12].toAscii() == 'M')
			{
				FileInsFeas[ReadInsFeaIndex]->type = DIAMETER;
			}
			else if (CurrLineStr[9].toAscii() == 'A' && CurrLineStr[10].toAscii() == 'U' && CurrLineStr[11].toAscii() == 'X' && CurrLineStr[12].toAscii() == 'I')
			{
				FileInsFeas[ReadInsFeaIndex]->type = AUXIALITY;
			}
			else if (CurrLineStr[9].toAscii() == 'F' && CurrLineStr[10].toAscii() == 'L' && CurrLineStr[11].toAscii() == 'A' && CurrLineStr[12].toAscii() == 'T')
			{
				FileInsFeas[ReadInsFeaIndex]->type = FLATNESS;
			}
			else
			{
				FileInsFeas[ReadInsFeaIndex]->type = UNKNOW;
			}
		}

		//读取误差数值
		if (CurrLineStr[0].toAscii() == 'T' && CurrLineStr[1].toAscii() == 'o' && CurrLineStr[2].toAscii() == 'l' && CurrLineStr[3].toAscii() == 'e')
		{
			FileInsFeas[ReadInsFeaIndex]->FInsToleranceValue = ExDouFrQsg(CurrLineStr);
		}
		if (CurrLineStr[0].toAscii() == 'U' && CurrLineStr[1].toAscii() == 'P')
		{
			FileInsFeas[ReadInsFeaIndex]->FInsUpDeviationValue = ExDouFrQsg(CurrLineStr);
		}
		if (CurrLineStr[0].toAscii() == 'D' && CurrLineStr[1].toAscii() == 'O')
		{
			FileInsFeas[ReadInsFeaIndex]->FInsDownDeviationValue = ExDouFrQsg(CurrLineStr);
		}
		
		//读取检测特征涉及的几何体
		vector<FSurface*> FSurfsFromConverter = m_pConverter->GetFSurfList();

		function<FSurface* (QString)> GetFSurfByName = [&](QString theName)->FSurface*
		{
			int tmpNbFace = ExNumFrQsg(theName);

			if (theName[0] == QString("平面")[0] && theName[1] == QString("平面")[1])
			{
				return FSurfsFromConverter[m_pConverter->GetPlaneInds()[tmpNbFace]];
			}
			else if (theName[0] == QString("B样")[0] && theName[1] == QString("B样")[1])
			{
				return FSurfsFromConverter[m_pConverter->GetBSplineInds()[tmpNbFace]];
			}
			else if (theName[0] == QString("锥面")[0] && theName[1] == QString("锥面")[1])
			{
				return FSurfsFromConverter[m_pConverter->GetConicalInds()[tmpNbFace]];
			}
			else if (theName[0] == QString("圆柱")[0] && theName[1] == QString("圆柱")[1])
			{
				return FSurfsFromConverter[m_pConverter->GetCylindricalInds()[tmpNbFace]];
			}
			else if (theName[0] == QString("球面")[0] && theName[1] == QString("球面")[1])
			{
				return FSurfsFromConverter[m_pConverter->GetSphericalInds()[tmpNbFace]];
			}
			else if (theName[0] == QString("其他")[0] && theName[1] == QString("其他")[1])
			{
				return FSurfsFromConverter[m_pConverter->GetOtherInds()[tmpNbFace]];
			}
			else
			{
				QMessageBox::warning(
					QApplication::activeWindow(), QObject::tr("Caution"), QString::fromLocal8Bit("读取配置时出错，曲面类型判断错误"));
				return NULL;
			}

		};


		if (CurrLineStr[0].toAscii() == 'G' && CurrLineStr[1].toAscii() == 'e' && CurrLineStr[2].toAscii() == 'o' && CurrLineStr[3].toAscii() == 'm' && CurrLineStr[4].toAscii() == '1')
		{
			QString SubString = CurrLineStr;
			SubString.replace("Geom1:", "");
			cout << "显示替换后的字符为：" << SubString.toStdString() << endl;

			vector<FSurface*> tmpFSurfList1;

			FileInsFeas[ReadInsFeaIndex]->m_FSurfList1.push_back(GetFSurfByName(SubString));
		}

		if (CurrLineStr[0].toAscii() == 'G' && CurrLineStr[1].toAscii() == 'e' && CurrLineStr[2].toAscii() == 'o' && CurrLineStr[3].toAscii() == 'm' && CurrLineStr[4].toAscii() == '2')
		{
			QString SubString = CurrLineStr;
			SubString.replace("Geom2:", "");
			cout << "显示替换后的字符为：" << SubString.toStdString() << endl;

			FileInsFeas[ReadInsFeaIndex]->m_FSurfList2.push_back(GetFSurfByName(SubString));
		}

		//结束一个检测特征的读取
		if (CurrLineStr[0].toAscii() == 'E' && CurrLineStr[1].toAscii() == 'n' && CurrLineStr[2].toAscii() == 'd' && CurrLineStr[4].toAscii() == 'F')
		{
			ReadInsFeaIndex += 1;
		}

		//读取变换参数
		if (CurrLineStr[0].toAscii() == 'T' && CurrLineStr[1].toAscii() == 'r' && CurrLineStr[2].toAscii() == 'a' && CurrLineStr[3].toAscii() == 'n')
		{
			FileTransList.push_back(TransPara());
			TransformIndex = ExNumFrQsg(CurrLineStr);
		}

		if (CurrLineStr[0].toAscii() == 'M' && CurrLineStr[1].toAscii() == 'o' && CurrLineStr[2].toAscii() == 'v' && CurrLineStr[5].toAscii() == 'X')
		{
			FileTransList[TransformIndex].Move_X = ExDouFrQsg(CurrLineStr);
		}

		if (CurrLineStr[0].toAscii() == 'M' && CurrLineStr[1].toAscii() == 'o' && CurrLineStr[2].toAscii() == 'v' && CurrLineStr[5].toAscii() == 'Y')
		{
			FileTransList[TransformIndex].Move_Y = ExDouFrQsg(CurrLineStr);
		}

		if (CurrLineStr[0].toAscii() == 'M' && CurrLineStr[1].toAscii() == 'o' && CurrLineStr[2].toAscii() == 'v' && CurrLineStr[5].toAscii() == 'Z')
		{
			FileTransList[TransformIndex].Move_Z = ExDouFrQsg(CurrLineStr);
		}

		if (CurrLineStr[0].toAscii() == 'R' && CurrLineStr[1].toAscii() == 'o' && CurrLineStr[2].toAscii() == 't' && CurrLineStr[4].toAscii() == 'X')
		{
			FileTransList[TransformIndex].Rotate_X = ExDouFrQsg(CurrLineStr);
		}

		if (CurrLineStr[0].toAscii() == 'R' && CurrLineStr[1].toAscii() == 'o' && CurrLineStr[2].toAscii() == 't' && CurrLineStr[4].toAscii() == 'Y')
		{
			FileTransList[TransformIndex].Rotate_Y = ExDouFrQsg(CurrLineStr);
		}

		if (CurrLineStr[0].toAscii() == 'R' && CurrLineStr[1].toAscii() == 'o' && CurrLineStr[2].toAscii() == 't' && CurrLineStr[4].toAscii() == 'Z')
		{
			FileTransList[TransformIndex].Rotate_Z = ExDouFrQsg(CurrLineStr);
		}

		//设定安全距离，回退距离
		if (CurrLineStr[0].toAscii() == 'S' && CurrLineStr[1].toAscii() == 'a' && CurrLineStr[2].toAscii() == 'f' && CurrLineStr[3].toAscii() == 'e')
		{
			if (!m_pPathAndExe)
			{
				m_pPathAndExe = new FPathPlanAndExeDlg();
			}
			m_pPathAndExe->SetSafeDis(ExDouFrQsg(CurrLineStr));
		}

		if (CurrLineStr[0].toAscii() == 'R' && CurrLineStr[1].toAscii() == 'e' && CurrLineStr[2].toAscii() == 't' && CurrLineStr[4].toAscii() == 'r')
		{
			if (!m_pPathAndExe)
			{
				m_pPathAndExe = new FPathPlanAndExeDlg();
			}
			m_pPathAndExe->SetRetreatDis(ExDouFrQsg(CurrLineStr));
		}

	}

	//设定模型变换参数
	if (!m_pCoorAltDlg)
	{
		m_pCoorAltDlg = new CoorTrans;
	}

	m_pCoorAltDlg->SetTransRec(FileTransList);
	m_pCoorAltDlg->ExeTeransRec();

	//设定检测特征
	for (int i = 0; i < FileInsFeas.size(); i++)
	{
		initTree_by_insfea(FileInsFeas[i], static_cast<FInsFeaTreeWnd*>(gWindow->GetSubWnd()[6])->GetTree());
		//static_cast<FInsFeaTreeWnd*>(gWindow->GetSubWnd()[6])->onUpdateMeasPnts();

		initTree_by_insfea(FileInsFeas[i], static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->GetTree());

		gWindow->GetMainDoc()->GetInsFeaList().push_back(FileInsFeas[i]);
	}

}

void FMainWindow::onViewTop()
{
	m_pModelWnd->GetView()->setWindowTitle(QObject::tr("Top(X/Y)"));
	m_pModelWnd->GetView()->GetKernelView()->SetViewType(FT::VIEW_TOP);
	m_pModelWnd->GetView()->update();
}

void FMainWindow::onViewBottom()
{
	m_pModelWnd->GetView()->setWindowTitle(QObject::tr("Bottom(X/Y)"));
	m_pModelWnd->GetView()->GetKernelView()->SetViewType(FT::VIEW_BOTTOM);
	m_pModelWnd->GetView()->update();
}

void FMainWindow::onViewFront()
{
	m_pModelWnd->GetView()->setWindowTitle(QObject::tr("Front(X/Z)"));
	m_pModelWnd->GetView()->GetKernelView()->SetViewType(FT::VIEW_FRONT);
	m_pModelWnd->GetView()->update();
}

void FMainWindow::onViewBack()
{
	m_pModelWnd->GetView()->setWindowTitle(QObject::tr("Front(X/Z)"));
	m_pModelWnd->GetView()->GetKernelView()->SetViewType(FT::VIEW_BACK);
	m_pModelWnd->GetView()->update();
}

void FMainWindow::onViewLeft()
{
	m_pModelWnd->GetView()->setWindowTitle(QObject::tr("Front(Y/Z)"));
	m_pModelWnd->GetView()->GetKernelView()->SetViewType(FT::VIEW_LEFT);
	m_pModelWnd->GetView()->update();
}

void FMainWindow::onViewRight()
{
	m_pModelWnd->GetView()->setWindowTitle(QObject::tr("Front(Y/Z)"));
	m_pModelWnd->GetView()->GetKernelView()->SetViewType(FT::VIEW_RIGHT);
	m_pModelWnd->GetView()->update();
}

void FMainWindow::onViewZoomIn()
{
	m_pModelWnd->GetView()->GetKernelView()->Zoom(1.5);
	m_pModelWnd->GetView()->update();
}

void FMainWindow::onViewZoomOut()
{
	m_pModelWnd->GetView()->GetKernelView()->Zoom(0.75);
	m_pModelWnd->GetView()->update();
}

void FMainWindow::onViewZoomAll()
{
	m_pModelWnd->GetView()->GetKernelView()->ZoomAll();
	m_pModelWnd->GetView()->update();
}

void FMainWindow::onViewSmooth()
{
	m_pModelWnd->GetView()->GetKernelView()->SetDispMode(FDispMode::SMOOTH_MODE);
	m_pModelWnd->GetView()->update();
	gSetHintMessage(QObject::tr("Smooth display"));
}

void FMainWindow::onViewMesh()
{
	m_pModelWnd->GetView()->GetKernelView()->SetDispMode(FDispMode::MESH_MODE);
	m_pModelWnd->GetView()->update();
	gSetHintMessage(QObject::tr("Mesh display"));
}

void FMainWindow::onViewWire()
{
	m_pModelWnd->GetView()->GetKernelView()->SetDispMode(FDispMode::WIRE_MODE);
	m_pModelWnd->GetView()->update();
	gSetHintMessage(QObject::tr("Wire display"));
}

void FMainWindow::onViewPan()
{
	m_pModelWnd->GetView()->GetKernelView()->SetCameraType(FCameraType::PAN_CAM);
	m_pModelWnd->GetView()->update();
	gSetHintMessage(QObject::tr("Drag any mouse to translate model"));
}

void FMainWindow::onViewOrbit()
{
	m_pModelWnd->GetView()->GetKernelView()->SetCameraType(FCameraType::ORBIT_CAM);
	m_pModelWnd->GetView()->update();
	gSetHintMessage(QObject::tr("Drag any mouse to rotate model"));
}

void FMainWindow::onInspectConfig()
{
	//【0】树窗口为第0位
	FTreeWnd* pWnd = new FTreeWnd(this, m_pDoc);
	//FAssemTreeWnd* pWnd = new FAssemTreeWnd(this, m_pDoc);
	//pWnd->setWindowTitle(QObject::tr("Assembly Tree"));
	pWnd->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	addDockWidget(Qt::LeftDockWidgetArea,pWnd);
	//	addDockWidget(static_cast<Qt::DockWidgetArea>(1), dockWidget);
	m_wndlist.push_back(pWnd);

	//【1】数据窗口为第1位
	FDataWnd* pDataWnd = new FDataWnd(this, m_pDoc);
	//pDataWnd->setWindowTitle(QObject::tr("Data"));
	pDataWnd->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	addDockWidget(Qt::RightDockWidgetArea,pDataWnd);
	m_wndlist.push_back(pDataWnd);

	//【2】代码窗口为第2位
	FCodeWnd* pCodeWnd = new FCodeWnd(this);
	//pCodeWnd->setWindowTitle(QObject::tr("Code"));
	pCodeWnd->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	addDockWidget(Qt::RightDockWidgetArea,pCodeWnd);
	//pCodeWnd->hide();
	m_wndlist.push_back(pCodeWnd);

	//【3】视图窗口为第3位
	//sub-window used to display 3D model
	m_pModelWnd = new FSubWindow(this, m_pDoc);
	m_pModelWnd->setWindowTitle("Model(3D)");
	//m_pModelWnd->setVisible(true);
	m_pModelWnd->GetView()->GetKernelView()->SetViewType(FViewType::VIEW_FRONT);
	m_pMdiArea->addSubWindow(m_pModelWnd);
	m_wndlist.push_back(m_pModelWnd);

	//【4】数据可视化窗口为第4位
	FStatVisualWnd* pStatWnd = new FStatVisualWnd(this, m_pDoc);
	pStatWnd->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	addDockWidget(Qt::RightDockWidgetArea,pStatWnd);
	m_wndlist.push_back(pStatWnd);

	//tabifyDockWidget(pCodeWnd, pStatWnd);
	//tabifyDockWidget(pStatWnd, pCodeWnd);

	//【5】数据配置窗口为第5位
	//FStatConfigWnd* pConfigWnd = new FStatConfigWnd(this, m_pDoc);
	//pConfigWnd->setFeatures(QDockWidget::DockWidgetClosable|QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	//addDockWidget(Qt::RightDockWidgetArea,pConfigWnd);
	//m_wndlist.push_back(pConfigWnd);
	m_wndlist.push_back(NULL);

	//【6】检测特征窗口为第6位
	FInsFeaTreeWnd* pInsFeatWnd = new FInsFeaTreeWnd(this, m_pDoc);
	pInsFeatWnd->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	addDockWidget(Qt::LeftDockWidgetArea, pInsFeatWnd);
	m_wndlist.push_back(pInsFeatWnd);

	//added by chengxi
	//【7】精度评定窗口为第7位
	AccuEvaluWnd* pAccuEvaluWnd = new AccuEvaluWnd(this, m_pDoc);
	//pAccuEvaluWnd->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	addDockWidget(Qt::RightDockWidgetArea, pAccuEvaluWnd);
	m_wndlist.push_back(pAccuEvaluWnd);

	////added by chengxi
	////【8】MySQL读取窗口为第8位
	//MySQL_ReaderWnd* pMySqlWnd = new MySQL_ReaderWnd(this);
	//addDockWidget(Qt::RightDockWidgetArea, pMySqlWnd);
	//m_wndlist.push_back(pMySqlWnd);

	//tabifyDockWidget(pDataWnd, pConfigWnd);
	//tabifyDockWidget(pConfigWnd, pDataWnd);

	//added by chengxi
	tabifyDockWidget(pDataWnd, pAccuEvaluWnd);
	tabifyDockWidget(pAccuEvaluWnd, pDataWnd);

	//tabifyDockWidget(pDataWnd, pMySqlWnd);
	//tabifyDockWidget(pMySqlWnd, pDataWnd);
	
	//connect(pConfigWnd->m_pByTimeBtn, SIGNAL(clicked()),pStatWnd,SLOT(PlotPic1()));

	m_pMdiArea->tileSubWindows();

	//QHBoxLayout* vboxLayout = new QHBoxLayout;
	//vboxLayout->addWidget(new QPushButton("Btn1"));
	//vboxLayout->addWidget(new QPushButton("Btn2"));
	//vboxLayout->addWidget(new QPushButton("Btn3"));
	//m_pMdiArea->setLayout(vboxLayout);
}

void FMainWindow::onInspectAnalysis()
{

}

void FMainWindow::onToolPathDlg()
{
	if (!m_pToolPathDlg)
	{
		m_pToolPathDlg = new FToolPathDlg;
		m_pToolPathDlg->show();
	}
	else
	{
		m_pToolPathDlg->show();
	}
}

void FMainWindow::onErrorComp()
{
	if (m_pErrorMapDlg != NULL)
	{
		m_pErrorMapDlg->show();
	}
	else
	{
		m_pErrorMapDlg = new ErrCompsteDlg;
		m_pErrorMapDlg->show();
	}

}

void FMainWindow::onInspectMode()
{

}


void FMainWindow::onMeasureStart()
{
	//if (m_threadworker.isTCPConnectted	==	false	&&	m_threadworker.enableThread == true)
	//{
	//	m_threadworker.establishTCPConnection();
	//}
	//m_threadworker.enableThread = true;
	//if (m_threadworker.isWorking)
	//{
	//	return;
	//}
	//m_threadworker.startWorking();
	

	//用于测试移动测头命令
	m_pProbeVis->Initial();

	if (!m_pDoc->GetPathList().size())
	{
		QMessageBox::warning(
			QApplication::activeWindow(), QObject::tr("Caution"), QString::fromLocal8Bit("未检测到检测路径，请进行路径规划及生成NC代码后再试"));
		return;
	}

	m_pDoc->AddDispEnt(m_pProbeVis);

	//if (!m_pVisThread->isRunning())
	//{
	//	m_pVisThread->start();
	//}
	m_pVisThread->start();

}

void FMainWindow::onMeasurePause()
{
	////m_threadworker.enableThread = false;

	//if(m_threadworker.isTCPConnectted	==	true)
	//{
	//	m_threadworker.disconnectFromtheHost();
	//	m_threadworker.isTCPConnectted	=	false;
	//}


	//用于测试移动测头命令
	if (!m_pDoc->GetPathList().size())
	{
		QMessageBox::warning(
			QApplication::activeWindow(), QObject::tr("Caution"), QString::fromLocal8Bit("未检测到检测路径，请进行路径规划及生成NC代码后再试"));
		return;
	}

	if (m_pVisThread->isRunning())
	{
		m_pVisThread->terminate();
	}
}

void FMainWindow::onMeasureReset()
{
	//if(m_threadworker.isWorking)
	//{
	//	if (m_pDoc->isAniRunning())
	//	{
	//		return;
	//	}
	//	m_threadworker.m_FPathControl->resetPath();
	//	static_cast<FCodeWnd*>(gWindow->GetSubWnd()[2])->clearHighlight();
	//	//m_threadworker.m_FPathData.ClearPoints();
	//}
	//
	////delete m_thread.m_FPathControl;



	//用于测试移动测头命令
	if (!m_pDoc->GetPathList().size())
	{
		QMessageBox::warning(
			QApplication::activeWindow(), QObject::tr("Caution"), QString::fromLocal8Bit("未检测到检测路径，请进行路径规划及生成NC代码后再试"));
		return;
	}

	m_pVisThread->terminate();

	m_pDoc->RemoveDispEnts(m_pProbeVis);
	m_pDoc->RemoveDispEnts(m_pProbeVis);
	m_pDoc->RemoveDispEnts(m_pProbeVis);

	m_pVisThread->exit();
	m_pVisThread->CurPntIndex = 0;

	UpdateModelView();
}

void FMainWindow::onSetCoorPrecisely()
{
	if (!m_pSetCoorPrecisely)
	{
		m_pSetCoorPrecisely = new SetCoorPrecisely;
	}

	m_pSetCoorPrecisely->show();

}

void FMainWindow::onMeasureOutNCFile()
{
	FIpPathList tPathList = m_pDoc->GetPathList();
	FTHP thp;
	thp.TH_NCFileWrite(tPathList);
}

void FMainWindow::onMeasureOutDimFile()
{
	//FIpPathList tPathList = m_pDoc->GetPathList();
	//int nPath = tPathList.size();
	//if(!m_pPathPlan || nPath<1)	return ;
	////step1. distinguish inspection  and intermediate points
	//QVector<FDimsPnt> tDimsPnts;	
	//int nPnt(0);
	//int totalCount(0);
	////handle the first path
	//nPnt = tPathList[0].GetSize();
	//for (int j=0; j<nPnt; j++)	//intermediate points
	//{
	//	totalCount++;
	//	FDimsPnt mesPnt;	//measure point
	//	mesPnt.x = tPathList[0].GetPoint(j).x;
	//	mesPnt.y = tPathList[0].GetPoint(j).y;
	//	mesPnt.z = tPathList[0].GetPoint(j).z;
	//	mesPnt.i = tPathList[0].GetPoint(j).i;
	//	mesPnt.j = tPathList[0].GetPoint(j).j;
	//	mesPnt.k = tPathList[0].GetPoint(j).k;
	//	if(j==nPnt-1)
	//		mesPnt.label = 1;
	//	else
	//		mesPnt.label = 0;
	//	mesPnt.name = QString::fromLocal8Bit("Pnt%1").arg(j).toStdString();
	//	tDimsPnts.push_back(mesPnt);
	//}
	////handle 1~(nPath-1) path
	//for (int i=1; i<nPath-1; i++)
	//{
	//	nPnt = tPathList[i].GetSize();
	//	if(nPnt==0)	continue;
	//	for (int j=1; j<nPnt; j++)	//intermediate points
	//	{
	//		totalCount++;
	//		FDimsPnt mesPnt;	//measure point
	//		mesPnt.x = tPathList[i].GetPoint(j).x;
	//		mesPnt.y = tPathList[i].GetPoint(j).y;
	//		mesPnt.z = tPathList[i].GetPoint(j).z;
	//		mesPnt.i = tPathList[i].GetPoint(j).i;
	//		mesPnt.j = tPathList[i].GetPoint(j).j;
	//		mesPnt.k = tPathList[i].GetPoint(j).k;
	//		if(j==nPnt-1)
	//			mesPnt.label = 1;
	//		else
	//			mesPnt.label = 0;
	//		mesPnt.name = QString::fromLocal8Bit("Pnt%1").arg(i*nPnt+j).toStdString();
	//		tDimsPnts.push_back(mesPnt);
	//	}
	//}
	////handle the last path
	//int i = nPath-1;
	//nPnt = tPathList[i].GetSize();
	//for (int j=1; j<nPnt; j++)	//intermediate points
	//{
	//	totalCount++;
	//	FDimsPnt mesPnt;	//measure point
	//	mesPnt.x = tPathList[i].GetPoint(j).x;
	//	mesPnt.y = tPathList[i].GetPoint(j).y;
	//	mesPnt.z = tPathList[i].GetPoint(j).z;
	//	mesPnt.i = tPathList[i].GetPoint(j).i;
	//	mesPnt.j = tPathList[i].GetPoint(j).j;
	//	mesPnt.k = tPathList[i].GetPoint(j).k;
	//	mesPnt.label = 0;
	//	mesPnt.name = QString::fromLocal8Bit("Pnt%1").arg(i*nPnt+j).toStdString();
	//	tDimsPnts.push_back(mesPnt);
	//}
	////step2. write into DIM commands
	//QString file = gAppPath + "/Dims.dim";    //
	//ofstream outdims;
	//outdims.open(file.toStdString());
	//if (!outdims.is_open())
	//	cout << "File open failed" << endl;
	//outdims << "UNITS/MM, ANGDEC" << endl;
	//outdims << "MODE/AUTO" << endl;
	//outdims << "GOHOME" << endl;
	//int np = tDimsPnts.size();
	//for (int i = 0; i <np; i++)      //
	//{
	//	if (tDimsPnts[i].label == 0)
	//	{
	//		outdims << tDimsPnts[i].DimsGoto()<< endl;
	//	};
	//	if (tDimsPnts[i].label == 1)
	//	{
	//		outdims << tDimsPnts[i].DimsDef() << endl;
	//		outdims << tDimsPnts[i].DimsMS()<< endl;
	//		outdims << tDimsPnts[i].DimsMeas() << endl;
	//		outdims << "ENDMES" << endl;
	//	};
	//	if (i==np-1)
	//	{
	//	//	outdims << "GOHOME" << endl;
	//	}
	//}
}

void FMainWindow::onMeasurePathPlanning()
{
	//int nsize = m_pDoc->DispEntCount();
	//if(nsize==0) return ;
	////F3DFeature* pEnt(NULL);
	//FEntity* pEnt(NULL);
	//for (int i=0; i<nsize; i++)
	//{		
	//	if(m_pDoc->GetDispEnt(i) && m_pDoc->GetDispEnt(i)->GetEntType()==FT_BODY)
	//	{
	//		pEnt =  m_pDoc->GetDispEnt(i);
	//		break;
	//	}
	//}
	////pEnt = (F3DFeature*)m_pDoc->GetDispEnt(0);
	//if (!m_pPathPlan)	
	//	m_pPathPlan = new FPathPlan(m_pDoc);
	//ON_Mesh allMesh;
	//ON_Mesh* tMesh(NULL);
	//if(pEnt)
	//{
	//	//FFreeBody* pBody = (FFreeBody*)pEnt;
	//	FBody* pBody = (FBody*)pEnt;
	//	//if(!pBody || !pBody->Body()) return ;
	//	if(pBody->Surfaces().size()>0)
	//	{
	//		nsize = pBody->Surfaces().size();
	//		for (int i=0; i<nsize; i++)
	//		{
	//			if(!pBody->Surfaces().at(i)->GetMesh())
	//				continue;
	//			tMesh = pBody->Surfaces().at(i)->GetMesh()->OnMesh();
	//			allMesh.Append(*tMesh);
	//		}
	//	} 
	//	m_pPathPlan->SetObstacleMesh(&allMesh);
	//}
	//m_pPathPlan->SetBackDist(6);
	//m_pPathPlan->SetSaftDist(30);
	//m_pPathPlan->SetIpPntList(m_pDoc->GetIpPntList());
	//FIntList orders;	
	//m_pDoc->ClearIpPathList();
	//m_pPathPlan->Planning(orders, m_pDoc->GetPathList());

}

void FMainWindow::onGroupPathPlan()
{
	if (!m_pGroupPathPlanDlg)
	{
		m_pGroupPathPlanDlg = new FGroupPathPlan;
	}

	m_pGroupPathPlanDlg->show();
}

void FMainWindow::onMeasureRePath()
{
	////a new tool to select points
	//FRePathTool* pTool = new FRePathTool(m_pDoc);
	////FAdjustUpSphere* pTool = new FAdjustUpSphere(m_pDoc);
	//m_pDoc->PushTool(pTool);
	//int nsize = m_pDoc->DispEntCount();
	//if(nsize==0) return ;
	//F3DFeature* pEnt(NULL);
	//for (int i=0; i<nsize; i++)
	//{		
	//	if(m_pDoc->GetDispEnt(i) && m_pDoc->GetDispEnt(i)->FeatureType()==FT_3D)
	//	{
	//		pEnt = (F3DFeature*) m_pDoc->GetDispEnt(i);
	//		break;
	//	}
	//}
	//ON_Mesh allMesh;
	//ON_Mesh* tMesh(NULL);
	//if(pEnt)
	//{
	//	FFreeBody* pBody = (FFreeBody*)pEnt;
	//	if(!pBody || !pBody->Body()) return ;
	//	if(pBody->Body()->Surfaces().size()>0)
	//	{
	//		nsize = pBody->Body()->Surfaces().size();
	//		for (int i=0; i<nsize; i++)
	//		{
	//			if(!pBody->Body()->Surfaces().at(i)->GetMesh())
	//				continue;
	//			tMesh = pBody->Body()->Surfaces().at(0)->GetMesh()->OnMesh();
	//			allMesh.Append(*tMesh);
	//		}
	//	} 
	//	pTool->SetObstacleMesh(&allMesh);
	//}
}

void FMainWindow::onPointPlan()
{
	if (!m_pPointPlan)
	{
		m_pPointPlan = new PointPlanDlg;
	}
	
	m_pPointPlan->show();
}

//void FMainWindow::onFiveAxisPathPlan()
//{
//	FiveAxisPathPlan* Five = new FiveAxisPathPlan(this);
//	Five->SetDoc(m_pDoc,m_pPathPlan);
//	Five->show();
//} 

void FMainWindow::onFiveAxisPathPlan()
{

	//QSplitter* splitterMain=new QSplitter(Qt::Vertical,0);//分割窗口垂直布局
	//splitterMain->setOpaqueResize(true);
	//splitterMain->setFixedWidth(475);
	//QListWidget* list=new QListWidget(splitterMain);
	//list->setFlow(QListView::LeftToRight); //列表从左到右横向布局
	//list->setFrameStyle(QFrame::Panel|QFrame::Raised);
	////list->setMovement(QListView::Static);
	////list->setViewMode(QListView::IconMode);
	//list->insertItem(0,QString::fromLocal8Bit("控制窗口"));
	//list->insertItem(1,QString::fromLocal8Bit("结果显示"));
	//list->setFixedHeight(40);

	//list->setStyleSheet("QListWidget{border:1px solid gray; color:black; }"
	//	"QListWidget::Item{padding-top:20px; padding-bottom:4px; }"
	//	"QListWidget::Item:hover{background:skyblue; }"
	//	"QListWidget::item:selected{background:lightgray; color:red; }"
	//	"QListWidget::item:selected:!active{border-width:0px; background:lightgreen; }"
	//	);

	//ContentWnd* content=new ContentWnd(splitterMain);
	//QObject::connect(list,SIGNAL(currentRowChanged(int)),content->stack,SLOT(setCurrentIndex(int)));
	//splitterMain->setWindowTitle(QString::fromLocal8Bit("五轴路径规划"));
	//splitterMain->show();
}

void FMainWindow::onOpc()
{
	QProcess *myProcess = new QProcess();
	myProcess->start("OpcUaTHU/OpcUaTHU.exe");
}

void FMainWindow::onToolResetWindowLayout()
{
	if(!m_pMdiArea) return;
	m_pMdiArea->setActivationOrder(QMdiArea::CreationOrder);
	m_pMdiArea->tileSubWindows();
}

void FMainWindow::onResetToDefaultSetting()
{
	gDlgSetting->setValue("IsOpenHelpDlg", true);
	gDlgSetting->setValue("IsHideUpAdjustSphereWarningFromNowOn", false);
	gDlgSetting->setValue("PartNumber", "WS119DK.08.015/035\\x7ec4\\x5408\\x7bb1\\x4f53");
	gDlgSetting->setValue("InspStaff", "\\x674e\\x5b81");
	gDlgSetting->setValue("InspEquipment", "\\x6570\\x63a7\\x5367\\x5f0f\\x52a0\\x5de5\\x4e2d\\x5fc3\\xff08SAJO16000\\xff09 020718500202010");
	gDlgSetting->setValue("OtherData", "\\x65e0");
}

void FMainWindow::onToolFAdjustMachine(){
	//FAdjustMachine* pFAdjustMachine = new FAdjustMachine(this);
 //   pFAdjustMachine->show();

	m_pMachineErrCompDlg = new FAdjustMachine(this);
	m_pMachineErrCompDlg->show();
}
void FMainWindow::onToolAdjustRingProbe(){
    FAdjustRingProbe* pFAdjustProbe0 = new FAdjustRingProbe(this);
    pFAdjustProbe0->show();
}
void FMainWindow::onToolAdjustAngleProbe(){
	FAdjustAngleProbe* pFAdjustProbe1 = new FAdjustAngleProbe(this);
    pFAdjustProbe1->show();
}
void FMainWindow::onToolAdjustUpSphere(){

	// 显示标定上半球窗口前的提示信息
	//if (!gDlgSetting->value("IsHideUpAdjustSphereWarningFromNowOn").toBool())
	//{
	//	AdjustUpSphereWarning theWarning;
	//	theWarning.show();
	//	theWarning.exec();
	//	theWarning.CloseDlg();
	//}

	if (!m_pUpSphereDlg)
	{
		m_pUpSphereDlg = new FAdjustUpSphere(this);
	}
	m_pUpSphereDlg->show();
}
void FMainWindow::onToolAdjustLowerSphere(){
	//FAdjustLowerSphere* pFAdjustProbe3 = new FAdjustLowerSphere(this);
 //   pFAdjustProbe3->show();
}

//tcp/ip configuration window
void FMainWindow::onTCPConfig(){
	//FConnectConfig* pConnectConfig = new FConnectConfig(this);
	//pConnectConfig->show();
	/*m_threadworker.hostIP = m_pDoc->GetIP();
	cout<<m_threadworker.hostIP.toString().toLocal8Bit().data()<<endl;
	m_threadworker.hostIP.setAddress(QString("172.17.24.23"));
	m_threadworker.port = 3328;*/
}

void FMainWindow::onToolSelectIpPoint()
{
	//add your code here
}

void FMainWindow::onToolSelectSurface()
{
	//if (m_pCurTool)
	//{
	//	m_pDoc->PopTool();
	//	delete m_pCurTool;
	//}
	//m_pCurTool = new FSelectTool(m_pDoc);
	//m_pDoc->PushTool(m_pCurTool);
	////((FSelectTool*)m_pCurTool)->SetSelEntType(FT_SURFACE);
	//((FSelectTool*)m_pCurTool)->SetSelEntType(FT_PLANAR_FACE);
	////((FSelectTool*)m_pCurTool)->SetSelEntType(FT_SPHERICAL_FACE);
	//int nsize = m_pDoc->DispEntCount();
	//PFEntityList tlist;
	//for (int i=0; i<nsize; i++)
	//	tlist.push_back(m_pDoc->GetDispEnt(i));
	//m_pDoc->ClearDispEnts();
	//F3DFeature* pFt(NULL);
	//FBody* pBody(NULL);
	//for (int i=0; i<nsize; i++)
	//{
	//	if (tlist[i]->FeatureType()==FT_3D)
	//	{
	//		pFt = (F3DFeature*)tlist[i];
	//		pBody = pFt->Body();
	//	}
	//	else if (tlist[i]->GetEntType()==FT_BODY)
	//		pBody = (FBody*) tlist[i];
	//	if(!pBody)	continue;
	//	int nSurf = pBody->Surfaces().size();
	//	for (int j=0; j<nSurf; j++)
	//	{
	//		m_pDoc->AddDispEnt(pBody->Faces()[j]);
	//	}
	//	pBody = NULL;
	//}
}

void FMainWindow::onToolSetLocalFrame()
{

}

void FMainWindow::onSurfEval()
{
	FAccuracyEvaluation* pAccuracy = new FAccuracyEvaluation(this);
    pAccuracy->show();
}

void FMainWindow::onTestOthers()
{
	
}

void FMainWindow::onTestDialog()
{
	FTest* pTestDlg = new FTest;
	pTestDlg->show();

}
void FMainWindow::onRecogRib()
{
	//auto a = m_pDoc->getConvertor();
	//a->simpleRibMatching();
	//a->ribRecognition();
	//auto pointer = LCollisionWorld::Instence();
	//auto surf = pointer->m_collisionShapeYu[0]->m_surfs_map;
	//for (auto itr = surf.begin(); itr != surf.end(); ++itr) {
	//	itr->second->SetColor(0.5, 0.5, 0.5);
	//}
	//set<int> rib;
	//set<set<int> > rib2;
	//for(auto iter1=pointer->m_ribset.begin();iter1!=pointer->m_ribset.end();iter1++)
	//{
	//	set<int>temprib;
	//	for(auto iter2=iter1->begin();iter2!=iter1->end();iter2++)
	//	{
	//		rib.insert(*iter2);
	//		temprib.insert(*iter2); 
	//	}
	//	rib2.insert(temprib);
	//}
	//for(auto iter1=pointer->m_simpleribset.begin();iter1!=pointer->m_simpleribset.end();iter1++)
	//{
	//	set<int>temprib;
	//	for(auto iter2=iter1->begin();iter2!=iter1->end();iter2++)
	//	{
	//		rib.insert(*iter2);
	//		temprib.insert(*iter2);
	//	}
	//	rib2.insert(temprib);
	//}
	//for(auto iter1=pointer->m_ribtop.begin();iter1!=pointer->m_ribtop.end();iter1++)
	//{
	//	rib.insert(*iter1);
	//}

	//m_pDoc->setrib(rib2);
	//m_pDoc->setribtop(pointer->m_ribtop);

	//for (auto itr2 = rib.begin(); itr2 != rib.end(); ++itr2) {
	//	if(surf.find(*itr2) == surf.end()) continue;
	//	surf[*itr2]->SetColor(1.0, 0.0, 0.0);
	//	cout << *itr2 << "  ";
	//}
	//cout << endl;
	//for (auto itr = surf.begin(); itr != surf.end(); ++itr) {
	//	itr->second->Update();
	//}
	//pointer->m_correntid++;
	//static_cast<FTreeWnd*>(m_wndlist[0])->clearTree();
	//static_cast<FTreeWnd*>(m_wndlist[0])->initTree_YU();
}

void FMainWindow::onRecogSlot()
{
	//auto a = m_pDoc->getConvertor();
	//a->cavityRecognition();
	//auto pointer = LCollisionWorld::Instence();
	//auto surf = pointer->m_collisionShapeYu[0]->m_surfs_map;
	//for (auto itr = surf.begin(); itr != surf.end(); ++itr) {
	//	itr->second->SetColor(0.5, 0.5, 0.5);
	//}
	//set<int> cavity;

	//m_pDoc->setslot(pointer->m_cavityset);

	//for(auto iter1=pointer->m_cavityset.begin();iter1!=pointer->m_cavityset.end();iter1++)
	//{
	//	for(auto iter2=iter1->begin();iter2!=iter1->end();iter2++)
	//	{
	//		cavity.insert(*iter2);
	//	}
	//}
	//
	//for (auto itr2 = cavity.begin(); itr2 != cavity.end(); ++itr2) {
	//	if(surf.find(*itr2) == surf.end()) continue;
	//	surf[*itr2]->SetColor(1.0, 0.0, 0.0);
	//	cout << *itr2 << "  ";
	//}
	//cout << endl;
	//for (auto itr = surf.begin(); itr != surf.end(); ++itr) {
	//	itr->second->Update();
	//}
	//pointer->m_correntid++;
	//static_cast<FTreeWnd*>(m_wndlist[0])->clearTree();
	//static_cast<FTreeWnd*>(m_wndlist[0])->initTree_YU();

}

void FMainWindow::onRecogProfile()
{
	//auto a = m_pDoc->getConvertor();
	//a->contourRecognition();
	//auto pointer = LCollisionWorld::Instence();
	//auto surf = pointer->m_collisionShapeYu[0]->m_surfs_map;
	//for (auto itr = surf.begin(); itr != surf.end(); ++itr) {
	//	itr->second->SetColor(0.5, 0.5, 0.5);
	//}
	//set<int> counter;

	//m_pDoc->setprofile(pointer->m_counterset);

	//for(auto iter1=pointer->m_counterset.begin();iter1!=pointer->m_counterset.end();iter1++)
	//{
	//	counter.insert(*iter1);
	//}
	//
	//for (auto itr2 = counter.begin(); itr2 != counter.end(); ++itr2) {
	//	if(surf.find(*itr2) == surf.end()) continue;
	//	surf[*itr2]->SetColor(1.0, 0.0, 0.0);
	//	cout << *itr2 << "  ";
	//}
	//cout << endl;
	//for (auto itr = surf.begin(); itr != surf.end(); ++itr) {
	//	itr->second->Update();
	//}
	//pointer->m_correntid++;
	//static_cast<FTreeWnd*>(m_wndlist[0])->clearTree();
	//static_cast<FTreeWnd*>(m_wndlist[0])->initTree_YU();

}
	
void FMainWindow::onRecogHole()
{
	//auto a = m_pDoc->getConvertor();
	//a->holeMatching();
	//auto pointer = LCollisionWorld::Instence();
	//auto surf = pointer->m_collisionShapeYu[0]->m_surfs_map;
	//for (auto itr = surf.begin(); itr != surf.end(); ++itr) {
	//	itr->second->SetColor(0.5, 0.5, 0.5);
	//}
	//set<int> hole;

	//m_pDoc->sethole(pointer->m_holeset);

	//for(auto iter1=pointer->m_holeset.begin();iter1!=pointer->m_holeset.end();iter1++)
	//{
	//	for(auto iter2=iter1->begin();iter2!=iter1->end();iter2++)
	//	{
	//		hole.insert(*iter2);
	//	}
	//}
	//
	//for (auto itr2 = hole.begin(); itr2 != hole.end(); ++itr2) {
	//	if(surf.find(*itr2) == surf.end()) continue;
	//	surf[*itr2]->SetColor(1.0, 0.0, 0.0);
	//	cout << *itr2 << "  ";
	//}
	//cout << endl;
	//for (auto itr = surf.begin(); itr != surf.end(); ++itr) {
	//	itr->second->Update();
	//}
	//pointer->m_correntid++;
	//static_cast<FTreeWnd*>(m_wndlist[0])->clearTree();
	//static_cast<FTreeWnd*>(m_wndlist[0])->initTree_YU();

}

void FMainWindow::resizeEvent(QResizeEvent* e)
{
	int nSize = m_wndlist.count();
	for (int i=0; i<nSize; i++)
	{
		if (m_wndlist[i])
		{
		//	m_wndlist[i]->showMaximized();
		//	m_wndlist[i]->resizeEvent(e);
		}
	}
}

void FMainWindow::UpdateAllViews()
{
	//cout	<<	"UpdateAllViews thread:"	<<	QThread::currentThreadId()	<<	endl;
	int nSize = m_wndlist.count();
	for (int i=0; i<nSize; i++)
	{
		if (m_wndlist[i] && !m_wndlist[i]->isHidden())		
			m_wndlist[i]->update();
	}
	if (m_pModelWnd)
		m_pModelWnd->Update();
	//int a = 0;
}

void FMainWindow::UpdateModelView()
{
	if (m_pModelWnd)
		m_pModelWnd->Update();
}

void FMainWindow::onImportGuideFile()
{
//	if (!m_pDoc)	return;
//		m_pDoc->AdjustProbeFlag = 0 ;
//	if (m_pDoc->GetIpPntList().size()!=0)
//	{
//		int tVal=QMessageBox::information(this,QString::fromLocal8Bit("提醒"),
//			QString::fromLocal8Bit("放弃当前已经导入的指导文件？"),QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
//		if(tVal==QMessageBox::Yes)
//			m_pDoc->clear();
//	}
//	m_pDoc->ClearIpPntList();
//	FFileDialog fileDlg(this,QString::fromLocal8Bit("请选择指导文件"),
//		"Guide file(*.xml);;All files (*.*)",false);
//	QString filename = fileDlg.GetFileNameAsOpen();
//
//	if (!filename.isEmpty())
//	{
//		QString	stlName	=	filename.split(".")[0] + QString(".stl");
//		if (OpenFile(stlName) && m_pModelWnd)
//		{
//			m_pDoc->UpdateAllViews();
//			m_pModelWnd->GetView()->GetKernelView()->ZoomAll();
//		}
//		else
//			QMessageBox::information(this,QString::fromLocal8Bit("对应模型文件不存在"),
//			QString::fromLocal8Bit("在当前目录下没有对应的stl文件！"),"OK");
//	}
//	else
//		return;
////	m_pDoc->clear();
//	//m_threadworker.displayPathVisual();
//	FXMLReader*	reader	=	new FXMLReader(m_pDoc);
//	reader->load(filename);
//
//	//等待线程读取完数据
//	while(!reader->isFinished())
//	{}
//
//	//建立NC代码段与相对应的测头检测路径的映射
//	FIpPathList		pathList	=	m_pDoc->GetPathList();
//	QVector<QVector<QString> >	mappedCodeList;
//	for ( int i = 0; i < pathList.size()-1; i++ )
//	{
//		//初始化大小与pathList相同的代码映射表(长度-1，因为最后一个路径不为监测点)
//		QVector<QString> tmp(pathList[i].GetSize(), "");
//		mappedCodeList.push_back(tmp);
//		FIpPntList pathPnts = pathList[i].GetPathPnts();
//		//处理路径途径点的代码映射关系（非测量点）
//		for ( int j = 1; j < pathList[i].GetSize()-1; j++ )
//		{
//			QString xStr	=	QString::number(pathPnts[j].x,'f',6);
//			QString yStr	=	QString::number(pathPnts[j].y,'f',6);
//			QString zStr	=	QString::number(pathPnts[j].z,'f',6);
//			QString	codeBlock = QString("R20=%1\nR21=%2\nR22=%3\nSMOVE\n")
//				.arg(xStr).arg(yStr).arg(zStr);
//			mappedCodeList[i][j]	=	codeBlock;
//		}
//	}
//	//cout	<<	"reader thread finished...\n";
//	QString a,b;
//	a	=	"";
//	int num1	=	0;
//	int m		=	reader->m_programList.size();
//	int totaln	=	reader->m_pointList.size();
//	for(int n=0;n<m;n++)
//	{
//		b=reader->m_programList[n];
//		if ((b[0]=='T')&&(b[1]=='H'))
//		{
//			//double xxxx=PointsList.GetAt(n)->x;
//			if (num1>=totaln)
//			{				
//				cout	<<	"Error:command more than points...\n";
//				return;
//			}
//			else
//			{
//				QString	tmp = FTHP::TH_GetBody(reader->m_pointList[num1].x,reader->m_pointList[num1].y,
//					reader->m_pointList[num1].z,reader->m_pointList[num1].i,
//					reader->m_pointList[num1].j,reader->m_pointList[num1].k);
//				tmp +=	"\n";
//				a	+=	tmp;
//				mappedCodeList[num1][mappedCodeList[num1].size()-1]	=	tmp;
//				num1++;
//			}
//		} 
//		else
//		{
//			a+=b;
//			a+="\n";
//		}
//	}
//	delete	reader;
//	FTHP::TH_NCFileWrite(a);
//	static_cast<FCodeWnd*>(m_wndlist[2])->setTextEdit(a);
//
//	//根据映射的代码段建立代码段在代码中起点与长度的映射
//	int startPos = 0;
//	QVector<QVector<FCodePosInfo> > mappedPosInfoList;
//	int lastPos = -1; int lastLine = 0;
//	for ( int i = 0; i < mappedCodeList.size(); i++ )
//	{
//		QVector<FCodePosInfo> tmp = QVector<FCodePosInfo>(mappedCodeList[i].size());
//		mappedPosInfoList.push_back(tmp);
//		for ( int j = 1; j < mappedCodeList[i].size(); j++ )
//		{
//			//cout	<<	mappedCodeList[i][j].toLocal8Bit().data()	<< endl;
//			int pos		= a.indexOf(mappedCodeList[i][j],startPos);
//			int length	= mappedCodeList[i][j].length();
//			FCodePosInfo info	=	FCodePosInfo(pos, length);
//			info.calcLineData(a, lastPos, lastLine);
//			lastPos		=	pos	+	length	-1;
//			lastLine	=	info.lineOfEnd;
//			mappedPosInfoList[i][j] = info;
//			//cout	<<	a.mid(pos, length).toLocal8Bit().data()	<< endl;
//			startPos	=	pos	+	length;
//		}
//	}
//	m_pDoc->SetMappedPosInfoList(mappedPosInfoList);
//
//	//m_wndlist[0]->setTextEdit( a );
//
//	/*QString test;
//	for (int i = 0; i < mappedCodeList.size(); i++)
//	{
//		for (int j = 1; j < mappedCodeList[i].size(); j++)
//		{
//			test += mappedCodeList[i][j];
//		}
//	}
//	QFile	NC("E:\\src\\test.txt");
//	if (!NC.open(QFile::WriteOnly|QFile::Truncate|QFile::Text))
//	{
//		return;
//	}
//	QTextStream	out(&NC);
//	out	<<	test;*/
//	
//	static_cast<FTreeWnd*>(m_wndlist[0])->initTree();
//	/*if (m_pDoc->GetProbe()==NULL)
//		m_pDoc->ImportProbe();*/
//	return;
}

void FMainWindow::onExportInspectDataToXML()
{
	//m_pDoc->ComputeAllFeatures();

	////如果对应文件夹不存在，则先创建
	//if (!gIsDirExist(gAppPath + tr("\\history")))
	//{
	//	return;
	//}
	//
	//QString partNameStr = "model";
	//QString machineIdStr = "01";
	//QString dateStr = QDate::currentDate().toString("yyyyMMdd");
	//QString idStr = "1";
	//QString fileName = partNameStr + tr("_") + machineIdStr + tr("_") +
	//	dateStr + tr("_") + idStr + tr(".xml");

	//QString xmlPath = gAppPath + tr("\\history\\") + fileName;
	//QFile file(xmlPath);  
	//if (!file.open(QFile::WriteOnly | QFile::Truncate)) {  
	//	return;  
	//}  
	//
	////根节点元素
	//QDomDocument	doc;
	//QDomElement		root = doc.createElement("part");
	//QDomAttr		part_name = doc.createAttribute("name");
	//part_name.setValue(partNameStr);
	//root.setAttributeNode(part_name);
	//doc.appendChild(root);

	////添加元素：测量点数据
	//FIpPntList mesPntList = m_pDoc->GetMesPntList();
	//QDomElement		mesPntElem = doc.createElement("point");
	//QDomAttr		mesPnt_count = doc.createAttribute("count");
	//mesPnt_count.setValue(QString::number(mesPntList.size()));
	//mesPntElem.setAttributeNode(mesPnt_count);
	//QDomText		mesPnt_text;
	//QString			mesPntStr;
	//for (int i = 0; i < mesPntList.size(); i++)
	//{
	//	mesPntStr += QString::number(mesPntList[i].x) + tr(" ") + 
	//				QString::number(mesPntList[i].y) + tr(" ") + 
	//				QString::number(mesPntList[i].z) + tr(";");
	//}
	//mesPnt_text = doc.createTextNode(mesPntStr);
	//mesPntElem.appendChild(mesPnt_text);
	//root.appendChild(mesPntElem);

	////添加元素：特征
	//QVector<FIpFeature*> PlaneFtVec = m_pDoc->getFeatureMap()[FIpFeature::FN];
	//for (int i = 0; i < PlaneFtVec.size(); i++)
	//{
	//	QDomElement		ftElem = doc.createElement("feature");
	//	QDomAttr		ft_name = doc.createAttribute("name");
	//	QDomAttr		ft_type = doc.createAttribute("type");
	//	QTreeWidgetItem* pItem = static_cast<FTreeWnd*>
	//		(m_wndlist[0])->GetItem(i, FIpFeature::FN);
	//	if (!pItem)
	//	{
	//		return;
	//	}
	//	ft_name.setValue(pItem->text(0));
	//	ft_type.setValue(QString::fromLocal8Bit("FN"));
	//	ftElem.setAttributeNode(ft_name);
	//	ftElem.setAttributeNode(ft_type);

	//	QDomElement idxElem = doc.createElement("index");
	//	QString		idxStr;
	//	FIpFeature*	pInsFt = PlaneFtVec[i];
	//	for (int j = 0; j < pInsFt->m_selectedPntList.size(); j++)
	//	{
	//		idxStr += QString::number(pInsFt->m_selectedPntList[j]) + tr(" ");
	//	}
	//	QDomText	text = doc.createTextNode(idxStr);
	//	idxElem.appendChild(text);
	//	ftElem.appendChild(idxElem);

	//	QDomElement valElem = doc.createElement("value");
	//	QString		valStr;
	//	for (int j = 0; j < pInsFt->GetResult().size(); j++)
	//	{
	//		valStr += QString::number(pInsFt->GetResult()[j]) + tr(" ");
	//	}
	//	text = doc.createTextNode(valStr);
	//	valElem.appendChild(text);
	//	ftElem.appendChild(valElem);
	//	
	//	root.appendChild(ftElem);
	//}

	////核心代码：Dom方式写xml文件  
	//QFile fileW(xmlPath);  
	//if (!fileW.open(QFile::WriteOnly | QFile::Text)){  
	//	return;  
	//}  
	//QTextStream out(&fileW);  
	//doc.save(out,4);  
	//fileW.close(); 

}

void FMainWindow::onPathPlanAndExe()
{
	if (!m_pPathAndExe)
	{
		m_pPathAndExe = new FPathPlanAndExeDlg();
	}

	m_pPathAndExe->show();
}

void FMainWindow::onCoordAlt()
{
	if (!m_pCoorAltDlg)
	{
		m_pCoorAltDlg = new CoorTrans;
	}
	m_pCoorAltDlg->show();

}

