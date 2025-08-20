#include "ToleranceFunction.h"
#include <ProjLib.hxx>
#include <gp_Pnt2d.hxx>
#include "FMainWindow.h"
#include "occConverter.h"
#include "math_Matrix.hxx"
#include <IntAna_IntConicQuad.hxx>
#include <math_Gauss.hxx>
#include <set>
#include <functional>
#include <BRep_Tool.hxx>
#include <Eigen/Dense>

extern FMainWindow* gWindow;

double ToleranceFunction::Roundness(vector<gp_Pnt> Pntlist, FSurface* Cylindrical)
{
	//利用最小二乘法获取测点所在平面
	gp_Pln Realplan = PntLocPlan(Pntlist);

	if (Cylindrical)
	{

		if (!ToleranceFunction::SearchNormPlaneOfCylinderAxis(Cylindrical, Realplan))
		{
			cout << "未发现柱面对应的曲面" << endl;
			goto CylinNF;
		}

	}
	else
	{
		CylinNF:

		///////////////////////////////////////////////////////未指定圆柱面并圆柱面上采点时，默认圆柱轴线是x、y、z向其中之一，如果不是，请修改基准面代码
		double MeanX = 0;
		double MeanY = 0;
		double MeanZ = 0;

		for (int i = 0; i < Pntlist.size(); i++)
		{
			MeanX += Pntlist[i].X();
			MeanY += Pntlist[i].Y();
			MeanZ += Pntlist[i].Z();
		}
		MeanX /= Pntlist.size();
		MeanY /= Pntlist.size();
		MeanZ /= Pntlist.size();

		double DistX = 0;
		double DistY = 0;
		double DistZ = 0;

		gp_Pln PlanX(gp_Pnt(MeanX, MeanY, MeanZ), gp_Dir(1, 0, 0));
		gp_Pln PlanY(gp_Pnt(MeanX, MeanY, MeanZ), gp_Dir(0, 1, 0));
		gp_Pln PlanZ(gp_Pnt(MeanX, MeanY, MeanZ), gp_Dir(0, 0, 1));

		for (int i = 0; i < Pntlist.size(); i++)
		{
			DistX += PlanX.Distance(Pntlist[i]);
			DistY += PlanY.Distance(Pntlist[i]);
			DistZ += PlanZ.Distance(Pntlist[i]);
		}

		if (DistX <= DistY && DistX <= DistZ)
		{
			Realplan = PlanX;
		}
		else if (DistY <= DistX && DistY <= DistZ)
		{
			Realplan = PlanY;
		}
		else
		{
			Realplan = PlanZ;
		}
	}

	//将三维点映射到测点拟合面的二维点
	vector<gp_Pnt2d> m_points;
	for (int i = 0; i < Pntlist.size(); i++)
	{
		gp_Pnt2d tmpPnt = ProjLib::Project(Realplan, Pntlist[i]);
		m_points.push_back(tmpPnt);
	}
	//利用最小二乘法计算圆心位置及直径，算法参考网站 https://blog.csdn.net/Jacky_Ponder/article/details/70314919

	double X1 = 0;
	double Y1 = 0;
	double X2 = 0;
	double Y2 = 0;
	double X3 = 0;
	double Y3 = 0;
	double X1Y1 = 0;
	double X1Y2 = 0;
	double X2Y1 = 0;
	int m_nNum = m_points.size();

	for (int i = 0; i < m_nNum; i++)
	{
		X1 = X1 + m_points[i].X();
		Y1 = Y1 + m_points[i].Y();
		X2 = X2 + m_points[i].X() * m_points[i].X();
		Y2 = Y2 + m_points[i].Y() * m_points[i].Y();
		X3 = X3 + m_points[i].X() * m_points[i].X() * m_points[i].X();
		Y3 = Y3 + m_points[i].Y() * m_points[i].Y() * m_points[i].Y();
		X1Y1 = X1Y1 + m_points[i].X() * m_points[i].Y();
		X1Y2 = X1Y2 + m_points[i].X() * m_points[i].Y() * m_points[i].Y();
		X2Y1 = X2Y1 + m_points[i].X() * m_points[i].X() * m_points[i].Y();
	}

	double C, D, E, G, H, N;
	double a, b, c;
	N = m_nNum;
	C = N * X2 - X1 * X1;
	D = N * X1Y1 - X1 * Y1;
	E = N * X3 + N * X1Y2 - (X2 + Y2) * X1;
	G = N * Y2 - Y1 * Y1;
	H = N * X2Y1 + N * Y3 - (X2 + Y2) * Y1;
	a = (H * D - E * G) / (C * G - D * D);
	b = (H * C - E * D) / (D * D - G * C);
	c = -(a * X1 + b * Y1 + X2 + Y2) / N;

	double A, B, R;
	A = a / (-2);
	B = b / (-2);
	R = sqrt(a * a + b * b - 4 * c) / 2;

	double m_fCenterX = A;
	double m_fCenterY = B;
	double m_fRadius = R;
	//计算转换后的二维点到圆心距离的最大最小值
	vector<double> distance;
	for (int i = 0; i < m_nNum; i++)
	{
		distance.push_back(sqrt(pow(m_points[i].X() - A, 2) + pow(m_points[i].Y() - B, 2)));
	}
	
	return MAX(distance) - MIN(distance);
}

Devdata ToleranceFunction::Diameter(vector<gp_Pnt> Pntlist, FSurface* Cylindrical)
{
	//利用最小二乘法获取测点所在平面
	gp_Pln Realplan = PntLocPlan(Pntlist);

	if (Cylindrical)
	{
		if (!ToleranceFunction::SearchNormPlaneOfCylinderAxis(Cylindrical, Realplan))
		{
			cout << "未发现柱面对应的曲面" << endl;
			goto CylinNF;
		}
	}
	else
	{
	CylinNF:

		///////////////////////////////////////////////////////未指定圆柱面并圆柱面上采点时，默认圆柱轴线是x、y、z向其中之一，如果不是，请修改基准面代码
		double MeanX = 0;
		double MeanY = 0;
		double MeanZ = 0;

		for (int i = 0; i < Pntlist.size(); i++)
		{
			MeanX += Pntlist[i].X();
			MeanY += Pntlist[i].Y();
			MeanZ += Pntlist[i].Z();
		}
		MeanX /= Pntlist.size();
		MeanY /= Pntlist.size();
		MeanZ /= Pntlist.size();

		double DistX = 0;
		double DistY = 0;
		double DistZ = 0;

		gp_Pln PlanX(gp_Pnt(MeanX, MeanY, MeanZ), gp_Dir(1, 0, 0));
		gp_Pln PlanY(gp_Pnt(MeanX, MeanY, MeanZ), gp_Dir(0, 1, 0));
		gp_Pln PlanZ(gp_Pnt(MeanX, MeanY, MeanZ), gp_Dir(0, 0, 1));

		for (int i = 0; i < Pntlist.size(); i++)
		{
			DistX += PlanX.Distance(Pntlist[i]);
			DistY += PlanY.Distance(Pntlist[i]);
			DistZ += PlanZ.Distance(Pntlist[i]);
		}

		if (DistX <= DistY && DistX <= DistZ)
		{
			Realplan = PlanX;
		}
		else if (DistY <= DistX && DistY <= DistZ)
		{
			Realplan = PlanY;
		}
		else
		{
			Realplan = PlanZ;
		}
	}

	//将三维点映射到测点拟合面的二维点
	vector<gp_Pnt2d> m_points;
	for (int i = 0; i < Pntlist.size(); i++)
	{
		gp_Pnt2d tmpPnt = ProjLib::Project(Realplan, Pntlist[i]);
		m_points.push_back(tmpPnt);
	}
	//利用最小二乘法计算圆心位置及直径，算法参考网站 https://blog.csdn.net/Jacky_Ponder/article/details/70314919

	double X1 = 0;
	double Y1 = 0;
	double X2 = 0;
	double Y2 = 0;
	double X3 = 0;
	double Y3 = 0;
	double X1Y1 = 0;
	double X1Y2 = 0;
	double X2Y1 = 0;
	int m_nNum = m_points.size();

	for (int i = 0; i < m_nNum; i++)
	{
		X1 = X1 + m_points[i].X();
		Y1 = Y1 + m_points[i].Y();
		X2 = X2 + m_points[i].X() * m_points[i].X();
		Y2 = Y2 + m_points[i].Y() * m_points[i].Y();
		X3 = X3 + m_points[i].X() * m_points[i].X() * m_points[i].X();
		Y3 = Y3 + m_points[i].Y() * m_points[i].Y() * m_points[i].Y();
		X1Y1 = X1Y1 + m_points[i].X() * m_points[i].Y();
		X1Y2 = X1Y2 + m_points[i].X() * m_points[i].Y() * m_points[i].Y();
		X2Y1 = X2Y1 + m_points[i].X() * m_points[i].X() * m_points[i].Y();
	}

	double C, D, E, G, H, N;
	double a, b, c;
	N = m_nNum;
	C = N * X2 - X1 * X1;
	D = N * X1Y1 - X1 * Y1;
	E = N * X3 + N * X1Y2 - (X2 + Y2) * X1;
	G = N * Y2 - Y1 * Y1;
	H = N * X2Y1 + N * Y3 - (X2 + Y2) * Y1;
	a = (H * D - E * G) / (C * G - D * D);
	b = (H * C - E * D) / (D * D - G * C);
	c = -(a * X1 + b * Y1 + X2 + Y2) / N;

	double A, B, R;
	A = a / (-2);
	B = b / (-2);
	R = sqrt(a * a + b * b - 4 * c) / 2;

	double m_fCenterX = A;
	double m_fCenterY = B;
	double m_fRadius = R;
	//计算转换后的二维点到圆心距离的最大最小值
	vector<double> distance;
	for (int i = 0; i < m_nNum; i++)
	{
		distance.push_back(sqrt(pow(m_points[i].X() - A, 2) + pow(m_points[i].Y() - B, 2)));
	}


	Devdata Diameter;
	Diameter.Value = 2 * m_fRadius;
	Diameter.DownDeviation = 2 * (MIN(distance) - R);
	Diameter.UpDeviation = 2 * (MAX(distance) - R);
	return Diameter;
}

Devdata ToleranceFunction::Distance(vector<gp_Pnt> Baselist, vector<gp_Pnt> Sublist, int xMode)
{
	//利用最小二乘法获取测点所在平面
	gp_Pln BasePlan = PntLocPlan(Baselist);

	//最小二乘法获取目标点所在平面
	double A = BasePlan.Axis().Direction().X();
	double B = BasePlan.Axis().Direction().Y();
	double C = BasePlan.Axis().Direction().Z();

	double D = 0;
	for (int i = 0; i < Sublist.size(); i++)
	{
		D += (A * Sublist[i].X() + B * Sublist[i].Y() + C * Sublist[i].Z());
	}
	D /= Sublist.size();
	D = -D;

	double RealDise = abs(D + A * BasePlan.Location().X() + B * BasePlan.Location().Y() + C * BasePlan.Location().Z()) / sqrt(A * A + B * B + C * C);

	vector<double> allDistn;
	double DistnSum = 0;
	for (int i = 0; i < Sublist.size(); i++)
	{
		allDistn.push_back(BasePlan.Distance(Sublist[i]));
		DistnSum += BasePlan.Distance(Sublist[i]);
	}

	double CosinDir = 0;
	switch (xMode)
	{
	case 0:
		CosinDir = 1;
		break;
	case 1:
		CosinDir = abs(A) / sqrt(A * A + B * B + C * C);
		break;
	case 2:
		CosinDir = abs(B) / sqrt(A * A + B * B + C * C);
		break;
	case 3:
		CosinDir = abs(C) / sqrt(A * A + B * B + C * C);
		break;
	default:
		break;
	}

	Devdata Distance;
	Distance.Value = RealDise * CosinDir;
	Distance.UpDeviation = (MAX(allDistn) - RealDise) * CosinDir;
	Distance.DownDeviation = (MIN(allDistn) - RealDise) * CosinDir;

	return Distance;
}

Devdata ToleranceFunction::Distance_AxisToAxis(vector<gp_Pnt> CylinPnts1, vector<gp_Pnt> CylinPnts2, FSurface* Cylinder1, FSurface* Cylinder2, int xMode)
{
	Devdata Res;

	//第一步，提取投影平面，与理论圆柱轴线垂直的任一平面均可
	gp_Pln ProjPln1;

	//找到Cylindrical对应的柱面轴线法平面

	if (!ToleranceFunction::SearchNormPlaneOfCylinderAxis(Cylinder1, ProjPln1))
	{
		cout << "未发现柱面对应的曲面" << endl;
		QMessageBox::warning(0, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("未发现柱面对应的曲面"), "OK");
		return Res;
	}

	//////////////////////////////////////////////////////////////
	gp_Pln ProjPln2;

	if (!ToleranceFunction::SearchNormPlaneOfCylinderAxis(Cylinder2, ProjPln2))
	{
		cout << "未发现柱面对应的曲面" << endl;
		QMessageBox::warning(0, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("未发现柱面对应的曲面"), "OK");
		return Res;
	}

	vector<gp_Pnt> CenPnt1;
	ToleranceFunction::CylinToCenr(CylinPnts1, ProjPln1, CenPnt1);

	vector<gp_Pnt> CenPnt2;
	ToleranceFunction::CylinToCenr(CylinPnts2, ProjPln2, CenPnt2);

	// 将所有点投影到与圆柱面垂直的平面上，得到二维点
	vector<gp_Pnt2d> CenPnt2D_1;
	vector<gp_Pnt2d> CenPnt2D_2;

	for (auto ita = CenPnt1.begin(); ita != CenPnt1.end(); ++ita)
	{
		CenPnt2D_1.push_back(ProjLib::Project(ProjPln1, (*ita)));
	}

	for (auto ita = CenPnt2.begin(); ita != CenPnt2.end(); ++ita)
	{
		CenPnt2D_2.push_back(ProjLib::Project(ProjPln2, (*ita)));
	}
	
	// 计算每组二维点的中心点
	double Cen2d_1X = 0.0, Cen2d_1Y = 0.0, Cen2d_2X = 0.0, Cen2d_2Y = 0.0;

	for (auto ita = CenPnt2D_1.begin(); ita != CenPnt2D_1.end(); ++ita)
	{
		Cen2d_1X += (*ita).X();
		Cen2d_1Y += (*ita).Y();
	}

	Cen2d_1X /= CenPnt2D_1.size();
	Cen2d_1Y /= CenPnt2D_1.size();

	for (auto ita = CenPnt2D_2.begin(); ita != CenPnt2D_2.end(); ++ita)
	{
		Cen2d_2X += (*ita).X();
		Cen2d_2Y += (*ita).Y();
	}

	Cen2d_2X /= CenPnt2D_2.size();
	Cen2d_2Y /= CenPnt2D_2.size();

	// 距离实际值即为两个中心点之间的距离
	Res.Value = sqrt(pow(Cen2d_1X - Cen2d_2X, 2) + pow(Cen2d_1Y - Cen2d_2Y, 2));

	// 计算上下偏差，上下偏差为垂直于中心连线的两条辅助直线之间（分别经过第一组测点和第二组测点，且分别位于最左侧和最右侧）的距离
	// 中心连线方向
	double CenDir_X = Cen2d_2X - Cen2d_1X;
	double CenDir_Y = Cen2d_2Y - Cen2d_1Y;

	// 辅助直线方向向量（与中心连线方向垂直）
	double AuxDir_X = CenDir_Y;
	double AuxDir_Y = -CenDir_X;

	// 直线方程 Ax + By + C = 0，C = -Ax -By，-B = AuxDir_X，A = AuxDir_Y
	// 第一组的 C
	vector<double> D1_Val;
	for (auto ita = CenPnt2D_1.begin(); ita != CenPnt2D_1.end(); ++ita)
	{
		D1_Val.push_back(-AuxDir_Y * (*ita).X() + AuxDir_X * (*ita).Y());
	}
	sort(D1_Val.begin(), D1_Val.end());

	// 第二组的 C
	vector<double> D2_Val;
	for (auto ita = CenPnt2D_2.begin(); ita != CenPnt2D_2.end(); ++ita)
	{
		D2_Val.push_back(-AuxDir_Y * (*ita).X() + AuxDir_X * (*ita).Y());
	}
	sort(D2_Val.begin(), D2_Val.end());

	// 计算四条直线距离的最大最小值
	vector<double> ExtremeDis;

	double ValD1_1 = D1_Val[0];
	double ValD1_2 = D1_Val[D1_Val.size() - 1];

	double ValD2_1 = D2_Val[0];
	double ValD2_2 = D2_Val[D2_Val.size() - 1];

	double DisCalDown = sqrt(AuxDir_Y * AuxDir_Y + AuxDir_X * AuxDir_X);

	ExtremeDis.push_back(abs(ValD1_1 - ValD2_1) / DisCalDown);
	ExtremeDis.push_back(abs(ValD1_1 - ValD2_2) / DisCalDown);
	ExtremeDis.push_back(abs(ValD1_2 - ValD2_1) / DisCalDown);
	ExtremeDis.push_back(abs(ValD1_2 - ValD2_2) / DisCalDown);

	sort(ExtremeDis.begin(), ExtremeDis.end());

	Res.UpDeviation = ExtremeDis[ExtremeDis.size() - 1] - Res.Value;
	Res.DownDeviation = ExtremeDis[0] - Res.Value;

	return Res;


	//double tmpX = 0, tmpY = 0, tmpZ = 0;

	//for (int i = 0; i < CenPnt1.size(); i++)
	//{
	//	tmpX += CenPnt1[i].X();
	//	tmpY += CenPnt1[i].Y();
	//	tmpZ += CenPnt1[i].Z();
	//}
	//tmpX /= CenPnt1.size();
	//tmpY /= CenPnt1.size();
	//tmpZ /= CenPnt1.size();

	//gp_Lin theAx1 = gp_Lin(gp_Pnt(tmpX, tmpY, tmpZ), ProjPln1.Axis().Direction());

	//tmpX = 0;
	//tmpY = 0;
	//tmpZ = 0;

	//for (int i = 0; i < CenPnt2.size(); i++)
	//{
	//	tmpX += CenPnt2[i].X();
	//	tmpY += CenPnt2[i].Y();
	//	tmpZ += CenPnt2[i].Z();
	//}
	//tmpX /= CenPnt2.size();
	//tmpY /= CenPnt2.size();
	//tmpZ /= CenPnt2.size();

	//gp_Lin theAx2 = gp_Lin(gp_Pnt(tmpX, tmpY, tmpZ), ProjPln2.Axis().Direction());

	//gp_Pnt Orig1 = theAx1.Location();
	//gp_Dir Dir = theAx1.Direction();
	//gp_Pnt Orig2 = theAx2.Location();

	////轴线
	//double ParaT1 = -(Dir.X() * Orig1.X() + Dir.Y() * Orig1.Y() + Dir.Z() * Orig1.Z()) / (Dir.X() * Dir.X() + Dir.Y() * Dir.Y() + Dir.Z() * Dir.Z());
	//double ParaT2 = -(Dir.X() * Orig2.X() + Dir.Y() * Orig2.Y() + Dir.Z() * Orig2.Z()) / (Dir.X() * Dir.X() + Dir.Y() * Dir.Y() + Dir.Z() * Dir.Z());

	//gp_Pnt InsecPnt1 = gp_Pnt(Orig1.X() + ParaT1 * Dir.X(), Orig1.Y() + ParaT1 * Dir.Y(), Orig1.Z() + ParaT1 * Dir.Z());
	//gp_Pnt InsecPnt2 = gp_Pnt(Orig2.X() + ParaT2 * Dir.X(), Orig2.Y() + ParaT2 * Dir.Y(), Orig2.Z() + ParaT2 * Dir.Z());

	////其他点
	//vector<gp_Pnt> InsecBurPnt1;
	//for (int i = 0; i < CenPnt1.size(); i++)
	//{
	//	double tmpPara = -(Dir.X() * CenPnt1[i].X() + Dir.Y() * CenPnt1[i].Y() + Dir.Z() * CenPnt1[i].Z()) / (Dir.X() * Dir.X() + Dir.Y() * Dir.Y() + Dir.Z() * Dir.Z());
	//	InsecBurPnt1.push_back(gp_Pnt(CenPnt1[i].X() + tmpPara * Dir.X(), CenPnt1[i].Y() + tmpPara * Dir.Y(), CenPnt1[i].Z() + tmpPara * Dir.Z()));
	//}
	//vector<gp_Pnt> InsecBurPnt2;
	//for (int i = 0; i < CenPnt2.size(); i++)
	//{
	//	double tmpPara = -(Dir.X() * CenPnt2[i].X() + Dir.Y() * CenPnt2[i].Y() + Dir.Z() * CenPnt2[i].Z()) / (Dir.X() * Dir.X() + Dir.Y() * Dir.Y() + Dir.Z() * Dir.Z());
	//	InsecBurPnt2.push_back(gp_Pnt(CenPnt2[i].X() + tmpPara * Dir.X(), CenPnt2[i].Y() + tmpPara * Dir.Y(), CenPnt2[i].Z() + tmpPara * Dir.Z()));
	//}

	//switch (xMode)
	//{
	//	case 0://直线距离
	//	{
	//		Res.Value = theAx1.Distance(theAx2);

	//		vector<double>Devia;

	//		for (int i = 0; i < CenPnt1.size(); i++)
	//		{
	//			Devia.push_back(theAx1.Distance(CenPnt1[i]));
	//		}
	//		for (int i = 0; i < CenPnt2.size(); i++)
	//		{
	//			Devia.push_back(theAx2.Distance(CenPnt2[i]));
	//		}

	//		Res.UpDeviation = 2 * MAX(Devia);
	//		Res.DownDeviation = -2 * MAX(Devia);
	//		return Res;

	//		break;
	//	}
	//	case 1 ://X向距离
	//	{
	//		Res.Value = abs(InsecPnt1.X() - InsecPnt2.X());

	//		vector<double>Devia;

	//		for (int i = 0; i < InsecBurPnt1.size(); i++)
	//		{
	//			Devia.push_back(abs(InsecPnt1.X() - InsecBurPnt1[i].X()));
	//		}
	//		for (int i = 0; i < InsecBurPnt2.size(); i++)
	//		{
	//			Devia.push_back(abs(InsecPnt2.X() - InsecBurPnt2[i].X()));
	//		}

	//		Res.UpDeviation = Res.DownDeviation = 2 * MAX(Devia);

	//		Res.UpDeviation = 2 * MAX(Devia);
	//		Res.DownDeviation = -2 * MAX(Devia);

	//		return Res;

	//		break;
	//	}
	//	case 2://Y向距离
	//	{
	//		Res.Value = abs(InsecPnt1.Y() - InsecPnt2.Y());

	//		vector<double>Devia;

	//		for (int i = 0; i < InsecBurPnt1.size(); i++)
	//		{
	//			Devia.push_back(abs(InsecPnt1.Y() - InsecBurPnt1[i].Y()));
	//		}
	//		for (int i = 0; i < InsecBurPnt2.size(); i++)
	//		{
	//			Devia.push_back(abs(InsecPnt2.Y() - InsecBurPnt2[i].Y()));
	//		}

	//		Res.UpDeviation = 2 * MAX(Devia);
	//		Res.DownDeviation = -2 * MAX(Devia);

	//		return Res;

	//		break;
	//	}
	//	case 3://Z向线距离
	//	{
	//		Res.Value = abs(InsecPnt1.Z() - InsecPnt2.Z());

	//		vector<double>Devia;

	//		for (int i = 0; i < InsecBurPnt1.size(); i++)
	//		{
	//			Devia.push_back(abs(InsecPnt1.Z() - InsecBurPnt1[i].Z()));
	//		}
	//		for (int i = 0; i < InsecBurPnt2.size(); i++)
	//		{
	//			Devia.push_back(abs(InsecPnt2.Z() - InsecBurPnt2[i].Z()));
	//		}

	//		Res.UpDeviation = 2 * MAX(Devia);
	//		Res.DownDeviation = -2 * MAX(Devia);

	//		return Res;

	//		break;
	//	}
	//}


}

double ToleranceFunction::Distance_AxisToPlane(vector<gp_Pnt> _cylinPnts, vector<gp_Pnt> _planePnts, FSurface* _cylinSurf, FSurface* _planeSurf)
{
	// 提取圆柱轴线方向
	TopoDS_Face cylinFace = gWindow->GetOccConverter()->FSurf2TDSFace(_cylinSurf);
	Handle(Geom_Surface) cylinSurface = BRep_Tool::Surface(cylinFace);
	Handle(Geom_CylindricalSurface) geomCylin = Handle(Geom_CylindricalSurface)::DownCast(cylinSurface);

	if (!geomCylin)
	{
		MesgBox2("曲面选择错误！");
		return -1;
	}

	gp_Vec cylinDir = geomCylin->Axis().Direction();

	// 提取平面轴线方向
	TopoDS_Face planeFace = gWindow->GetOccConverter()->FSurf2TDSFace(_planeSurf);
	Handle(Geom_Surface) planeSurface = BRep_Tool::Surface(planeFace);
	Handle(Geom_Plane) geomPln = Handle(Geom_Plane)::DownCast(planeSurface);

	if (!geomPln)
	{
		MesgBox2("平面选择错误！");
		return -1;
	}

	gp_Vec plnNorm = geomPln->Pln().Axis().Direction();

	// 检查平面与圆柱轴线是否平行
	if (!plnNorm.IsParallel(cylinDir, Precision::Angular()))
	{
		MesgBox2("选择的平面和圆柱轴线不平行！");
		return -1;
	}

	// 拟合圆柱轴线上的一点
	gp_Pnt centroid = massCenter(_cylinPnts);

	// 拟合平面
	double a = plnNorm.X();
	double b = plnNorm.Y();
	double c = plnNorm.Z();

	double sumD = 0.0;
	int nPntSize = _planePnts.size();
	for (int i = 0; i < nPntSize; i++)
	{
		double x = _planePnts[i].X();
		double y = _planePnts[i].Y();
		double z = _planePnts[i].Z();
		sumD += -(a * x + b * y + c * z);
	}
	double d = sumD / nPntSize;

	gp_Pln fitPln = gp_Pln(a, b, c, d);

	return fitPln.Distance(centroid);
}

double ToleranceFunction::PositionalTolerance(vector<gp_Pnt> _surfPnts, FSurface* _surf)
{
	// 先判断是平面还是圆柱
	SURFACE_TYPE ctype = gWindow->GetOccConverter()->FSurfType(_surf);
	gp_Pnt mass = massCenter(_surfPnts);
	double res = -1.;

	TopoDS_Face aFace = gWindow->GetOccConverter()->FSurf2TDSFace(_surf);
	Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aFace);

	switch (ctype)
	{
	case BSPLINE:
		return 0.;
		break;
	case CONE:
		return 0.;
		break;
	case CYLINDER:
	{
		Handle(Geom_CylindricalSurface) geomCylin = Handle(Geom_CylindricalSurface)::DownCast(aSurface);
		res = gp_Lin(geomCylin->Axis()).Distance(mass);

		break;
	}
	case PLANE:
	{
		Handle(Geom_Plane) geomPlane = Handle(Geom_Plane)::DownCast(aSurface);
		res = geomPlane->Pln().Distance(mass);

		break;
	}
	case SPHERE:
		return 0.;
		break;
	case OTHER_SURF:
		return 0.;
		break;
	default:
		return 0.;
		break;
	}

	return res;

}

double ToleranceFunction::Parallelism(vector<gp_Pnt> Baselist, vector<gp_Pnt> Sublist)
{
	gp_Pln BasePln = PntLocPlan(Baselist);

	double A = BasePln.Axis().Direction().X();
	double B = BasePln.Axis().Direction().Y();
	double C = BasePln.Axis().Direction().Z();

	vector<double> D;
	for (int i = 0; i < Sublist.size(); i++)
	{
		D.push_back(A * Sublist[i].X() + B * Sublist[i].Y() + C * Sublist[i].Z());
	}
	double D_a = MAX(D);
	double D_i = MIN(D);

	double Tolerance = abs(D_a - D_i) / sqrt(A * A + B * B + C * C);

	return Tolerance;
}

double ToleranceFunction::Parallelism_AxisToAxis(vector<gp_Pnt> AxisPntlist1, vector<gp_Pnt> AxisPntlist2, FSurface* Cylinder)
{
	//第一步，提取投影平面，与理论圆柱轴线垂直的任一平面均可
	gp_Pln ProjPln;

	if (!ToleranceFunction::SearchNormPlaneOfCylinderAxis(Cylinder, ProjPln))
	{
		cout << "未发现柱面对应的曲面" << endl;
		QMessageBox::warning(0, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("未发现柱面对应的曲面"), "OK");
		return 0.0;
	}

	vector<gp_Pnt> CenPnt1;
	ToleranceFunction::CylinToCenr(AxisPntlist1, ProjPln, CenPnt1);

	vector<gp_Pnt> CenPnt2;
	ToleranceFunction::CylinToCenr(AxisPntlist2, ProjPln, CenPnt2);

	double tmpX = 0, tmpY = 0, tmpZ = 0;

	for (int i = 0; i < CenPnt1.size(); i++)
	{
		tmpX += CenPnt1[i].X();
		tmpY += CenPnt1[i].Y();
		tmpZ += CenPnt1[i].Z();
	}
	tmpX /= CenPnt1.size();
	tmpY /= CenPnt1.size();
	tmpZ /= CenPnt1.size();

	gp_Lin theAx1 = gp_Lin(gp_Pnt(tmpX, tmpY, tmpZ), ProjPln.Axis().Direction());

	tmpX = 0;
	tmpY = 0;
	tmpZ = 0;

	for (int i = 0; i < CenPnt2.size(); i++)
	{
		tmpX += CenPnt2[i].X();
		tmpY += CenPnt2[i].Y();
		tmpZ += CenPnt2[i].Z();
	}
	tmpX /= CenPnt2.size();
	tmpY /= CenPnt2.size();
	tmpZ /= CenPnt2.size();

	gp_Lin theAx2 = gp_Lin(gp_Pnt(tmpX, tmpY, tmpZ), ProjPln.Axis().Direction());

	vector<double>Devia;

	for (int i = 0; i < CenPnt1.size(); i++)
	{
		Devia.push_back(theAx1.Distance(CenPnt1[i]));
	}
	for (int i = 0; i < CenPnt2.size(); i++)
	{
		Devia.push_back(theAx2.Distance(CenPnt2[i]));
	}

	return 2 * MAX(Devia);
}

double ToleranceFunction::Verticality(vector<gp_Pnt> Baselist, vector<gp_Pnt> Sublist)
{
	//先找到基准所在平面，然后将目标点向基准面投影，投影后的点拟合直线，两条直线间距离即为公差值
	double VerTolce1 = 0;

	double N = Sublist.size();

	gp_Pln BasePln = PntLocPlan(Baselist);
	vector<gp_Pnt2d> PntTrans;
	for (int i = 0; i < Sublist.size(); i++)
	{
		gp_Pnt2d tmpPnt = ProjLib::Project(BasePln, Sublist[i]);
		PntTrans.push_back(tmpPnt);
	}
	
	double XX = 0, Y_ = 0, X_ = 0, XY = 0;


	for (int i = 0; i < PntTrans.size(); i++)
	{
		XX += PntTrans[i].X() * PntTrans[i].X();
		Y_ += PntTrans[i].Y();
		XY += PntTrans[i].X() * PntTrans[i].Y();
		X_ += PntTrans[i].X();
	}

	math_Matrix Left(0, 1, 0, 1);
	Left(0, 0) = XX;
	Left(0, 1) = X_;
	Left(1, 0) = X_;
	Left(1, 1) = N;

	double DeterDelt = Left.Determinant();

	double a = 0, b = 0;

	if (abs(DeterDelt)>1e-5)// 判断分母不为0  
	{
		math_Matrix LeA(0, 1, 0, 1);
		LeA(0, 0) = XY;
		LeA(0, 1) = X_;
		LeA(1, 0) = Y_;
		LeA(1, 1) = N;

		math_Matrix LeB(0, 1, 0, 1);
		LeB(0, 0) = XX;
		LeB(0, 1) = XY;
		LeB(1, 0) = X_;
		LeB(1, 1) = Y_;

		a = LeA.Determinant() / DeterDelt;
		b = LeB.Determinant() / DeterDelt;

		vector<double> bDev;
		for (int i = 0; i < PntTrans.size(); i++)
		{
			bDev.push_back(PntTrans[i].Y() - a * PntTrans[i].X()-b);
		}

		double bDevMax = MAX(bDev);
		double dDevMin = MIN(bDev);

		VerTolce1 = abs(bDevMax - dDevMin) / sqrt(1 + a * a);
	}
	else//直线垂直于x轴
	{
		vector<double> bDev;
		for (int i = 0; i < PntTrans.size(); i++)
		{
			bDev.push_back(PntTrans[i].X());
		}

		double bDevMax = MAX(bDev);
		double dDevMin = MIN(bDev);

		VerTolce1 = abs(bDevMax - dDevMin);
	}

	return VerTolce1;
}

double ToleranceFunction::Flatness(vector<gp_Pnt> Pntlist)
{
	gp_Pln theFitPlan = PntLocPlan(Pntlist);

	vector<double> PlaneD;
	double A = theFitPlan.Axis().Direction().X();
	double B = theFitPlan.Axis().Direction().Y();
	double C = theFitPlan.Axis().Direction().Z();

	for (int i = 0; i < Pntlist.size(); i++)
	{
		PlaneD.push_back(-(A * Pntlist[i].X() + B * Pntlist[i].Y() + C * Pntlist[i].Z()));
	}

	double D_MAX = MAX(PlaneD);
	double D_MIN = MIN(PlaneD);

	double CalRes = abs(D_MAX - D_MIN) / (A * A + B * B + C * C);

	return CalRes;
}

double ToleranceFunction::Verticality_PlanToAxis(vector<gp_Pnt> PlanPntlist, vector<gp_Pnt> AxisPntlist, FSurface* Cylinder)
{
	gp_Pln theFitPlan = PntLocPlan(PlanPntlist);

	gp_Pln ProjPln;

	//找到Cylindrical对应的柱面轴线法平面
	if (!ToleranceFunction::SearchNormPlaneOfCylinderAxis(Cylinder, ProjPln))
	{
		cout << "未发现柱面对应的曲面" << endl;
		QMessageBox::warning(0, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("未发现柱面对应的曲面"), "OK");
		return 0.0;
	}

	vector<gp_Pnt> theCyCen;
	ToleranceFunction::CylinToCenr(AxisPntlist, ProjPln, theCyCen);
	//ToleranceFunction::CylinToCenr(AxisPntlist, theFitPlan, theCyCen);

	vector<gp_Pnt2d> m_points;
	for (int i = 0; i < theCyCen.size(); i++)
	{
		gp_Pnt2d tmpPnt = ProjLib::Project(theFitPlan, theCyCen[i]);
		m_points.push_back(tmpPnt);
	}

	double cirX = 0, cirY = 0, radius = 0;

	vector<double> theDisList;
	for (int i = 0; i < m_points.size(); i++)
	{
		for (int j = 0; j < m_points.size(); j++)
		{
			theDisList.push_back(m_points[i].Distance(m_points[j]));
		}
	}

	return MAX(theDisList);
}

gp_Pln ToleranceFunction::PntLocPlan(vector<gp_Pnt> Pntlist)//使用最小二乘法计算点所在的平面
{
	//应该添加判断列表里的点是否在同一直线上的算法

	double n = Pntlist.size();
	double x1, x2, y1, y2, z1, z2, xz, yz, xy, r;
	x1 = x2 = y1 = y2 = z1 = z2 = xz = yz = xy = 0;

	math_Matrix A(1, 3, 1, 3);
	math_Vector b_Sol(1, 3);
	math_Vector X_Solve(1, 3);
	X_Solve.Init(1);

	math_Matrix B(1, 3, 1, 3);
	math_Matrix C(1, 3, 1, 3);
	math_Matrix D(1, 3, 1, 3);

	for (int i = 0; i < n; i++)
	{
		x1 += Pntlist[i].X();
		x2 += pow(Pntlist[i].X(), 2);
		xz += Pntlist[i].X() * Pntlist[i].Z();

		y1 += Pntlist[i].Y();
		y2 += pow(Pntlist[i].Y(), 2);
		yz += Pntlist[i].Y() * Pntlist[i].Z();

		z1 += Pntlist[i].Z();
		z2 += Pntlist[i].Z() * Pntlist[i].Z();
		xy += Pntlist[i].X() * Pntlist[i].Y();
	}

	//在三个方向上进行最小二乘法拟合，最后判断哪个误差小。

	//以Z为因变量的方向
	gp_Pln approxPln_Z;
	bool IsValid_Z = true;

	A(1, 1) = x2; A(1, 2) = xy; A(1, 3) = x1;
	A(2, 1) = xy; A(2, 2) = y2; A(2, 3) = y1;
	A(3, 1) = x1; A(3, 2) = y1; A(3, 3) = n;

	r = A.Determinant();

	if (abs(r) < 1e-4)//与Z平行
	{
		IsValid_Z = false;
	}
	else
	{
		b_Sol(1) = xz;
		b_Sol(2) = yz;
		b_Sol(3) = z1;

		math_Gauss Sol(A);

		Sol.Solve(b_Sol, X_Solve);

		approxPln_Z = gp_Pln(X_Solve(1), X_Solve(2), -1, X_Solve(3));


		//B(1, 1) = xz; B(1, 2) = xy; B(1, 3) = x1;
		//B(2, 1) = yz; B(2, 2) = y2; B(2, 3) = y1;
		//B(3, 1) = z1; B(3, 2) = y1; B(3, 3) = n;

		//double a = B.Determinant() / r;

		//C(1, 1) = x2; C(1, 2) = xz; C(1, 3) = x1;
		//C(2, 1) = xy; C(2, 2) = yz; C(2, 3) = y1;
		//C(3, 1) = x1; C(3, 2) = z1; C(3, 3) = n;

		//double b = C.Determinant() / r;

		//D(1, 1) = x2; D(1, 2) = xy; D(1, 3) = xz;
		//D(2, 1) = xy; D(2, 2) = y2; D(2, 3) = yz;
		//D(3, 1) = x1; D(3, 2) = y1; D(3, 3) = z1;

		//double c = D.Determinant() / r;

		//approxPln_Z = gp_Pln(a, b, -1, c);
	}

	//以Y为因变量的方向
	gp_Pln approxPln_Y;
	bool IsValid_Y = true;

	A(1, 1) = x2; A(1, 2) = xz; A(1, 3) = x1;
	A(2, 1) = xz; A(2, 2) = z2; A(2, 3) = z1;
	A(3, 1) = x1; A(3, 2) = z1; A(3, 3) = n;

	r = A.Determinant();

	if (abs(r) < 1e-4)//与Y平行
	{
		IsValid_Y = false;
	}
	else
	{
		b_Sol(1) = xy;
		b_Sol(2) = yz;
		b_Sol(3) = y1;

		math_Gauss Sol(A);

		Sol.Solve(b_Sol, X_Solve);

		approxPln_Y = gp_Pln(X_Solve(1), -1, X_Solve(2), X_Solve(3));
	}

	//以X为因变量的方向
	gp_Pln approxPln_X;
	bool IsValid_X = true;

	A(1, 1) = y2; A(1, 2) = yz; A(1, 3) = y1;
	A(2, 1) = yz; A(2, 2) = z2; A(2, 3) = z1;
	A(3, 1) = y1; A(3, 2) = z1; A(3, 3) = n;

	r = A.Determinant();

	if (abs(r) < 1e-4)//与X平行
	{
		IsValid_X = false;
	}
	else
	{
		b_Sol(1) = xy;
		b_Sol(2) = xz;
		b_Sol(3) = x1;

		math_Gauss Sol(A);

		Sol.Solve(b_Sol, X_Solve);

		approxPln_X = gp_Pln(-1, X_Solve(1), X_Solve(2), X_Solve(3));
	}

	double AllErr_X = 0, AllErr_Y = 0, AllErr_Z = 0;

	if (IsValid_X)
	{
		AllErr_X = DistanceOfListToPlane(Pntlist, approxPln_X);
	}
	else
	{
		AllErr_X = 9999999;
	}

	if (IsValid_Y)
	{
		AllErr_Y = DistanceOfListToPlane(Pntlist, approxPln_Y);
	}
	else
	{
		AllErr_Y = 9999999;
	}

	if (IsValid_Z)
	{
		AllErr_Z = DistanceOfListToPlane(Pntlist, approxPln_Z);
	}
	else
	{
		AllErr_Z = 9999999;
	}

	if ((AllErr_X <= AllErr_Y) && (AllErr_X <= AllErr_Z))
	{
		return approxPln_X;
	}
	else if ((AllErr_Y <= AllErr_X) && (AllErr_Y <= AllErr_Z))
	{
		return approxPln_Y;
	}
	else if ((AllErr_Z <= AllErr_X) && (AllErr_Z <= AllErr_Y))
	{
		return approxPln_Z;
	}
	else
	{
		QMessageBox::warning(
			QApplication::activeWindow(), QObject::tr("Caution"), QString::fromLocal8Bit("平面拟合时出错"));
		return approxPln_Z;
	}
	//if (abs(r) < 1e-4)//当平面与z轴平行，直线拟合
	//{
	//	math_Matrix Left(0, 1, 0, 1);
	//	Left(0, 0) = x2;
	//	Left(0, 1) = x1;
	//	Left(1, 0) = x1;
	//	Left(1, 1) = n;

	//	double DeterDelt = Left.Determinant();

	//	double a = 0, b = 0;

	//	if (abs(DeterDelt) > 1e-5)// 判断分母不为0  
	//	{
	//		math_Matrix LeA(0, 1, 0, 1);
	//		LeA(0, 0) = xy;
	//		LeA(0, 1) = x1;
	//		LeA(1, 0) = y1;
	//		LeA(1, 1) = n;

	//		math_Matrix LeB(0, 1, 0, 1);
	//		LeB(0, 0) = x2;
	//		LeB(0, 1) = xy;
	//		LeB(1, 0) = x1;
	//		LeB(1, 1) = y1;

	//		a = LeA.Determinant() / DeterDelt;
	//		b = LeB.Determinant() / DeterDelt;

	//		gp_Pln approxPln = gp_Pln(a, -1, 0, b);
	//		return approxPln;
	//	}
	//	else//与y，z轴均平行
	//	{
	//		gp_Pln approxPln = gp_Pln(1, 0, 0, -x1 / n);
	//		return approxPln;
	//	}
	//	
	//}
	//else
	//{
	//	B(1, 1) = xz; B(1, 2) = xy; B(1, 3) = x1;
	//	B(2, 1) = yz; B(2, 2) = y2; B(2, 3) = y1;
	//	B(3, 1) = z1; B(3, 2) = y1; B(3, 3) = n;

	//	double a = B.Determinant() / r;

	//	C(1, 1) = x2; C(1, 2) = xz; C(1, 3) = x1;
	//	C(2, 1) = xy; C(2, 2) = yz; C(2, 3) = y1;
	//	C(3, 1) = x1; C(3, 2) = z1; C(3, 3) = n;

	//	double b = C.Determinant() / r;

	//	D(1, 1) = x2; D(1, 2) = xy; D(1, 3) = xz;
	//	D(2, 1) = xy; D(2, 2) = y2; D(2, 3) = yz;
	//	D(3, 1) = x1; D(3, 2) = y1; D(3, 3) = z1;

	//	double c = D.Determinant() / r;

	//	gp_Pln approxPln = gp_Pln(a, b, -1, c);

	//	return approxPln;
	//}

}

double ToleranceFunction::Axiality(vector<gp_Pnt> CylinPnts1, vector<gp_Pnt> CylinPnts2, FSurface* Cylinder)
{
	//第一步，提取投影平面，与理论圆柱轴线垂直的任一平面均可
	gp_Pln ProjPln;
	gp_Dir theTheoDir(0, 0, 1);

	//找到Cylindrical对应的柱面轴线法平面

	if (!ToleranceFunction::SearchNormPlaneOfCylinderAxis(Cylinder, ProjPln))
	{
		cout << "未发现柱面对应的曲面" << endl;
		QMessageBox::warning(0, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("未发现柱面对应的曲面"), "OK");
		return 0.0;
	}
	else
	{
		double pline_A, pline_B, pline_C, pline_D;
		ProjPln.Coefficients(pline_A, pline_B, pline_C, pline_D);

		if (abs(pline_A) < 1e-6) pline_A = 0;
		if (abs(pline_B) < 1e-6) pline_B = 0;
		if (abs(pline_C) < 1e-6) pline_C = 0;

		theTheoDir.SetX(pline_A);
		theTheoDir.SetY(pline_B);
		theTheoDir.SetZ(pline_C);

	}

	////测点归类，找到属于同一个圆的测点(第一个圆柱)
	//vector<double> DisList;
	//for (int i = 0; i < CylinPnts1.size(); i++)
	//{
	//	DisList.push_back(ProjPln.Distance(CylinPnts1[i]));
	//}
	////找到两个圆的各自距离
	//vector<double> DisClas;
	//DisClas.push_back(DisList[0]);
	//for (int i = 0; i < DisList.size(); i++)
	//{
	//	if (abs(DisClas[0] - DisList[i]) > 1)
	//	{
	//		DisClas.push_back(DisList[i]);
	//		break;
	//	}
	//}
	////找到每个圆各自的测点
	//vector<gp_Pnt> Cylin1_Cir1;
	//vector<gp_Pnt> Cylin1_Cir2;
	//for (int i = 0; i < CylinPnts1.size(); i++)
	//{
	//	if (abs(ProjPln.Distance(CylinPnts1[i]) - DisClas[0]) < 1)
	//	{
	//		Cylin1_Cir1.push_back(CylinPnts1[i]);
	//	}
	//	else if (abs(ProjPln.Distance(CylinPnts1[i]) - DisClas[1]) < 1)
	//	{
	//		Cylin1_Cir2.push_back(CylinPnts1[i]);
	//	}
	//}
	////最小二乘法拟合第一圆柱第一圈所在平面
	//double A = ProjPln.Axis().Direction().X();
	//double B = ProjPln.Axis().Direction().Y();
	//double C = ProjPln.Axis().Direction().Z();
	//double Dup = 0;
	//for (int i = 0; i < Cylin1_Cir1.size(); i++)
	//{
	//	Dup -= (A * Cylin1_Cir1[i].X() + B * Cylin1_Cir1[i].Y() + C * Cylin1_Cir1[i].Z());
	//}
	//double D = Dup / Cylin1_Cir1.size();
	//gp_Pln Pln_Cylin1_Cir1 = gp_Pln(A, B, C, D);
	//gp_Pnt CirCen_1 = ToleranceFunction::CircleCenture(Pln_Cylin1_Cir1, Cylin1_Cir1);
	////最小二乘法拟合第一圆柱第二圈所在平面
	//Dup = 0;
	//for (int i = 0; i < Cylin1_Cir2.size(); i++)
	//{
	//	Dup -= (A * Cylin1_Cir2[i].X() + B * Cylin1_Cir2[i].Y() + C * Cylin1_Cir2[i].Z());
	//}
	//D = Dup / Cylin1_Cir2.size();
	//gp_Pln Pln_Cylin1_Cir2 = gp_Pln(A, B, C, D);
	//gp_Pnt CirCen_2 = ToleranceFunction::CircleCenture(Pln_Cylin1_Cir2, Cylin1_Cir2);
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	////测点归类，找到属于同一个圆的测点(第二个圆柱)
	//DisList.clear();
	//for (int i = 0; i < CylinPnts2.size(); i++)
	//{
	//	DisList.push_back(ProjPln.Distance(CylinPnts2[i]));
	//}
	////找到两个圆的各自距离
	//DisClas.clear();
	//DisClas.push_back(DisList[0]);
	//for (int i = 0; i < DisList.size(); i++)
	//{
	//	if (abs(DisClas[0] - DisList[i]) > 1)
	//	{
	//		DisClas.push_back(DisList[i]);
	//		break;
	//	}
	//}
	////找到每个圆各自的测点
	//vector<gp_Pnt> Cylin2_Cir1;
	//vector<gp_Pnt> Cylin2_Cir2;
	//for (int i = 0; i < CylinPnts2.size(); i++)
	//{
	//	if (abs(ProjPln.Distance(CylinPnts2[i]) - DisClas[0]) < 1)
	//	{
	//		Cylin2_Cir1.push_back(CylinPnts2[i]);
	//	}
	//	else if (abs(ProjPln.Distance(CylinPnts2[i]) - DisClas[1]) < 1)
	//	{
	//		Cylin2_Cir2.push_back(CylinPnts2[i]);
	//	}
	//}
	////最小二乘法拟合第一圆柱第一圈所在平面
	//Dup = 0;
	//for (int i = 0; i < Cylin2_Cir1.size(); i++)
	//{
	//	Dup -= (A * Cylin2_Cir1[i].X() + B * Cylin2_Cir1[i].Y() + C * Cylin2_Cir1[i].Z());
	//}
	//D = Dup / Cylin2_Cir1.size();
	//gp_Pln Pln_Cylin2_Cir1 = gp_Pln(A, B, C, D);
	//gp_Pnt CirCen_3 = ToleranceFunction::CircleCenture(Pln_Cylin2_Cir1, Cylin2_Cir1);
	////最小二乘法拟合第一圆柱第二圈所在平面
	//Dup = 0;
	//for (int i = 0; i < Cylin2_Cir2.size(); i++)
	//{
	//	Dup -= (A * Cylin2_Cir2[i].X() + B * Cylin2_Cir2[i].Y() + C * Cylin2_Cir2[i].Z());
	//}
	//D = Dup / Cylin2_Cir2.size();
	//gp_Pln Pln_Cylin2_Cir2 = gp_Pln(A, B, C, D);
	//gp_Pnt CirCen_4 = ToleranceFunction::CircleCenture(Pln_Cylin2_Cir2, Cylin2_Cir2);
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//double origLine_x = 0, origLine_y = 0, origLine_z = 0;
	//origLine_x = (CirCen_1.X() + CirCen_2.X() + CirCen_3.X() + CirCen_4.X()) / 4;
	//origLine_y = (CirCen_1.Y() + CirCen_2.Y() + CirCen_3.Y() + CirCen_4.Y()) / 4;
	//origLine_z = (CirCen_1.Z() + CirCen_2.Z() + CirCen_3.Z() + CirCen_4.Z()) / 4;
	//gp_Lin theAxis = gp_Lin(gp_Pnt(origLine_x, origLine_y, origLine_z), theTheoDir);
	//vector<double> TolList;
	//TolList.push_back(theAxis.Distance(CirCen_1));
	//TolList.push_back(theAxis.Distance(CirCen_2));
	//TolList.push_back(theAxis.Distance(CirCen_3));
	//TolList.push_back(theAxis.Distance(CirCen_4));

	vector<gp_Pnt> AllPntList;

	for (int i = 0; i < CylinPnts1.size(); i++)
	{
		AllPntList.push_back(CylinPnts1[i]);
	}

	for (int i = 0; i < CylinPnts2.size(); i++)
	{
		AllPntList.push_back(CylinPnts2[i]);
	}

	vector<gp_Pnt> theCenlist;

	ToleranceFunction::CylinToCenr(AllPntList, ProjPln, theCenlist);

	double origLine_x = 0, origLine_y = 0, origLine_z = 0;

	for (int i = 0; i < theCenlist.size(); i++)
	{
		origLine_x += theCenlist[i].X();
		origLine_y += theCenlist[i].Y();
		origLine_z += theCenlist[i].Z();
	}

	origLine_x /= theCenlist.size();
	origLine_y /= theCenlist.size();
	origLine_z /= theCenlist.size();

	gp_Lin theAxis = gp_Lin(gp_Pnt(origLine_x, origLine_y, origLine_z), theTheoDir);

	vector<double> TolList;

	for (int i = 0; i < theCenlist.size(); i++)
	{
		TolList.push_back(theAxis.Distance(theCenlist[i]));
	}

	return 2 * MAX(TolList);
}

gp_Pnt ToleranceFunction::CircleCenture(gp_Pln Realplan, vector<gp_Pnt> Pntlist)
{
	//将三维点映射到测点拟合面的二维点
	vector<gp_Pnt2d> m_points;
	for (int i = 0; i < Pntlist.size(); i++)
	{
		gp_Pnt2d tmpPnt = ProjLib::Project(Realplan, Pntlist[i]);
		m_points.push_back(tmpPnt);
	}
	//利用最小二乘法计算圆心位置及直径，算法参考网站 https://blog.csdn.net/Jacky_Ponder/article/details/70314919

	double X1 = 0;
	double Y1 = 0;
	double X2 = 0;
	double Y2 = 0;
	double X3 = 0;
	double Y3 = 0;
	double X1Y1 = 0;
	double X1Y2 = 0;
	double X2Y1 = 0;
	int m_nNum = m_points.size();

	for (int i = 0; i < m_nNum; i++)
	{
		X1 = X1 + m_points[i].X();
		Y1 = Y1 + m_points[i].Y();
		X2 = X2 + m_points[i].X() * m_points[i].X();
		Y2 = Y2 + m_points[i].Y() * m_points[i].Y();
		X3 = X3 + m_points[i].X() * m_points[i].X() * m_points[i].X();
		Y3 = Y3 + m_points[i].Y() * m_points[i].Y() * m_points[i].Y();
		X1Y1 = X1Y1 + m_points[i].X() * m_points[i].Y();
		X1Y2 = X1Y2 + m_points[i].X() * m_points[i].Y() * m_points[i].Y();
		X2Y1 = X2Y1 + m_points[i].X() * m_points[i].X() * m_points[i].Y();
	}

	double C, D, E, G, H, N;
	double a, b, c;
	N = m_nNum;
	C = N * X2 - X1 * X1;
	D = N * X1Y1 - X1 * Y1;
	E = N * X3 + N * X1Y2 - (X2 + Y2) * X1;
	G = N * Y2 - Y1 * Y1;
	H = N * X2Y1 + N * Y3 - (X2 + Y2) * Y1;
	a = (H * D - E * G) / (C * G - D * D);
	b = (H * C - E * D) / (D * D - G * C);
	c = -(a * X1 + b * Y1 + X2 + Y2) / N;

	double A, B, R;
	A = a / (-2);
	B = b / (-2);
	R = sqrt(a * a + b * b - 4 * c) / 2;

	gp_Pnt PlnOrig = Realplan.Location();
	gp_Dir DirX = Realplan.XAxis().Direction();
	gp_Dir DirY = Realplan.YAxis().Direction();

	gp_Pnt CirCen = gp_Pnt(PlnOrig.X() + (A * DirX + B * DirY).X(), PlnOrig.Y() + (A * DirX + B * DirY).Y(), PlnOrig.Z() + (A * DirX + B * DirY).Z());

	return CirCen;
}

gp_Lin ToleranceFunction::LineFromPnts(vector<gp_Pnt> PntList)
{
	gp_Lin LinRes;

	bool IsParallX_Y = false;
	//先判断目标直线是否与X-Y平面平行
	double Zp = 0;
	for (int i = 0; i < PntList.size(); i++)
	{
		Zp += PntList[i].Z();
	}
	Zp /= PntList.size();

	double Zc = 0;
	for (int i = 0; i < PntList.size(); i++)
	{
		Zc += abs(PntList[i].Z() - Zp);
	}
	Zc /= PntList.size();

	if (Zc <= 1)//说明拟合直线与XY平行
	{
		IsParallX_Y = true;
	}

	if (!IsParallX_Y)//不平行
	{
		double ZZ = 0;
		double Z_ = 0;
		double XZ = 0;
		double X_ = 0;
		double YZ = 0;
		double Y_ = 0;
		double N_ = PntList.size();

		for (int i = 0; i < PntList.size(); i++)
		{
			ZZ += PntList[i].Z() * PntList[i].Z();
			Z_ += PntList[i].Z();
			XZ += PntList[i].X() * PntList[i].Z();
			X_ += PntList[i].X();
			YZ += PntList[i].Y() * PntList[i].Z();
			Y_ += PntList[i].Y();
		}

		double Fnt = 1 / (ZZ * N_ - Z_ * Z_);

		double fit_a = Fnt * (XZ * N_ + X_ * (-Z_));
		double fit_b = Fnt * (XZ * (-Z_) + X_ * ZZ);
		double fit_c = Fnt * (YZ * N + Y_ * (-Z_));
		double fit_d = Fnt * (YZ * (-Z_) + Y_ * ZZ);

		LinRes = gp_Lin(gp_Pnt(fit_b, fit_d, 0), gp_Dir(fit_a, fit_c, 1));
	}
	else//平行
	{
		double XX = 0, X_ = 0, ZX = 0, Z_ = 0, YX = 0, Y_ = 0;
		double N_ = PntList.size();

		for (int i = 0; i < PntList.size(); i++)
		{
			XX += PntList[i].X() * PntList[i].X();
			X_ += PntList[i].X();
			ZX += PntList[i].Z() * PntList[i].X();
			Z_ += PntList[i].Z();
			YX += PntList[i].Y() * PntList[i].X();
			Y_ += PntList[i].Y();
		}

		double Fnt = 1 / (XX * N_ - X_ * X_);

		double fit_a = Fnt * (ZX * N_ + Z_ * (-X_));
		double fit_b = Fnt * (ZX * (-X_) + Z_ * XX);
		double fit_c = Fnt * (YX * N + Y_ * (-X_));
		double fit_d = Fnt * (YX * (-X_) + Y_ * XX);

		LinRes = gp_Lin(gp_Pnt(0, fit_d, fit_b), gp_Dir(1, fit_c, fit_a));
	}

	return LinRes;
}

void ToleranceFunction::Circle2DFit(vector<gp_Pnt2d> PntList, double& CenX, double& CenY, double& Radius)
{
	double X1 = 0;
	double Y1 = 0;
	double X2 = 0;
	double Y2 = 0;
	double X3 = 0;
	double Y3 = 0;
	double X1Y1 = 0;
	double X1Y2 = 0;
	double X2Y1 = 0;
	int m_nNum = PntList.size();

	for (int i = 0; i < m_nNum; i++)
	{
		X1 = X1 + PntList[i].X();
		Y1 = Y1 + PntList[i].Y();
		X2 = X2 + PntList[i].X() * PntList[i].X();
		Y2 = Y2 + PntList[i].Y() * PntList[i].Y();
		X3 = X3 + PntList[i].X() * PntList[i].X() * PntList[i].X();
		Y3 = Y3 + PntList[i].Y() * PntList[i].Y() * PntList[i].Y();
		X1Y1 = X1Y1 + PntList[i].X() * PntList[i].Y();
		X1Y2 = X1Y2 + PntList[i].X() * PntList[i].Y() * PntList[i].Y();
		X2Y1 = X2Y1 + PntList[i].X() * PntList[i].X() * PntList[i].Y();
	}

	double C, D, E, G, H, N;
	double a, b, c;
	N = m_nNum;
	C = N * X2 - X1 * X1;
	D = N * X1Y1 - X1 * Y1;
	E = N * X3 + N * X1Y2 - (X2 + Y2) * X1;
	G = N * Y2 - Y1 * Y1;
	H = N * X2Y1 + N * Y3 - (X2 + Y2) * Y1;
	a = (H * D - E * G) / (C * G - D * D);
	b = (H * C - E * D) / (D * D - G * C);
	c = -(a * X1 + b * Y1 + X2 + Y2) / N;

	double A, B, R;
	A = a / (-2);
	B = b / (-2);
	R = sqrt(a * a + b * b - 4 * c) / 2;

	CenX = A;
	CenY = B;
	Radius = R;
}

void ToleranceFunction::CylinToCenr(vector<gp_Pnt> CylinPnts, gp_Pln ProjPln, vector<gp_Pnt>& CenrList)
{
	double InvThrea = 1.0;//判断是否在同一圈的距离阈值

	//测点归类，找到属于同一个圆的测点(第一个圆柱)
	//vector<double> DisList;
	//for (int i = 0; i < CylinPnts.size(); i++)
	//{
	//	DisList.push_back(ProjPln.Distance(CylinPnts[i]));
	//}

	///////////////////
	double ExPln_A = 0, ExPln_B = 0, ExPln_C = 0, ExPln_D = 0;
	ProjPln.Coefficients(ExPln_A, ExPln_B, ExPln_C, ExPln_D);
	gp_Pln AnotherPln(ExPln_A, ExPln_B, ExPln_C, 9999999);

	vector<double> DisList2;
	for (int i = 0; i < CylinPnts.size(); i++)
	{
		DisList2.push_back(AnotherPln.Distance(CylinPnts[i]));
	}

	// 升序排列
	//sort(DisList2.begin(), DisList2.end());

	// 判断 Val 与 ClassList 中所有数之间距离最小值是否低于 WithinThread，若是，返回true
	function<bool(double, double, vector<double>)> IsClassWithin = [&](double WithinThread, double Val, vector<double>ClassList)->bool
	{
		for (auto ita = ClassList.begin(); ita != ClassList.end(); ++ita)
		{
			if (abs(Val - (*ita)) < abs(WithinThread))
			{
				return true;
			}
		}

		return false;
	};

	//找到每个圆的各自距离
	vector<double> DisClas;
	DisClas.push_back(DisList2[0]);

	for (auto ita = DisList2.begin(); ita != DisList2.end(); ++ita)
	{
		if (!IsClassWithin(InvThrea, (*ita), DisClas))
		{
			DisClas.push_back((*ita));
		}
	}

	//找到每个圆各自的测点
	vector<vector<gp_Pnt>>PntClafy;
	vector<gp_Pnt>tmpPntList;

	for (int i = 0; i < DisClas.size(); i++)
	{
		tmpPntList.clear();
		for (int j = 0; j < DisList2.size(); j++)
		{
			if (abs(DisClas[i] - DisList2[j]) < InvThrea)
			{
				tmpPntList.push_back(CylinPnts[j]);
			}
		}
		PntClafy.push_back(tmpPntList);
	}

	CenrList.clear();

	//计算圆心
	double A = ProjPln.Axis().Direction().X();
	double B = ProjPln.Axis().Direction().Y();
	double C = ProjPln.Axis().Direction().Z();
	for (int i = 0; i < DisClas.size(); i++)
	{
		if (PntClafy[i].size() < 3)
			continue;//如果某圈测点数小于3，不计算圆心

		double Dup = 0;
		for (int j = 0; j < PntClafy[i].size(); j++)
		{
			Dup -= (A * PntClafy[i][j].X() + B * PntClafy[i][j].Y() + C * PntClafy[i][j].Z());
		}
		double D = Dup / PntClafy[i].size();

		gp_Pln Pln_Cylin1_Cir1 = gp_Pln(A, B, C, D);

		gp_Pnt CirCen = ToleranceFunction::CircleCenture(Pln_Cylin1_Cir1, PntClafy[i]);

		CenrList.push_back(CirCen);
	}


//	double InvThrea = 0.5;//判断是否在同一圈的距离阈值
//
////测点归类，找到属于同一个圆的测点(第一个圆柱)
//	vector<double> DisList;
//	for (int i = 0; i < CylinPnts.size(); i++)
//	{
//		DisList.push_back(ProjPln.Distance(CylinPnts[i]));
//	}
//
//	///////////////////
//	double ExPln_A = 0, ExPln_B = 0, ExPln_C = 0, ExPln_D = 0;
//	ProjPln.Coefficients(ExPln_A, ExPln_B, ExPln_C, ExPln_D);
//	gp_Pln AnotherPln(ExPln_A, ExPln_B, ExPln_C, 9999999);
//
//	vector<double> DisList2;
//	for (int i = 0; i < CylinPnts.size(); i++)
//	{
//		DisList2.push_back(AnotherPln.Distance(CylinPnts[i]));
//	}
//
//	InvThrea = (abs(MAX(DisList2) - MIN(DisList2)) / 10);
//	///////////////////
//
//	//找到每个圆的各自距离
//	vector<double> DisClas;
//	DisClas.push_back(DisList[0]);
//
//	vector<double> RemovedDis;
//
//	do
//	{
//		RemovedDis.clear();
//		RemovedDis.push_back(DisList[0]);
//
//		for (int i = 0; i < DisList.size(); i++)
//		{
//			bool IsWithin = false;
//
//			for (int j = 0; j < DisClas.size(); j++)
//			{
//				if (abs(DisList[i] - DisClas[j]) < InvThrea)
//				{
//					IsWithin = true;
//					break;
//				}
//			}
//			if (!IsWithin)
//			{
//				DisClas.push_back(DisList[i]);
//			}
//			else
//			{
//				RemovedDis.push_back(DisList[i]);
//			}
//		}
//
//	} while (RemovedDis.size() < DisList.size());
//
//	//找到每个圆各自的测点
//	vector<vector<gp_Pnt>>PntClafy;
//	vector<gp_Pnt>tmpPntList;
//
//	for (int i = 0; i < DisClas.size(); i++)
//	{
//		tmpPntList.clear();
//		for (int j = 0; j < DisList.size(); j++)
//		{
//			if (abs(DisClas[i] - DisList[j]) < InvThrea)
//			{
//				tmpPntList.push_back(CylinPnts[j]);
//			}
//		}
//		PntClafy.push_back(tmpPntList);
//	}
//
//	CenrList.clear();
//
//	//计算圆心
//	double A = ProjPln.Axis().Direction().X();
//	double B = ProjPln.Axis().Direction().Y();
//	double C = ProjPln.Axis().Direction().Z();
//	for (int i = 0; i < DisClas.size(); i++)
//	{
//		if (PntClafy[i].size() < 3)
//			continue;//如果某圈测点数小于3，不计算圆心
//
//		double Dup = 0;
//		for (int j = 0; j < PntClafy[i].size(); j++)
//		{
//			Dup -= (A * PntClafy[i][j].X() + B * PntClafy[i][j].Y() + C * PntClafy[i][j].Z());
//		}
//		double D = Dup / PntClafy[i].size();
//
//		gp_Pln Pln_Cylin1_Cir1 = gp_Pln(A, B, C, D);
//
//		gp_Pnt CirCen = ToleranceFunction::CircleCenture(Pln_Cylin1_Cir1, PntClafy[i]);
//
//		CenrList.push_back(CirCen);
//	}
}

double ToleranceFunction::DistanceOfListToPlane(vector<gp_Pnt> Pntlist, gp_Pln thePlane)
{
	double Alldistance = 0;
	int IteN = 0;

	for (auto it = Pntlist.begin(); it != Pntlist.end(); ++it)
	{
		Alldistance += thePlane.Distance(*it);

		IteN += 1;
	}

	if (IteN != Pntlist.size())
	{
		cout << Pntlist[9999999].Z();
	}

	return Alldistance;
}

bool ToleranceFunction::SearchNormPlaneOfCylinderAxis(FSurface* paraSurf, gp_Pln& paraNormPlane)
{
	SURFACE_TYPE the_type;
	int ind_in_all = 0;
	int ind_in_type = 0;
	QString tmpFSurfName;

	gWindow->GetSTEPModelConverter()->FSurfInfo(paraSurf, the_type, ind_in_all, ind_in_type, tmpFSurfName);

	if (the_type != CYLINDER)
	{
		cout << "未发现柱面对应的曲面" << endl;
		return false;
	}

	Handle(Geom_CylindricalSurface) geom_Cylin = Handle(Geom_CylindricalSurface)::DownCast(BRep_Tool::Surface(gWindow->GetSTEPModelConverter()->GetTopoDS_FaceList()[ind_in_all]));

	paraNormPlane = gp_Pln(gp_Pnt(0, 0, 0), gp_Dir(ZeroLim(geom_Cylin->Axis().Direction().X()), ZeroLim(geom_Cylin->Axis().Direction().Y()), ZeroLim(geom_Cylin->Axis().Direction().Z())));

	return true;
}

double ToleranceFunction::ZeroLim(double Val, double Zero_Limit)
{
	if (abs(Val) < abs(Zero_Limit))
	{
		return 0.0;
	}
	else
	{
		return Val;
	}
}
