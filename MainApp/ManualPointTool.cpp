#include "MainDoc.h"
#include "FMainWindow.h"
#include "FSubwindow.h"
#include "MainView.h"
#include "FUtilities.h"
#include "FPathPlan.h"
#include "ManualPointTool.h"
#include "MeasPt.h"
#include "np.h"
#include <BRepIntCurveSurface_Inter.hxx>
#include "occConverter.h"
#include <gp_Lin.hxx>

extern FMainWindow* gWindow;
extern clock_t gTStart, gTEnd;

void ManualPointTool::Init()
{
	m_pDoc = NULL;
	m_pSelEnt = NULL;
}

ManualPointTool :: ManualPointTool()
{
	Init();
}

ManualPointTool :: ManualPointTool(FMainDoc * pDoc)
{
	Init();
	m_pDoc = pDoc;
}

ManualPointTool :: ~ManualPointTool()
{

}

int ManualPointTool::ProcessEvent(FEventType eventType, const FPickEvent& pe)
{
	gTStart = clock();

	if (eventType==MOUSE_BUTTON_DOWN && pe.nFlags==MK_LBUTTON && m_iStep == 0)	//left button down, choose a new point for add evaluation
	{
		// 先选择一个面，手动布点的点必定在选中的面内
		pe.view->PickEntity(pe.x, pe.y, m_pSelEnt, FT_FACE);
		FSurface* selFSurf = static_cast<FFace*>(m_pSelEnt)->GetSurface();
		TopoDS_Face selTDSFace = gWindow->GetSTEPModelConverter()->FSurf2TDSFace(selFSurf);

		if (!m_pSelEnt) return 1;

		// 计算屏幕法线
		F3dPoint tPnt;
		F3dPoint viewPnt1;
		F3dPoint viewPnt2;
		F3dPoint viewPnt3;
		pe.view->Screen2WCS(pe.x, pe.y, tPnt);
		pe.view->Screen2WCS(pe.x + 1, pe.y, viewPnt1);
		pe.view->Screen2WCS(pe.x, pe.y + 1, viewPnt3);
		ON_3dVector viewX = ON_3dVector(viewPnt1.x - tPnt.x, viewPnt1.y - tPnt.y, viewPnt1.z - tPnt.z);
		ON_3dVector viewY = ON_3dVector(viewPnt3.x - tPnt.x, viewPnt3.y - tPnt.y, viewPnt3.z - tPnt.z);
		ON_3dVector unitView = ON_CrossProduct(viewX, viewY);
		unitView = unitView / unitView.Length();
		F3dPoint endPoint = F3dPoint(tPnt.x + unitView.x * 10000, tPnt.y + unitView.y * 10000, tPnt.z + unitView.z * 10000);

		// 计算相交点
		gp_Pnt screenPnt = GP_PNT(tPnt);
		gp_Lin screenNorm = gp_Lin(screenPnt, gp_Dir(unitView.x, unitView.y, unitView.z));
		BRepIntCurveSurface_Inter inter;
		inter.Init(selTDSFace, screenNorm, Precision::Intersection());

		if (inter.More()) 
		{
			map<double, gp_Pnt> pointsInt;

			for (; inter.More(); inter.Next()) 
			{
				gp_Pnt intersectionPoint = inter.Pnt();
				pointsInt[screenPnt.Distance(intersectionPoint)] = intersectionPoint;

			}

			if (pointsInt.size() == 0)
			{
				std::cout << "No intersection found." << std::endl;
			}
			else
			{
				// 交叉点
				gp_Pnt intPnt = (*(pointsInt.begin())).second;

				// 计算法线
				gp_Vec intNorm = NormalAt(selTDSFace, intPnt);

				MeasPt* measPnt = new MeasPt(intPnt, intNorm);
				gWindow->GetMainDoc()->AddDispEnt(measPnt);
				//gWindow->GetMainDoc()->getPntsShowList().push_back(measPnt);

				//实现测点数据的列表显示
				FIpPntList newPntList = gWindow->GetMainDoc()->GetIpPntList();
				gWindow->GetMainDoc()->ClearIpPntList();
				FIpPoint dataPnt = FIpPoint(intPnt.X(), intPnt.Y(), intPnt.Z());
				dataPnt.i = intNorm.X(); dataPnt.j = intNorm.Y(); dataPnt.k = intNorm.Z();
				newPntList.push_back(dataPnt);
				gWindow->GetMainDoc()->SetIpPntList(newPntList);

			}

		}
		else 
		{
			std::cout << "No intersection found." << std::endl;
		}

		gTEnd = clock();
		double time_taken = double(gTEnd - gTStart) / CLOCKS_PER_SEC; // 计算耗时（秒）
		std::cout << "函数运行时间: " << time_taken << " 秒" << std::endl;

	}

	if (eventType==MOUSE_BUTTON_DOWN && pe.nFlags==MK_RBUTTON && m_iStep == 0)	//right button down, choose a new point for delete evaluation
	{
		//先判断显示法矢的点和显示位置的点数目是否相同，不相同说明出错
		int n_show_list = gWindow->GetMainDoc()->getPntsShowList().size();
		int n_ip_list = gWindow->GetMainDoc()->GetIpPntList().size();

		if (gWindow->GetMainDoc()->getPntsShowList().size() != gWindow->GetMainDoc()->GetIpPntList().size())
		{
			QMessageBox::information(0, QString::fromLocal8Bit("Info"),
				QString::fromLocal8Bit("无法删除测点，因为显示的测点和内部数据不一致"), "OK");
			return 1;
		}

		//将测点映射到屏幕上，并计算与鼠标之间的距离
		int theSelPntIndex;
		bool IsCaped = false;
		for (int i = 0; i < gWindow->GetMainDoc()->getPntsShowList().size(); i++)
		{
			int sx = 0, sy = 0;
			pe.view->WCS2Screen(F3dPoint(gWindow->GetMainDoc()->getPntsShowList()[i]->getm_x(), gWindow->GetMainDoc()->getPntsShowList()[i]->getm_y(), gWindow->GetMainDoc()->getPntsShowList()[i]->getm_z()), sx, sy);

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

			//删除m_pArrPntList和Doc中理论测点表中的对应测点
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

}