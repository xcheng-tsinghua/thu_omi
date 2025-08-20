/*
该类为FInsFeatureDlg类和FToolPathDlg类手动选择曲面使用
程曦-2021/6/5
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

	// 命令模式
	// = 1: 装夹补偿
	// = 2: 孔位补偿
	// = 3: 壁厚补偿
	ErrCompsteTool(QWidget* fpTab, int fpCmdMode);

	~ErrCompsteTool();

	void			SetSelEntType(FEntType iType) { m_selType = iType; }
	virtual int		ProcessEvent(FEventType eventType, const FPickEvent& pe);

	// 命令模式
	// = 0: 未选择
	// = 1: 装夹补偿
	// = 2: 孔位补偿
	// = 3: 壁厚补偿
	void setCmdMode(const int& _mode);

	// = 0: 未选择
	// = 1: 选择第一组
	// = 2: 选择第二组
	void setPlaneSelMode(const int& _mode);

protected:
	void init();

	QWidget* m_pTab;

	FEntity* m_pEnt;			///lost select entity
	FEntType m_selType;	///entities of this type can be selected

	// 命令模式
	// = 0: 未选择
	// = 1: 装夹补偿
	// = 2: 孔位补偿
	// = 3: 壁厚补偿
	int m_cmdMode;

	// = 0: 未选择
	// = 1: 选择第一组
	// = 2: 选择第二组
	int m_planeSelMode;

};