#include "InsFeaTool.h"
#include "../MainDoc.h"
#include "../FMainWindow.h"
#include "../FSubwindow.h"
#include "../MainView.h"
#include "../FUtilities.h"
#include "qwidget.h"
#include "../occConverter.h"
#include "../FInsFeaTreeWnd.h"
#include "../np.h"
#include <GeomConvert.hxx>
#include <BRep_Tool.hxx>

extern FMainWindow* gWindow;
extern clock_t gTStart, gTEnd;

void InsFeaTool::Init()
{
	m_pEnt = NULL;
	m_selType = FT_FACE;
	m_pInsFeaDlg = NULL;
	m_pDoc = NULL;

}
InsFeaTool::InsFeaTool(FInsFeatureDlg* _feaDlg)
	:FFeatureTool(/*FT_SELECT_TOOL*/)
{
	Init();

	m_pInsFeaDlg = _feaDlg;

}
InsFeaTool::InsFeaTool(FInsFeatureDlg* _feaDlg, FMainDoc* pDoc)
	: FFeatureTool(pDoc/*, FT_SELECT_TOOL*/)
{
	Init();

	m_pInsFeaDlg = _feaDlg;
	m_pDoc = pDoc;

}
InsFeaTool::~InsFeaTool()
{

}

int InsFeaTool::ProcessEvent(FEventType eventType, const FPickEvent& pe)
{
	if (eventType == MOUSE_BUTTON_DOWN && pe.nFlags == MK_LBUTTON && m_iStep == 0)	//left button down, start of mouse drag
	{
		gTStart = clock();

		pe.view->PickEntity(pe.x, pe.y, m_pEnt, m_selType);

		if (!m_pEnt)
		{
			//若未选中，将全部型面置为默认颜色
			//gWindow->ResetToDefaultCountenance();
			//gWindow->UpdateModelView();
			return 0;
		}

		if (!m_pInsFeaDlg) return 0;

		FSurface* selSurf = static_cast<FFace*>(m_pEnt)->GetSurface();
		m_pInsFeaDlg->addSurf(selSurf);

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
