#include "SamplingFreeSurf.h"
#include "MainDoc.h"
#include "FMainWindow.h"
#include "np.h"
#include "MeasPt.h"
#include "BRep_Tool.hxx"

extern FMainWindow* gWindow;

using namespace std;
using namespace FT;

SamplingFreeSurf::SamplingFreeSurf()
{
	initialize();
}

SamplingFreeSurf::~SamplingFreeSurf()
{

}

void SamplingFreeSurf::initialize()
{
	curve_parau_num = 61;
	curve_parav_num = 61;/////////反函数精度

	nu = 100;
	nv = 100;///////////二重积分网格密度

	MarginalRetreat = 0.05;
}

void SamplingFreeSurf::integral_initialize()
{
	rux.clear();
	ruy.clear();
	ruz.clear();
	rvx.clear();
	rvy.clear();
	rvz.clear();

	ustart = pOCCT_Surface->UKnot(pOCCT_Surface->FirstUKnotIndex());
	uend = pOCCT_Surface->UKnot(pOCCT_Surface->LastUKnotIndex());
	cout << ustart << endl << uend << endl;/////////////////////

	vstart = pOCCT_Surface->VKnot(pOCCT_Surface->FirstVKnotIndex());
	vend = pOCCT_Surface->VKnot(pOCCT_Surface->LastVKnotIndex());
	cout << vstart << endl << vend << endl;//////////////////////

	hu = (uend - ustart) / nu;
	hv = (vend - vstart) / nv;

	u = linspace(0, uend, nu + 1);
	v = linspace(0, vend, nv + 1);

	vector<double>ass;

	for (int i = 0; i < nu + 1; i++)
	{
		for (int j = 0; j < nv + 1; j++)
		{
			ass.push_back(D1u(u[i], v[j]));
		}
		rux.push_back(ass);
		ass.clear();
	}

	for (int i = 0; i < nu + 1; i++)
	{
		for (int j = 0; j < nv + 1; j++)
		{
			ass.push_back(D1u(u[i], v[j], 2));
		}
		ruy.push_back(ass);
		ass.clear();
	}

	for (int i = 0; i < nu + 1; i++)
	{
		for (int j = 0; j < nv + 1; j++)
		{
			ass.push_back(D1u(u[i], v[j], 3));
		}
		ruz.push_back(ass);
		ass.clear();
	}

	for (int i = 0; i < nu + 1; i++)
	{
		for (int j = 0; j < nv + 1; j++)
		{
			ass.push_back(D1v(u[i], v[j]));
		}
		rvx.push_back(ass);
		ass.clear();
	}

	for (int i = 0; i < nu + 1; i++)
	{
		for (int j = 0; j < nv + 1; j++)
		{
			ass.push_back(D1v(u[i], v[j], 2));
		}
		rvy.push_back(ass);
		ass.clear();
	}

	for (int i = 0; i < nu + 1; i++)
	{
		for (int j = 0; j < nv + 1; j++)
		{
			ass.push_back(D1v(u[i], v[j], 3));
		}
		rvz.push_back(ass);
		ass.clear();
	}

	area_doun_u = Au_calculate(nu);
	curv_down_u = ku_calculate(nu);
	area_doun_v = Av_calculate(nv);
	curv_down_v = kv_calculate(nv);

}

void SamplingFreeSurf::onArrange(TopoDS_Face TDSFace, int SamplingMode, int NbSamp_u, int NbSamp_v, double CurvWeight)
{
	Handle(Geom_BSplineSurface) SelBSplineSurf = GeomConvert::SurfaceToBSplineSurface(BRep_Tool::Surface(TDSFace));
	m_SelFace = TDSFace;

	pOCCT_Surface = SelBSplineSurf;
	sam_nu = NbSamp_u;
	sam_nv = NbSamp_v;
	weight = CurvWeight;

	integral_initialize();//必须放在得到曲面之后

	if (SamplingMode == 1)//参数空间均匀布点
	{
		mean_sampling();
	}
	else if (SamplingMode == 2)//欧式空间均匀布点
	{
		mean_area_sampling();
	}
	else if (SamplingMode == 3)//混合布点
	{
		mix_sampling();
	}
	else
	{
		return;
	}
}

void SamplingFreeSurf::setMarginalRetreat(double _retreat)
{
	MarginalRetreat = _retreat;
}

void SamplingFreeSurf::setNormReverse(double _reverse)
{
	NormReverse = _reverse;
}

vector<gp_Pnt> SamplingFreeSurf::getSamplesAll()
{
	return m_samples;
}

double SamplingFreeSurf::D1u(double para_u, double para_v, int axis)
{
	int error[3] = { 1,2,3 };

	gp_Pnt pnt = gp_Pnt(0, 0, 0);
	gp_Vec vec1u = gp_Vec(0, 0, 0);
	gp_Vec vec1v = gp_Vec(0, 0, 0);
	gp_Vec vec2u = gp_Vec(0, 0, 0);
	gp_Vec vec2v = gp_Vec(0, 0, 0);
	gp_Vec vec2uv = gp_Vec(0, 0, 0);
	pOCCT_Surface->D2(para_u, para_v, pnt, vec1u, vec1v, vec2u, vec2v, vec2uv);
	if (axis == 1)
		return vec1u.X();
	else if (axis == 2)
		return vec1u.Y();
	else if (axis == 3)
		return vec1u.Z();
	else
		int qwert = error[4];
		return 0;
}

double SamplingFreeSurf::D1v(double para_u, double para_v, int axis)
{
	int error[3] = { 1,2,3 };

	gp_Pnt pnt = gp_Pnt(0, 0, 0);
	gp_Vec vec1u = gp_Vec(0, 0, 0);
	gp_Vec vec1v = gp_Vec(0, 0, 0);
	gp_Vec vec2u = gp_Vec(0, 0, 0);
	gp_Vec vec2v = gp_Vec(0, 0, 0);
	gp_Vec vec2uv = gp_Vec(0, 0, 0);
	pOCCT_Surface->D2(para_u, para_v, pnt, vec1u, vec1v, vec2u, vec2v, vec2uv);
	if (axis == 1)
		return vec1v.X();
	else if (axis == 2)
		return vec1v.Y();
	else if (axis == 3)
		return vec1v.Z();
	else
		int qwert = error[4];
		return 0;
}

double SamplingFreeSurf::D2u(double para_u, double para_v, int axis)
{
	int error[3] = { 1,2,3 };

	gp_Pnt pnt = gp_Pnt(0, 0, 0);
	gp_Vec vec1u = gp_Vec(0, 0, 0);
	gp_Vec vec1v = gp_Vec(0, 0, 0);
	gp_Vec vec2u = gp_Vec(0, 0, 0);
	gp_Vec vec2v = gp_Vec(0, 0, 0);
	gp_Vec vec2uv = gp_Vec(0, 0, 0);
	pOCCT_Surface->D2(para_u, para_v, pnt, vec1u, vec1v, vec2u, vec2v, vec2uv);
	if (axis == 1)
		return vec2u.X();
	else if (axis == 2)
		return vec2u.Y();
	else if (axis == 3)
		return vec2u.Z();
	else
		int qwert = error[4];
		return 0;
}

double SamplingFreeSurf::D2v(double para_u, double para_v, int axis)
{
	int error[3] = { 1,2,3 };

	gp_Pnt pnt = gp_Pnt(0, 0, 0);
	gp_Vec vec1u = gp_Vec(0, 0, 0);
	gp_Vec vec1v = gp_Vec(0, 0, 0);
	gp_Vec vec2u = gp_Vec(0, 0, 0);
	gp_Vec vec2v = gp_Vec(0, 0, 0);
	gp_Vec vec2uv = gp_Vec(0, 0, 0);
	pOCCT_Surface->D2(para_u, para_v, pnt, vec1u, vec1v, vec2u, vec2v, vec2uv);
	if (axis == 1)
		return vec2v.X();
	else if (axis == 2)
		return vec2v.Y();
	else if (axis == 3)
		return vec2v.Z();
	else
		int qwert = error[4];
		return 0;
}

double SamplingFreeSurf::D2uv(double para_u, double para_v, int axis)
{
	int error[3] = { 1,2,3 };

	gp_Pnt pnt = gp_Pnt(0, 0, 0);
	gp_Vec vec1u = gp_Vec(0, 0, 0);
	gp_Vec vec1v = gp_Vec(0, 0, 0);
	gp_Vec vec2u = gp_Vec(0, 0, 0);
	gp_Vec vec2v = gp_Vec(0, 0, 0);
	gp_Vec vec2uv = gp_Vec(0, 0, 0);
	pOCCT_Surface->D2(para_u, para_v, pnt, vec1u, vec1v, vec2u, vec2v, vec2uv);
	if (axis == 1)
		return vec2uv.X();
	else if (axis == 2)
		return vec2uv.Y();
	else if (axis == 3)
		return vec2uv.Z();
	else
		int qwert = error[4];
		return 0;
}

double SamplingFreeSurf::Au_calculate(int u_index)//0<=u_index<=nu
{
	double sum = 0;
	for (int i = 0; i < u_index + 1; i++)
	{
		for (int j = 0; j < nv + 1; j++)
		{
			//if (i != 0)////////强制从零开始
				sum += sqrt(pow((ruy[i][j] * rvz[i][j] - rvy[i][j] * ruz[i][j]), 2) + pow((ruz[i][j] * rvx[i][j] - rux[i][j] * rvz[i][j]), 2) + pow((rux[i][j] * rvy[i][j] - rvx[i][j] * ruy[i][j]), 2));
		}
	}
	sum *= hu * hv;
	return sum;
}

double SamplingFreeSurf::Av_calculate(int v_index)//0<=u_index<=nu
{
	double sum = 0;
	for (int j = 0; j <v_index + 1; j++)
	{
		for (int i = 0; i < nu + 1; i++)
		{
			//if(j!=0)////////强制从零开始
				sum += sqrt(pow((ruy[i][j] * rvz[i][j] - rvy[i][j] * ruz[i][j]), 2) + pow((ruz[i][j] * rvx[i][j] - rux[i][j] * rvz[i][j]), 2) + pow((rux[i][j] * rvy[i][j] - rvx[i][j] * ruy[i][j]), 2));
		}
	}
	sum *= hu * hv;
	return sum;
}

double SamplingFreeSurf::curvpara_k(int u_index, int v_index)
{
	gp_Pnt pnt = gp_Pnt(0, 0, 0);
	gp_Vec vec1u = gp_Vec(0, 0, 0);
	gp_Vec vec1v = gp_Vec(0, 0, 0);
	gp_Vec vec2u = gp_Vec(0, 0, 0);
	gp_Vec vec2v = gp_Vec(0, 0, 0);
	gp_Vec vec2uv = gp_Vec(0, 0, 0);
	pOCCT_Surface->D2(u[u_index], v[v_index], pnt, vec1u, vec1v, vec2u, vec2v, vec2uv);

	gp_Vec n = gp_Vec(0, 0, 0);
	CROSS(n, vec1u, vec1v);
	n.Normalize();
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	double L = DOT(n, vec2u);
	double M = DOT(n, vec2uv);
	double N = DOT(n, vec2v);
	double E = DOT(vec1u, vec1u);
	double F = DOT(vec1u, vec1v);
	double G = DOT(vec1v, vec1v);

	double res = (L * G - 2 * M * F + N * E) / (2 * (E * G - F * F));
	return abs(res);
}

double SamplingFreeSurf::ku_calculate(int u_index)
{
	double sum = 0;
	for (int i = 0; i < u_index + 1; i++)
	{
		for (int j = 0; j < nv + 1; j++)
		{
			//if(i!=0)////////强制从零开始
				sum += curvpara_k(i, j) * sqrt(pow((ruy[i][j] * rvz[i][j] - rvy[i][j] * ruz[i][j]), 2) + pow((ruz[i][j] * rvx[i][j] - rux[i][j] * rvz[i][j]), 2) + pow((rux[i][j] * rvy[i][j] - rvx[i][j] * ruy[i][j]), 2));
		}
	}
	sum *= hu * hv;
	return sum;
}

double SamplingFreeSurf::kv_calculate(int v_index)
{
	double sum = 0;
	for (int j = 0; j < v_index + 1; j++)
	{
		for (int i = 0; i < nu + 1; i++)
		{
			//if(j!=0)////////强制从零开始
				sum += curvpara_k(i, j) * sqrt(pow((ruy[i][j] * rvz[i][j] - rvy[i][j] * ruz[i][j]), 2) + pow((ruz[i][j] * rvx[i][j] - rux[i][j] * rvz[i][j]), 2) + pow((rux[i][j] * rvy[i][j] - rvx[i][j] * ruy[i][j]), 2));
		}
	}
	sum *= hu * hv;
	return sum;
}

double SamplingFreeSurf::pu(double para_u)
{
	vector<double>dec;
	double decrease = 0;
	for (int i = 0; i <= nu; i++)
	{
		decrease = abs(para_u - u[i]);
		dec.push_back(decrease);
	}
	pair<int, double>data;
	data.first = 0;
	data.second = 100;
	for (int i = 0; i <= nu; i++)
	{
		if (dec[i]<= data.second)
		{
			data.first = i;
			data.second = dec[i];
		}
	}
	//cout << data.first << endl;////////////////////////////////////////////////////////////////////
	double res_pu = 0;
	//res_pu = (1-weight)*(Au_calculate(data.first) / Au_calculate(nu)) + weight*(ku_calculate(data.first) / ku_calculate(nu));
	if (curv_down_u == 0)
	{
		res_pu = Au_calculate(data.first) / area_doun_u;
	}
	else
	{
		res_pu = (1 - weight) * (Au_calculate(data.first) / area_doun_u) + weight * (ku_calculate(data.first) / curv_down_u);
	}
	return res_pu;
}

double SamplingFreeSurf::pv(double para_v)
{
	vector<double>dec;
	double decrease = 0;
	for (int i = 0; i <= nv; i++)
	{
		decrease = abs(para_v - v[i]);
		dec.push_back(decrease);
	}
	pair<int, double>data;
	data.first = 0;
	data.second = 100;
	for (int i = 0; i <= nv; i++)
	{
		if (dec[i]<= data.second)
		{
			data.first = i;
			data.second = dec[i];
		}
	}
	double res_pv = 0;
	//res_pv = (1-weight)*(Av_calculate(data.first) / Av_calculate(nv)) + weight*(kv_calculate(data.first) / kv_calculate(nv));
	if (curv_down_v == 0)
	{
		res_pv = Av_calculate(data.first) / area_doun_v;
	}
	else
	{
		res_pv = (1 - weight) * (Av_calculate(data.first) / area_doun_v) + weight * (kv_calculate(data.first) / curv_down_v);
	}
	return res_pv;
}

void SamplingFreeSurf::mix_sampling()
{
	curve_parau.clear();
	curve_resu.clear();
	curve_parav.clear();
	curve_resv.clear();

	//int curve_parau_num = 51;//曲线u方向分段数，该值越大计算反函数精度越高
	curve_parau = linspace(ustart, uend, curve_parau_num);
	curve_resu.push_back(0);/////////////////////////////////强制从零开始
	for (int i = 1; i < curve_parau_num; i++)
	{
		curve_resu.push_back(pu(curve_parau[i]));
	}

	vector<double>curve_para_reu;

	if (MarginalRetreat)
	{
		vector<double> RetU;
		RetU.push_back(ustart);
		RetU.push_back(ustart + MarginalRetreat);
		RetU.push_back(uend - MarginalRetreat);
		RetU.push_back(uend);

		vector<double> ReRetU= reverse(RetU, curve_parau, curve_resu);

		curve_para_reu= linspace(ReRetU[1], ReRetU[2], sam_nu);

		pair<double, int> tmpPairU;
		tmpPairU.first = ustart;
		tmpPairU.second = 0;

		curve_para_reu = VecInsert(curve_para_reu, tmpPairU);
		curve_para_reu.push_back(uend);
	}

	else
	{
		curve_para_reu = linspace(0, 1, sam_nu);
	}
	
	vector<double>para_resam_u = reverse(curve_para_reu, curve_resu, curve_parau);

	//int curve_parav_num = 51;//曲线v方向分段数
	curve_parav = linspace(vstart, vend, curve_parav_num);
	curve_resv.push_back(0);//////////////////////////////////强制从零开始
	for (int i = 1; i < curve_parav_num; i++)
	{
		curve_resv.push_back(pv(curve_parav[i]));
	}

	vector<double>curve_para_rev;

	if (MarginalRetreat)
	{
		vector<double> RetV;
		RetV.push_back(vstart);
		RetV.push_back(vstart + MarginalRetreat);
		RetV.push_back(vend - MarginalRetreat);
		RetV.push_back(vend);

		vector<double> ReRetV = reverse(RetV, curve_parav, curve_resv);

		curve_para_rev = linspace(ReRetV[1], ReRetV[2], sam_nv);

		pair<double, int> tmpPairV;
		tmpPairV.first = vstart;
		tmpPairV.second = 0;

		curve_para_rev = VecInsert(curve_para_rev, tmpPairV);
		curve_para_rev.push_back(vend);
	}

	else
	{
		curve_para_rev = linspace(0, 1, sam_nv);
	}

	vector<double>para_resam_v = reverse(curve_para_rev, curve_resv, curve_parav);

	if (MarginalRetreat)
	{
		para_resam_u = VecDelete(para_resam_u, 0);
		para_resam_u = VecDelete(para_resam_u, para_resam_u.size() - 1);
		para_resam_v = VecDelete(para_resam_v, 0);
		para_resam_v = VecDelete(para_resam_v, para_resam_v.size() - 1);

		para_resam_u[0] = ustart + MarginalRetreat;
		para_resam_u[para_resam_u.size() - 1] = uend - MarginalRetreat;
		para_resam_v[0] = vstart + MarginalRetreat;
		para_resam_v[para_resam_v.size() - 1] = vend - MarginalRetreat;
	}

	for (int i = 0; i < para_resam_u.size(); i++)
	{
		for (int j = 0; j < para_resam_v.size(); j++)
		{
			gp_Pnt my_pnt = pOCCT_Surface->Value(para_resam_u[i], para_resam_v[j]);
			m_samples.push_back(my_pnt);
		}
	}

}

void SamplingFreeSurf::mean_sampling()
{
	m_samples.clear();

	vector<double>mean_parau;
	vector<double>mean_parav;

	if (MarginalRetreat)
	{
		mean_parau = linspace(ustart + MarginalRetreat, uend - MarginalRetreat, sam_nu);
		mean_parav = linspace(vstart + MarginalRetreat, vend - MarginalRetreat, sam_nv);
	}
	else
	{
		mean_parau = linspace(ustart, uend, sam_nu);
		mean_parav = linspace(vstart, vend, sam_nv);
	}

	gp_Pnt mean_pnt_ass(0, 0, 0);
	for (int i = 0; i < sam_nu; i++)
	{
		for (int j = 0; j < sam_nv; j++)
		{
			gp_Pnt mean_pnt_ass = pOCCT_Surface->Value(mean_parau[i], mean_parav[j]);
			m_samples.push_back(mean_pnt_ass);
		}
	}
}

double SamplingFreeSurf::Au(double para_u)
{
	vector<double>dec;
	double decrease = 0;
	for (int i = 0; i <= nu; i++)
	{
		decrease = abs(para_u - u[i]);
		dec.push_back(decrease);
	}
	pair<int, double>data;
	data.first = 0;
	data.second = 100;
	for (int i = 0; i <= nu; i++)
	{
		if (dec[i] <= data.second)
		{
			data.first = i;
			data.second = dec[i];
		}
	}

	double res_pu = 0;
	res_pu = Au_calculate(data.first) / area_doun_u;
	return res_pu;
}

double SamplingFreeSurf::Av(double para_v)
{
	vector<double>dec;
	double decrease = 0;
	for (int i = 0; i <= nv; i++)
	{
		decrease = abs(para_v - v[i]);
		dec.push_back(decrease);
	}
	pair<int, double>data;
	data.first = 0;
	data.second = 100;
	for (int i = 0; i <= nv; i++)
	{
		if (dec[i] <= data.second)
		{
			data.first = i;
			data.second = dec[i];
		}
	}
	double res_pv = 0;
	res_pv = Av_calculate(data.first) / area_doun_v;
	return res_pv;
}

void SamplingFreeSurf::mean_area_sampling()
{
	m_samples.clear();

	//int curve_parau_num = 51;//曲线u方向分段数，该值越大计算反函数精度越高
	vector<double>curve_parau = linspace(ustart, uend, curve_parau_num);
	vector<double>curve_resu;
	for (int i = 0; i < curve_parau_num; i++)
	{
		curve_resu.push_back(Au(curve_parau[i]));
	}

	vector<double>curve_para_reu;

	if (MarginalRetreat)
	{
		vector<double> RetU;
		RetU.push_back(ustart);
		RetU.push_back(ustart + MarginalRetreat);
		RetU.push_back(uend - MarginalRetreat);
		RetU.push_back(uend);

		vector<double> ReRetU = reverse(RetU, curve_parau, curve_resu);

		curve_para_reu = linspace(ReRetU[1], ReRetU[2], sam_nu);

		//curve_para_reu[0] = RetU[1];
		//curve_para_reu[sam_nu - 1] = RetU[2];

		pair<double, int> tmpPairU;
		tmpPairU.first = ustart;
		tmpPairU.second = 0;

		curve_para_reu = VecInsert(curve_para_reu, tmpPairU);
		curve_para_reu.push_back(uend);
	}

	else
	{
		curve_para_reu = linspace(0, 1, sam_nu);
	}


	vector<double>para_resam_u = reverse(curve_para_reu, curve_resu, curve_parau);

	//int curve_parav_num = 51;//曲线v方向分段数
	vector<double>curve_parav = linspace(vstart, vend, curve_parav_num);
	vector<double>curve_resv;
	for (int i = 0; i < curve_parav_num; i++)
	{
		curve_resv.push_back(Av(curve_parav[i]));
	}

	vector<double>curve_para_rev;

	if (MarginalRetreat)
	{
		vector<double> RetV;
		RetV.push_back(vstart);
		RetV.push_back(vstart + MarginalRetreat);
		RetV.push_back(vend - MarginalRetreat);
		RetV.push_back(vend);

		vector<double> ReRetV = reverse(RetV, curve_parav, curve_resv);

		curve_para_rev = linspace(ReRetV[1], ReRetV[2], sam_nv);

		pair<double, int> tmpPairV;
		tmpPairV.first = vstart;
		tmpPairV.second = 0;

		curve_para_rev = VecInsert(curve_para_rev, tmpPairV);
		curve_para_rev.push_back(vend);
	}

	else
	{
		curve_para_rev = linspace(0, 1, sam_nv);
	}

	vector<double>para_resam_v = reverse(curve_para_rev, curve_resv, curve_parav);

	if (MarginalRetreat)
	{
		para_resam_u = VecDelete(para_resam_u, 0);
		para_resam_u = VecDelete(para_resam_u, para_resam_u.size() - 1);
		para_resam_v = VecDelete(para_resam_v, 0);
		para_resam_v = VecDelete(para_resam_v, para_resam_v.size() - 1);

		para_resam_u[0] = ustart + MarginalRetreat;
		para_resam_u[para_resam_u.size() - 1] = uend - MarginalRetreat;
		para_resam_v[0] = vstart + MarginalRetreat;
		para_resam_v[para_resam_v.size() - 1] = vend - MarginalRetreat;
	}

	for (int i = 0; i < para_resam_u.size(); i++)
	{
		for (int j = 0; j < para_resam_v.size(); j++)
		{
			gp_Pnt my_pnt = pOCCT_Surface->Value(para_resam_u[i], para_resam_v[j]);
			m_samples.push_back(my_pnt);
		}
	}
}
