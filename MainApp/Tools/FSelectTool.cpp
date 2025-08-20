#include "../MainDoc.h"
#include "../FMainWindow.h"
#include "../FSubwindow.h"
#include "../MainView.h"
#include "../FUtilities.h"
#include "FSelectTool.h"
#include "qwidget.h"
#include "../PointPlanDlg.h"
#include "../occConverter.h"
#include "../MainApp/MeasPt.h"

extern FMainWindow* gWindow;

void FSelectTool::Init()
{
	m_pEnt = NULL;
	m_iKeyState = 0;
	m_selType = FT_ALL;
}

FSelectTool::FSelectTool(PointPlanDlg* _plan)
	:FFeatureTool(/*FT_SELECT_TOOL*/)
{
	pPointPlan = _plan;
	Init();
}

FSelectTool::FSelectTool(PointPlanDlg* _plan, FMainDoc* pDoc)
	:FFeatureTool(pDoc/*, FT_SELECT_TOOL*/)
{
	pPointPlan = _plan;
	m_pDoc = pDoc;
	Init();
}

FSelectTool::~FSelectTool()
{

}

int FSelectTool::ProcessEvent(FEventType eventType, const FPickEvent& pe)
{
	if (!m_pDoc)	return 0;
	if(eventType== KEY_DOWN)//The key have been pushed down
	{
		if (pe.nChar==Qt::Key_Shift) //"Shift" key have been pushed down 
		{
			m_iKeyState = 2;
			gSetHintMessage(tr("'Shift Key' has been pushed down, left button click the curve's point will delete the point."));
		}
		else if (pe.nChar==Qt::Key_Control)  //"Ctrl" key have been pushed down
		{
			m_iKeyState = 1;
			gSetHintMessage(tr("'Ctrl Key' has been pushed down, left button click the curve's point will add repeat point."));
		}
		else 
		{
			m_iKeyState = 0;
		}
	}
	else if (eventType==KEY_UP)//release the key
	{
		m_iKeyState = 0;
	}
	if (eventType==MOUSE_BUTTON_DOWN && pe.nFlags==MK_LBUTTON && m_iStep==0)	//left button down, start of mouse drag
	{
		pe.view->PickEntity(pe.x, pe.y, m_pEnt, m_selType);

		if (!m_pEnt)
		{
			//��δѡ�У���ȫ��������ΪĬ����ɫ
			gWindow->ResetToDefaultCountenance();

			return 0;
		}

		if (m_iKeyState!=1)	//ctrl key
		{
			int nDispEnt = m_pDoc->SelectedEntCount();
			for (int i=0; i<nDispEnt; i++)
			{	if (m_pDoc->GetSelectedEnt(i)!=m_pEnt)
					m_pDoc->RemoveSelectedEnts(m_pDoc->GetSelectedEnt(i));
			}			
		}
		m_pDoc->AddSelectedEnt(m_pEnt); //added to FDoc::m_selectEnts by FView class

		FSurface* tmpSelFSurf = static_cast<FFace*>(m_pEnt)->GetSurface();

		gWindow->ResetToDefaultCountenance();
		tmpSelFSurf->SetColor(1, 0, 0);

		SURFACE_TYPE theType;
		int IndexInAllList = 0;
		int IndexInTypeList = 0;
		QString tmpFSurfName;

		gWindow->GetSTEPModelConverter()->FSurfInfo(tmpSelFSurf, theType, IndexInAllList, IndexInTypeList, tmpFSurfName);

		pPointPlan->SetNotReverse();
		pPointPlan->setSelSurf(tmpSelFSurf);
		pPointPlan->GetComboBox()->setEnabled(true);
		//pPointPlan->setSampleReverseBtnDisable();

		if (theType == BSPLINE)
		{
			pPointPlan->setCBoxStatus(true);
		}
		else
		{
			pPointPlan->setCBoxStatus(false);
		}

		pPointPlan->GetSelPlaneLabel()->setText(tmpFSurfName);

		//switch (theType)
		//{
		//case BSPLINE:
		//{
		//	pPointPlan->GetSelPlaneLabel()->setText(QString::fromLocal8Bit("B��������") + QString::number(IndexInTypeList));
		//}
		//	break;
		//case CONE:
		//{
		//	pPointPlan->GetSelPlaneLabel()->setText(QString::fromLocal8Bit("׶��") + QString::number(IndexInTypeList));
		//}
		//	break;
		//case CYLINDER:
		//{
		//	pPointPlan->GetSelPlaneLabel()->setText(QString::fromLocal8Bit("Բ����") + QString::number(IndexInTypeList));
		//}
		//	break;
		//case PLANE:
		//{
		//	pPointPlan->GetSelPlaneLabel()->setText(QString::fromLocal8Bit("ƽ��") + QString::number(IndexInTypeList));
		//}
		//	break;
		//case SPHERE:
		//{
		//	pPointPlan->GetSelPlaneLabel()->setText(QString::fromLocal8Bit("����") + QString::number(IndexInTypeList));
		//}
		//	break;
		//case OTHER_SURF:
		//{
		//	pPointPlan->GetSelPlaneLabel()->setText(QString::fromLocal8Bit("��������") + QString::number(IndexInTypeList));
		//}
		//	break;
		//default:_DEBUG_ERROR("�ж�����������ʱ����");
		//	break;
		//}

		m_pEnt = NULL;
	}
	else if (eventType==MOUSE_MOVE && pe.nFlags==MK_LBUTTON && m_iStep==1)	//left button drag, mouse dragging
	{

	}
	else if (eventType==MOUSE_BUTTON_UP && pe.nFlags==MK_LBUTTON && m_iStep==1)	//left button up, end of mouse drag
	{
		m_iStep=0;
	}
	else if (eventType == MOUSE_BUTTON_DOWN && pe.nFlags == MK_RBUTTON)//�Ҽ������ɾ�����
	{
		//���ж���ʾ��ʸ�ĵ����ʾλ�õĵ���Ŀ�Ƿ���ͬ������ͬ˵������
		if (gWindow->GetMainDoc()->getPntsShowList().size() != gWindow->GetMainDoc()->GetIpPntList().size())
		{
			QMessageBox::information(0, QString::fromLocal8Bit("Info"),
				QString::fromLocal8Bit("���ִ��󣬸ù�����ʱ�޷�ʹ��"), "OK");
			return 1;
		}

		//�����ӳ�䵽��Ļ�ϣ������������֮��ľ���
		int theSelPntIndex;
		bool IsCaped = false;
		for (int i = 0; i < gWindow->GetMainDoc()->getPntsShowList().size(); i++)
		{
			int sx = 0, sy = 0;
			MeasPt* cPnt = gWindow->GetMainDoc()->getPntsShowList()[i];
			pe.view->WCS2Screen(F3dPoint(cPnt->getm_x(), cPnt->getm_y(), cPnt->getm_z()), sx, sy);

			if (sqrt((sx - pe.x) * (sx - pe.x) + (sy - pe.y) * (sy - pe.y) + 0.0) < 6)
			{
				theSelPntIndex = i;
				IsCaped = true;
				break;
			}
		}

		if (IsCaped)
		{
			gWindow->GetMainDoc()->RemoveDispEnts(gWindow->GetMainDoc()->getPntsShowList()[theSelPntIndex]);

			//ɾ��m_pArrPntList��Doc�����۲����еĶ�Ӧ���
			vector<MeasPt*> tmpArrList;
			FIpPntList tmpIpList;
			FIpPntList tmpIpList2 = gWindow->GetMainDoc()->GetIpPntList();
			for (int i = 0; i < gWindow->GetMainDoc()->getPntsShowList().size(); i++)
			{
				if (i != theSelPntIndex)
				{
					tmpArrList.push_back(gWindow->GetMainDoc()->getPntsShowList()[i]);
					tmpIpList.push_back(tmpIpList2[i]);
				}
			}
			gWindow->GetMainDoc()->getPntsShowList() = tmpArrList;
			gWindow->GetMainDoc()->ClearIpPntList();
			gWindow->GetMainDoc()->SetIpPntList(tmpIpList);

			gWindow->UpdateModelView();
		}
	}
	else if (eventType==MOUSE_BUTTON_UP && pe.nFlags==MK_RBUTTON)
	{
		m_iStep = 0;
		int nDispEnt = m_pDoc->SelectedEntCount();
		for (int i=0; i<nDispEnt; i++)
			m_pDoc->RemoveSelectedEnts(m_pDoc->GetSelectedEnt(i));
	}
	return 1;
}
