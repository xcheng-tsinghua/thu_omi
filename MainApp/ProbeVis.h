#pragma once
#include "../External/fkernel/Include/FKernelDefines.h"
#include "../External/fkernel/Include/FEntity.h"
#include "MainDoc.h"

class ProbeVis : public FT::FEntity
{
public:
	ProbeVis(FMainDoc* Doc);
	~ProbeVis();

	void Initial();
	void IniTo(F3dPoint AimPt);

	void Draw(const FT::FRenderOpt& renderOpt);//»æÍ¼

//protected:
	FMainDoc* m_pDoc;			///doc pointer
	FFreeBody* m_pProbe;	///probe model imported from hard disk
	F3dVector curPnt;
};