#include "InspVis_thread.h"
#include "FMainWindow.h"
#include "../External/fkernel/Include/FKernelUtilities.h"

InspVis_thread::InspVis_thread()
{
	CurPntIndex = 0;
	LastLoc = ON_3dPoint(0, 0, 0);
}

void InspVis_thread::run()
{
	//整理检测路径中的点
	FIpPathList CurrPathList = gWindow->GetMainDoc()->GetPathList();
	FIpPntList PathPntList;

	for (int i = 0; i < CurrPathList.size(); i++)
	{
		for (int j = 0; j < CurrPathList[i].GetSize(); j++)
		{
			if (i == 0)//第一条把所有的点都加进去
			{
				PathPntList.push_back(CurrPathList[i].GetPoint(j));
			}
			else//其它的第一个点不加
			{
				if (j == 0)
					continue;

				PathPntList.push_back(CurrPathList[i].GetPoint(j));
			}
		}
	}

	if (!CurPntIndex)
	{
		MoveTo(F3dPoint(PathPntList[0].x, PathPntList[0].y, PathPntList[0].z), 0, 1);
	}
	else
	{
		MoveTo(LastLoc, 0, 1);
	}


	//Sleep(1000);

	int RunSta = CurPntIndex;

	for (int i = RunSta + 1; i < PathPntList.size(); i++)
	{
		F3dPoint tmpAim = F3dPoint(PathPntList[i].x, PathPntList[i].y, PathPntList[i].z);

		MoveTo(tmpAim);

		CurPntIndex = i;
		if (i == PathPntList.size() - 1)
		{
			CurPntIndex = 0;
		}
	}
}

void InspVis_thread::MoveTo(F3dPoint theAim, int theMoveTime, int Smooth)
{
	F3dVector MoveVec = theAim - gWindow->GetVisProbe()->curPnt;

	F3dVector MoveStep = F3dVector(MoveVec.x / double(Smooth), MoveVec.y / double(Smooth), MoveVec.z / double(Smooth));

	int PauseInv = double(theMoveTime * 1000) / double(Smooth);

	vector<F3dPoint> tmpAimList;

	for (int i = 0; i < Smooth; i++)
	{
		tmpAimList.push_back(gWindow->GetVisProbe()->curPnt + (i + 1) * MoveStep);
	}

	for (int i = 0; i < Smooth; i++)
	{
		//gWindow->GetVisProbe()->curPnt += MoveStep;
		//gWindow->GetVisProbe()->IniTo(tmpAimList[i]);

		F3dVector tVec = tmpAimList[i] - gWindow->GetVisProbe()->curPnt;
		FForm tForm;
		tForm.Translation(tVec);
		gWindow->GetVisProbe()->m_pProbe->DoTransform(tForm);
		gWindow->GetVisProbe()->curPnt = tmpAimList[i];
		LastLoc.x = tmpAimList[i].x;
		LastLoc.y = tmpAimList[i].y;
		LastLoc.z = tmpAimList[i].z;

		Sleep(PauseInv);
		gWindow->UpdateModelView();
	}

	gWindow->GetVisProbe()->curPnt = theAim;
}
