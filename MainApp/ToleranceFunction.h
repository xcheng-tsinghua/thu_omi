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

	static double Roundness(vector<gp_Pnt> Pntlist, FSurface* Cylindrical = NULL);//����Բ��

	static Devdata Diameter(vector<gp_Pnt> Pntlist, FSurface* Cylindrical = NULL);//����ֱ��

	// ������ƽ��֮��ľ���
	static Devdata Distance(vector<gp_Pnt> Baselist, vector<gp_Pnt> Sublist, int xMode = 0);

	//������Բ�����߾��룬xMode=0��ֱ�߾��룬xMode=1��X����룬xMode=2��Y����룬xMode=3��Z����룬����ΪԲ���ϵĵ�
	static Devdata Distance_AxisToAxis(vector<gp_Pnt> CylinPnts1, vector<gp_Pnt> CylinPnts2, FSurface* Cylinder1, FSurface* Cylinder2, int xMode = 0);

	//����Բ��������ƽ��ľ��룬xMode=0��ֱ�߾��룬xMode=1��X����룬xMode=2��Y����룬xMode=3��Z����룬����ΪԲ���ϵĵ�
	static double Distance_AxisToPlane(vector<gp_Pnt> _cylinPnts, vector<gp_Pnt> _planePnts, FSurface* _cylinSurf, FSurface* _planeSurf);

	// ����λ�öȣ�������, Ŀǰ�������ڵ�������Բ���Ͼ��ȷֲ��������
	// ƽ�棺���߷����ϵ�ƫ����
	// Բ��������ƫ����
	static double PositionalTolerance(vector<gp_Pnt> _surfPnts, FSurface* _surf);

	static double Parallelism(vector<gp_Pnt> Baselist, vector<gp_Pnt> Sublist);//����ƽ�жȣ���ƽ��֮��

	static double Parallelism_AxisToAxis(vector<gp_Pnt> AxisPntlist1, vector<gp_Pnt> AxisPntlist2, FSurface* Cylinder);//����ƽ�жȣ�Բ�����߶�Բ�����ߣ�����ΪԲ���ϵĵ�

	static double Verticality(vector<gp_Pnt> Baselist, vector<gp_Pnt> Sublist);//���㴹ֱ��

	static double Flatness(vector<gp_Pnt> Pntlist);//����ƽ���

	static double Verticality_PlanToAxis(vector<gp_Pnt> PlanPntlist, vector<gp_Pnt> AxisPntlist, FSurface* Cylinder);//���㴹ֱ�ȣ�ƽ���Բ�����ߣ���������ΪԲ���ϵĵ�

	static gp_Pln PntLocPlan(vector<gp_Pnt> Pntlist);//ʹ����С���˷���������ڵ�ƽ��

	static double Axiality(vector<gp_Pnt> CylinPnts1, vector<gp_Pnt> CylinPnts2, FSurface* Cylinder);//ͬ��ȼ��㣬ʹ�ù������߷� https://wenku.baidu.com/view/d93bfa0103d8ce2f006623b0.html ÿ��Բ�����Ϲ̶���Ȧ��㣬ÿȦ֮�����ټ��2mm��ÿȦ�����ͬһ��ƽ���ڡ�ͬһƽ���ڵĲ��ÿȦƽ��Ȳ�����1mm������ΪԲ���ϵĵ�

	static gp_Pnt CircleCenture(gp_Pln Realplan, vector<gp_Pnt> Pntlist);

	static gp_Lin LineFromPnts(vector<gp_Pnt> PntList);

	static void Circle2DFit(vector<gp_Pnt2d> PntList, double& CenX, double& CenY, double& Radius);

	static void CylinToCenr(vector<gp_Pnt> CylinPnts, gp_Pln ProjPln, vector<gp_Pnt>& CenrList);//����Բ����ÿȦԲ��Բ��λ�ã����кܶ�Ȧ��ÿȦ��ͬһ����ֱ��Բ�����ߵ�ƽ���ϣ���������ΪԲ���ϵĵ㣬CylinPntsΪ��Բ�����ߴ�ֱ����һƽ�棬��������Զ

	//static double MIN(vector<double> mylist)//����mylist����Сֵ
	//{
	//	double assistant = mylist[0];
	//	for (int i = 1; i < mylist.size(); i++)
	//	{
	//		if (mylist[i] < assistant)
	//			assistant = mylist[i];
	//	}
	//	return assistant;
	//}

	//static double MAX(vector<double> mylist)//����mylist�����ֵ
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

	// �� abs(Val) < Zero_Limit������ 0
	static double ZeroLim(double Val, double Zero_Limit = 1e-6);

};
