#include "np.h"
#include <math_Matrix.hxx>
#include <functional>
#include <math_Gauss.hxx>
//#include "occConverter.h"
#include "FMainWindow.h"
#include <random>
#include <BRepAdaptor_Curve.hxx>
#include <GeomAPI.hxx>
#include <CPnts_MyGaussFunction.hxx>
#include <math_GaussSingleIntegration.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <GeomLProp_CLProps.hxx>
#include <GeomConvert.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomLProp_SLProps.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <GeomPlate_PointConstraint.hxx>
#include <GeomPlate_BuildPlateSurface.hxx>
#include <GeomPlate_MakeApprox.hxx>
#include <ShapeAnalysis.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <BRepTools.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <STEPControl_StepModelType.hxx>
#include <STEPControl_Writer.hxx>
#include "MeasPt.h"
#include "PathShow.h"
#include <gp_Pln.hxx>

vector<double> linspace(double start, double end, int sec)//sec>=3
{
	vector<double>tmp;

	if (sec == 1)
	{
		tmp.push_back((start + end) / 2);
		return tmp;
	}


	tmp.push_back(start);
	double section = (end - start) / (sec - 1);
	for (int i = 1; i <= sec - 2; i++)
	{
		tmp.push_back(start + section * i);
	}
	tmp.push_back(end);
	return tmp;
}

vector<double> reverse(vector<double>lit, vector<double>lar, vector<double>aim)//return the correspond index in lar
{
	vector<double> res_cal;
	res_cal.push_back(aim[0]);
	int coorindex[3] = { 0,0,0 };

	for (int i = 1; i < lit.size() - 1; i++)
	{
		for (int j = 0; j < lar.size() - 1; j++)
		{
			if ((lit[i] > lar[j] && lit[i] < lar[j + 1]) || (lit[i] < lar[j] && lit[i] > lar[j + 1]))
			{
				coorindex[0] = j;
				coorindex[1] = j + 1;
				coorindex[2] = 0;
			}
			else if (lit[i] == lar[j])
			{
				coorindex[0] = j;
				coorindex[1] = j;
				coorindex[2] = 1;
			}
			else if (lit[i] == lar[j + 1])
			{
				coorindex[0] = j + 1;
				coorindex[1] = j + 1;
				coorindex[2] = 1;
			}
		}
		if (coorindex[2] == 1)
			res_cal.push_back(aim[coorindex[0]]);
		else if (coorindex[2] == 0)
		{
			double the_orig = lit[i];

			double coor_sta = lar[coorindex[0]];
			double coor_end = lar[coorindex[1]];
			double aim_sta = aim[coorindex[0]];
			double aim_end = aim[coorindex[1]];

			double k_clinat = (aim_end - aim_sta) / (coor_end - coor_sta);
			double the_aim = aim_sta + k_clinat * (the_orig - coor_sta);
			res_cal.push_back(the_aim);
		}
	}

	res_cal.push_back(aim[aim.size() - 1]);
	return res_cal;
}

double MIN(vector<double> mylist)//mylist中一定是正数
{
	double assistant = mylist[0];
	for (int i = 1; i < mylist.size(); i++)
	{
		if (mylist[i] < assistant)
			assistant = mylist[i];
	}
	return assistant;
}

pair<double, int> MIN_INDEX(vector<double> mylist)//mylist中一定是正数
{
	pair<double, int> thispair;
	thispair.first = mylist[0];
	thispair.second = 0;
	for (int i = 1; i < mylist.size(); i++)
	{
		if (mylist[i] < thispair.first)
		{
			thispair.first = mylist[i];
			thispair.second = i;
		}

	}
	return thispair;
}

pair<double, coor> MIN(vector<vector<double>> mylist)
{
	int ind_i = mylist.size();
	int ind_j = mylist[0].size();

	pair<double, coor> tmpPair;
	tmpPair.first = mylist[0][0];
	tmpPair.second.i = 0;
	tmpPair.second.j = 0;

	for (int i = 0; i < ind_i; i++)
	{
		for (int j = 0; j < ind_j; j++)
		{
			if (mylist[i][j] < tmpPair.first)
			{
				tmpPair.first = mylist[i][j];
				tmpPair.second.i = i;
				tmpPair.second.j = j;
			}
		}
	}

	return tmpPair;
}

double MAX(vector<double> mylist)//mylist中一定是正数
{
	double assistant = mylist[0];
	for (int i = 1; i < mylist.size(); i++)
	{
		if (mylist[i] > assistant)
			assistant = mylist[i];
	}
	return assistant;
}

pair<double, coor> MAX(vector<vector<double>> mylist)
{
	int ind_i = mylist.size();
	int ind_j = mylist[0].size();

	pair<double, coor> tmpPair;
	tmpPair.first = mylist[0][0];
	tmpPair.second.i = 0;
	tmpPair.second.j = 0;

	for (int i = 0; i < ind_i; i++)
	{
		for (int j = 0; j < ind_j; j++)
		{
			if (mylist[i][j] > tmpPair.first)
			{
				tmpPair.first = mylist[i][j];
				tmpPair.second.i = i;
				tmpPair.second.j = j;
			}
		}
	}

	return tmpPair;
}

pair<double, int> MAX_INDEX(vector<double> mylist)
{
	pair<double, int> thispair;
	thispair.first = mylist[0];
	thispair.second = 0;
	for (int i = 1; i < mylist.size(); i++)
	{
		if (mylist[i] > thispair.first)
		{
			thispair.first = mylist[i];
			thispair.second = i;
		}
	}
	return thispair;
}

double LEN(gp_Vec dest)
{
	return sqrt(dest.X() * dest.X() + dest.Y() * dest.Y() + dest.Z() * dest.Z());
}

pair<double, int> PntToLine(double xp, double yp, double L1x, double L1y, double L2x, double L2y)
{
	gp_Vec AB = gp_Vec(L2x - L1x, L2y - L1y, 0);
	gp_Vec AC = gp_Vec(xp - L1x, yp - L1y, 0);
	gp_Vec AB_AC = gp_Vec(0, 0, 0);
	CROSS(AB_AC, AB, AC);

	if (LEN(AB) == 0)
		cout << "此段出错";////////////////////////////////////

	pair<double, int> result;

	if (LEN(AC) == 0)
	{
		result.first = 0;
		result.second = 1;
		return result;
	}

	result.first = LEN(AB_AC) / LEN(AB);//距离

	//double up1 = DOT(AB, AC);
	//double down11 = LEN(AB);
	//double down12 = LEN(AC);

	double cos1 = DOT(AB, AC) / (LEN(AB) * LEN(AC));
	gp_Vec BA = gp_Vec(L1x - L2x, L1y - L2y, 0);
	gp_Vec BC = gp_Vec(xp - L2x, yp - L2y, 0);

	if (LEN(BC) == 0)
	{
		result.first = 0;
		result.second = 1;
		return result;
	}

	//double up2 = DOT(BA, BC);
	//double down21 = LEN(BA);
	//double down22 = LEN(BC);

	double cos2 = DOT(BA, BC) / (LEN(BA) * LEN(BC));//判断垂足是否在线段上

	if (cos1 >= -0.2 && cos2 >= -0.2)//////////////////////////////////判断垂足是否在线段内
	{
		result.second = 1;
	}
	else
		result.second = 0;

	return result;
}

vector<double>VecInsert(vector<double> dest, pair<double, int> InsAim)
{
	vector<double> tmpvec;
	for (int i = 0; i < dest.size(); i++)
	{
		if (i == InsAim.second)
		{
			tmpvec.push_back(InsAim.first);
		}
		tmpvec.push_back(dest[i]);
	}
	return tmpvec;
}

bool distance(double& result, gp_Pnt orig, gp_Pnt vert0, gp_Pnt vert1, gp_Pnt vert2)
{
	gp_Pnt edge1(0.0, 0.0, 0.0);
	gp_Pnt edge2(0.0, 0.0, 0.0);
	SUB(edge1, vert1, vert0);
	SUB(edge2, vert2, vert0);
	gp_Pnt pvec(0.0, 0.0, 0.0);
	gp_Pnt dir(0.0, 0.0, 0.0);
	CROSS(dir, edge1, edge2);
	CROSS(pvec, dir, edge2);
	double det = DOT(edge1, pvec);
	//if (abs(det) < 0.00001)
	//	return false;

	gp_Pnt tvec(0.0, 0.0, 0.0);
	SUB(tvec, orig, vert0);
	double inv_det = 1.0 / det;
	gp_Pnt qvec(0.0, 0.0, 0.0);
	CROSS(qvec, tvec, edge1);

	double u = DOT(tvec, pvec);
	double v = DOT(dir, qvec);
	u *= inv_det;
	v *= inv_det;
	if (u >= -0.1 && v >= -0.1 && (u + v) <= 1.1)
	{
		gp_Pnt thispoint(0.0, 0.0, 0.0);
		thispoint.SetX((1 - u - v) * vert0.X() + u * vert1.X() + v * vert2.X());
		thispoint.SetY((1 - u - v) * vert0.Y() + u * vert1.Y() + v * vert2.Y());
		thispoint.SetZ((1 - u - v) * vert0.Z() + u * vert1.Z() + v * vert2.Z());
		result = sqrt(pow(thispoint.X() - orig.X(), 2) + pow(thispoint.Y() - orig.Y(), 2) + pow(thispoint.Z() - orig.Z(), 2));
		return true;
	}
	else
		return false;
}

bool Edistance(double& result, gp_Pnt vert0, gp_Pnt vert1, gp_Pnt vert2, gp_Pnt vert3)
{
	double VecL = (vert2.Y() - vert1.Y()) * (vert3.Z() - vert1.Z()) - (vert3.Y() - vert1.Y()) * (vert2.Z() - vert1.Z());
	double VecM = (vert2.Z() - vert1.Z()) * (vert3.X() - vert1.X()) - (vert3.Z() - vert1.Z()) * (vert2.X() - vert1.X());
	double VecN = (vert2.X() - vert1.X()) * (vert3.Y() - vert1.Y()) - (vert3.X() - vert1.X()) * (vert2.Y() - vert1.Y());

	double Mt = (VecL * (vert1.X() - vert0.X()) + VecM * (vert1.Y() - vert0.Y()) + VecN * (vert1.Z() - vert0.Z())) / (VecL * VecL + VecM * VecM + VecN * VecN);

	double Xx = vert0.X() + Mt * VecL;
	double Yy = vert0.Y() + Mt * VecM;
	double Zz = vert0.Z() + Mt * VecN;

	gp_Pnt NPnt(Xx, Yy, Zz);

	//三点三角形面积
	gp_Pnt edge1(0.0, 0.0, 0.0);
	gp_Pnt edge2(0.0, 0.0, 0.0);
	SUB(edge1, vert2, vert1);
	SUB(edge2, vert3, vert1);

	gp_Vec Areas;
	CROSS(Areas, edge1, edge2);
	double SA = Areas.X() * Areas.X() + Areas.Y() * Areas.Y() + Areas.Z() * Areas.Z();

	SUB(edge1, vert1, vert0);
	SUB(edge2, vert2, vert0);
	CROSS(Areas, edge1, edge2);
	double S1 = Areas.X() * Areas.X() + Areas.Y() * Areas.Y() + Areas.Z() * Areas.Z();

	SUB(edge1, vert2, vert0);
	SUB(edge2, vert3, vert0);
	CROSS(Areas, edge1, edge2);
	double S2 = Areas.X() * Areas.X() + Areas.Y() * Areas.Y() + Areas.Z() * Areas.Z();

	SUB(edge1, vert3, vert0);
	SUB(edge2, vert1, vert0);
	CROSS(Areas, edge1, edge2);
	double S3 = Areas.X() * Areas.X() + Areas.Y() * Areas.Y() + Areas.Z() * Areas.Z();

	if (abs(SA - S1 - S2 - S3) > 0.01 * SA)
		return false;
	else
	{
		result = NPnt.Distance(vert0);
		return true;
	}
}

double FSurfaceArea(FSurface* paraFSurf)
{
	ON_SimpleArray<ON_MeshFace> FacetList = paraFSurf->GetMesh()->OnMesh()->m_F;
	ON_3fPointArray VertList = paraFSurf->GetMesh()->OnMesh()->m_V;

	int nSize = FacetList.size();

	function<double(ON_3fPoint, ON_3fPoint, ON_3fPoint)> TriangleArea = [&](ON_3fPoint vert1, ON_3fPoint vert2, ON_3fPoint vert3)->double
	{
		gp_Pnt vec_21 = gp_Pnt(vert1.x - vert2.x, vert1.y - vert2.y, vert1.z - vert2.z);

		gp_Pnt vec_23 = gp_Pnt(vert3.x - vert2.x, vert3.y - vert2.y, vert3.z - vert2.z);

		gp_Pnt cross_vec(0, 0, 0);

		CROSS(cross_vec, vec_21, vec_23);

		return LENGTH(cross_vec) / 2;
	};

	double AllTrianglesArea = 0;

	for (int i = 0; i < nSize; i++)
	{
		AllTrianglesArea += TriangleArea(VertList[FacetList[i].vi[0]], VertList[FacetList[i].vi[1]], VertList[FacetList[i].vi[2]]);
	}

	return AllTrianglesArea;
}

int ExNumFrQsg(QString aString)
{
	QString Number;
	for (int j = 0; j < aString.length(); j++)
	{
		if (aString[j] >= '0' && aString[j] <= '9')
			Number.append(aString[j]);
	}

	return Number.toInt();
}

double ExDouFrQsg(QString aString)
{
	QString Number;
	for (int j = 0; j < aString.length(); j++)
	{
		if ((aString[j] >= '0' && aString[j] <= '9') || (aString[j] == '.'))
			Number.append(aString[j]);
	}

	return Number.toDouble();
}

ON_NurbsSurface* OcctSurfToNurbs(Handle(Geom_BSplineSurface) theOcctSurf)
{
	int uDeg = theOcctSurf->UDegree();
	int vDeg = theOcctSurf->VDegree();
	bool isRational = false;
	for (int i = 0; i < theOcctSurf->NbUPoles(); i++)
	{
		for (int j = 0; j < theOcctSurf->NbVPoles(); j++)
		{
			if (abs(theOcctSurf->Weight(i + 1, j + 1) - 1) < 1e-7)
			{
				isRational = true;
				break;
			}
		}
	}

	ON_NurbsSurface* TheConvertedNURBS = new ON_NurbsSurface(3, isRational, uDeg, vDeg, theOcctSurf->NbUPoles(), theOcctSurf->NbVPoles());

	//设置控制点及权重
	//for (int i = 0; i < theOcctSurf->NbUPoles(); i++)
	//{
	//	for (int j = 0; j < theOcctSurf->NbVPoles(); j++)
	//	{
	//		gp_Pnt ConVert = theOcctSurf->Pole(i + 1, j + 1);
	//		if (isRational)
	//		{
	//			double tmpWeight = theOcctSurf->Weight(i + 1, j + 1);
	//			TheConvertedNURBS->SetCV(i, j, ON_4dPoint(ConVert.X() * tmpWeight, ConVert.Y() * tmpWeight, ConVert.Z() * tmpWeight, tmpWeight));
	//		}
	//		else
	//		{
	//			TheConvertedNURBS->SetCV(i, j, ON_3dPoint(ConVert.X(), ConVert.Y(), ConVert.Z()));
	//		}
	//	}
	//}

	for (int i = 0; i < theOcctSurf->NbUPoles(); i++)
	{
		for (int j = 0; j < theOcctSurf->NbVPoles(); j++)
		{
			gp_Pnt ConVert = theOcctSurf->Pole(i + 1, j + 1);
			TheConvertedNURBS->SetCV(i, j, ON_3dPoint(ConVert.X(), ConVert.Y(), ConVert.Z()));

			TheConvertedNURBS->SetWeight(i, j, theOcctSurf->Weight(i + 1, j + 1));
		}
	}

	//设置节点向量及重数
	int KnotsClock = 0;
	for (int i = 0; i < theOcctSurf->NbUKnots(); i++)
	{
		int EndsDecrease = 0;
		if (i == 0 || i == (theOcctSurf->NbUKnots() - 1))
		{
			EndsDecrease = 0;
		}
		else
		{
			EndsDecrease = 1;
		}

		for (int j = 0; j < theOcctSurf->UMultiplicity(i + 1) - EndsDecrease; j++)
		{
			TheConvertedNURBS->SetKnot(0, KnotsClock, theOcctSurf->UKnot(i + 1));
			KnotsClock += 1;
		}
	}

	KnotsClock = 0;
	for (int i = 0; i < theOcctSurf->NbVKnots(); i++)
	{
		int EndsDecrease = 0;
		if (i == 0 || i == (theOcctSurf->NbVKnots() - 1))
		{
			EndsDecrease = 0;
		}
		else
		{
			EndsDecrease = 1;
		}

		for (int j = 0; j < theOcctSurf->VMultiplicity(i + 1) - EndsDecrease; j++)
		{
			TheConvertedNURBS->SetKnot(1, KnotsClock, theOcctSurf->VKnot(i + 1));
			KnotsClock += 1;
		}
	}

	return TheConvertedNURBS;
}

Handle(Geom_BSplineSurface) NurbsToOcctSurf(const ON_NurbsSurface& theSurface)
{
	TColgp_Array2OfPnt aPoles(1, theSurface.CVCount(0), 1, theSurface.CVCount(1));
	TColStd_Array2OfReal aWeights(1, theSurface.CVCount(0), 1, theSurface.CVCount(1));

	TColStd_Array1OfReal aUKnotSequence(1, theSurface.KnotCount(0) + 2);
	TColStd_Array1OfReal aVKnotSequence(1, theSurface.KnotCount(1) + 2);

	bool IsRational = theSurface.IsRational();
	bool IsUPeriodic = (theSurface.IsPeriodic(0)) ? true : false;
	bool IsVPeriodic = (theSurface.IsPeriodic(1)) ? true : false;

	// control point and its weight.
	for (int i = 0; i < theSurface.CVCount(0); ++i)
	{
		for (int j = 0; j < theSurface.CVCount(1); ++j)
		{
			if (IsRational)
			{
				ON_4dPoint aPole;

				theSurface.GetCV(i, j, aPole);

				aPoles.SetValue(i + 1, j + 1, gp_Pnt(aPole.x / aPole.w, aPole.y / aPole.w, aPole.z / aPole.w));
				aWeights.SetValue(i + 1, j + 1, aPole.w);
			}
			else
			{
				ON_3dPoint aPole;

				theSurface.GetCV(i, j, aPole);

				aPoles.SetValue(i + 1, j + 1, gp_Pnt(aPole.x, aPole.y, aPole.z));
			}

		}
	}

	// Knot vector and its multiplicity.
	for (int i = 0; i < theSurface.KnotCount(0); ++i)
	{
		aUKnotSequence.SetValue(i + 2, theSurface.Knot(0, i));
	}

	aUKnotSequence.SetValue(aUKnotSequence.Lower(), theSurface.Knot(0, 0));
	aUKnotSequence.SetValue(aUKnotSequence.Upper(), theSurface.Knot(0, theSurface.KnotCount(0) - 1));

	TColStd_Array1OfReal aUKnots(1, BSplCLib::KnotsLength(aUKnotSequence, IsUPeriodic));
	TColStd_Array1OfInteger aUMults(1, aUKnots.Upper());

	BSplCLib::Knots(aUKnotSequence, aUKnots, aUMults);

	for (int j = 0; j < theSurface.KnotCount(1); ++j)
	{
		aVKnotSequence.SetValue(j + 2, theSurface.Knot(1, j));
	}

	aVKnotSequence.SetValue(aVKnotSequence.Lower(), theSurface.Knot(1, 0));
	aVKnotSequence.SetValue(aVKnotSequence.Upper(), theSurface.Knot(1, theSurface.KnotCount(1) - 1));

	TColStd_Array1OfReal aVKnots(1, BSplCLib::KnotsLength(aVKnotSequence, IsVPeriodic));
	TColStd_Array1OfInteger aVMults(1, aVKnots.Upper());

	BSplCLib::Knots(aVKnotSequence, aVKnots, aVMults);

	Handle_Geom_BSplineSurface aBSplineSurface = new Geom_BSplineSurface(aPoles,
		aWeights, aUKnots, aVKnots, aUMults, aVMults,
		theSurface.Degree(0), theSurface.Degree(1),
		IsUPeriodic, IsVPeriodic);

	return aBSplineSurface;
}

double Distance_IpPnt(FIpPoint Pnt1, FIpPoint Pnt2)
{
	double DeltaX = Pnt1.x - Pnt2.x;
	double DeltaY = Pnt1.y - Pnt2.y;
	double DeltaZ = Pnt1.z - Pnt2.z;

	return sqrt(DeltaX * DeltaX + DeltaY * DeltaY + DeltaZ * DeltaZ);
}

double LengthOfPath(FIpPathList ipPathList)
{
	vector<FIpPoint> thePntList;

	int pSize = ipPathList.size();

	for (auto it = ipPathList.begin(); it != ipPathList.end(); ++it)
	{
		int nSize = it->GetSize();
		for (int i = 0; i < nSize; i++)
		{
			thePntList.push_back(it->GetPoint(i));
		}
	}

	double PathLength = 0;
	for (auto it = thePntList.begin(); it != thePntList.end() - 1; ++it)
	{
		PathLength += Distance_IpPnt(*it, *(it + 1));
	}

	return PathLength;
}

bool LeastSquareFitSphere(vector<gp_Pnt> PntList, gp_Pnt& theCenter, double& theRadius)
{
	double NumOfPnts = PntList.size();

	math_Matrix aMatrix(1, 4, 1, 4);
	math_Vector b1(1, 4);

	function<double(double)> DOUIC = [&](double Aim)->double {return Aim * Aim; };

	double X = 0, Y = 0, Z = 0, XY = 0, YZ = 0, ZX = 0, XX = 0, YY = 0, ZZ = 0, XR2 = 0, YR2 = 0, ZR2 = 0, R2 = 0;

	for (int i = 0; i < NumOfPnts; i++)
	{
		X += PntList[i].X();
		Y += PntList[i].Y();
		Z += PntList[i].Z();

		XY += PntList[i].X() * PntList[i].Y();
		YZ += PntList[i].Y() * PntList[i].Z();
		ZX += PntList[i].Z() * PntList[i].X();

		XX += PntList[i].X() * PntList[i].X();
		YY += PntList[i].Y() * PntList[i].Y();
		ZZ += PntList[i].Z() * PntList[i].Z();

		XR2 += PntList[i].X() * (DOUIC(PntList[i].X()) + DOUIC(PntList[i].Y()) + DOUIC(PntList[i].Z()));
		YR2 += PntList[i].Y() * (DOUIC(PntList[i].X()) + DOUIC(PntList[i].Y()) + DOUIC(PntList[i].Z()));
		ZR2 += PntList[i].Z() * (DOUIC(PntList[i].X()) + DOUIC(PntList[i].Y()) + DOUIC(PntList[i].Z()));

		R2 += DOUIC(PntList[i].X()) + DOUIC(PntList[i].Y()) + DOUIC(PntList[i].Z());
	}

	aMatrix(1, 1) = XX;
	aMatrix(1, 2) = XY;
	aMatrix(1, 3) = ZX;
	aMatrix(1, 4) = -X;

	aMatrix(2, 1) = XY;
	aMatrix(2, 2) = YY;
	aMatrix(2, 3) = YZ;
	aMatrix(2, 4) = -Y;

	aMatrix(3, 1) = ZX;
	aMatrix(3, 2) = YZ;
	aMatrix(3, 3) = ZZ;
	aMatrix(3, 4) = -Z;

	aMatrix(4, 1) = -X;
	aMatrix(4, 2) = -Y;
	aMatrix(4, 3) = -Z;
	aMatrix(4, 4) = NumOfPnts;

	b1(1) = XR2;
	b1(2) = YR2;
	b1(3) = ZR2;
	b1(4) = -R2;

	math_Vector x1(1, 4);
	math_Gauss sol(aMatrix);
	if (sol.IsDone())
	{
		sol.Solve(b1, x1);

		theCenter.SetX(x1(1) / 2);
		theCenter.SetY(x1(2) / 2);
		theCenter.SetZ(x1(3) / 2);

		theRadius = sqrt(DOUIC(x1(1) / 2) + DOUIC(x1(2) / 2) + DOUIC(x1(3) / 2) - x1(4));

		return true;
	}
	else
	{
		try
		{
			sol.Solve(b1, x1);
		}
		catch (Standard_Failure)
		{
			Handle(Standard_Failure) error = Standard_Failure::Caught();
			cout << error << endl;
		}
	}

	return false;
}

double sci2db(const std::string& strSci)
{
	int    iPower = 0;  //幂
	double dMntsa = 0;  //尾数
	double dCoefficient = 1;  //系数

	std::string strPower, strMntsa;

	if (std::string::npos == strSci.find("E"))
	{
		return atof(strSci.c_str());
	}

	strMntsa = strSci.substr(0, strSci.find("E"));
	strPower = strSci.substr(strSci.find("E") + 1);

	dMntsa = atof(strMntsa.c_str());
	iPower = atoi(strPower.c_str());

	while (iPower != 0)
	{
		if (iPower > 0)
		{
			dCoefficient *= 10;
			iPower--;
		}
		else
		{
			dCoefficient *= 0.1;
			iPower++;
		}
	}

	return dMntsa * dCoefficient;
}

double sci2dbEX(const std::string& strSci)
{
	int    iPower = 0;  //幂
	double dMntsa = 0;  //尾数
	double dCoefficient = 1;  //系数

	std::string strPower, strMntsa;

	if (std::string::npos == strSci.find("EX"))
	{
		return atof(strSci.c_str());
	}

	strMntsa = strSci.substr(0, strSci.find("EX"));
	strPower = strSci.substr(strSci.find("EX") + 2);

	dMntsa = atof(strMntsa.c_str());
	iPower = atoi(strPower.c_str());

	while (iPower != 0)
	{
		if (iPower > 0)
		{
			dCoefficient *= 10;
			iPower--;
		}
		else
		{
			dCoefficient *= 0.1;
			iPower++;
		}
	}

	return dMntsa * dCoefficient;
}

FIpPntList readPoinsFromTXT(QString theFileName)
{
	FIpPntList ExtData;

	ifstream infile;
	infile.open(theFileName.toStdString().data());

	if (!infile.is_open())
	{
		cout << "打开文件失败" << endl;
		return ExtData;
	}

	string s;
	vector<string> vec;

	while (getline(infile, s))
	{
		vec.push_back(s);
	}

	infile.close();

	char pattern = '\t';

	for (int i = 0; i < vec.size(); i++)
	{
		stringstream input(vec.at(i));
		string temp;
		vector<string> res;

		while (getline(input, temp, pattern))
		{
			res.push_back(temp);
		}

		int Num = sci2db(res.at(0));
		double mes_x = sci2db(res.at(1));
		double mes_y = sci2db(res.at(2));
		double mes_z = sci2db(res.at(3));

		FIpPoint tmpPT = FIpPoint(mes_x, mes_y, mes_z);

		ExtData.push_back(tmpPT);
	}

	return ExtData;
}

vector<gp_Pnt> readPoinsFromTXT(QString theFileName, int x_col, int y_col, int z_col, char _sep)
{
	vector<gp_Pnt> ExtData;

	ifstream infile;
	infile.open(theFileName.toStdString().data());

	if (!infile.is_open())
	{
		cout << "打开文件失败" << endl;
		return ExtData;
	}

	string s;
	vector<string> vec;

	while (getline(infile, s))
	{
		vec.push_back(s);
	}

	infile.close();

	char pattern = _sep;

	for (int i = 0; i < vec.size(); i++)
	{
		stringstream input(vec.at(i));
		string temp;
		vector<string> res;

		while (getline(input, temp, pattern))
		{
			res.push_back(temp);
		}

		double mes_x = sci2db(res.at(x_col));
		double mes_y = sci2db(res.at(y_col));
		double mes_z = sci2db(res.at(z_col));

		gp_Pnt tmpPT = gp_Pnt(mes_x, mes_y, mes_z);

		ExtData.push_back(tmpPT);
	}

	return ExtData;
}

vector<gp_Pnt> IpPntToGpPnt(FIpPntList thePntList)
{
	vector<gp_Pnt> aimVec;

	for (auto ita = thePntList.begin(); ita != thePntList.end(); ++ita)
	{
		aimVec.push_back(gp_Pnt((*ita).x, (*ita).y, (*ita).z));
	}

	return aimVec;
}

gp_Pnt CoorOrg()
{
	FIpPntList m_DataGroup1 = readPoinsFromTXT("Data1.txt");
	FIpPntList m_DataGroup2 = readPoinsFromTXT("Data2.txt");
	FIpPntList m_DataGroup3 = readPoinsFromTXT("Data3.txt");

	function<double(vector<double>)> Avg = [](vector<double> list)->double
	{
		double Res = 0;
		for (auto ita = list.begin(); ita != list.end(); ++ita)
		{
			Res += *ita;
		}
		return Res / list.size();
	};

	function<double(int)> ft = [&](int Dir)->double//Dir=1:X   Dir=2:Y   Dir=3:Z
	{
		vector<double> List;
		if (Dir == 1)
		{
			for (auto ita = m_DataGroup1.begin(); ita != m_DataGroup1.end(); ++ita)
			{
				List.push_back((*ita).x);
			}
		}
		else if (Dir == 2)
		{
			for (auto ita = m_DataGroup2.begin(); ita != m_DataGroup2.end(); ++ita)
			{
				List.push_back((*ita).y);
			}
		}
		else
		{
			for (auto ita = m_DataGroup3.begin(); ita != m_DataGroup3.end(); ++ita)
			{
				List.push_back((*ita).z);
			}
		}

		return Avg(List);
	};

	gp_Pnt Orig = gp_Pnt(0, 0, 0);
	Orig.SetX(ft(1));
	Orig.SetY(ft(2));
	Orig.SetZ(ft(3));

	return Orig;
}

void initTree_by_insfea(FInsFeature* FinsFea, QTreeWidget* theTree)
{
	function<QString(FSurface*)> theFSurfName = [](FSurface* aFSurf)->QString
	{
		SURFACE_TYPE theSurfType;
		int ind_in_all = 0;
		int ind_in_type = 0;
		QString tmpFSurfName;

		gWindow->GetSTEPModelConverter()->FSurfInfo(aFSurf, theSurfType, ind_in_all, ind_in_type, tmpFSurfName);

		return tmpFSurfName;
	};

	function<QString(FSurface*)> theFSurfIcon = [](FSurface* aFSurf)->QString
	{
		SURFACE_TYPE theSurfType;
		int ind_in_all = 0;
		int ind_in_type = 0;
		QString tmpFSurfName;

		gWindow->GetSTEPModelConverter()->FSurfInfo(aFSurf, theSurfType, ind_in_all, ind_in_type, tmpFSurfName);

		switch (theSurfType)
		{
		case BSPLINE:return "../icons/BsplineSurface.png";
			break;
		case CONE:return "../icons/ConicalSurface.png";
			break;
		case CYLINDER:return "../icons/CylindricalSurface.png";
			break;
		case PLANE:return "../icons/PlaneSurface.png";
			break;
		case SPHERE:return "../icons/SphericalSurface.png";
			break;
		case OTHER_SURF:return "../icons/OtherSurface.png";
			break;
		default:return "../icons/OtherSurface.png";
			break;
		}
	};

	//检测特征名，主要
	QTreeWidgetItem* m_pMainItem = new QTreeWidgetItem;
	m_pMainItem->setText(0, QString::fromLocal8Bit("检测特征") + QString::number(FinsFea->InsFeatureindex));
	m_pMainItem->setData(0, Qt::UserRole, QVariant::fromValue((void*)(FinsFea)));

	//曲面，第一
	QTreeWidgetItem* m_pFaceItem = new QTreeWidgetItem;
	m_pFaceItem->setText(0, QString::fromLocal8Bit("曲面"));

	for (int i = 0; i < FinsFea->m_FSurfList1.size(); i++)
	{
		QTreeWidgetItem* TempItem = new QTreeWidgetItem;
		TempItem->setData(0, Qt::UserRole, QVariant::fromValue((void*)(FinsFea->m_FSurfList1[i])));
		TempItem->setText(0, theFSurfName(FinsFea->m_FSurfList1[i]));
		TempItem->setIcon(0, QIcon(theFSurfIcon(FinsFea->m_FSurfList1[i])));
		m_pFaceItem->addChild(TempItem);
	}

	for (int i = 0; i < FinsFea->m_FSurfList2.size(); i++)
	{
		QTreeWidgetItem* TempItem = new QTreeWidgetItem;
		TempItem->setData(0, Qt::UserRole, QVariant::fromValue((void*)(FinsFea->m_FSurfList2[i])));
		TempItem->setText(0, theFSurfName(FinsFea->m_FSurfList2[i]));
		TempItem->setIcon(0, QIcon(theFSurfIcon(FinsFea->m_FSurfList2[i])));
		m_pFaceItem->addChild(TempItem);
	}

	//检测类型，第二
	QTreeWidgetItem* m_pTypeItemMain = new QTreeWidgetItem;
	m_pTypeItemMain->setText(0, QString::fromLocal8Bit("检测类型"));
	QTreeWidgetItem* m_pTypeItem = new QTreeWidgetItem;
	switch (FinsFea->type)
	{
	case PARALLELISM:
	{
		m_pTypeItem->setText(0, QString::fromLocal8Bit("平行度"));
		break;
	}
	case ROUNDNESS:
	{
		m_pTypeItem->setText(0, QString::fromLocal8Bit("圆度"));
		break;
	}
	case DISTANCE:
	{
		if (FinsFea->DirState == 1)
		{
			m_pTypeItem->setText(0, QString::fromLocal8Bit("X向距离"));
		}
		else if (FinsFea->DirState == 2)
		{
			m_pTypeItem->setText(0, QString::fromLocal8Bit("Y向距离"));
		}
		else if (FinsFea->DirState == 3)
		{
			m_pTypeItem->setText(0, QString::fromLocal8Bit("Z向距离"));
		}
		else
		{
			m_pTypeItem->setText(0, QString::fromLocal8Bit("距离"));
		}

		break;
	}
	case VERTICALITY:
	{
		m_pTypeItem->setText(0, QString::fromLocal8Bit("垂直度"));
		break;
	}
	case DIAMETER:
	{
		m_pTypeItem->setText(0, QString::fromLocal8Bit("直径"));
		break;
	}
	case AUXIALITY:
	{
		m_pTypeItem->setText(0, QString::fromLocal8Bit("同轴度"));
		break;
	}
	case FLATNESS:
	{
		m_pTypeItem->setText(0, QString::fromLocal8Bit("平面度"));
		break;
	}
	default:
	{
		m_pTypeItem->setText(0, QString::fromLocal8Bit("未选择"));
		break;
	}
	}
	m_pTypeItemMain->addChild(m_pTypeItem);

	//检测参数，第三
	QTreeWidgetItem* m_pValueItem = new QTreeWidgetItem;
	m_pValueItem->setText(0, QString::fromLocal8Bit("参数"));

	QTreeWidgetItem* pValueItem = new QTreeWidgetItem();
	QTreeWidgetItem* pUpDeviationItem = new QTreeWidgetItem();
	QTreeWidgetItem* pDownDeviationItem = new QTreeWidgetItem();

	if (FinsFea->type == PARALLELISM)
	{
		pValueItem->setText(0, QString::fromLocal8Bit("平行度误差值：") + QString::number(FinsFea->FInsToleranceValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pValueItem);
	}
	else if (FinsFea->type == ROUNDNESS)
	{
		pValueItem->setText(0, QString::fromLocal8Bit("圆度误差值：") + QString::number(FinsFea->FInsToleranceValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pValueItem);
	}
	else if (FinsFea->type == DISTANCE)
	{

		pValueItem->setText(0, QString::fromLocal8Bit("距离标准值：") + QString::number(FinsFea->FInsToleranceValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pValueItem);
		pUpDeviationItem->setText(0, QString::fromLocal8Bit("上偏差：+") + QString::number(FinsFea->FInsUpDeviationValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pUpDeviationItem);
		pDownDeviationItem->setText(0, QString::fromLocal8Bit("下偏差：-") + QString::number(FinsFea->FInsDownDeviationValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pDownDeviationItem);
	}
	else if (FinsFea->type == VERTICALITY)
	{
		pValueItem->setText(0, QString::fromLocal8Bit("垂直度误差值：") + QString::number(FinsFea->FInsToleranceValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pValueItem);
	}
	else if (FinsFea->type == DIAMETER)
	{
		pValueItem->setText(0, QString::fromLocal8Bit("直径标准值：") + QString::number(FinsFea->FInsToleranceValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pValueItem);
		pUpDeviationItem->setText(0, QString::fromLocal8Bit("上偏差：+") + QString::number(FinsFea->FInsUpDeviationValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pUpDeviationItem);
		pDownDeviationItem->setText(0, QString::fromLocal8Bit("下偏差：-") + QString::number(FinsFea->FInsDownDeviationValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pDownDeviationItem);
	}
	else if (FinsFea->type == AUXIALITY)
	{
		pValueItem->setText(0, QString::fromLocal8Bit("同轴度误差值：") + QString::number(FinsFea->FInsToleranceValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pValueItem);
	}
	else if (FinsFea->type == FLATNESS)
	{
		pValueItem->setText(0, QString::fromLocal8Bit("平面度误差值：") + QString::number(FinsFea->FInsToleranceValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pValueItem);
	}

	m_pMainItem->addChild(m_pFaceItem);
	m_pMainItem->addChild(m_pTypeItemMain);
	m_pMainItem->addChild(m_pValueItem);

	theTree->addTopLevelItem(m_pMainItem);
	theTree->collapseAll();
}

void MesgBox(QString ShowMsge)
{
	QMessageBox::warning(
		QApplication::activeWindow(), QObject::tr("Caution"), ShowMsge);
}

void MesgBox2(const char* _chineseMsge)
{
	QString mesgC = QString::fromLocal8Bit(_chineseMsge);

	QMessageBox::warning(
		QApplication::activeWindow(), QObject::tr("Caution"), mesgC);
}

int RandSeedByTimeNow()
{
	time_t nowtime;

	time(&nowtime);

	//cout << "获取时间：" << nowtime << nowtime << endl;

	tm* p = localtime(&nowtime);
	int randseed_time = p->tm_yday + p->tm_hour + p->tm_min + p->tm_sec;
	//delete p;

	return randseed_time;
}

vector<QString> RandomTime(int Month, int Count)
{
	int DayBegin = 0, DayEnd = 0;
	if (Month == 6)
	{
		DayBegin = 6;
		DayEnd = 30;
	}
	else if (Month == 7)
	{
		DayBegin = 1;
		DayEnd = 31;
	}
	else if (Month == 8)
	{
		DayBegin = 13;
		DayEnd = 31;
	}
	else if (Month == 9)
	{
		DayBegin = 13;
		DayEnd = 29;
	}
	else if (Month == 10)
	{
		DayBegin = 8;
		DayEnd = 31;
	}
	else if (Month == 11)
	{
		DayBegin = 2;
		DayEnd = 29;
	}
	else
	{
		_DEBUG_ERROR("输入了错误的月份");
	}

	int HourBegin = 8, HourEnd = 21;

	int MinBegin = 0, MinEnd = 59;

	int SecBegin = 0, SecEnd = 59;

	default_random_engine rande(RandSeedByTimeNow());
	uniform_int_distribution<int> DayRand(DayBegin, DayEnd);
	uniform_int_distribution<int> HourRand(HourBegin, HourEnd);
	uniform_int_distribution<int> MinRand(MinBegin, MinEnd);
	uniform_int_distribution<int> SecRand(SecBegin, SecEnd);

	vector<QString> AllTime;

	for (int i = 0; i < Count; i++)
	{
		AllTime.push_back("2022-" + QString::number(Month) + "-" + QString::number(DayRand(rande)) + "-" + QString::number(HourRand(rande)) + "-" + QString::number(MinRand(rande)) + "-" + QString::number(SecRand(rande)));
	}

	return AllTime;
}

void ShowOcctEdge(TopoDS_Edge theEdge)
{
	int Sections = 100;

	// 寻找参数范围
	double pStart = 0, pEnd = 0;
	BRep_Tool::Range(theEdge, pStart, pEnd);
	vector<double> ParaList = linspace(pStart, pEnd, Sections);

	Handle(Geom_Curve) theCurve = BRep_Tool::Curve(theEdge, pStart, pEnd);

	PathShow* aEntity = new PathShow;

	for (int i = 0; i < Sections; i++)
	{
		aEntity->appendPnt(theCurve->Value(ParaList[i]));
	}

	gWindow->GetMainDoc()->AddDispEnt(aEntity);

	gWindow->UpdateModelView();
}

void ShowShapeEdge(TopoDS_Shape ShapeShow)
{
	// 先提取所有边
	for (TopExp_Explorer aExpEdge(ShapeShow, TopAbs_EDGE); aExpEdge.More(); aExpEdge.Next())
	{
		TopoDS_Edge aEdge = TopoDS::Edge(aExpEdge.Current());

		ShowOcctEdge(aEdge);
	}

	gWindow->UpdateModelView();
}

void ShowPoints(gp_Pnt fpPnt, double fpR, double fpG, double fpB, double fpPntSize)
{
	MeasPt* apointshow = new MeasPt(fpPnt.X(), fpPnt.Y(), fpPnt.Z(), 0, 0, 0, fpR, fpG, fpB, 0);
	apointshow->setPointSize(fpPntSize);

	gWindow->GetMainDoc()->AddDispEnt(apointshow);
	gWindow->UpdateModelView();

}

void ShowPoints(double fpX, double fpY, double fpZ, double fpR, double fpG, double fpB, double fpPntSize)
{
	MeasPt* apointshow = new MeasPt(fpX, fpY, fpZ, 0, 0, 0, fpR, fpG, fpB, 0);
	apointshow->setPointSize(fpPntSize);

	gWindow->GetMainDoc()->AddDispEnt(apointshow);
	gWindow->UpdateModelView();

}

void ShowPoints(vector<gp_Pnt> fpPnt, double fpR, double fpG, double fpB, double fpPntSize)
{
	for (auto ita = fpPnt.begin(); ita != fpPnt.end(); ++ita)
	{
		ShowPoints(*ita, fpR, fpG, fpB, fpPntSize);
	}

}

vector<gp_Pnt> ExWireCorners(TopoDS_Shape fpWire)
{
	// 先提取wire的所有角点






	return vector<gp_Pnt>();
}

vector<gp_Pnt> ExEdgePnts(TopoDS_Edge fpEdge, bool IsReverse, int fpSec)
{
	vector<gp_Pnt> PntList;

	double pStart = 0, pEnd = 0;
	BRep_Tool::Range(fpEdge, pStart, pEnd);
	Handle(Geom_Curve) theCurve = BRep_Tool::Curve(fpEdge, pStart, pEnd);

	vector<double> paraList;

	if (IsReverse)
	{
		paraList = linspace(pEnd, pStart, fpSec);
	}
	else
	{
		paraList = linspace(pStart, pEnd, fpSec);
	}

	for (auto ita = paraList.begin(); ita != paraList.end(); ++ita)
	{
		PntList.push_back(theCurve->Value(*ita));
	}

	return PntList;
}

Standard_Real EdgeLength(const TopoDS_Edge& edge)
{
	// 定义变量来存储属性
	GProp_GProps props;
	// 计算边的属性
	BRepGProp::LinearProperties(edge, props);
	// 获取边的长度
	return props.Mass();

}

Handle(Geom_BSplineCurve) fit_BSplineCurve(vector<gp_Pnt> DataPntList, int CurveDeg)
{
	//使用NURBS拟合曲线//////////////////////////////
	int pntNum = DataPntList.size();
	//测点表
	TColgp_Array1OfPnt PointArray(1, pntNum);

	for (int i = 0; i < pntNum; i++)
	{
		PointArray.SetValue(i + 1, DataPntList[i]);
	}

	Standard_Integer minDeg = CurveDeg;
	Standard_Integer maxDeg = CurveDeg;
	GeomAbs_Shape continuity = GeomAbs_C2;
	Standard_Real precise = 0.00001;
	Handle(Geom_BSplineCurve) approxCurve = GeomAPI_PointsToBSpline(PointArray, minDeg, maxDeg, continuity, precise);//在这里构造拟合曲线

	return approxCurve;
}

Standard_Real CurveCurvature(const Handle(Geom_BSplineCurve)& curve, const Standard_Real parameter)
{
	// 初始化曲线属性对象
	GeomLProp_CLProps props(curve, parameter, 2, 1e-6);

	return props.Curvature();
}

void ShowOcctCurve(Handle(Geom_BSplineCurve) theCurve)
{
	int Sections = 501;

	double U_Start = theCurve->Knot(theCurve->FirstUKnotIndex());
	double U_End = theCurve->Knot(theCurve->LastUKnotIndex());

	vector<double> ParaList = linspace(U_Start, U_End, Sections);
	PathShow* aEntity = new PathShow;

	for (int i = 0; i < Sections; i++)
	{
		aEntity->appendPnt(theCurve->Value(ParaList[i]));
	}

	gWindow->GetMainDoc()->AddDispEnt(aEntity);
}

gp_Vec NormalAt(TopoDS_Face fpFace, gp_Pnt fpPnt)
{
	Handle_Geom_Surface surface = BRep_Tool::Surface(fpFace);
	gp_Vec normal(0, 0, 0);

	const gp_Pnt& vertex = fpPnt;
	GeomAPI_ProjectPointOnSurf SrfProp(vertex, surface);

	Standard_Real fU, fV;
	SrfProp.Parameters(1, fU, fV);

	GeomLProp_SLProps faceprop(surface, fU, fV, 2, gp::Resolution());

	if (!faceprop.IsNormalDefined())
	{
		return normal;
	}

	normal = faceprop.Normal();

	bool reversed = (fpFace.Orientation() == TopAbs_REVERSED);
	if (reversed)
	{
		normal *= -1;
	}

	if (normal.SquareMagnitude() > 1.0e-10) {
		normal.Normalize();
	}

	return normal;
}

//bool IsPntInFace(TopoDS_Face face, gp_Pnt aPnt, double prec)
//{
//	if (DistPoint2Shape(aPnt, face) <= prec)
//	{
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//
//}

//bool IsPntInFace(Handle(Poly_Triangulation) facing, gp_Pnt aPnt, double prec)
//{
//	//check mesh status
//	if (facing.IsNull()) {
//		std::cout << "No triangulation created" << endl;
//		return NULL;
//	}
//
//	set<int> all_isin;
//
//	//getting the number of triangles, nodes(vertices) & whether the face has normals
//	int faceNum = facing->NbTriangles();
//	int vertexNum = facing->NbNodes();
//	bool hasNorm = facing->HasNormals();
//
//	//getting the nodes list
//	TColgp_Array1OfPnt tPnts(1, vertexNum);
//	tPnts = facing->Nodes();
//
//	//transfer nodes in occt into vertices in openNurbs/////////////////////////////////////////转移角点
//	for (int i = 1; i <= vertexNum; i++)/////////////////////////////////////////角点表从一开始
//	{
//		double x = tPnts.Value(i).X();
//		double y = tPnts.Value(i).Y();
//		double z = tPnts.Value(i).Z();
//
//		ON_3dPoint point = ON_3dPoint(x, y, z);
//	}
//
//	//getting the triangle list (poly_lists)//////////////////////////////////////////////三角面片表
//	Poly_Array1OfTriangle tTri(1, faceNum);
//	tTri = facing->Triangles();
//
//	//transfer triangles in occt into triangles in openNurbs
//	for (int j = 1; j <= faceNum; j++)
//	{
//		int n1, n2, n3;
//		tTri.Value(j).Get(n1, n2, n3);
//
//		double tmpdis = 0;
//
//		all_isin.insert(DPointToTriangle(tmpdis, aPnt, tPnts.Value(n1), tPnts.Value(n2), tPnts.Value(n3), prec));
//	}
//
//	return *(--all_isin.end());
//}

bool DPointToTriangle(double& result, gp_Pnt orig, gp_Pnt vert0, gp_Pnt vert1, gp_Pnt vert2, double prec)
{
	gp_Pnt edge1(0.0, 0.0, 0.0);
	gp_Pnt edge2(0.0, 0.0, 0.0);
	SUB(edge1, vert1, vert0);
	SUB(edge2, vert2, vert0);
	gp_Pnt pvec(0.0, 0.0, 0.0);
	gp_Pnt dir(0.0, 0.0, 0.0);
	CROSS(dir, edge1, edge2);
	CROSS(pvec, dir, edge2);
	double det = DOT(edge1, pvec);
	//if (abs(det) < 0.00001)
	//	return false;

	gp_Pnt tvec(0.0, 0.0, 0.0);
	SUB(tvec, orig, vert0);
	double inv_det = 1.0 / det;
	gp_Pnt qvec(0.0, 0.0, 0.0);
	CROSS(qvec, tvec, edge1);

	double u = DOT(tvec, pvec);
	double v = DOT(dir, qvec);
	u *= inv_det;
	v *= inv_det;
	if (u >= -prec && v >= -prec && (u + v) <= 1.0 + 2 * prec)
	{
		gp_Pnt thispoint(0.0, 0.0, 0.0);
		thispoint.SetX((1 - u - v) * vert0.X() + u * vert1.X() + v * vert2.X());
		thispoint.SetY((1 - u - v) * vert0.Y() + u * vert1.Y() + v * vert2.Y());
		thispoint.SetZ((1 - u - v) * vert0.Z() + u * vert1.Z() + v * vert2.Z());
		result = sqrt(pow(thispoint.X() - orig.X(), 2) + pow(thispoint.Y() - orig.Y(), 2) + pow(thispoint.Z() - orig.Z(), 2));
		return true;
	}
	else
		return false;
}

double DistPoint2Shape(const gp_Pnt& fpPoint, const TopoDS_Shape& fpShape)
{
	TopoDS_Vertex shapePoint = BRepBuilderAPI_MakeVertex(fpPoint);
	// 计算点到边的最短距离
	BRepExtrema_DistShapeShape extrema(shapePoint, fpShape);
	extrema.Perform();
	if (!extrema.IsDone() || extrema.NbSolution() == 0) {
		std::cerr << "Error: Failed to compute distance." << std::endl;
		return -1.0;
	}

	// 找到最近点
	const gp_Pnt& nearestPoint = extrema.PointOnShape2(1);

	// 计算点到最近点的距离
	return fpPoint.Distance(nearestPoint);
}

vector<gp_Pnt> mix_sampling(Handle(Geom_BSplineCurve) theCurve, int theNum, double CurWeight)
{
	Handle(Geom_BSplineCurve) OcctCurve = theCurve;
	int sam_nu = theNum;
	double uStart = theCurve->Knot(theCurve->FirstUKnotIndex());
	double uEnd = theCurve->Knot(theCurve->LastUKnotIndex());
	double weight_curv = CurWeight;

	int Cur_section = 500;

	vector<double> x_para = linspace(uStart, uEnd, Cur_section);
	vector<double> x_orig;
	vector<double> y_orig;
	for (int i3 = 0; i3 < Cur_section; i3++)
	{
		x_orig.push_back(OcctCurve->Value(x_para[i3]).X());
		y_orig.push_back(OcctCurve->Value(x_para[i3]).Y());
	}

	clock_t startTime, endTime;
	startTime = clock();//计时开始

	int section_reCurve = 100;//重新参数化曲线精度
	vector<double>reCurve_u = linspace(uStart, uEnd, section_reCurve);
	vector<double>reCurve_value;

	function<gp_Vec(double)> Derivative1 = [&](double u)->gp_Vec
	{
		gp_Pnt pnt = gp_Pnt(0, 0, 0);
		gp_Vec vec = gp_Vec(0, 0, 0);
		OcctCurve->D1(u, pnt, vec);
		return vec;
	};

	function<gp_Vec(double)> Derivative2 = [&](double u)->gp_Vec
	{
		gp_Pnt pnt = gp_Pnt(0, 0, 0);
		gp_Vec vec1 = gp_Vec(0, 0, 0);
		gp_Vec vec2 = gp_Vec(0, 0, 0);
		OcctCurve->D2(u, pnt, vec1, vec2);
		return vec2;
	};

	function<double(double)> length = [&](double u)->double
	{
		if (u == uStart)
			return 0;

		int section_inte = int(((u - uStart) / (uEnd - uStart)) * 200);

		//int section_inte = 100;
		double ArcLength = 0;
		double pace = (u - uStart) / (section_inte - 1);
		vector<double> para = linspace(uStart, u, section_inte);

		double tmpInterItem = 0;
		for (int i = 1; i < section_inte - 1; i++)
		{
			tmpInterItem += LEN(Derivative1(para[i]));
		}
		ArcLength = (pace / 2) * (LEN(Derivative1(uStart)) + 2 * tmpInterItem + LEN(Derivative1(u)));

		return ArcLength;
	};

	function<double(double)> k_curvature = [&](double u)->double
	{
		gp_Vec dest = gp_Vec(0, 0, 0);
		CROSS(dest, Derivative1(u), Derivative2(u));
		double tmp1 = LEN(dest);
		return tmp1 / pow(LEN(Derivative1(u)), 3);
	};

	function<double(double)> k_pt = [&](double u)->double
	{
		if (u == uStart)
			return 0;

		int section_inte = int(((u - uStart) / (uEnd - uStart)) * 200);
		//int section_inte = 100;
		double tmp = 0;
		double pace = (u - uStart) / (section_inte - 1);
		vector<double> para = linspace(uStart, u, section_inte);

		double tmpInterItem = 0;//复合梯形公式中间项
		for (int i = 1; i < section_inte - 1; i++)
		{
			tmpInterItem += k_curvature(para[i]) * LEN(Derivative1(para[i]));
		}

		tmp = (pace / 2) * (k_curvature(uStart) * LEN(Derivative1(uStart)) + 2 * tmpInterItem + k_curvature(u) * LEN(Derivative1(u)));

		return tmp;
	};

	function<double(double)> rePara = [&](double u)->double
	{
		return (1 - weight_curv) * (length(u) / length(uEnd)) + weight_curv * (k_pt(u) / k_pt(uEnd));
	};

	for (int i = 0; i < section_reCurve; i++)
	{
		reCurve_value.push_back(rePara(reCurve_u[i]));
	}

	vector<double>curve_para_rev = linspace(0, 1, sam_nu);
	vector<double>para_resam_u = reverse(curve_para_rev, reCurve_value, reCurve_u);
	//cout << "混合第2个参数" << para_resam_u[1] << endl;
	//para_resam_u[1] = 0.6;
	//vector<double>para_uniform_u = linspace(uStart, uEnd, 10);//测试

	vector<double>mix_x;
	vector<double>mix_y;

	vector<gp_Pnt>theMixPnts;
	vector<double>theMixPara;

	vector<gp_Pnt>MeasPnts;

	ofstream of("ZSamplingpntdata.txt");//输出测点位置
	for (int i = 0; i < para_resam_u.size(); i++)
	{
		theMixPara.push_back(para_resam_u[i]);
		gp_Pnt my_pnt = OcctCurve->Value(para_resam_u[i]);

		MeasPnts.push_back(my_pnt);

		theMixPnts.push_back(my_pnt);
		//testpnt* dPnt = new testpnt(my_pnt.X(), my_pnt.Y(), my_pnt.Z());
		//gWindow->GetMainDoc()->AddDispEnt(dPnt);
		//存放混合布点数据
		mix_x.push_back(my_pnt.X());
		mix_y.push_back(my_pnt.Y());
		of << my_pnt.X() << "	" << my_pnt.Y() << endl;
	}

	return theMixPnts;
}

Handle(Geom_BSplineSurface) ConvertFaceToBSplineSurface(const TopoDS_Face& face)
{
	// 从面提取基础几何体
	Handle(Geom_Surface) surface = BRep_Tool::Surface(face);

	// 检查几何体是否为 B样条曲面
	Handle(Geom_BSplineSurface) bsplineSurface = Handle(Geom_BSplineSurface)::DownCast(surface);

	// 如果是 B样条曲面，直接返回
	if (!bsplineSurface.IsNull()) {
		return bsplineSurface;
	}

	// 如果不是 B样条曲面，将其转换为 B样条曲面

	// 先转换为裁剪曲面
	Handle_Standard_Type type = surface->DynamicType();

	//裁剪无界曲面
	Standard_Real Umin, Umax, Vmin, Vmax;
	ShapeAnalysis::GetFaceUVBounds(face, Umin, Umax, Vmin, Vmax);

	if (type == STANDARD_TYPE(Geom_Plane))
	{
		surface = new Geom_RectangularTrimmedSurface(surface, Umin, Umax, Vmin, Vmax, Standard_True, Standard_True);
	}
	else if (type == STANDARD_TYPE(Geom_CylindricalSurface) || type == STANDARD_TYPE(Geom_ConicalSurface))
	{
		surface = new Geom_RectangularTrimmedSurface(surface, 0, 2 * M_PI, -10000, 10000, Standard_True, Standard_True);
	}

	bsplineSurface = GeomConvert::SurfaceToBSplineSurface(surface);

	// 返回转换后的 B样条曲面
	return bsplineSurface;

}

FSurface* BSpSurf2FSurf(Handle(Geom_BSplineSurface) fpBspSurf)
{
	TopoDS_Face TDSFace = BRepBuilderAPI_MakeFace(fpBspSurf, Precision::Confusion());

	BRepTools::Clean(TDSFace);
	Standard_Real aDeflection = 0.1;//初始0.1，数值越小Mesh精度越高
	BRepMesh_IncrementalMesh(TDSFace, aDeflection);

	ON_Mesh* pON_Mesh = NULL;

	//check the face status
	if (TDSFace.IsNull()) {
		std::cout << "Face is Null" << endl;
		return NULL;
	}

	bool reversed = (TDSFace.Orientation() == TopAbs_REVERSED);

	//setting triangulation in occt
	TopLoc_Location L;
	Handle(Poly_Triangulation) facing = BRep_Tool::Triangulation(TDSFace, L);

	//check mesh status
	if (facing.IsNull()) {
		std::cout << "No triangulation created" << endl;
		return NULL;
	}

	//getting the number of triangles, nodes(vertices) & whether the face has normals
	int faceNum = facing->NbTriangles();
	int vertexNum = facing->NbNodes();
	bool hasNorm = facing->HasNormals();

	//situation that the triangluation has normals
	if (hasNorm == true)
	{
		//according to the info above to generate an ON_Mesh object
		pON_Mesh = new ON_Mesh(faceNum, vertexNum, hasNorm, false);

		//getting the nodes list
		TColgp_Array1OfPnt tPnts(1, vertexNum);
		tPnts = facing->Nodes();
		gp_Trsf aTrsf = L.Transformation();

		//transfer nodes in occt into vertices in openNurbs
		for (int i = 1; i <= vertexNum; i++)
		{
			gp_Pnt pnt = tPnts.Value(i).Transformed(aTrsf);

			double x = pnt.X();
			double y = pnt.Y();
			double z = pnt.Z();

			ON_3dPoint point = ON_3dPoint(x, y, z);

			pON_Mesh->SetVertex(i - 1, point);

		}

		//getting the triangle list (poly_lists)
		Poly_Array1OfTriangle tTri(1, faceNum);
		tTri = facing->Triangles();

		//transfer triangles in occt into triangles in openNurbs
		for (int j = 1; j <= faceNum; j++)
		{
			int n1, n2, n3;
			tTri.Value(j).Get(n1, n2, n3);

			pON_Mesh->SetTriangle(j - 1, n1 - 1, n2 - 1, n3 - 1);
		}

		//getting the normal talbe if normal exists
		if (facing->HasNormals())
		{
			const TShort_Array1OfShortReal& tNor = facing->Normals();

			//transfer node normals into openNurbs

			for (int i = 0; i < vertexNum; i++)
			{
				const Standard_ShortReal& nx = tNor(i * 3 + 1);
				const Standard_ShortReal& ny = tNor(i * 3 + 2);
				const Standard_ShortReal& nz = tNor(i * 3 + 3);

				double NX, NY, NZ;
				NX = nx; NY = ny; NZ = nz;

				ON_3dVector vec = ON_3dVector(NX, NY, NZ);

				pON_Mesh->SetVertexNormal(i, vec);
			}

		}
	}

	//situation that the normals need to be calculated
	else
	{
		//according to the info above to generate an ON_Mesh object
		pON_Mesh = new ON_Mesh(faceNum, vertexNum, true, false);

		//getting the nodes list
		TColgp_Array1OfPnt tPnts(1, vertexNum);
		tPnts = facing->Nodes();
		gp_Trsf aTrsf = L.Transformation();

		//transfer nodes in occt into vertices in openNurbs
		for (int i = 1; i <= vertexNum; i++)
		{
			gp_Pnt pnt = tPnts.Value(i).Transformed(aTrsf);

			double x = pnt.X();
			double y = pnt.Y();
			double z = pnt.Z();

			ON_3dPoint point = ON_3dPoint(x, y, z);

			pON_Mesh->SetVertex(i - 1, point);

		}

		//getting the triangle list (poly_lists)
		Poly_Array1OfTriangle tTri(1, faceNum);
		tTri = facing->Triangles();

		//transfer triangles in occt into triangles in openNurbs
		for (int j = 1; j <= faceNum; j++)
		{
			int n1, n2, n3;
			tTri.Value(j).Get(n1, n2, n3);

			if (reversed)
			{
				pON_Mesh->SetTriangle(j - 1, n2 - 1, n1 - 1, n3 - 1);
			}

			else
			{
				pON_Mesh->SetTriangle(j - 1, n1 - 1, n2 - 1, n3 - 1);
			}

		}

		//calculating the node normals
		Handle_Geom_Surface surface = BRep_Tool::Surface(TDSFace);
		gp_Vec normal;

		//vertex normal calculating method for bounded surfaces
		if (facing->HasUVNodes()) {

			const TColgp_Array1OfPnt2d& uvnodes = facing->UVNodes();
			for (int i = 0; i < vertexNum; i++) {

				const gp_Pnt2d& uv = uvnodes(i + 1);
				double fU = uv.X();
				double fV = uv.Y();

				GeomLProp_SLProps faceprop(surface, fU, fV, 2, gp::Resolution());
				if (!faceprop.IsNormalDefined())
				{
					continue;
				}
				normal = faceprop.Normal();

				if (normal.SquareMagnitude() > 1.0e-10) {
					normal.Normalize();
				}

				double nx, ny, nz;

				if (reversed)
				{
					normal *= -1.0;
				}

				nx = normal.X(); ny = normal.Y(); nz = normal.Z();

				ON_3dVector vec = ON_3dVector(nx, ny, nz);
				pON_Mesh->SetVertexNormal(i, vec);
			}
		}

		else {
			for (int i = 0; i < vertexNum; i++) {

				const gp_Pnt& vertex = tPnts(i + 1);
				GeomAPI_ProjectPointOnSurf SrfProp(vertex, surface);

				Standard_Real fU, fV;
				SrfProp.Parameters(1, fU, fV);

				GeomLProp_SLProps faceprop(surface, fU, fV, 2, gp::Resolution());


				if (!faceprop.IsNormalDefined())
				{
					continue;
				}

				normal = faceprop.Normal();
				if (normal.SquareMagnitude() > 1.0e-10) {
					normal.Normalize();
				}

				if (reversed)
				{
					normal *= -1.0;
				}

				double nx, ny, nz;

				nx = normal.X(); ny = normal.Y(); nz = normal.Z();
				ON_3dVector vec = ON_3dVector(nx, ny, nz);
				pON_Mesh->SetVertexNormal(i, vec);

			}
		}
	}

	if (pON_Mesh == NULL || !pON_Mesh->IsValid())
	{
		return NULL;
	}

	FMesh* pMesh = new FMesh();
	pMesh->SetOnMesh(pON_Mesh);
	FSurface* pSurf = new FSurface(pMesh);
	pSurf->Update();

	FFace* pFace = new FFace();
	FBody* m_pBody = new FBody();
	m_pBody->AddFace(pFace);

	pFace->SetSurface(pSurf);

	return pSurf;

}

void SaveToStepFile(Handle(Geom_BSplineSurface) TheSurf, Standard_CString filename)
{
	TopoDS_Face Face = BRepBuilderAPI_MakeFace(TheSurf, Precision::Confusion());

	Handle(TopTools_HSequenceOfShape) aHSequenceOfShape = new TopTools_HSequenceOfShape;
	aHSequenceOfShape->Clear();
	aHSequenceOfShape->Append(Face);

	IFSelect_ReturnStatus status;

	STEPControl_StepModelType aValue = STEPControl_AsIs;
	STEPControl_Writer aWriter;

	for (Standard_Integer i = 1; i <= aHSequenceOfShape->Length(); i++)
	{
		//转换三维模型到aWriter
		status = aWriter.Transfer(aHSequenceOfShape->Value(i), aValue);
		if (status != IFSelect_RetDone)
			std::cout << "转化失败" << endl;
	}

	if (aWriter.Model().IsNull())
	{
		std::cout << "无任何形状可保存" << endl;
		return;
	}

	//保存数据到磁盘
	status = aWriter.Write(filename);
	
}

bool isPointInShape(const gp_Pnt& fpPnt, const TopoDS_Shape& fpShape, double Tol)
{
	double distTo = DistPoint2Shape(fpPnt, fpShape);

	if (distTo <= Tol) return true;
	else return false;

}

bool isPointInFace(const gp_Pnt& fpPnt, FSurface* fpShape, double Tol)
{
	TopoDS_Face tFace = gWindow->GetOccConverter()->FSurf2TDSFace(fpShape);

	return isPointInShape(fpPnt, tFace, Tol);

}

bool isPointInFace(const gp_Pnt& fpPnt, const unordered_set<FSurface*>& fpShape, double Tol)
{
	for (auto ita = fpShape.begin(); ita != fpShape.end(); ++ita)
	{
		if (isPointInFace(fpPnt, *ita, Tol))
		{
			return true;
		}
	}

	return false;
}

Handle(Geom_BSplineSurface) fit_BSplSurfLeastSquare(const vector<gp_Pnt>& fpPointsAll)
{
	if (fpPointsAll.size() < 3)
	{
		MesgBox(QString::fromLocal8Bit("点数小于3，无法重构曲面"));
		return NULL;
	}

	// 构建板式曲面
	GeomPlate_BuildPlateSurface plateSurface;

	for (auto ita = fpPointsAll.begin(); ita != fpPointsAll.end(); ++ita)
	{
		plateSurface.Add(new GeomPlate_PointConstraint(*ita, -1));
	}

	plateSurface.Perform();

	if (plateSurface.IsDone())
	{
		// 拟合曲面
		GeomPlate_MakeApprox approx = GeomPlate_MakeApprox(plateSurface.Surface(), 0.00001, 30, 17, 0, -1, GeomAbs_C2, 1.3);
		Handle(Geom_BSplineSurface) surface = approx.Surface();
		return surface;
	}
	else
	{
		MesgBox(QString::fromLocal8Bit("曲面重构失败"));
		return NULL;
	}

}

QString FileToString(QString fpFilePath)
{
	QFile file(fpFilePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "Cannot open file for reading:" << file.errorString();
	}

	QTextStream in(&file);
	QString content = in.readAll();
	file.close();

	return content;
}

void SurfParaSpace(Handle(Geom_BSplineSurface) theSurf, double& uStart, double& uEnd, double& vStart, double& vEnd)
{
	uStart = theSurf->UKnot(theSurf->FirstUKnotIndex());
	uEnd = theSurf->UKnot(theSurf->LastUKnotIndex());
	vStart = theSurf->VKnot(theSurf->FirstVKnotIndex());
	vEnd = theSurf->VKnot(theSurf->LastVKnotIndex());
}

bool isSuffixXXX(string _filename, string _suffix)
{
	// 找到最后一个点的位置
	size_t dotPosition = _filename.find_last_of('.');

	// 如果没有找到点，或者点是路径中的第一个字符，返回空字符串
	if (dotPosition == std::string::npos || dotPosition == 0)
	{
		cout << "未发现后缀" << endl;

		return false;
	}

	// 返回从最后一个点到字符串结尾的子串
	string file_suffix = _filename.substr(dotPosition + 1);

	if (file_suffix == _suffix)
	{
		return true;
	}
	else
	{
		return false;
	}

}

bool isFaceIntLineseg(const TopoDS_Face& _face, const gp_Pnt& _startPnt, const gp_Pnt& _endPnt, const double& _min_dist)
{
	TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(_startPnt, _endPnt);

	// 计算线段和面的距离
	BRepExtrema_DistShapeShape dist(_face, edge, Extrema_ExtFlag_MIN);

	// 检查是否有交点，最近距离为零则有交点
	if (dist.Value() < PRECISION + _min_dist)
	{
		return true;

	}
	else
	{
		return false;
	}

}

bool isIntersectToModel(const gp_Pnt& _startPnt, const gp_Pnt& _endPnt, const double& _min_dist)
{
	//clock_t start = clock();

	vector<TopoDS_Face> stepFaceAll = gWindow->GetOccConverter()->GetTopoDS_FaceList();

	for (auto ita = stepFaceAll.begin(); ita != stepFaceAll.end(); ++ita)
	{
		if (isFaceIntLineseg(*ita, _startPnt, _endPnt, _min_dist))
		{
			return true;
		}

	}

	//clock_t end = clock();
	//double duration = static_cast<double>(end - start) / CLOCKS_PER_SEC;
	//std::cout << "判断相交时间: " << duration << " seconds" << std::endl;

	return false;

}

void faceBndUseVert(const TopoDS_Face& _F, Standard_Real& _UMin, Standard_Real& _UMax, Standard_Real& _VMin, Standard_Real& _VMax)
{
	// 先提取全部的 vertex
	set<double> uVertParas;
	set<double> vVertParas;

	Handle(Geom_Surface) surface = BRep_Tool::Surface(_F);

	for (TopExp_Explorer aExpVert(_F, TopAbs_VERTEX); aExpVert.More(); aExpVert.Next())
	{
		TopoDS_Vertex aVert = TopoDS::Vertex(aExpVert.Current());
		gp_Pnt point = BRep_Tool::Pnt(aVert);

		ShowPoints(point);

		GeomAPI_ProjectPointOnSurf SrfProp(point, surface);
		Standard_Real fU, fV;
		SrfProp.Parameters(1, fU, fV);

		cout << "距离对比" << surface->Value(fU, fV).Distance(point) << endl;

		uVertParas.insert(fU);
		vVertParas.insert(fV);

	}

	_UMin = *uVertParas.begin();
	_UMax = *(--uVertParas.end());

	_VMin = *vVertParas.begin();
	_VMax = *(--vVertParas.end());

}

vector<double> Hammersley(int _N)
{
	vector<double> t;

	if (_N == 1)
	{
		t.push_back(0.0);
		return t;
	}
	else if (_N <= 0)
	{
		cout << "生成HAMMERSLEY序列时输入了负数" << endl;
		return t;
	}

	int tempN = _N - 1;
	int length = 0;   //lenth=k
	while (tempN)
	{
		length++;
		tempN >>= 1;
	}
	int di = 1;
	di <<= length;
	for (unsigned int j = 0; j < _N; j++)
	{
		unsigned int n = j;

		unsigned int rn = 0;
		for (int i = 0; i < length; i++)
		{
			rn <<= 1;
			rn += (n & 0x01);
			n >>= 1;

		}

		t.push_back(rn / static_cast<double>(di));
	}

	// 归一化
	double MaxVal = MAX(t);

	// 最大值不等于 1 的时候才除
	if (abs(MaxVal - 1.0) > PRECISION)
	{
		for (int i = 0; i < N; i++)
		{
			t[i] /= MaxVal;
		}
	}

	return t;
}

bool isSurfType(FSurface* _surf, const SURFACE_TYPE& _target)
{
	SURFACE_TYPE the_type;
	int ind_in_all = 0;
	int ind_in_type = 0;
	QString tmpFSurfName;

	gWindow->GetSTEPModelConverter()->FSurfInfo(_surf, the_type, ind_in_all, ind_in_type, tmpFSurfName);

	if (the_type == _target)
	{
		return true;
	}
	else
	{
		return false;
	}

}

void removeListItem(QListWidget* _widget, FSurface* _surf)
{
	// 遍历 QListWidget
	int itemCount = _widget->count();
	for (int i = 0; i < itemCount; ++i) {
		QListWidgetItem* item = _widget->item(i);

		FSurface* itemSurf = (FSurface*)item->data(Qt::UserRole).value<void*>();
		if (itemSurf == _surf)
		{
			_widget->takeItem(i);
			break;
		}
	}
}

double zeroLim(double _val)
{
	if (abs(_val) < PRECISION)
	{
		return 0;
	}
	else
	{
		return _val;
	}

}

gp_Pnt massCenter(vector<gp_Pnt> _points)
{
	gp_Vec sum(0, 0, 0);
	for (std::vector<gp_Pnt>::const_iterator it = _points.begin(); it != _points.end(); ++it) {
		sum.Add(gp_Vec(it->X(), it->Y(), it->Z()));
	}
	sum.Divide(_points.size());
	gp_Pnt centroid = gp_Pnt(sum.X(), sum.Y(), sum.Z());

	return centroid;

}

gp_Pln extractPln(const TopoDS_Face& _face)
{
	Handle(Geom_Surface) surface = BRep_Tool::Surface(_face);
	Handle(Geom_Plane) geomPlane = Handle(Geom_Plane)::DownCast(surface);

	if (geomPlane.IsNull())
	{
		MesgBox2("输入的面不具备平面类型！");
		return gp_Pln();
	}

	gp_Pln basePln = geomPlane->Pln();

	return basePln;
}


