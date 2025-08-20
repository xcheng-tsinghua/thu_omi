/*	
\brief	A window class to define the FAdjustMachine window.
\author: Xuhe
\date    2017-01
*/

//ת������Thu-OMV�Ľ��л������궨���㷨
//����-2022-8-11

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

	//����ĳ��Ļ�����ʵ���ΪinspPnt����Ӧ���۵�ΪTheoPnt��ʵ����޷�ʧ�����������ȡ���۵㷨ʧΪʵ��㷨ʧ
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
	//��������ϵԭ���ڱ궨����ϵ�µ�����
	F3dPoint D_RealInCali;

	//�����׼
	void GlobalRegistration();

	QLineEdit* m_pRealInCali_X;
	QLineEdit* m_pRealInCali_Y;
	QLineEdit* m_pRealInCali_Z;

	QPushButton* m_pUseOfMachineCompBnt;

};
#endif F_AjustMachine_H