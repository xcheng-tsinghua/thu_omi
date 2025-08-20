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

///平面度精度评定类
class FAccuracyEvaluation :	public QWidget/*,public FFeatureTool*//*QMdiSubWindow*/
{
	Q_OBJECT
public:
	FAccuracyEvaluation(QWidget* parent=NULL);//构造函数
	~FAccuracyEvaluation(void);//析构函数
 std::vector<double>   ComputeCoef(double X[12],double Y[12],double Z[12],int n);//根据输入点的坐标计算系数矩阵∑xi*yi等
 double           ComputeHanglieshi(double A[3][3]);//根据输入的3x3行列式计算其值
 std::vector<double>   Cramer(double C[3][3], double B[3]);//根据系数矩阵和B利用克拉默法则求解线性方程组得到平面方程系数

 //信号槽
private slots:
//	void  OnAdjustProbeStart() ;	
	double  OnComputeS();	//计算按钮槽
	void  OnFlatQuit() ;    //退出按钮槽

protected:
	double		S;				
	int			n;			
	
	QVBoxLayout* m_pMainVLayout;
	QLineEdit*	s_flatnessLineEdit;	
	QLineEdit*	n_pointLineEdit;
	QPushButton* m_pCreateBtn;	
};

#endif F_AccuracyEvaluation_H