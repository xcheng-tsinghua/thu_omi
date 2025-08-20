#include "FInsFeature.h"
#include "FMainWindow.h"

extern FMainWindow* gWindow;

FInsFeature::FInsFeature(vector<FSurface*> para_FSurfList1, vector<FSurface*> para_FSurfList2,
	InsType ToleranceType, double ToleranceValue, double para_UpDeviation, double para_DownDeviation, int DistDir,
	int index)
{
	InsFeatureindex = index;

	type = ToleranceType;

	FInsToleranceValue = ToleranceValue;
	FInsUpDeviationValue = para_UpDeviation;
	FInsDownDeviationValue = para_DownDeviation;

	DirState = DistDir;

	m_FSurfList1 = para_FSurfList1;
	m_FSurfList2 = para_FSurfList2;

}

FInsFeature::~FInsFeature()
{

}

