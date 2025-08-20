/*	
\brief	A setting dialog for calibrate upper sphere.
\author: Xuhe
\date    2017-12
*/
#ifndef F_AdjustUpSphere_H
#define F_AdjustUpSphere_H

#include "../External/fkernel/Include/FKernelDefines.h"
#include "qwidget.h"

#include "qdir.h"
#include <String>
#include "FUtilities.h"
#include "GLU.h"
#include "MainDoc.h"
#include "FTHP.h"
#include "FMainWindow.h"
#include "Tools\DataTransferTool.h"

#include <vector>
using namespace std;

class FAdjustUpSphere :	public QWidget/*,public FFeatureTool*//*QMdiSubWindow*/
{
	Q_OBJECT
public:
	FAdjustUpSphere(QWidget* parent=NULL);
	~FAdjustUpSphere(void);
	
protected:
	void ComputeInspectPnts(double D,int latNum, int longNum);	//compute inspection points w.r.t.load m_D, m_latNum, and m_logNum
	void GenerateNCFile2D(double D, int latNum, int longNum);	//generate NC code run on NC machine
	vector<double> WeightLSM(FIpPntList FP,vector<double> S);         //Calculation of spherical center by weighted least square method

private slots:

	//生成NC代码（OPCUA）
	void OnGenPointsAndNC() ;	//start calibration

	void OnAdjustProbeCreate();	//generate the error compensation file
	void OnAdjustProbeQuit() ;
	void OnImportData();
	void OnDataTransfer();

protected:
	double		m_D;				///diameter of the calibration sphere
	int			m_latNum;			///number of sample strips in the latitude direction  每圈的点数
	int			m_logNum;			///number of sample strips in the longitude direction  圈数减一
	double      m_d;                ///diameter of ruby

	QLineEdit*	m_diameterLineEdit;	///assign radius value to the calibration sphere
	QLineEdit*	m_latNumLineEdit;	///latitude number 
	QLineEdit*	m_logNumLineEdit;	///longitude number

	QLineEdit*	m_dLineEdit;	    ///assign radius value to the ruby
	QPushButton* m_pCreateBtn;      ///generate the error compensation file
	QPushButton* m_pDataBtn;        ///import measuring data form txt
	QPushButton* m_pDataTransfer; // 数据传输，在昌飞使用数据传输按钮代替机床连接

	QLabel* m_pProbeOnComdLabel;	//输入测头开启指令
	QLineEdit* m_pProbeOnComdLine;
	QString m_ProbeOnComdStr;

	QLabel* m_pProbeOffComdLabel;	//输入测头关闭指令
	QLineEdit* m_pProbeOffComdLine;
	QString m_ProbeOffComdStr;

	vector <double> W;          //存放权重
	
};

#endif F_AdjustUpSphere_H