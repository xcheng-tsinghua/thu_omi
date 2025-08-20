/*
用于路径的显示
包含检测路径，刀具路径
*/
#pragma once
#include "../External/fkernel/Include/FEntity.h"
#include <vector>
#include "FGuideDataType.h"

using namespace std;

class PathShow : public FT::FEntity
{
public:
	PathShow();
	PathShow(vector<gp_Pnt> _pathPoints, const double& _r = 1, const double& _g = 0, const double& _b = 0, const double& _width = 2);
	~PathShow();

	void setPathPnts(const vector<gp_Pnt>& _pathPoints);
	void setRGB(const double& _r, const double& _g, const double& _b);
	void setLineWidth(double _width);

	void clearPathPoints();
	void appendPnt(const gp_Pnt& _pnt);

	void Draw(const FT::FRenderOpt& renderOpt);//绘图

private:
	vector<gp_Pnt> m_PathPoints;

	double m_R;
	double m_G;
	double m_B;

	double m_LineWidth;

};


