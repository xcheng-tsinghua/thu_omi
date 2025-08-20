/*	
\brief	A setting dialog for point planning.
\author: xiechun 
\date    2018-11
\modidfied by: Haoyu Jiang
\date    2020-10
*/

#pragma once
#include <qwidget.h>
#include "../External/fkernel/Include/FKernelDefines.h"
#include <vector>
#include <TopoDS_Face.hxx>
#include "FGuideDataType.h"
#include <gp_Pnt.hxx>

class MeasPt;

//PointPlanDlg子窗口类
class PointPlanDlg : public QWidget
{
	Q_OBJECT
public:
	PointPlanDlg(QWidget* parent=NULL);//构造函数
	~PointPlanDlg(void);//析构函数

	//当重复使用自适应布点法在同一曲面布点时，删除之前使用自适应布点法布置的测点
	void RemoveSampledSurfPnts();

	bool GetIsPlanned() { return m_IsPlaned; };

	QComboBox* GetComboBox() { return m_pComboBox; };

	QLabel* GetSelPlaneLabel();

	void setSelSurf(FSurface* _surf);

	void SetNotReverse();

	void setCBoxStatus(bool _isBspline);

protected:
	void closeEvent(QCloseEvent* event);

	//信号槽
private slots:
	void OnPointPlanQuit();//退出按钮槽
	void OnSelectSurface();//选择型面槽
	void onAutoPointPlan();//布点按钮槽
	void OnAutoSampling();//开始布点槽
	void getMethod(int);//获得布点方法
	void OnManualPointPlan();//手动布点槽
	void OnPathPlanning();//链接到路径生成工具
	void OnArrReverse();
	void onClearPnts();

private:

	struct SurfSA
	{
		FSurface* TheSurf;
		FIpPntList IpPntLst;
		std::vector<MeasPt*> MeasPtLst;
	};


	void initDlg();
	void setLinesDisabled();

	QPushButton* m_pSelPlaneBtn;
	QPushButton* m_pAutoSampBtn;
	QPushButton* m_pArrReverse;

	QLabel* m_pSelPlane;

	QLineEdit* m_pSection_U;
	QLineEdit* m_pSection_V;
	QLineEdit* m_pWeightOfCurvature;
	QLineEdit* m_pNumOfPoints;
	QLineEdit* m_pDeviaU;
	QLineEdit* m_pDeviaV;
	QLineEdit* m_pMarginalRetreatLineEdit;
	QLineEdit* m_pMarginalRetreat_U;
	QLineEdit* m_pMarginalRetreat_V;
	QLineEdit* m_pOffsetVal;
	QLineEdit* m_pNeiTolVal;

	QComboBox* m_pComboBox;

	FFeatureTool* m_pCurTool;		///select tool

	// = 0 未选择
	// = 1 参数空间均匀布点
	// = 2 欧式空间均匀布点
	// = 3 混合布点
	// = 4 哈默斯利
	// = 5 均匀布点
	// = 6 边缘布点
	int m_SamplingMode;

	void addMesPnts(const std::vector<gp_Pnt>& _samples, const TopoDS_Face& _cFace);

	std::vector<SurfSA> SampedSurf;
	SurfSA CuurSA;

	double m_ArrReverse;//1或-1

	// 是否进行了测点规划
	bool m_IsPlaned;

	// 选中的曲面
	FSurface* SelSurface;

};

