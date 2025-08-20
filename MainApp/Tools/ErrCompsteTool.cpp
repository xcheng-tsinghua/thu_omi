#include "ErrCompsteTool.h"
#include "../MainDoc.h"
#include "../occConverter.h"
#include "../np.h"
#include "../FMainWindow.h"
#include "../ErrCompsteDlg.h"
#include "../CompCylinTab.h"
#include "../CompSurfTab.h"
#include "../CompWThickTab.h"

extern FMainWindow* gWindow;

ErrCompsteTool::ErrCompsteTool(QWidget* fpTab, int fpCmdMode)
{
	init();
	m_pTab = fpTab;
	m_cmdMode = fpCmdMode;
}

ErrCompsteTool::~ErrCompsteTool()
{

}

int ErrCompsteTool::ProcessEvent(FEventType eventType, const FPickEvent& pe)
{
	if (eventType == MOUSE_BUTTON_DOWN && pe.nFlags == MK_LBUTTON && m_iStep == 0)	//left button down, start of mouse drag
	{
		pe.view->PickEntity(pe.x, pe.y, m_pEnt, m_selType);

		//»ÙŒ¥—°÷–
		if (!m_pEnt)
		{
			return 0;
		}

		FSurface* SelFSurf = static_cast<FFace*>(m_pEnt)->GetSurface();

		switch (m_cmdMode)
		{
		case 0:
		{
			break;
		}
		case 1:
		{
			reinterpret_cast<CompSurfTab*>(m_pTab)->setSurface(SelFSurf);

			break;
		}
		case 2:
		{
			reinterpret_cast<CompCylinTab*>(m_pTab)->addSurf(SelFSurf);

			break;
		}
		case 3:
		{
			reinterpret_cast<CompWThickTab*>(m_pTab)->addPlane(SelFSurf, m_planeSelMode);

			break;
		}
		default:
			break;
		}

		m_pEnt = NULL;

	}
	else if (eventType == MOUSE_MOVE && pe.nFlags == MK_LBUTTON && m_iStep == 1)	//left button drag, mouse dragging
	{

	}
	else if (eventType == MOUSE_BUTTON_UP && pe.nFlags == MK_LBUTTON && m_iStep == 1)	//left button up, end of mouse drag
	{
		m_iStep = 0;
	}
	else if (eventType == MOUSE_BUTTON_UP && pe.nFlags == MK_RBUTTON)
	{
		m_iStep = 0;
	}
	return 1;
}

void ErrCompsteTool::setCmdMode(const int& _mode)
{
	m_cmdMode = _mode;
}

void ErrCompsteTool::setPlaneSelMode(const int& _mode)
{
	m_planeSelMode = _mode;
}

void ErrCompsteTool::init()
{
	m_pEnt = NULL;
	m_selType = FT_FACE;
	m_cmdMode = 0;
	m_planeSelMode = 0;

}
