#include "EntityHighlight.h"
#include "FMainWindow.h"
#include "MainDoc.h"
#include "MainView.h"
#include "FSubwindow.h"
#include "FUtilities.h"
#include "occConverter.h"

//extern FMainWindow* gWindow;

EntityHighlight::EntityHighlight()
	:FFeatureTool(/*FT_SELECT_TOOL*/)
{
	m_pEnt = NULL;
	LastSelectedEntity = NULL;
	LastSelItem = NULL;
	//IsFaceTranced = 0;
	m_iControlKeyState = 0;

	IsMouseMove = false;

	m_pShowNearbySurf = new QAction(QString::fromLocal8Bit("显示邻面"), this);
	m_pShowNearbySurf->setIcon(QIcon("../icons/Nearby.png"));

	m_pPopMenu = QSharedPointer<QMenu>(new QMenu());

	m_pPopMenu->setStyleSheet(
		"QMenu"
		"{"
		"background-color:rgb(233,238,255);"
		//"border:0px solid rgb(148,166,202);"
		"border:1px solid rgb(93,107,153);"
		"}"
		"QMenu::item:selected"
		"{"
		"color:black;"
		"background-color:rgb(177,197,255);"
		"}"
		"QMenu::item:pressed"
		"{"
		"color:black;"
		"background-color:rgb(177,197,255);"
		"}");

	//m_pActionRename = new QAction(QString::fromLocal8Bit("重命名"), this);
	//m_pActionConfig = new QAction(QString::fromLocal8Bit("配置"), this);
	//m_pActionTest = new QAction(QString::fromLocal8Bit("测试"), this);
	//m_pActionNewFeat = new QAction(QString::fromLocal8Bit("新检测特征"), this);
	//m_pActionAddEle = new QAction(QString::fromLocal8Bit("添加到检测特征面列表"), this);

	connect(m_pShowNearbySurf, SIGNAL(triggered()), this, SLOT(onShowNearSurf()));

}

EntityHighlight::~EntityHighlight()
{

}

int EntityHighlight::ProcessEvent(FEventType eventType, const FPickEvent& pe)
{
	if (eventType == KEY_DOWN)//The key have been pushed down
	{
		if (pe.nChar == Qt::Key_Control)  //"Ctrl" key have been pushed down
		{
			m_iControlKeyState = 1;
			gSetHintMessage(tr("'Ctrl Key' has been pushed down, left button click the curve's point will add repeat point."));
		}
		else
		{
			m_iControlKeyState = 0;
		}
	}
	else if (eventType == KEY_UP)//release the key
	{
		m_iControlKeyState = 0;
	}

	if (eventType == MOUSE_BUTTON_DOWN && pe.nFlags == MK_LBUTTON && m_iStep == 0 && m_iControlKeyState == 0)	//left button down, start of mouse drag，未按下ctrl
	{
		pe.view->PickEntity(pe.x, pe.y, m_pEnt, m_selType);

		//若未选中，将全部型面置为默认颜色
		if (!m_pEnt)
		{
			gWindow->ResetToDefaultCountenance();
			ResetToDefaultItemCheckedStatus();
			LastSelectedEntity = NULL;
			LastSelItem = NULL;
			return 1;
		}
		//若已高亮，将其取消高亮
		float r, g, b;
		static_cast<FFace*>(m_pEnt)->GetSurface()->GetColor(r, g, b);

		if (r == 1.0)
		{
			gWindow->ResetToDefaultCountenance();
			ResetToDefaultItemCheckedStatus();

			LastSelectedEntity = NULL;
			m_pEnt = NULL;
			return 1;
		}

		//选中了未高亮的曲面
		gWindow->ResetToDefaultCountenance();
		ResetToDefaultItemCheckedStatus();

		//将左侧模型结构树选中实体对应的项目高亮
		QTreeWidgetItemIterator m_pCurrItem(static_cast<FTreeWnd*>(gWindow->GetSubWnd()[0])->getTreeWidget());
		FSurface* SelSurface = static_cast<FFace*>(m_pEnt)->GetSurface();

		while (*m_pCurrItem)
		{
			FSurface* tmpSurf = (FSurface*)(*m_pCurrItem)->data(0, Qt::UserRole).value<void*>();
			if (tmpSurf == SelSurface)
			{
				(*m_pCurrItem)->setSelected(1);
				LastSelItem = (*m_pCurrItem);
				break;
			}
			++m_pCurrItem;
		}

		//曲面高亮
		static_cast<FFace*>(m_pEnt)->GetSurface()->SetColor(1, 0, 0);

		gWindow->UpdateModelView();

		LastSelectedEntity = m_pEnt;
		m_pEnt = NULL;
	}
	if (eventType == MOUSE_BUTTON_DOWN && pe.nFlags == MK_LBUTTON && m_iStep == 0 && m_iControlKeyState == 1)	//left button down, start of mouse drag，已按下ctrl
	{
		pe.view->PickEntity(pe.x, pe.y, m_pEnt, m_selType);

		if (!m_pEnt) return 1;

		QTreeWidgetItemIterator m_pCurrItem(static_cast<FTreeWnd*>(gWindow->GetSubWnd()[0])->getTreeWidget());
		FSurface* SelSurface = static_cast<FFace*>(m_pEnt)->GetSurface();

		while (*m_pCurrItem)
		{
			FSurface* tmpSurf = (FSurface*)(*m_pCurrItem)->data(0, Qt::UserRole).value<void*>();
			if (tmpSurf == SelSurface)
			{
				LastSelItem = (*m_pCurrItem);
				break;
			}
			++m_pCurrItem;
		}

		//若已高亮，将其取消高亮
		float r, g, b;
		static_cast<FFace*>(m_pEnt)->GetSurface()->GetColor(r, g, b);

		if (r == 1.0)
		{
			gWindow->GetOccConverter()->setSurfColorDefault(static_cast<FFace*>(m_pEnt)->GetSurface());

			LastSelItem->setSelected(0);

			LastSelectedEntity = NULL;
			m_pEnt = NULL;
			return 1;
		}

		//选中了未高亮的曲面
		//曲面高亮
		static_cast<FFace*>(m_pEnt)->GetSurface()->SetColor(1, 0, 0);

		LastSelItem->setSelected(1);

		gWindow->UpdateModelView();

		LastSelectedEntity = m_pEnt;
		m_pEnt = NULL;
		
	}
	else if (eventType == MOUSE_MOVE && pe.nFlags == MK_LBUTTON && m_iStep == 1)	//left button drag, mouse dragging
	{
		IsMouseMove = true;
	}
	else if (eventType == MOUSE_BUTTON_UP && pe.nFlags == MK_LBUTTON && m_iStep == 1)	//left button up, end of mouse drag
	{
		m_iStep = 0;
	}
	else if (eventType == MOUSE_BUTTON_UP && pe.nFlags == MK_RBUTTON && m_iStep == 0)  // 右键菜单
	{
		//pe.view->PickEntity(pe.x, pe.y, m_pEnt, m_selType);
		//if (!m_pEnt) return 1;
		//
		//m_pPopMenu->clear();
		//m_pPopMenu->addAction(m_pShowNearbySurf);
		//m_pPopMenu->exec(dynamic_cast<FSubWindow*>(gWindow->GetSubWnd()[3])->cursor().pos());

		m_iStep = 0;
	}
	return 1;
}

void EntityHighlight::ResetToDefaultItemCheckedStatus()
{
	gWindow->ResetToDefaultCountenance();

	QTreeWidgetItemIterator m_pCurrItem(static_cast<FTreeWnd*>(gWindow->GetSubWnd()[0])->getTreeWidget());
	while (*m_pCurrItem)
	{
		(*m_pCurrItem)->setSelected(0);
		++m_pCurrItem;
	}
}

void EntityHighlight::onShowNearSurf()
{
	gWindow->ResetToDefaultCountenance();

	FSurface* SelSurface = static_cast<FFace*>(m_pEnt)->GetSurface();

	vector<FSurface*> AllFSurfList = gWindow->GetSTEPModelConverter()->GetFSurfList();

	vector<SurfNearbyGraph_Node*> NodeList = gWindow->GetMainDoc()->GetSurfNearbyGraph();

	for (auto ita = NodeList.begin(); ita != NodeList.end(); ++ita)
	{
		if (AllFSurfList[(*ita)->GetGeomIndex()] == SelSurface)
		{
			AllFSurfList[(*ita)->GetGeomIndex()]->SetColor(1, 0, 0);

			for (auto ita2 = (*ita)->GetNearbyNodeList().begin(); ita2 != (*ita)->GetNearbyNodeList().end(); ++ita2)
			{
				AllFSurfList[(*ita2)->GetGeomIndex()]->SetColor(1, 0, 0);
			}

			break;
		}

	}

}
