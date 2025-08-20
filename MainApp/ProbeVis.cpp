#include "ProbeVis.h"
#include "../External/fkernel/Include/FKernelUtilities.h"

ProbeVis::ProbeVis(FMainDoc* Doc)
{
	m_pDoc = Doc;
}

ProbeVis::~ProbeVis()
{

}

void ProbeVis::Initial()
{
	QString tStr = gAppPath + QString("/data/probe.stl");
	m_pProbe = FT::gImportSTLFile(tStr.toLocal8Bit().data());
	curPnt = F3dVector(0, 0, 0);

	FForm RotaFormX;
	RotaFormX.Rotation(PI / 2, ON_3dVector(1, 0, 0), ON_3dPoint(0, 0, 0));
	m_pProbe->DoTransform(RotaFormX);

	FForm MoveForm;
	MoveForm.Translation(ON_3dVector(-20, 20, -3));
	m_pProbe->DoTransform(MoveForm);
}

void ProbeVis::IniTo(F3dPoint AimPt)
{
	QString tStr = gAppPath + QString("/data/probe.stl");
	m_pProbe = FT::gImportSTLFile(tStr.toLocal8Bit().data());
	curPnt = AimPt;

	FForm RotaFormX;
	RotaFormX.Rotation(PI / 2, ON_3dVector(1, 0, 0), ON_3dPoint(0, 0, 0));
	m_pProbe->DoTransform(RotaFormX);

	FForm MoveForm;

	ON_3dVector VecToOrig = ON_3dVector(-20, 20, -3);

	ON_3dVector VecEs = VecToOrig + ON_3dVector(AimPt.x, AimPt.y, AimPt.z);

	MoveForm.Translation(VecEs);

	m_pProbe->DoTransform(MoveForm);
}

void ProbeVis::Draw(const FT::FRenderOpt& renderOpt)
{
	FForm tForm;
	F3dVector aVec = F3dVector(90, 90, 90) - F3dVector(90, 90, 90);
	tForm.Translation(aVec);
	m_pProbe->DoTransform(tForm);
	m_pProbe->Update();
	m_pProbe->Draw(renderOpt);
}

