/*
\brief	坐标系转换工具
\author: 程曦
\date    2021-07-24
*/
#pragma once

#include "../fkernel/include/FKernelDefines.h"
#include "../fkernel/include/FFeatureTool.h"
#include "CoorTrans.h"

class OrignManualTool :public FFeatureTool
{
	//	Q_OBJECT
public:
	OrignManualTool(CoorTrans* aCoorAlt);
	~OrignManualTool();

	void			SetSelEntType(FEntType iType) { m_selType = iType; }
	virtual int		ProcessEvent(FEventType eventType, const FPickEvent& pe);
	void RemoveOrign();

	vector<FSurface*> theSelSurfaceList;

	MeasPt* theOrignX;
	MeasPt* theOrignY;
	MeasPt* theOrignZ;

protected:
	FEntity* m_pEnt;			///lost select entity
	F3dPoint		m_prevClick;	///remember old mouse click position in space coordinate	
	FEntType		m_selType;	///entities of this type can be selected
	int m_iControlKeyState;

	CoorTrans* m_pCoorAlt;

	bool IsCylinder(FSurface* testSurface);
	bool IsCylinderOrPlane(FSurface* testSurface);
	ON_Plane CreatPlane(FSurface* pSurf);

};