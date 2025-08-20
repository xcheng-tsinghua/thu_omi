#pragma once
#include <vector>
//#include "stable.h"
#include <math.h>

#include "../External/fkernel/Include/FKernelDefines.h"
#include "qwidget.h"
#include <conio.h>
#include "../External/fkernel/Include/FKernelHeader.h"
#include "../External/fkernel/Include/DataStructure/FBody.h"
#include "../External/fkernel/Include/DataStructure/FCurve.h"
#include "../External/fkernel/Include//Features/FFreeCurve.h"
#include "../External/fkernel/Include/Features/FExtend.h"
#include "FTest.h"

#include <BRepMesh_IncrementalMesh.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <Poly_Triangulation.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomPlate_BuildPlateSurface.hxx>
#include <GeomPlate_PointConstraint.hxx>
#include <GeomPlate_MakeApprox.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <ShapeFix_Shape.hxx>
#include <Standard_Type.hxx>
#include <GeomConvert.hxx>
#include <Geom2dConvert.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TShort_Array1OfShortReal.hxx>
#include <ShapeAnalysis.hxx>

#include <BSplCLib.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <GeomTools_SurfaceSet.hxx>
#include <TopoDS_Face.hxx>
#include <Precision.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

#include <GeomAPI_PointsToBSplineSurface.hxx>

#include <qprogressdialog.h>
#include "FGuideDataType.h"
#include "MeasPt.h"
//#include "FiveAxisPathPlan.h"//测点类

using namespace std;

class SamplingFreeSurf
{
public:
	SamplingFreeSurf();
	~SamplingFreeSurf();

	/// 本函数用于自动布点，提供三种布点方法
	/// 各参数意义如下：
	/// TopoDS_Face TDSFace：需要布点的曲面
	/// int SamplingMode：布点模式，SamplingMode=1:参数均匀，=2:空间均匀，=3:混合布点
	/// int NbSamp_u：u向分段数
	/// int NbSamp_v：v向分段数
	/// double CurvWeight：混合布点曲率权重，在0到1之间，默认值0.5
	void onArrange(TopoDS_Face TDSFace, int SamplingMode, int NbSamp_u, int NbSamp_v, double CurvWeight=0.5);//SamplingMode=1:参数均匀，=2:空间均匀，3:混合
	void setMarginalRetreat(double _retreat);
	void setNormReverse(double _reverse);

	vector<gp_Pnt> getSamplesAll();

private:
	void initialize();
	void integral_initialize();
	
	double Au_calculate(int u_index);//0<=u_index<=nu
	double Av_calculate(int v_index);//0<=v_index<=nv

	double curvpara_k(int u_index, int v_index);
	double ku_calculate(int u_index);
	double kv_calculate(int v_index);

	double pu(double para_u);//essential
	double pv(double para_v);//essential

	////////////////////////////////////////////////////////////////////////
	double Au(double para_u);
	double Av(double para_v);
	////////////////////////////////////////////////////////////////////////just for mean area sampling

	double D1u(double para_u, double para_v,int axis=1);//axis=1:return x;axis=2:return y;axis=3:return z
	double D1v(double para_u, double para_v,int axis=1);

	double D2u(double para_u, double para_v, int axis=1);
	double D2v(double para_u, double para_v, int axis=1);
	double D2uv(double para_u, double para_v, int axis=1);

	void mean_sampling();
	void mix_sampling();
	void mean_area_sampling();

	Handle(Geom_BSplineSurface) pOCCT_Surface;

	vector<double>curve_parau;
	vector<double>curve_resu;

	vector<double>curve_parav;
	vector<double>curve_resv;/////////////////////////////////////参数重新分布

	double ustart;
	double uend;
	double vstart;
	double vend;

	vector<double>u;
	vector<double>v;
	vector<vector<double>>rux;
	vector<vector<double>>ruy;
	vector<vector<double>>ruz;
	vector<vector<double>>rvx;
	vector<vector<double>>rvy;
	vector<vector<double>>rvz;
	double hu;
	double hv;
	double area_doun_u;
	double curv_down_u;
	double area_doun_v;
	double curv_down_v;
	int nu;///////////////////
	int nv;///////////////////计算二重积分的网格密度，该值越大计算二重积分精度越高

	int curve_parau_num;
	int curve_parav_num;//计算反函数曲线对应u，v的分段数，该值越大计算反函数精度越高

	int sam_nu;
	int sam_nv;//////////////////布点分段数

	double weight;///////曲率权重,在0和1之间

	TopoDS_Face m_SelFace;

	double MarginalRetreat;//由于曲面边缘无法检测的情况，布点时向内偏移量，读取的曲面一般在0到0.5之间

	double NormReverse;

	// 采样获得的点
	vector<gp_Pnt> m_samples;

};