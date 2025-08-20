/*	
\brief	 Tool class to define the mouse operation in manually point arrangement, connected with class FManualPoint.
\author: Haoyu Jiang
\date    2020-5
*/

 
#ifndef MANUAL_POINT_TOOL_H
#define MANUAL_POINT_TOOL_H

//#include "FManulPoint.h"
#include "../fkernel/include/FKernelDefines.h"
#include "../fkernel/include/FFeatureTool.h"
#include "qwidget.h"

class FMainDoc;

class ManualPointTool:public FFeatureTool
{
	Q_OBJECT
public:
	ManualPointTool();
	ManualPointTool(FMainDoc* pDoc);
	~ManualPointTool();

	void			Init();
	virtual int		ProcessEvent(FEventType eventType, const FPickEvent& pe);

protected:
	FMainDoc*		m_pDoc;
	FEntity* m_pSelEnt;			///lost select entity

public:
	//FManualPoint*   m_manualPoint;
};
#endif