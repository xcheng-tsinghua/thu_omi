/*
该类为FInsFeatureDlg类和FToolPathDlg类手动选择曲面使用
程曦-2021/6/5
*/

#pragma once
#include "../fkernel/include/FKernelDefines.h"
#include "../fkernel/include/FFeatureTool.h"

class FMainDoc;
class FInsFeatureDlg;

class InsFeaTool :public FFeatureTool
{
public:
	InsFeaTool(FInsFeatureDlg* _feaDlg);
	InsFeaTool(FInsFeatureDlg* _feaDlg, FMainDoc* pDoc);
	~InsFeaTool();

	FEntity* GetSelEnt() { return m_pEnt; }

	void			SetSelEntType(FEntType iType) { m_selType = iType; }
	void			Init();
	virtual int		ProcessEvent(FEventType eventType, const FPickEvent& pe);



protected:
	FEntity* m_pEnt;			///lost select entity
	FEntType		m_selType;	///entities of this type can be selected

	FInsFeatureDlg* m_pInsFeaDlg;

};