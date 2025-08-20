/*
为基本曲面（平面、圆柱面、圆锥面、球面）提供采样的类
*/
#pragma once

#include <vector>
#include <gp_Pnt.hxx>
#include <TopoDS_Face.hxx>
#include <qstring.h>
#include <TopoDS_Wire.hxx>

using namespace std;

class SamplingBasicFace
{
public:
	// _face: 布点面
	// _nu: u 向点数
	// _nv: v 向点数
	// _eRetreatU: u 向边缘回退量（百分比）
	// _eRetreatV: v 向边缘回退量（百分比）
	// _deviaU: u 向偏移量（百分比）
	// _deviaV: v 向偏移量（百分比）
	static vector<gp_Pnt> uniform(
		const TopoDS_Face& _face, 
		const int& _nu, 
		const int& _nv, 
		const double& _eRetreatU, 
		const double& _eRetreatV, 
		const double& _deviaU, 
		const double& _deviaV
	);

	static vector<gp_Pnt> hammersley(
		const TopoDS_Face& _face,
		const int& _nAll,
		const double& _eRetreatU,
		const double& _eRetreatV,
		const double& _deviaU,
		const double& _deviaV
	);

	static vector<gp_Pnt> marginal(
		const TopoDS_Face& _face,
		const QString& _offsetCmd,
		const QString& _tolCmd
	);





private:
	static bool isBasicFace(const TopoDS_Face& _face);

	static TopoDS_Shape OffsetWire(const std::vector<TopoDS_Wire>& wires, double offset);

};

