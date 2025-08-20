/*	
\brief	A setting dialog for calibrate upper sphere.
\author: Xuhe
\date    2017-12
*/
#ifndef F_AccuracyEvaluation_H
#define F_ccuracyEvaluation_H
//#pragma once
#include "../External/fkernel/Include/FKernelDefines.h"
#include "qwidget.h"

///ƽ��Ⱦ���������
class FAccuracyEvaluation :	public QWidget/*,public FFeatureTool*//*QMdiSubWindow*/
{
	Q_OBJECT
public:
	FAccuracyEvaluation(QWidget* parent=NULL);//���캯��
	~FAccuracyEvaluation(void);//��������
 std::vector<double>   ComputeCoef(double X[12],double Y[12],double Z[12],int n);//�����������������ϵ�������xi*yi��
 double           ComputeHanglieshi(double A[3][3]);//���������3x3����ʽ������ֵ
 std::vector<double>   Cramer(double C[3][3], double B[3]);//����ϵ�������B���ÿ���Ĭ����������Է�����õ�ƽ�淽��ϵ��

 //�źŲ�
private slots:
//	void  OnAdjustProbeStart() ;	
	double  OnComputeS();	//���㰴ť��
	void  OnFlatQuit() ;    //�˳���ť��

protected:
	double		S;				
	int			n;			
	
	QVBoxLayout* m_pMainVLayout;
	QLineEdit*	s_flatnessLineEdit;	
	QLineEdit*	n_pointLineEdit;
	QPushButton* m_pCreateBtn;	
};

#endif F_AccuracyEvaluation_H