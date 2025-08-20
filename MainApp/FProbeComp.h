/*	
\brief	 Compensation function for inspection process.Heritaged from old version ThuOMV.
\author: Haoyu Jiang
\date    2020-9
*/

#pragma once

#include <iostream>
#include <string>
#include <cassert>
#include "MainDoc.h"
#include "FMainWindow.h"
#include <vector>
#include <Windows.h>
#include <cstdio>
#include "qwidget.h"
#include "qdir.h"

using namespace std;

class FProbeComp
{

public:
	FProbeComp(FMainDoc* m_Doc);//Contructor
	~FProbeComp();//Deconstructor
	void errCompFixedAngle(int num);//Compensation for Class FAdjustAngleProbe
	void errCompUpperSphere(int num);//Compensation for Class FAdjustUpperSphere

private:
	bool IsMind;
	FMainDoc* m_ppDoc;
};