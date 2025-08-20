#pragma once
#include <gp_Pln.hxx>
#include <math_Matrix.hxx>
#include <vector>
#include <qvector.h>
#include "../External/fkernel/Include/FEntity.h"
#include "np.h"

using namespace std;

struct Devdata
{
	double Value;
	double UpDeviation;
	double DownDeviation;
};

class ToleranceFunction
{
public:

	static double Roundness(vector<gp_Pnt> Pntlist, FSurface* Cylindrical = NULL);//计算圆度

	static Devdata Diameter(vector<gp_Pnt> Pntlist, FSurface* Cylindrical = NULL);//计算直径

	// 计算两平面之间的距离
	static Devdata Distance(vector<gp_Pnt> Baselist, vector<gp_Pnt> Sublist, int xMode = 0);

	//计算两圆柱轴线距离，xMode=0：直线距离，xMode=1：X向距离，xMode=2：Y向距离，xMode=3：Z向距离，输入为圆柱上的点
	static Devdata Distance_AxisToAxis(vector<gp_Pnt> CylinPnts1, vector<gp_Pnt> CylinPnts2, FSurface* Cylinder1, FSurface* Cylinder2, int xMode = 0);

	//计算圆柱轴线与平面的距离，xMode=0：直线距离，xMode=1：X向距离，xMode=2：Y向距离，xMode=3：Z向距离，输入为圆柱上的点
	static double Distance_AxisToPlane(vector<gp_Pnt> _cylinPnts, vector<gp_Pnt> _planePnts, FSurface* _cylinSurf, FSurface* _planeSurf);

	// 计算位置度（待完善, 目前仅适用于点在整个圆周上均匀分布的情况）
	// 平面：法线方向上的偏移量
	// 圆柱：轴线偏移量
	static double PositionalTolerance(vector<gp_Pnt> _surfPnts, FSurface* _surf);

	static double Parallelism(vector<gp_Pnt> Baselist, vector<gp_Pnt> Sublist);//计算平行度，两平面之间

	static double Parallelism_AxisToAxis(vector<gp_Pnt> AxisPntlist1, vector<gp_Pnt> AxisPntlist2, FSurface* Cylinder);//计算平行度，圆柱轴线对圆柱轴线，输入为圆柱上的点

	static double Verticality(vector<gp_Pnt> Baselist, vector<gp_Pnt> Sublist);//计算垂直度

	static double Flatness(vector<gp_Pnt> Pntlist);//计算平面度

	static double Verticality_PlanToAxis(vector<gp_Pnt> PlanPntlist, vector<gp_Pnt> AxisPntlist, FSurface* Cylinder);//计算垂直度，平面对圆柱轴线，轴线输入为圆柱上的点

	static gp_Pln PntLocPlan(vector<gp_Pnt> Pntlist);//使用最小二乘法计算点所在的平面

	static double Axiality(vector<gp_Pnt> CylinPnts1, vector<gp_Pnt> CylinPnts2, FSurface* Cylinder);//同轴度计算，使用公共轴线法 https://wenku.baidu.com/view/d93bfa0103d8ce2f006623b0.html 每个圆柱面上固定两圈测点，每圈之间最少间隔2mm，每圈测点在同一个平面内。同一平面内的测点每圈平面度不超多1mm，输入为圆柱上的点

	static gp_Pnt CircleCenture(gp_Pln Realplan, vector<gp_Pnt> Pntlist);

	static gp_Lin LineFromPnts(vector<gp_Pnt> PntList);

	static void Circle2DFit(vector<gp_Pnt2d> PntList, double& CenX, double& CenY, double& Radius);

	static void CylinToCenr(vector<gp_Pnt> CylinPnts, gp_Pln ProjPln, vector<gp_Pnt>& CenrList);//返回圆柱上每圈圆的圆心位置，可有很多圈，每圈在同一个垂直于圆柱轴线的平面上，轴线输入为圆柱上的点，CylinPnts为与圆柱轴线垂直的任一平面，最好离测点很远

	//static double MIN(vector<double> mylist)//计算mylist中最小值
	//{
	//	double assistant = mylist[0];
	//	for (int i = 1; i < mylist.size(); i++)
	//	{
	//		if (mylist[i] < assistant)
	//			assistant = mylist[i];
	//	}
	//	return assistant;
	//}

	//static double MAX(vector<double> mylist)//计算mylist中最大值
	//{
	//	double assistant = mylist[0];
	//	for (int i = 1; i < mylist.size(); i++)
	//	{
	//		if (mylist[i] > assistant)
	//			assistant = mylist[i];
	//	}
	//	return assistant;
	//}

private:
	static double DistanceOfListToPlane(vector<gp_Pnt> Pntlist, gp_Pln thePlane);

	static bool SearchNormPlaneOfCylinderAxis(FSurface* paraSurf, gp_Pln& paraNormPlane);

	// 若 abs(Val) < Zero_Limit，返回 0
	static double ZeroLim(double Val, double Zero_Limit = 1e-6);

};
