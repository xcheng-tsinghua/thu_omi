#pragma once
/*
\brief	Default tool
\author: cheng xi
\date    2021-07-24
*/

#include "../fkernel/include/FKernelDefines.h"
#include "../fkernel/include/FFeatureTool.h"

class EntityHighlight :public FFeatureTool
{
	Q_OBJECT
public:
	EntityHighlight();
	~EntityHighlight();

	FEntity* GetSelEnt() { return LastSelectedEntity; }
	void			SetSelEntType(FEntType iType) { m_selType = iType; }
	virtual int		ProcessEvent(FEventType eventType, const FPickEvent& pe);
	void			ResetToDefaultItemCheckedStatus();

private slots:
	void onShowNearSurf();


protected:
	FEntity* LastSelectedEntity;			///last select entity
	FEntity* m_pEnt;			///lost select entity
	F3dPoint		m_prevClick;	///remember old mouse click position in space coordinate	
	FEntType		m_selType;	///entities of this type can be selected
	QTreeWidgetItem* LastSelItem;
	int m_iControlKeyState;

	QSharedPointer<QMenu>	m_pPopMenu;
	QAction* m_pShowNearbySurf;

	bool IsMouseMove;

	//QAction* m_pActionRename;
	//QAction* m_pActionConfig;
	//QAction* m_pActionTest;
	//QAction* m_pActionNewFeat;
	//QAction* m_pActionAddEle;

	//int IsMouseMove;
};