/*
Ϊ�������棨ƽ�桢Բ���桢Բ׶�桢���棩�ṩ��������
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
	// _face: ������
	// _nu: u �����
	// _nv: v �����
	// _eRetreatU: u ���Ե���������ٷֱȣ�
	// _eRetreatV: v ���Ե���������ٷֱȣ�
	// _deviaU: u ��ƫ�������ٷֱȣ�
	// _deviaV: v ��ƫ�������ٷֱȣ�
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

