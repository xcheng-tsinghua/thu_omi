#include "ToolPathTool.h"
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

void ToolPathTool::Init()
{
	m_pEnt = NULL;
	m_selType = FT_FACE;
	m_pToolPathDlg = NULL;
}

ToolPathTool::ToolPathTool(FToolPathDlg* _toolPathDlg)
	:FFeatureTool(/*FT_SELECT_TOOL*/)
{
	m_pToolPathDlg = _toolPathDlg;
	Init();
}

ToolPathTool::ToolPathTool(FToolPathDlg* _toolPathDlg, FMainDoc* pDoc)
	: FFeatureTool(pDoc/*, FT_SELECT_TOOL*/)
{
	m_pToolPathDlg = _toolPathDlg;
	m_pDoc = pDoc;
	Init();
}

ToolPathTool::~ToolPathTool()
{

}

int ToolPathTool::ProcessEvent(FEventType eventType, const FPickEvent& pe)
{
	if (eventType == MOUSE_BUTTON_DOWN && pe.nFlags == MK_LBUTTON && m_iStep == 0)	//left button down, start of mouse drag
	{
		pe.view->PickEntity(pe.x, pe.y, m_pEnt, m_selType);

		if (!m_pEnt)
		{
			//若未选中，将全部型面置为默认颜色
			gWindow->ResetToDefaultCountenance();
			gWindow->UpdateModelView();
			return 0;
		}

		if (!m_pToolPathDlg) return 1;

		FSurface* TmpSurf = static_cast<FFace*>(m_pEnt)->GetSurface();

		int SelIndex = -1;

		vector<int> BSplineSurfInds = gWindow->GetSTEPModelConverter()->GetBSplineInds();
		int nSize = BSplineSurfInds.size();

		for (int i = 0; i < nSize; i++)
		{
			if (TmpSurf == gWindow->GetSTEPModelConverter()->GetFSurfList()[BSplineSurfInds[i]]);
			{
				SelIndex = i;
				gWindow->ResetToDefaultCountenance();

				float r, g, b;
				TmpSurf->GetColor(r, g, b);
				if (r == 1.0)
				{
					gWindow->GetOccConverter()->setSurfColorDefault(TmpSurf);
				}
				else
				{
					TmpSurf->SetColor(1, 0, 0);
				}
				break;
			}
		}

		Handle(Geom_BSplineSurface) tmpSelOcctBSplineSurf = GeomConvert::SurfaceToBSplineSurface(BRep_Tool::Surface(gWindow->GetSTEPModelConverter()->GetTopoDS_FaceList()[BSplineSurfInds[SelIndex]]));

		m_pToolPathDlg->SetOcctSurf(tmpSelOcctBSplineSurf);

		static_cast<FFace*>(m_pEnt)->GetSurface()->SetColor(1, 0, 0);
		gWindow->UpdateModelView();
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
