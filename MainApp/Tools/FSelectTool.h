/*	
\brief	Select one or multiple entities, which are added to FDoc::m_selectedEnts
\author: Jacky
\date    2018-05-10
*/
#pragma once
#ifndef F_SELECT_TOOL_H
#define F_SELECT_TOOL_H

#include "../fkernel/include/FKernelDefines.h"
#include "../fkernel/include/FFeatureTool.h"


class FMainDoc;
class PointPlanDlg;

class FSelectTool:public FFeatureTool
{
//	Q_OBJECT
public:
	FSelectTool(PointPlanDlg* _plan);
	FSelectTool(PointPlanDlg* _plan, FMainDoc* pDoc);
	~FSelectTool();

	FEntity*		GetSelEnt(){return m_pEnt;}
	void			SetSelEntType(FEntType iType){m_selType = iType;}

	void			Init();
	virtual int		ProcessEvent(FEventType eventType, const FPickEvent& pe);

protected:
	FEntity*		m_pEnt;			///lost select entity
	F3dPoint		m_prevClick;	///remember old mouse click position in space coordinate	
	FEntType		m_selType;	///entities of this type can be selected
	uint			m_iKeyState;

	PointPlanDlg* pPointPlan;

};
#endif