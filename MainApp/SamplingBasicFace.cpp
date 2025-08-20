#include "SamplingBasicFace.h"
#include <BRep_Tool.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include "np.h"
#include <BRepTools.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <set>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <TopExp.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <TopoDS_Wire.hxx>
#include <BRepLib.hxx>
#include <functional>
#include <BRepOffsetAPI_MakeOffset.hxx>

vector<gp_Pnt> SamplingBasicFace::uniform(
	const TopoDS_Face& _face,
	const int& _nu,
	const int& _nv,
	const double& _eRetreatU,
	const double& _eRetreatV,
	const double& _deviaU,
	const double& _deviaV)
{
	// 不是基本曲面不布点
	if (!isBasicFace(_face)) return vector<gp_Pnt>();

	Handle(Geom_Surface) surface = BRep_Tool::Surface(_face);
	Handle(Standard_Type) type = surface->DynamicType();

	vector<gp_Pnt> pointsAll;

	// 计算参数范围
	Standard_Real Umin, Umax, Vmin, Vmax;
	BRepTools::UVBounds(_face, Umin, Umax, Vmin, Vmax);

	double lenU = Umax - Umin;
	double deviaU = lenU * _deviaU;

	double lenV = Vmax - Vmin;
	double deviaV = lenV * _deviaV;

	// 平面
	if (type == STANDARD_TYPE(Geom_Plane))
	{

		// 计算修正参数范围
		vector<double> parasU = linspace(0 + _eRetreatU, 1 - _eRetreatU, _nu);
		vector<double> parasV = linspace(0 + _eRetreatV, 1 - _eRetreatV, _nv);

		for (int i = 0; i < _nu; i++)
		{
			for (int j = 0; j < _nv; j++)
			{
				gp_Pnt cPnt = surface->Value(deviaU + Umin + lenU * parasU[i], deviaV + Vmin + lenV * parasV[j]);

				// 判断点是否在平面内
				if (isPointInShape(cPnt, _face))
				{
					pointsAll.push_back(cPnt);
				}

			}

		}

	}
	// 圆柱面，参数 u 为圆周方向，参数 v 为轴线方向
	else if (
		type == STANDARD_TYPE(Geom_CylindricalSurface) || 
		type == STANDARD_TYPE(Geom_ConicalSurface) ||
		type == STANDARD_TYPE(Geom_SphericalSurface))
	{
		//Handle(Geom_CylindricalSurface) cylin_surf = Handle(Geom_CylindricalSurface)::DownCast(surface);

		// 计算修正参数范围
		// 圆周方向需要减掉一个元素，因为圆周首个元素与尾元素重合
		vector<double> parasU;
		vector<double> parasV;

		if (abs(lenU - M_PI) < PRECISION || abs(lenU - M_PI / 2.0) < PRECISION)
		{
			lenU = 2 * M_PI;
			deviaU = 2 * M_PI * (_deviaU + 0.1345973);
			Umin = 0;

			parasU = linspace(0 + _eRetreatU, 1 - _eRetreatU, _nu + 1);
			parasV = linspace(0 + _eRetreatV, 1 - _eRetreatV, _nv);
			parasU.pop_back();
		}
		else
		{
			parasU = linspace(0 + _eRetreatU, 1 - _eRetreatU, _nu);
			parasV = linspace(0 + _eRetreatV, 1 - _eRetreatV, _nv);
		}

		for (int i = 0; i < _nu; i++)
		{
			for (int j = 0; j < _nv; j++)
			{
				gp_Pnt cPnt = surface->Value(deviaU + Umin + lenU * parasU[i], deviaV + Vmin + lenV * parasV[j]);

				// 判断点是否在平面内
				if (isPointInShape(cPnt, _face))
				{
					pointsAll.push_back(cPnt);
				}

			}

		}

	}

	return pointsAll;
}

vector<gp_Pnt> SamplingBasicFace::hammersley(
	const TopoDS_Face& _face, 
	const int& _nAll, 
	const double& _eRetreatU, 
	const double& _eRetreatV, 
	const double& _deviaU, 
	const double& _deviaV)
{
	Handle(Geom_Surface) surface = BRep_Tool::Surface(_face);

	vector<gp_Pnt> pointsAll;

	// 计算参数范围
	Standard_Real Umin, Umax, Vmin, Vmax;
	BRepTools::UVBounds(_face, Umin, Umax, Vmin, Vmax);

	double lenU = Umax - Umin;
	double deviaU = lenU * _deviaU;

	double lenV = Vmax - Vmin;
	double deviaV = lenV * _deviaV;

	vector<double> parasU = linspace(0 + _eRetreatU, 1 - _eRetreatU, _nAll);
	vector<double> parasV = Hammersley(_nAll);

	double lenStrech = 1 - 2 * _eRetreatV;
	for (int i = 0; i < _nAll; i++)
	{
		parasV[i] = _eRetreatV + parasV[i] * lenStrech;
	}

	for (int i = 0; i < _nAll; i++)
	{
		gp_Pnt cPnt = surface->Value(deviaU + Umin + lenU * parasU[i], deviaV + Vmin + lenV * parasV[i]);

		// 判断点是否在平面内
		if (isPointInShape(cPnt, _face))
		{
			pointsAll.push_back(cPnt);
		}
	}

	return pointsAll;

}

vector<gp_Pnt> SamplingBasicFace::marginal(
	const TopoDS_Face& _face,
	const QString& _offsetCmd,
	const QString& _tolCmd)
{
	if (_face.IsNull())
	{
		return vector<gp_Pnt>();
	}

	std::vector<TopoDS_Wire> using_wires;

	// 由高到低检查
	TopTools_IndexedMapOfShape sub_shapes;
	TopExp::MapShapes(_face, TopAbs_WIRE, sub_shapes); // wire 或 面的外环
	if (sub_shapes.Size() > 0)
	{
		for (auto it = sub_shapes.cbegin(); it != sub_shapes.cend(); ++it)
		{
			using_wires.emplace_back(TopoDS::Wire(*it));
		}
	}
	else
	{
		TopExp::MapShapes(_face, TopAbs_EDGE, sub_shapes);
		if (sub_shapes.Size() > 0)
		{
			TopoDS_Edge edge = TopoDS::Edge(*sub_shapes.cbegin());
			using_wires.push_back(BRepBuilderAPI_MakeWire(edge));
		}
	}

	if (using_wires.empty())
	{
		qDebug() << "Invalid selection, selected shape is not Edge or Wire!!";
		return vector<gp_Pnt>();
	}

	for (size_t i = 0; i < using_wires.size(); ++i)
	{
		TopoDS_Wire copy_w = TopoDS::Wire(BRepBuilderAPI_Copy(using_wires[i]));
		// OCC 7.6 无法处理 2D Edge，需要转换为 3D Edge
		bool ok = BRepLib::BuildCurves3d(copy_w);
		//if (!ok)
		//{
		//	DrawShape(copy_w, QString("Build 3d curve failed ") + QString::number(i));
		//}
		using_wires[i] = copy_w;
	}

	function<bool(gp_Pnt, vector<gp_Pnt>, double)> isRepeat = [&](gp_Pnt fpPnt, vector<gp_Pnt> fpList, double fpTol)->bool
	{

		for (auto ita = fpList.begin(); ita != fpList.end(); ++ita)
		{
			if (fpPnt.Distance(*ita) <= fpTol)
			{
				return true;
			}
		}
		return false;
	};

	QString offsetStr = _offsetCmd;
	QStringList offsetValStrs = offsetStr.split(" ");
	vector<double> offsetVals;

	QString neitolStr = _tolCmd;
	QStringList neitolValStrs = neitolStr.split(" ");
	vector<double> neitolVals;

	if (neitolValStrs.size() != 1 && neitolValStrs.size() != offsetValStrs.size())
	{
		MesgBox("请输入正确的表达式，不同距离之间使用空格分开，距离容差长度为一或与缩进距离数相等。");
		return vector<gp_Pnt>();
	}

	// 将str转化为数字
	for (auto ita = offsetValStrs.begin(); ita != offsetValStrs.end(); ++ita)
	{
		bool ok;
		double number = (*ita).toDouble(&ok);
		if (ok) {
			offsetVals.push_back(number);
		}
		else {
			MesgBox("请输入正确的表达式，不同距离之间使用空格分开，距离容差长度为一或与缩进距离数相等。");
			return vector<gp_Pnt>();
		}
	}

	for (auto ita = neitolValStrs.begin(); ita != neitolValStrs.end(); ++ita)
	{
		bool ok;
		double number = (*ita).toDouble(&ok);
		if (ok) {
			neitolVals.push_back(number);
		}
		else {
			MesgBox("请输入正确的表达式，不同距离之间使用空格分开，距离容差长度为一或与缩进距离数相等。");
			return vector<gp_Pnt>();
		}
	}

	vector<vector<gp_Pnt>> allOffsetPnts;
	for (int i = 0; i < offsetVals.size(); i++)
	{
		double offsetVal = offsetVals[i];
		double neitolVal;

		if (neitolVals.size() == 1)
		{
			neitolVal = neitolVals[0];
		}
		else
		{
			neitolVal = neitolVals[i];
		}

		//double offsetVal = m_pOffsetVal->text().toDouble();
		//double neitolVal = m_pNeiTolVal->text().toDouble();

		TopoDS_Shape theOffsetWire = OffsetWire(using_wires, offsetVal);

		//ShowShapeEdge(theOffsetWire);
		vector<gp_Pnt> allCorners;

		// 先提取所有边端点
		for (TopExp_Explorer aExpEdge(theOffsetWire, TopAbs_EDGE); aExpEdge.More(); aExpEdge.Next())
		{
			TopoDS_Edge aEdge = TopoDS::Edge(aExpEdge.Current());

			double pStart = 0, pEnd = 0;
			BRep_Tool::Range(aEdge, pStart, pEnd);
			Handle(Geom_Curve) theCurve = BRep_Tool::Curve(aEdge, pStart, pEnd);

			gp_Pnt pntSt = theCurve->Value(pStart);
			gp_Pnt pntEd = theCurve->Value(pEnd);

			if (isPointInShape(pntSt, _face)) allCorners.push_back(pntSt);
			if (isPointInShape(pntEd, _face)) allCorners.push_back(pntEd);

		}
		//ShowPoints(allCorners);
		if (allCorners.size() == 0)
		{
			return vector<gp_Pnt>();
		}

		vector<gp_Pnt> CornersNoRepeat;
		CornersNoRepeat.push_back(allCorners[0]);
		int nSize = allCorners.size();
		for (int i = 1; i < nSize; i++)
		{
			if (!isRepeat(allCorners[i], CornersNoRepeat, neitolVal))
			{
				CornersNoRepeat.push_back(allCorners[i]);
			}
		}

		allOffsetPnts.push_back(CornersNoRepeat);
	}

	vector<gp_Pnt> CornersNoRepeat;
	for (int i = 0; i < allOffsetPnts.size(); i++)
	{
		for (int j = 0; j < allOffsetPnts[i].size(); j++)
		{
			CornersNoRepeat.push_back(allOffsetPnts[i][j]);
		}
	}

	return CornersNoRepeat;

}

bool SamplingBasicFace::isBasicFace(const TopoDS_Face& _face)
{
	Handle(Geom_Surface) surface = BRep_Tool::Surface(_face);
	Handle(Standard_Type) type = surface->DynamicType();

	if (type == STANDARD_TYPE(Geom_Plane) || 
		type == STANDARD_TYPE(Geom_CylindricalSurface) || 
		type == STANDARD_TYPE(Geom_ConicalSurface) || 
		type == STANDARD_TYPE(Geom_SphericalSurface))
	{
		return true;
	}
	else
	{
		return false;
	}

}

TopoDS_Shape SamplingBasicFace::OffsetWire(const std::vector<TopoDS_Wire>& wires, double offset)
{
	if (wires.empty())
	{
		return TopoDS_Shape();
	}

	BRepOffsetAPI_MakeOffset mk_offset;

#if (OCC_VERSION_HEX >= 0x070700)
	const bool approx = _ctrlUi->checkBox_splineToPoly->isChecked();
	mk_offset.SetApprox(approx);
#endif
	for (int i = 0; i < wires.size(); i++)
	{
		mk_offset.AddWire(wires[i]);
	}
	mk_offset.Init();

	try
	{
		mk_offset.Perform(offset);
		if (!mk_offset.IsDone())
		{
			qDebug() << "Offset [" << offset << "] failed!\n";

			return TopoDS_Shape();
		}

		return mk_offset.Shape();
	}
	catch (...)
	{
		qDebug() << "Exception occured!";
		return TopoDS_Shape();
	}
}
