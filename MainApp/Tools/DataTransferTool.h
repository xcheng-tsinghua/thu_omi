/*	
\brief	 File reader including data transformation from string to double, which is appied to transfer data from C# APP OpcUaThu
\author: Haoyu Jiang
\date    2020-9
*/

#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../External/fkernel/Include/FKernelDefines.h"
#include "MainApp\FGuideDataType.h"
#include "MainApp\MainDoc.h"
#include "MainApp\FMainWindow.h"
#include <sstream>
#include <Eigen\Dense>
#include "MainApp\FProbeComp.h"

using namespace std;

class CSDataTrans
{
public:
	CSDataTrans();
	~CSDataTrans();

	void txtFileReader(FMainDoc* m_ppDoc,int mode);   //txt file reader.If mode == 0, no compensation for calibration,if mode == 1, include compensation for upper sphere inspection;if mode == 2, include compensation for fixed angle inspection
	//double sci2db(const std::string &strSci);//transformation tool from string to double for string in scientific couting mode
	//double sci2dbEX(const std::string& strSci);
	void txtFileReader(QString name);   //file reader for importing inspection data
	bool txtFileReader(FMainDoc* m_ppDoc, int mode, double m_dd);

	// 导入测量数据，不进行误差补偿
	static void importMesData(QString _filename);

	void importTheoData(QString name);
	FProbeComp* pMc;

private:
	int num;
	double m_dd;
	double mes_x;
	double mes_y;
	double mes_z;

};