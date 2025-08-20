/*
����ΪFInsFeatureDlg���FToolPathDlg���ֶ�ѡ������ʹ��
����-2021/6/5
*/

#pragma once
#include "../fkernel/include/FKernelDefines.h"
#include "../fkernel/include/FFeatureTool.h"

class FMainDoc;
class CompCylinTab;
class CompSurfTab;
class CompWThickTab;

class ErrCompsteTool :public FFeatureTool
{
public:

	// ����ģʽ
	// = 1: װ�в���
	// = 2: ��λ����
	// = 3: �ں񲹳�
	ErrCompsteTool(QWidget* fpTab, int fpCmdMode);

	~ErrCompsteTool();

	void			SetSelEntType(FEntType iType) { m_selType = iType; }
	virtual int		ProcessEvent(FEventType eventType, const FPickEvent& pe);

	// ����ģʽ
	// = 0: δѡ��
	// = 1: װ�в���
	// = 2: ��λ����
	// = 3: �ں񲹳�
	void setCmdMode(const int& _mode);

	// = 0: δѡ��
	// = 1: ѡ���һ��
	// = 2: ѡ��ڶ���
	void setPlaneSelMode(const int& _mode);

protected:
	void init();

	QWidget* m_pTab;

	FEntity* m_pEnt;			///lost select entity
	FEntType m_selType;	///entities of this type can be selected

	// ����ģʽ
	// = 0: δѡ��
	// = 1: װ�в���
	// = 2: ��λ����
	// = 3: �ں񲹳�
	int m_cmdMode;

	// = 0: δѡ��
	// = 1: ѡ���һ��
	// = 2: ѡ��ڶ���
	int m_planeSelMode;

};