#pragma once
#include <qthread.h>
#include "../External/fkernel/Include/FKernelDefines.h"
#include "../External/fkernel/Include/FEntity.h"

class InspVis_thread :public QThread
{
	//Q_OBJECT
public:
	InspVis_thread();
	int CurPntIndex;

protected:
    void run();

private:
	void MoveTo(F3dPoint theAim, int theMoveTime = 1, int Smooth = 100);//theAim目标点，theMoveTime移动时间，Smooth流畅度
	ON_3dPoint LastLoc;
};