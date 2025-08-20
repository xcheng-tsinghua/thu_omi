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

//PointPlanDlg�Ӵ�����
class PointPlanDlg : public QWidget
{
	Q_OBJECT
public:
	PointPlanDlg(QWidget* parent=NULL);//���캯��
	~PointPlanDlg(void);//��������

	//���ظ�ʹ������Ӧ���㷨��ͬһ���沼��ʱ��ɾ��֮ǰʹ������Ӧ���㷨���õĲ��
	void RemoveSampledSurfPnts();

	bool GetIsPlanned() { return m_IsPlaned; };

	QComboBox* GetComboBox() { return m_pComboBox; };

	QLabel* GetSelPlaneLabel();

	void setSelSurf(FSurface* _surf);

	void SetNotReverse();

	void setCBoxStatus(bool _isBspline);

protected:
	void closeEvent(QCloseEvent* event);

	//�źŲ�
private slots:
	void OnPointPlanQuit();//�˳���ť��
	void OnSelectSurface();//ѡ�������
	void onAutoPointPlan();//���㰴ť��
	void OnAutoSampling();//��ʼ�����
	void getMethod(int);//��ò��㷽��
	void OnManualPointPlan();//�ֶ������
	void OnPathPlanning();//���ӵ�·�����ɹ���
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

	// = 0 δѡ��
	// = 1 �����ռ���Ȳ���
	// = 2 ŷʽ�ռ���Ȳ���
	// = 3 ��ϲ���
	// = 4 ��Ĭ˹��
	// = 5 ���Ȳ���
	// = 6 ��Ե����
	int m_SamplingMode;

	void addMesPnts(const std::vector<gp_Pnt>& _samples, const TopoDS_Face& _cFace);

	std::vector<SurfSA> SampedSurf;
	SurfSA CuurSA;

	double m_ArrReverse;//1��-1

	// �Ƿ�����˲��滮
	bool m_IsPlaned;

	// ѡ�е�����
	FSurface* SelSurface;

};

