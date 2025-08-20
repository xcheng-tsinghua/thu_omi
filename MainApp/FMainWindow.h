/*!	
\brief	Create the main window UI, i.e. menus, toolbar, status bar, etc.
		Handle events from these UIs.
\author: jacky
\date    2014-03
*/
//////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef F_MAINWINDOW_H
#define F_MAINWINDOW_H

#include <QtGui>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include "ProbeVis.h"
#include "InspVis_thread.h"
#include "WindowTitle.h"

namespace Ui {
	class UiMainWindow;
}

namespace FT{
	class FBody;
	class FEntity;
	class FCurve;
	class FFreeCurve;
}

class FMainDoc;
class FSubWindow;
class FThread;
class FSelectTool;
class PointPlanDlg;
class FInsFeatureDlg;
class FInsReportDlg;
class FPathPlanAndExeDlg;
class CoorTrans;
class FAdjustUpSphere;
class occConverter;
class FToolPathDlg;
class FGroupPathPlan;
class FAdjustMachine;
class AutoInsFea;
class SetCoorPrecisely;
class ErrCompsteDlg;
class MeasPt;

using namespace FT;
typedef QVector<FSubWindow*> PSubWndList;

class FMainWindow :public QMainWindow
{
	Q_OBJECT
public:
	FMainWindow(QWidget* parent=NULL);
	~FMainWindow();

	/////////////////////////////// Accessing methods ///////////////////////////////////////////
	FMainDoc*	GetMainDoc(){return m_pDoc;}
	QMdiArea*	GetMdiArea(){return m_pMdiArea;}
	QWidget*	GetCurrentWnd();
	QLabel*		GetHintLabel(){return m_hintLabel;}
	FFeatureTool* GetCurTool() { return m_pCurTool; };

	// [0]: 模型结构树窗口
	// [1]: 数据窗口
	// [2]: 代码窗口
	// [3]: 3D模型显示窗口
	// [4]: 数据可视化窗口
	// [5]: 数据配置窗口 - NULL
	// [6]: 检测特征窗口
	// [7]: 精度评定窗口
	// [8]: MySQL数据读取窗口 - NULL
	QVector<QWidget*>	GetSubWnd() {return m_wndlist;}

	occConverter* GetSTEPModelConverter() { return m_pConverter; }
	occConverter* GetOccConverter() { return m_pConverter; }

	PointPlanDlg* GetPointPlanDlg() { return m_pPointPlan; }
	FPathPlanAndExeDlg* GetPathPlanAndExeDlg() { return m_pPathAndExe; }
	FInsFeatureDlg* GetCreateInsFeatureDlg() { return m_pInsFeaDlg; }
	FInsReportDlg* GetInsReportDlg() { return m_pInsReport; }
	CoorTrans* GetCoorTransDlg() { return m_pCoorAltDlg; }
	FAdjustUpSphere* GetAdjustUpSphereDlg() { return m_pUpSphereDlg; }
	FGroupPathPlan* GetGroupPathPlanDlg() { return m_pGroupPathPlanDlg; }
	FAdjustMachine* GetMachineErrCompDlg() { return m_pMachineErrCompDlg; }
	SetCoorPrecisely* GetSetCoorPreciselyDlg() { return m_pSetCoorPrecisely; }
	ErrCompsteDlg* GetErrorMapDlg() { return m_pErrorMapDlg; }

	void SetInsReportDlg(FInsReportDlg* NewInsReport) { m_pInsReport = NewInsReport; }
	void SetPointPlanDlg(PointPlanDlg* NewPointPlan) { m_pPointPlan = NewPointPlan; }
	void SetCreateInsFeatureDlg(FInsFeatureDlg* NewInsFeature) { m_pInsFeaDlg = NewInsFeature; }
	void SetPathPlanAndExeDlg(FPathPlanAndExeDlg* NewPathPlanAndExeDlg) { m_pPathAndExe = NewPathPlanAndExeDlg; }

	// 为读取到的节点分配邻接点
	void AllocateSurfNode();

	FramelessWidget* GetFrameLessWidget() { return selMainWidget; }
	WindowTitle* GetWindowTitle() { return pTitleBar; }

	//vector<MeasPt*>& GetArrPntList() { return m_pArrPntList; }

	ProbeVis* GetVisProbe() { return m_pProbeVis; }
	InspVis_thread* GetVisThread() { return m_pVisThread; }

	/////////////////////////////// Public methods ///////////////////////////////////////////
	void		CreateStatusBar();
	void		CreateUI();	
	void		CreateSignalSlots();
	void		UpdateAllViews();
	void		UpdateModelView();
	bool		OpenFile(QString fname);

	virtual void	resizeEvent(QResizeEvent* e);
	virtual void	dragEnterEvent(QDragEnterEvent * e);
	virtual void	dropEvent(QDropEvent* e);

	//为检测特征匹配测点
	void AssignTheoPnts(FInsFeature* pInsFea, vector<int>& FacePntIndexList, const FIpPntList& pointsAll);//为检测特征匹配测点

	//将所有面恢复至默认颜色
	void ResetToDefaultCountenance();

	//子窗口统一处理函数
	void PointerInit();
	void MinimizeAll();
	void CloseAll();

	Ui::UiMainWindow* getUI() { return m_pUI; }			///UI pointer

public slots:
	//File
	bool		onFileNew();
	bool		onFileOpen();
	bool		onFileSave();
	bool		onFileClose();
	void        onImportMeaData();
	void        onImportTheoData();
	void        onExportTheoData();
	void        onExportResult();
	void        onExportReport();
	void		onImportInspConfig();

	//View
	void		onViewTop();
	void		onViewBottom();
	void		onViewFront();
	void		onViewBack();
	void		onViewLeft();
	void		onViewRight();
	void		onViewZoomIn();
	void		onViewZoomOut();
	void		onViewZoomAll();
	void		onViewPan();
	void		onViewOrbit();
	void		onViewSmooth();
	void		onViewMesh();
	void		onViewWire();

	//Configuration menu
	void		onInspectConfig();
	void		onInspectMode();
	void		onInspectAnalysis();

	//Processing menu
	void		onToolPathDlg();
	void		onErrorComp();

	//Measure menu
	void		onMeasureStart();
	void		onMeasurePause();
	void		onMeasureReset();
	void		onSetCoorPrecisely();

	void		onMeasureOutNCFile();
	void		onMeasureOutDimFile();
	void		onMeasurePathPlanning();
	void		onGroupPathPlan();
	void		onMeasureRePath();
	void        onFiveAxisPathPlan();
	void		onPointPlan();
	void        onOpc(); //open the out source c# app OPC UA THU
	void        onPathPlanAndExe();
	void        onCoordAlt();

	//Tool menu
	void onToolResetWindowLayout();
	void onResetToDefaultSetting();
	void onToolFAdjustMachine();
	void onToolAdjustRingProbe();
	void onToolAdjustAngleProbe();
	void onToolAdjustUpSphere();
	void onToolAdjustLowerSphere();
	void onToolSelectIpPoint();
	void onToolSelectSurface();		//select FSurface
	void onToolSetLocalFrame();		//set part's local frame, transforming parts correspondingly
	void onSurfEval();              //Evaluation of the surface plain

	//Test menu
	void		onTestDialog();
	void		onTestOthers();

	//原位检测函数
	void		onImportGuideFile();
	void		onExportInspectDataToXML();
	void        onRecogRib();
	void        onRecogSlot();
	void        onRecogProfile();
	void        onRecogHole();
	void        onTCPConfig();

private:
	Ui::UiMainWindow*	m_pUI;			///UI pointer（此处已转移至公有成员）
	QMdiArea*			m_pMdiArea;		///multiple documents area handle
	QVector<QWidget*>	m_wndlist;		///list of sub-windows 
	FSubWindow*			m_pModelWnd;	///used to display 3D model
	FMainDoc*			m_pDoc;			///Doc pointer
	FFeatureTool*		m_pCurTool;		///select tool
	QLabel*				m_hintLabel;	///label display hint message in status bar
	FT::FEntity*		m_pClass;		///used to test class
	QThread				m_qthread;

	occConverter* m_pConverter;

	// 子窗口
	// 每新增一个子窗口，有如下部分需要修改：
	// ① 此处的声明
	// ② MainWindow 的构造函数里将其初始化为NULL
	// ③ MainWindow::PointerInit()函数里添加对应语句
	// ④ MainWindow::MinimizeAll()函数里添加对应语句
	// ⑤ MainWindow::CloseAll()函数里添加对应语句
	// ⑥ 本文件中加入Get()函数
	PointPlanDlg* m_pPointPlan;
	FPathPlanAndExeDlg* m_pPathAndExe;
	FInsFeatureDlg* m_pInsFeaDlg;
	FInsReportDlg* m_pInsReport;
	CoorTrans* m_pCoorAltDlg;
	FAdjustUpSphere* m_pUpSphereDlg;
	FGroupPathPlan* m_pGroupPathPlanDlg;
	FAdjustMachine* m_pMachineErrCompDlg;
	AutoInsFea* m_pAutoInsFeaDlg;
	SetCoorPrecisely* m_pSetCoorPrecisely;
	ErrCompsteDlg* m_pErrorMapDlg;
	FToolPathDlg* m_pToolPathDlg;//刀具路径规划

	/////////////////////////////////////////////无边框
	FramelessWidget* selMainWidget;
	WindowTitle* pTitleBar;

	//用于删除测点功能
	//vector<MeasPt*> m_pArrPntList;

	//仿真
	ProbeVis* m_pProbeVis;
	InspVis_thread* m_pVisThread;

	// 测试按键转换模型
	FT::FFreeBody* m_pFreeBody;

//protected:
//	bool winEvent(MSG* message, long* result)override;

};

#endif

