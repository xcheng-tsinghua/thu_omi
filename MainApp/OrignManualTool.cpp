#include "OrignManualTool.h"
#include "FMainWindow.h"
#include "MainDoc.h"
#include "MainView.h"
#include "FSubwindow.h"
#include "FUtilities.h"
#include "occConverter.h"
#include <math_Gauss.hxx>
#include "MeasPt.h"
#include <BRep_Tool.hxx>
#include <gp_Pln.hxx>

OrignManualTool::OrignManualTool(CoorTrans* aCoorAlt) :FFeatureTool(/*FT_SELECT_TOOL*/)
{
	m_pEnt = NULL;
	m_iControlKeyState = 0;
	gWindow->ResetToDefaultCountenance();
	m_pCoorAlt = aCoorAlt;

	double lenAxis = 8;
	theOrignX = new MeasPt(0, 0, 0, lenAxis, 0, 0);
	theOrignY = new MeasPt(0, 0, 0, 0, lenAxis, 0);
	theOrignZ = new MeasPt(0, 0, 0, 0, 0, lenAxis);

}

OrignManualTool::~OrignManualTool()
{

}

int OrignManualTool::ProcessEvent(FEventType eventType, const FPickEvent& pe)
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
		gWindow->GetMainDoc()->RemoveDispEnts(theOrignX);
		gWindow->GetMainDoc()->RemoveDispEnts(theOrignY);
		gWindow->GetMainDoc()->RemoveDispEnts(theOrignZ);

		pe.view->PickEntity(pe.x, pe.y, m_pEnt, m_selType);

		//若未选中，将全部型面置为默认颜色
		if (!m_pEnt)
		{
			gWindow->ResetToDefaultCountenance();

			theSelSurfaceList.clear();
			return 1;
		}
		//若已高亮，将其取消高亮
		float r, g, b;
		static_cast<FFace*>(m_pEnt)->GetSurface()->GetColor(r, g, b);

		if (g == 1.0)
		{
			//删除选中列表中的曲面
			vector<FSurface*> tmpSurfList;
			for (int i = 0; i < theSelSurfaceList.size(); i++)
			{
				if (static_cast<FFace*>(m_pEnt)->GetSurface() != theSelSurfaceList[i])
				{
					tmpSurfList.push_back(theSelSurfaceList[i]);
				}
			}
			theSelSurfaceList = tmpSurfList;

			gWindow->GetOccConverter()->setSurfColorDefault(static_cast<FFace*>(m_pEnt)->GetSurface());

			m_pEnt = NULL;

			return 1;
		}

		//选中了未高亮的曲面
		//如果选中的不是圆柱或平面，不高亮
		if (!IsCylinderOrPlane(static_cast<FFace*>(m_pEnt)->GetSurface()))
		{
			return 1;
		}

		//如果当前选中圆柱其之前也选中圆柱，删除之前的圆柱
		if (IsCylinder(static_cast<FFace*>(m_pEnt)->GetSurface()))
		{
			int CyIndex = 9999;
			for (int i = 0; i < theSelSurfaceList.size(); i++)
			{
				if (IsCylinder(theSelSurfaceList[i]))
				{
					CyIndex = i;
					gWindow->GetOccConverter()->setSurfColorDefault(theSelSurfaceList[i]);
					break;
				}
			}
			if (CyIndex != 9999)
			{
				vector<FSurface*> tmpSelSurfList;
				for (int i = 0; i < theSelSurfaceList.size(); i++)
				{
					if (i != CyIndex)
					{
						tmpSelSurfList.push_back(theSelSurfaceList[i]);
					}
				}
				theSelSurfaceList = tmpSelSurfList;
			}
		}

		//如果已有三个面且当前选中圆柱面，移除2个曲面
		if (IsCylinder(static_cast<FFace*>(m_pEnt)->GetSurface()) && (theSelSurfaceList.size() == 3))
		{
			gWindow->GetOccConverter()->setSurfColorDefault(theSelSurfaceList[0]);
			gWindow->GetOccConverter()->setSurfColorDefault(theSelSurfaceList[1]);

			vector<FSurface*> tmpSelSurfList;
			for (int i = 2; i < theSelSurfaceList.size(); i++)
			{
				tmpSelSurfList.push_back(theSelSurfaceList[i]);
			}
			theSelSurfaceList = tmpSelSurfList;
		}
		else if (IsCylinder(static_cast<FFace*>(m_pEnt)->GetSurface()) && (theSelSurfaceList.size() == 2))//如果已有2个面且当前选中圆柱面，移除1个曲面
		{
			gWindow->GetOccConverter()->setSurfColorDefault(theSelSurfaceList[0]);

			vector<FSurface*> tmpSelSurfList;
			for (int i = 1; i < theSelSurfaceList.size(); i++)
			{
				tmpSelSurfList.push_back(theSelSurfaceList[i]);
			}
			theSelSurfaceList = tmpSelSurfList;
		}
		else
		{
			//若有圆柱面，最多高亮2个曲面，
			bool IsContCylin = false;
			for (int i = 0; i < theSelSurfaceList.size(); i++)
			{
				if (IsCylinder(theSelSurfaceList[i]))
				{
					IsContCylin = true;
				}
			}

			if (IsContCylin)//包含圆柱面，最多高亮2个
			{
				if (theSelSurfaceList.size() >= 2)//若已高亮2个，删除一个
				{
					gWindow->GetOccConverter()->setSurfColorDefault(theSelSurfaceList[0]);

					vector<FSurface*> tmpSelSurfList;
					for (int i = 1; i < theSelSurfaceList.size(); i++)
					{
						tmpSelSurfList.push_back(theSelSurfaceList[i]);
					}
					theSelSurfaceList = tmpSelSurfList;
				}
			}
			else//不包含圆柱面，最多高亮3个
			{
				if (theSelSurfaceList.size() >= 3)//若已高亮3个，删除一个
				{
					gWindow->GetOccConverter()->setSurfColorDefault(theSelSurfaceList[0]);

					vector<FSurface*> tmpSelSurfList;
					for (int i = 1; i < theSelSurfaceList.size(); i++)
					{
						tmpSelSurfList.push_back(theSelSurfaceList[i]);
					}
					theSelSurfaceList = tmpSelSurfList;
				}
			}
		}

		//曲面高亮
		static_cast<FFace*>(m_pEnt)->GetSurface()->SetColor(0, 1, 1);
		theSelSurfaceList.push_back(static_cast<FFace*>(m_pEnt)->GetSurface());

		gWindow->UpdateModelView();

		m_pEnt = NULL;

		/// //////////////////////////////////////////////
		bool IsContCylin = false;
		for (int i = 0; i < theSelSurfaceList.size(); i++)
		{
			if (IsCylinder(theSelSurfaceList[i]))
			{
				IsContCylin = true;
				break;
			}
		}
		if (IsContCylin && (theSelSurfaceList.size() == 2))//如果存在圆柱面且选中2个曲面，计算原点
		{
			//找到圆柱面
			int CylinSurIndex = 9999;
			for (int i = 0; i < theSelSurfaceList.size(); i++)
			{
				if (IsCylinder(theSelSurfaceList[i]))
				{
					CylinSurIndex = i;
					break;
				}
			}
			//找到对应的OCCT曲面

			SURFACE_TYPE theSurfType;
			int ind_in_all = 0;
			int ind_in_type = 0;
			QString tmpFSurfName;

			gWindow->GetSTEPModelConverter()->FSurfInfo(theSelSurfaceList[CylinSurIndex], theSurfType, ind_in_all, ind_in_type, tmpFSurfName);

			Handle(Geom_CylindricalSurface) tmpOcctCylin = Handle(Geom_CylindricalSurface)::DownCast(BRep_Tool::Surface(gWindow->GetSTEPModelConverter()->GetTopoDS_FaceList()[ind_in_all]));

			if (CylinSurIndex)
			{
				CylinSurIndex = 0;
			}
			else
			{
				CylinSurIndex = 1;
			}

			ON_Plane tmpPlane = CreatPlane(theSelSurfaceList[CylinSurIndex]);
			ON_3dVector tmpVec = tmpPlane.Normal();

			double A = tmpVec.x;
			double B = tmpVec.y;
			double C = tmpVec.z;
			double x0 = tmpPlane.Origin().x;
			double y0 = tmpPlane.Origin().y;
			double z0 = tmpPlane.Origin().z;
			double O = tmpOcctCylin->Axis().Location().X();
			double P = tmpOcctCylin->Axis().Location().Y();
			double K = tmpOcctCylin->Axis().Location().Z();
			double M = tmpOcctCylin->Axis().Direction().X();
			double N = tmpOcctCylin->Axis().Direction().Y();
			double L = tmpOcctCylin->Axis().Direction().Z();

			if (abs(A * M + B * N + C * L) < 1e-5)
			{
				QMessageBox::information(0, QString::fromLocal8Bit("WARINING"),
					QString::fromLocal8Bit("平面法线与圆柱面平行，参考几何体无效"), "OK");
				return 1;
			}

			double ParaMeter = (A * x0 + B * y0 + C * z0 - A * O - B * P - C * K) / (A * M + B * N + C * L);
			
			double ResX = O + M * ParaMeter;
			double ResY = P + N * ParaMeter;
			double ResZ = K + L * ParaMeter;

			theOrignX->getm_x() = ResX;
			theOrignX->getm_y() = ResY;
			theOrignX->getm_z() = ResZ;

			theOrignY->getm_x() = ResX;
			theOrignY->getm_y() = ResY;
			theOrignY->getm_z() = ResZ;

			theOrignZ->getm_x() = ResX;
			theOrignZ->getm_y() = ResY;
			theOrignZ->getm_z() = ResZ;

			gWindow->GetMainDoc()->AddDispEnt(theOrignX);
			gWindow->GetMainDoc()->AddDispEnt(theOrignY);
			gWindow->GetMainDoc()->AddDispEnt(theOrignZ);

			m_pCoorAlt->SetMoveX_Val(-ResX);
			m_pCoorAlt->SetMoveY_Val(-ResY);
			m_pCoorAlt->SetMoveZ_Val(-ResZ);

			m_pCoorAlt->SetRotateX_Val(0);
			m_pCoorAlt->SetRotateY_Val(0);
			m_pCoorAlt->SetRotateZ_Val(0);

			m_pCoorAlt->SetValInGroupPathPlan();

		}
		if ((!IsContCylin) && (theSelSurfaceList.size() == 3))//如果不存在圆柱面且选中3个曲面，计算原点
		{
			ON_Plane tmpPlane0 = CreatPlane(theSelSurfaceList[0]);
			ON_Plane tmpPlane1 = CreatPlane(theSelSurfaceList[1]);
			ON_Plane tmpPlane2 = CreatPlane(theSelSurfaceList[2]);

			gp_Dir tmpDir0 = gp_Dir(tmpPlane0.Normal().x, tmpPlane0.Normal().y, tmpPlane0.Normal().z);
			gp_Dir tmpDir1 = gp_Dir(tmpPlane1.Normal().x, tmpPlane1.Normal().y, tmpPlane1.Normal().z);
			gp_Dir tmpDir2 = gp_Dir(tmpPlane2.Normal().x, tmpPlane2.Normal().y, tmpPlane2.Normal().z);

			//如果存在任意两平面平行，无法计算
			if (tmpDir0.IsParallel(tmpDir1, 1e-5) || tmpDir1.IsParallel(tmpDir2, 1e-5) || tmpDir2.IsParallel(tmpDir0, 1e-5))
			{
				QMessageBox::information(0, QString::fromLocal8Bit("WARINING"),
					QString::fromLocal8Bit("存在平行平面，参考几何体无效"), "OK");
				return 1;
			}
			//如果三平面无交点，无法计算
			gp_Dir tmpIsValidDir1 = gp_Dir(tmpDir1.X() - tmpDir0.X(), tmpDir1.Y() - tmpDir0.Y(), tmpDir1.Z() - tmpDir0.Z());
			gp_Dir tmpIsValidDir2 = gp_Dir(tmpDir2.X() - tmpDir0.X(), tmpDir2.Y() - tmpDir0.Y(), tmpDir2.Z() - tmpDir0.Z());
			gp_Dir tmpIsValid = gp_Dir(tmpIsValidDir1.Y() * tmpIsValidDir2.Z() - tmpIsValidDir1.Z() * tmpIsValidDir2.Y(), tmpIsValidDir1.Z() * tmpIsValidDir2.X() - tmpIsValidDir1.X() * tmpIsValidDir2.Z(), tmpIsValidDir1.X() * tmpIsValidDir2.Y() - tmpIsValidDir1.Y() * tmpIsValidDir2.X());

			gp_Pln tmpIsValidPlan = gp_Pln(gp_Pnt(tmpPlane0.Normal().x, tmpPlane0.Normal().y, tmpPlane0.Normal().z), tmpIsValid);
			if (tmpIsValidPlan.Distance(gp_Pnt(0, 0, 0)) < 1e-5)
			{
				QMessageBox::information(0, QString::fromLocal8Bit("WARINING"),
					QString::fromLocal8Bit("三平面无交点，参考几何体无效"), "OK");
				return 1;
			}
			math_Matrix MXA(1, 3, 1, 3);
			
			math_Vector MXB(1, 3);
			math_Vector x1(1, 3);
			x1.Init(1);

			MXA(1, 1) = tmpDir0.X(); MXA(1, 2) = tmpDir0.Y(); MXA(1, 3) = tmpDir0.Z();
			MXA(2, 1) = tmpDir1.X(); MXA(2, 2) = tmpDir1.Y(); MXA(2, 3) = tmpDir1.Z();
			MXA(3, 1) = tmpDir2.X(); MXA(3, 2) = tmpDir2.Y(); MXA(3, 3) = tmpDir2.Z();

			MXB(1) = tmpDir0.X() * tmpPlane0.Origin().x + tmpDir0.Y() * tmpPlane0.Origin().y + tmpDir0.Z() * tmpPlane0.Origin().z;
			MXB(2) = tmpDir1.X() * tmpPlane1.Origin().x + tmpDir1.Y() * tmpPlane1.Origin().y + tmpDir1.Z() * tmpPlane1.Origin().z;
			MXB(3) = tmpDir2.X() * tmpPlane2.Origin().x + tmpDir2.Y() * tmpPlane2.Origin().y + tmpDir2.Z() * tmpPlane2.Origin().z;

			math_Gauss  sol(MXA);
			if (sol.IsDone())
			{
				sol.Solve(MXB, x1);
			}
			else
			{
				try
				{
					sol.Solve(MXB, x1);
				}
				catch (Standard_Failure)
				{
					Handle(Standard_Failure) error = Standard_Failure::Caught();
					cout << error << endl;
				}
			}

			double ResX = x1(1);
			double ResY = x1(2);
			double ResZ = x1(3);

			theOrignX->getm_x() = ResX;
			theOrignX->getm_y() = ResY;
			theOrignX->getm_z() = ResZ;

			theOrignY->getm_x() = ResX;
			theOrignY->getm_y() = ResY;
			theOrignY->getm_z() = ResZ;

			theOrignZ->getm_x() = ResX;
			theOrignZ->getm_y() = ResY;
			theOrignZ->getm_z() = ResZ;

			gWindow->GetMainDoc()->AddDispEnt(theOrignX);
			gWindow->GetMainDoc()->AddDispEnt(theOrignY);
			gWindow->GetMainDoc()->AddDispEnt(theOrignZ);

			m_pCoorAlt->SetMoveX_Val(-ResX);
			m_pCoorAlt->SetMoveY_Val(-ResY);
			m_pCoorAlt->SetMoveZ_Val(-ResZ);

			m_pCoorAlt->SetRotateX_Val(0);
			m_pCoorAlt->SetRotateY_Val(0);
			m_pCoorAlt->SetRotateZ_Val(0);

			m_pCoorAlt->SetValInGroupPathPlan();
		}
	}
	if (eventType == MOUSE_BUTTON_DOWN && pe.nFlags == MK_LBUTTON && m_iStep == 0 && m_iControlKeyState == 1)	//left button down, start of mouse drag，已按下ctrl
	{

	}
	else if (eventType == MOUSE_MOVE && pe.nFlags == MK_LBUTTON && m_iStep == 1)	//left button drag, mouse dragging
	{

	}
	else if (eventType == MOUSE_BUTTON_UP && pe.nFlags == MK_LBUTTON && m_iStep == 1)	//left button up, end of mouse drag
	{
		m_iStep = 0;
	}
	else if (eventType == MOUSE_BUTTON_DOWN && pe.nFlags == MK_RBUTTON)
	{
		RemoveOrign();
	}
	else if (eventType == MOUSE_BUTTON_UP && pe.nFlags == MK_RBUTTON)
	{

		//m_pPopMenu->clear();
		//m_pPopMenu->addAction(m_pActionRename);
		//m_pPopMenu->addAction(m_pActionConfig);
		//m_pPopMenu->addAction(m_pActionTest);
		//m_pPopMenu->addAction(m_pActionNewFeat);
		//m_pPopMenu->addAction(m_pActionAddEle);

		//m_pPopMenu->exec(QCursor::pos());

		m_iStep = 0;
	}
	return 1;
}

bool OrignManualTool::IsCylinder(FSurface* testSurface)
{
	SURFACE_TYPE theSurfType;
	int ind_in_all = 0;
	int ind_in_type = 0;
	QString tmpFSurfName;

	gWindow->GetSTEPModelConverter()->FSurfInfo(testSurface, theSurfType, ind_in_all, ind_in_type, tmpFSurfName);

	if (theSurfType == CYLINDER)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool OrignManualTool::IsCylinderOrPlane(FSurface* testSurface)
{
	SURFACE_TYPE theSurfType;
	int ind_in_all = 0;
	int ind_in_type = 0;
	QString tmpFSurfName;

	gWindow->GetSTEPModelConverter()->FSurfInfo(testSurface, theSurfType, ind_in_all, ind_in_type, tmpFSurfName);

	if (theSurfType == CYLINDER || theSurfType == PLANE)
	{
		return true;
	}
	else
	{
		return false;
	}

}

ON_Plane OrignManualTool::CreatPlane(FSurface* pSurf)
{
	FFace* pFace = new FFace();
	ON_Mesh* fMesh = pSurf->GetMesh()->OnMesh();
	fMesh->ComputeFaceNormals();

	for (int i = 0; i < gWindow->GetMainDoc()->SelectedEntCount(); i++)
	{
		pFace = (FFace*)gWindow->GetMainDoc()->GetSelectedEnt(i);
		if (pFace->GetSurface() == pSurf)
		{
			break;
		}
	}
	//PFVertexList pntList;
	ON_3fPointArray pntList;
	//pFace->VerticesAround(pntList);
	pntList = fMesh->m_V;
	int nsize = pntList.Count();
	ON_Plane transPlane = ON_Plane();//创建一个平面


	//计算平面法向量
	ON_3dVector localArray1 = ON_3dVector(pntList[2].x - pntList[0].x, pntList[2].y - pntList[0].y, pntList[2].z - pntList[0].z);
	ON_3dVector localArray2 = ON_3dVector(pntList[1].x - pntList[0].x, pntList[1].y - pntList[0].y, pntList[1].z - pntList[0].z);
	ON_3dVector surfNorm = ON_CrossProduct(localArray1, localArray2);
	surfNorm.Unitize();
	ON_3dVector refNorm;
	refNorm = fMesh->m_FN[0];
	refNorm.Unitize();

	if (surfNorm != refNorm)
	{
		surfNorm = -1.0 * surfNorm;
	}

	//由法向量和质心得到新的平面
	transPlane.CreateFromNormal((ON_3dPoint)pntList[0], surfNorm);

	return transPlane;
}

void OrignManualTool::RemoveOrign()
{
	gWindow->GetMainDoc()->RemoveDispEnts(theOrignX);
	gWindow->GetMainDoc()->RemoveDispEnts(theOrignY);
	gWindow->GetMainDoc()->RemoveDispEnts(theOrignZ);
}