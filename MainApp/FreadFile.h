#ifndef F_READFILE_H
#define  F_READFILE_H
#pragma once

#include <iostream>
#include <fstream>
using namespace std;

#include <windows.h>
#include <math.h>
#include "../External/fkernel/Include/FKernelDefines.h"
#include "../External/fkernel/Include/FEntity.h"
#include "MainDoc.h"

using namespace FT;

class FMainDoc;

class FreadFile
{
public:
	void readFile(){ifstream fin("InspectionInfo3-22.txt");fin.getline(temp,num-1,0);fin.close();}
	void outTemp(){cout<<temp<<endl;};
	void readTemp();

	const static int num = 1000000;
	char temp[num];

	QVector<double> temporaryNum;

	struct PointData {F3dPoint Position;};
	QVector<PointData> m_PointData;
};

#endif