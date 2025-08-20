/*
该类为FToolPathDlg类手动选择曲面使用
*/

#pragma once
#include "../fkernel/include/FKernelDefines.h"
#include "../fkernel/include/FFeatureTool.h"
#include "../FToolPathDlg.h"

class FMainDoc;
class FInsFeatureDlg;

class ToolPathTool :public FFeatureTool
{
public:
	ToolPathTool(FToolPathDlg* _toolPathDlg);
	ToolPathTool(FToolPathDlg* _toolPathDlg, FMainDoc* pDoc);
	~ToolPathTool();

	FEntity* GetSelEnt() { return m_pEnt; }

	void			SetSelEntType(FEntType iType) { m_selType = iType; }
	void			Init();
	virtual int		ProcessEvent(FEventType eventType, const FPickEvent& pe);

protected:
	FEntity* m_pEnt;			///lost select entity
	FEntType m_selType;	///entities of this type can be selected

	FToolPathDlg* m_pToolPathDlg;

};