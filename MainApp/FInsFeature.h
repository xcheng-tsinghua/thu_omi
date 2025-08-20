#pragma once
#include<vector>
//#include<FEntity.h>
#include"../External/fkernel/Include/FEntity.h"

using namespace std;

enum InsType
{
	PARALLELISM, ROUNDNESS, DISTANCE, VERTICALITY, DIAMETER, AUXIALITY, FLATNESS, PROFILE, POSITION, UNKNOW
};

class FInsFeature//检测特征的InsFeatureindex作为唯一标识
{
public:
	FInsFeature() 
	{
		type = UNKNOW;
		InsFeatureindex = 0;
		FInsToleranceValue = 0;
		FInsUpDeviationValue = 0;
		FInsDownDeviationValue = 0;
		DirState = 0;
	};

	FInsFeature(vector<FSurface*> para_FSurfList1, vector<FSurface*> para_FSurfList2,
		InsType ToleranceType, double ToleranceValue, double para_UpDeviation, double para_DownDeviation, int DistDir,
		int index);
	~FInsFeature();

	//检测参数
	vector<FSurface*> m_FSurfList1;
	vector<FSurface*> m_FSurfList2;

	int InsFeatureindex;

	InsType type;

	double FInsToleranceValue;
	double FInsUpDeviationValue;
	double FInsDownDeviationValue;

	int DirState;
};
Q_DECLARE_METATYPE(FInsFeature)