/*	
\brief	A window class to define the ajustprobe1 window.
\author: Xuhe
\date    2017-01
*/
#ifndef F_AdjustAngleProbe_H
#define F_AdjustAngleProbe_H
//#pragma once
#include "../External/fkernel/Include/FKernelDefines.h"
#include "qwidget.h"
#include "qdir.h"
#include <String>
#include "FUtilities.h"
#include "GLU.h"
#include "MainDoc.h"
#include "FTHP.h"
#include "Tools\DataTransferTool.h"
#include <vector>
using namespace std;

class FAdjustAngleProbe :	public QWidget/*QMdiSubWindow*/
{
	Q_OBJECT
public:
	FAdjustAngleProbe(QWidget* parent=NULL);
	~FAdjustAngleProbe(void);


protected:
	//FIpPntList ACrotation(FIpPntList FP,float probeAngle,float startAngle);	
	vector<double> SphereFit(FIpPntList FP);   //最小二乘法计算球心
	void ComputeInspectPnts(double D,int logNum, int latNum,float probeAngle,float startAngle);  // compute inspection points
	void GenerateNCFile2D(double D,int logNum, int latNum,float probeAngle,float startAngle);    //gnerate NC code run on NC machine

private slots:
	
	void OnAdjustProbeStart() ;         //start calibration
	void OnAdjustProbeCreate();         //generate the error compensation file
	void OnAdjustProbeQuit() ;
	void OnImportData();
	
	void OnTCPConfig();
	void OnOPCUA();
	
protected:
	float	m_D;                   //diameter of the calibration sphere
	int		m_logNum;              //纬向圈数，径向点数-1
	int		m_latNum;               //径向条数，纬向点数
	float	m_probeAngle;                //A轴转角
	float	m_startAngle;              //C轴转角
	float   m_d;                ///diameter of ruby

	QLineEdit *m_diameterLineEdit;   //assign diameter value to the calibration sphere
	QLineEdit *m_logNumLineEdit;      //latitude number
	QLineEdit *m_latNumLineEdit;     //longitude number
	QLineEdit *m_probeAngleLineEdit;    
	QLineEdit *m_startAngleLineEdit;

	QLineEdit *m_dLineEdit;
	QPushButton* m_pCreateBtn;

	QPushButton* m_pDataBtn;        ///import measuring data form txt
	QPushButton* m_pTCPConnect;     ///configuration for TCP/IP mode
	QPushButton* m_pOPCConnect;     ///configuration for OPC UA mode


};

#endif F_AdjustAngleProbe_H