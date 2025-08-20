/*	
\brief	A window class to define the FAdjustMachine window.
\author: Xuhe
\date    2017-01
*/

//转移来自Thu-OMV的进行机床误差标定的算法
//程曦-2022-8-11

#ifndef F_AjustMachine_H
#define F_AjustMachine_H

#include "../External/fkernel/Include/FKernelDefines.h"
#include <Qtgui>
#include <QMdiSubWindow>

class FIpPoint;

class FAdjustMachine :	public QWidget/*QMdiSubWindow*/
{
	Q_OBJECT
public:
	FAdjustMachine(QWidget* parent=NULL);
	~FAdjustMachine(void);

	//补偿某点的机床误差，实测点为inspPnt，对应理论点为TheoPnt，实测点无法失，计算过程中取理论点法失为实测点法失
	void MachineErrComp(FIpPoint& inspPnt, FIpPoint TheoPnt);

private slots:
	//void	onPauseBtn();		///triggered when the Pause button is clicked
	void OnMachinefilex();
	void OnMachinefiley();
	void OnMachinefilez();
	void OnConfirmCoor();
	void OnOpenCloseMachineComp();
	void OnAdjustProbeQuit() { close(); };

private:
	//测量坐标系原点在标定坐标系下的坐标
	F3dPoint D_RealInCali;

	//测点配准
	void GlobalRegistration();

	QLineEdit* m_pRealInCali_X;
	QLineEdit* m_pRealInCali_Y;
	QLineEdit* m_pRealInCali_Z;

	QPushButton* m_pUseOfMachineCompBnt;

};
#endif F_AjustMachine_H