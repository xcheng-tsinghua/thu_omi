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

	// [0]: ģ�ͽṹ������
	// [1]: ���ݴ���
	// [2]: ���봰��
	// [3]: 3Dģ����ʾ����
	// [4]: ���ݿ��ӻ�����
	// [5]: �������ô��� - NULL
	// [6]: �����������
	// [7]: ������������
	// [8]: MySQL���ݶ�ȡ���� - NULL
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

	// Ϊ��ȡ���Ľڵ�����ڽӵ�
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

	//Ϊ�������ƥ����
	void AssignTheoPnts(FInsFeature* pInsFea, vector<int>& FacePntIndexList, const FIpPntList& pointsAll);//Ϊ�������ƥ����

	//��������ָ���Ĭ����ɫ
	void ResetToDefaultCountenance();

	//�Ӵ���ͳһ������
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

	//ԭλ��⺯��
	void		onImportGuideFile();
	void		onExportInspectDataToXML();
	void        onRecogRib();
	void        onRecogSlot();
	void        onRecogProfile();
	void        onRecogHole();
	void        onTCPConfig();

private:
	Ui::UiMainWindow*	m_pUI;			///UI pointer���˴���ת�������г�Ա��
	QMdiArea*			m_pMdiArea;		///multiple documents area handle
	QVector<QWidget*>	m_wndlist;		///list of sub-windows 
	FSubWindow*			m_pModelWnd;	///used to display 3D model
	FMainDoc*			m_pDoc;			///Doc pointer
	FFeatureTool*		m_pCurTool;		///select tool
	QLabel*				m_hintLabel;	///label display hint message in status bar
	FT::FEntity*		m_pClass;		///used to test class
	QThread				m_qthread;

	occConverter* m_pConverter;

	// �Ӵ���
	// ÿ����һ���Ӵ��ڣ������²�����Ҫ�޸ģ�
	// �� �˴�������
	// �� MainWindow �Ĺ��캯���ｫ���ʼ��ΪNULL
	// �� MainWindow::PointerInit()��������Ӷ�Ӧ���
	// �� MainWindow::MinimizeAll()��������Ӷ�Ӧ���
	// �� MainWindow::CloseAll()��������Ӷ�Ӧ���
	// �� ���ļ��м���Get()����
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
	FToolPathDlg* m_pToolPathDlg;//����·���滮

	/////////////////////////////////////////////�ޱ߿�
	FramelessWidget* selMainWidget;
	WindowTitle* pTitleBar;

	//����ɾ����㹦��
	//vector<MeasPt*> m_pArrPntList;

	//����
	ProbeVis* m_pProbeVis;
	InspVis_thread* m_pVisThread;

	// ���԰���ת��ģ��
	FT::FFreeBody* m_pFreeBody;

//protected:
//	bool winEvent(MSG* message, long* result)override;

};

#endif

