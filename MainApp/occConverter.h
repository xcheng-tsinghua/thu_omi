#pragma once
#include <array>
#include <map>
#include "../External/fkernel/include/FKernelHeader.h"
#include <QtGui>
#include <qvector.h>
#include "FTreeWnd.h"
#include "qprogressdialog.h"
#include <unordered_map>
#include <unordered_set>
#include <TopoDS_Face.hxx>
#include <Geom_BSplineSurface.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ColorTool.hxx>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef WNT
#define EOL "\r\n"
#else
#define EOL "\n"
#endif

enum SURFACE_TYPE { BSPLINE, CONE, CYLINDER, PLANE, SPHERE, OTHER_SURF };

class occConverter 
{
public:
	occConverter();
	~occConverter();

	FBody* bodyGetter() { return m_pBody; }

	QString GetPartName() { return m_ImportPartName; }

	// 导入STEP文件，后一次导入会覆盖之前的模型
	void importStep(QString fileName);

	// 获取BSpline曲面的索引
	// 从 GetTopoDS_FaceList() 中获取 TopoDS_face，GetFSurfList() 中获取FSurface*
	// 转化1 :Handle(Geom_Surface) = BRep_Tool::Surface(TopoDS_Face);
	// 转化2 :Handle(Geom_BSplineSurface) = GeomConvert::SurfaceToBSplineSurface(Handle(Geom_Surface));
	vector<int> GetBSplineInds() { return m_BSplineInds; }

	// 获取圆锥曲面的索引
	// 从 GetTopoDS_FaceList() 中获取 TopoDS_face，GetFSurfList() 中获取FSurface*
	// 转化1 :Handle(Geom_Surface) = BRep_Tool::Surface(TopoDS_Face);
	// 转化2 :Handle(Geom_ConicalSurface) = Handle(Geom_ConicalSurface)::DownCast(Handle(Geom_Surface));
	vector<int> GetConicalInds() { return m_ConicalInds; }

	// 获取圆柱曲面的索引
	// 从 GetTopoDS_FaceList() 中获取 TopoDS_face，GetFSurfList() 中获取FSurface*
	// 转化1 :Handle(Geom_Surface) = BRep_Tool::Surface(TopoDS_Face);
	// 转化2 :Handle(Geom_CylindricalSurface) = Handle(Geom_CylindricalSurface)::DownCast(Handle(Geom_Surface));
	vector<int> GetCylindricalInds() { return m_CylindricalInds; }

	// 获取平面的索引
	// 从 GetTopoDS_FaceList() 中获取 TopoDS_face，GetFSurfList() 中获取FSurface*
	// 转化1 :Handle(Geom_Surface) = BRep_Tool::Surface(TopoDS_Face);
	// 转化2 :Handle(Geom_Plane) = Handle(Geom_Plane)::DownCast(Handle(Geom_Surface));
	vector<int> GetPlaneInds() { return m_PlaneInds; }

	// 获取球面的索引
	// 从 GetTopoDS_FaceList() 中获取 TopoDS_face，GetFSurfList() 中获取FSurface*
	// 转化1 :Handle(Geom_Surface) = BRep_Tool::Surface(TopoDS_Face);
	// 转化2 :Handle(Geom_SphericalSurface) = Handle(Geom_SphericalSurface)::DownCast(Handle(Geom_Surface));
	vector<int> GetSphericalInds() { return m_SphericalInds; }

	// 获取其它曲面的索引
	// 从 GetTopoDS_FaceList() 中获取 TopoDS_face，GetFSurfList() 中获取FSurface*
	vector<int> GetOtherInds() { return m_OtherInds; }

	void GetMaxMinCorner(ON_3dPoint& maxCorner, ON_3dPoint& minCorner) { maxCorner = m_MaxCorner; minCorner = m_MinCorner; }
	QProgressDialog* GetProgressBar() { return m_pProgBar; }

	unordered_map<int, int> GetSameCylinderMap() { return m_SameCylinderNearby; }
	unordered_map<int, int> GetSameCylinderMapRev() { return m_SameCylinderNearbyRev; }

	// 获取所有型面的 TopoDS_Face 列表
	// Geom_Face 转化：Handle(Geom_Surface) = BRep_Tool::Surface(TopoDS_Face);
	// BSpline转化 :Handle(Geom_BSplineSurface) = GeomConvert::SurfaceToBSplineSurface(Handle(Geom_Surface));
	// 圆锥转化 :Handle(Geom_ConicalSurface) = Handle(Geom_ConicalSurface)::DownCast(Handle(Geom_Surface));
	// 圆柱转化 :Handle(Geom_CylindricalSurface) = Handle(Geom_CylindricalSurface)::DownCast(Handle(Geom_Surface));
	// 平面转化 :Handle(Geom_Plane) = Handle(Geom_Plane)::DownCast(Handle(Geom_Surface));
	// 球面转化 :Handle(Geom_SphericalSurface) = Handle(Geom_SphericalSurface)::DownCast(Handle(Geom_Surface));
	vector<TopoDS_Face> GetTopoDS_FaceList() { return m_TopoDS_SurfList; }

	// 获取所有型面的 FSurface* 列表
	vector<FSurface*> GetFSurfList() { return m_FSurfList; }

	QList<QTreeWidgetItem*> getTreeList() { return m_itemList; }

	// 判断 FSurface* 的类型及查找索引
	// paraSurfType:类型
	// paraIndexInAllList:在 m_FSurfList 中的索引
	// paraIndexInTypeList:在对应的m_XXXX_Inds中的索引
	// 例如 aFSurf 是 BSpline曲面，m_FSurfList[9] = aFSurf，9这个索引在 m_BSplineInds 中的索引为5，即m_BSplineInds[5] = 9
	// 则返回值为：paraSurfType = BSPLINE，paraIndexInAllList = 9，paraIndexInTypeList = 6;
	// paraSurfName:曲面名
	// 例如 aFSurf 是圆柱面，且在圆柱面索引数组中索引为3，则其名字为：圆柱面3
	void FSurfInfo(FSurface* aFSurf, SURFACE_TYPE& paraSurfType, int& paraIndexInAllList, int& paraIndexInTypeList, QString& paraSurfName);

	// 找到 FSurface* 对应的 TopoDS_Face
	TopoDS_Face FSurf2TDSFace(FSurface* aFSurf);
	
	// 通过曲面名返回曲面
	FSurface* GetFSurf(QString surfName);

	// 找到 FSurface* 对应的 Geom_BSplineSurface
	Handle(Geom_BSplineSurface) FSurf2BSpline(FSurface* aFSurf);

	// 获取 FSurface* 的名字
	QString FSurfName(FSurface* _surf);

	// 获取FSurface* 的类型
	SURFACE_TYPE FSurfType(FSurface* _surf);

	// 将 FSurface* 的颜色设置成默认颜色
	void setSurfColorDefault(FSurface* _surf);

	// 将所有面设置成默认颜色
	void setAllSurfColorDefault();

private:

	void faceConvert(TColgp_SequenceOfPnt aPoints, Handle_Geom_Surface aSurf);

	void convert(TopoDS_Shape aShape);

	ON_Mesh* meshConvert(TopoDS_Face aFace, TopLoc_Location mLoc);

	void getName(TDF_Label label, QString& name);

	QTreeWidgetItem* getTree() { return m_pTree; }

	SURFACE_TYPE addFaceToTree(Handle(Geom_Surface) surf, QList<QTreeWidgetItem*>& faceList, FSurface* CurSurf = 0);

	void initPartTree(QList<QTreeWidgetItem*>& faceList);

	void load(QTreeWidgetItem* topItem, Handle(XCAFDoc_ShapeTool) aMainTool);

	QList<QTreeWidgetItem*> loadAssembly(TDF_Label aLabel, Handle(XCAFDoc_ShapeTool) aMainTool, TopLoc_Location aLoc);

	QList<QTreeWidgetItem*> loadPart(TDF_Label pLabel, Handle(XCAFDoc_ShapeTool) aMainTool, TopLoc_Location pLoc);

	void SetMinMaxPnt(ON_3dPoint& TarPnt, ON_3dPoint CompPnt, bool IsMin = true);

	// 清除上一次导入的模型 - 未完善
	void ClearPreviousSTEP();

	// 找到属于同一个圆柱的次圆柱面，可能2个，可能三个
	void SearchSubCylinder();

	FBody*                         m_pBody;
	QTreeWidgetItem*               m_pTree;
	QList<QTreeWidgetItem*>        m_itemList;

	//各类曲面在 m_FSurfList 中的索引
	vector<int> m_BSplineInds;
	vector<int> m_ConicalInds;
	vector<int> m_CylindricalInds;
	vector<int> m_PlaneInds;
	vector<int> m_SphericalInds;
	vector<int> m_OtherInds;

	// 用于快速判断曲面类型
	// key:目标类型曲面在m_FSurfList中的索引，value:目标类型曲面在对应的indexList中的索引
	unordered_map<int, int> m_Hashmap_BSplineInds;
	unordered_map<int, int> m_Hashmap_ConicalInds;
	unordered_map<int, int> m_Hashmap_CylindricalInds;
	unordered_map<int, int> m_Hashmap_PlaneInds;
	unordered_map<int, int> m_Hashmap_SphericalInds;
	unordered_map<int, int> m_Hashmap_OtherInds;

	// 记录 FSurface* 和其在m_FSurfList 中索引
	unordered_map<FSurface*, int> m_HashFSurf2Ind;

	QProgressDialog* m_pProgBar;

	QString m_ImportPartName;

	// 本次STEP导入的所有 FSurface* 数组
	vector<FSurface*> m_FSurfList;

	// 本次STEP导入的所有 TopoDS_Face 数组
	vector<TopoDS_Face> m_TopoDS_SurfList;

	unordered_map<int, int> m_SameCylinderNearby;
	unordered_map<int, int> m_SameCylinderNearbyRev;

	ON_3dPoint m_MinCorner;
	ON_3dPoint m_MaxCorner;

	int CurrentFaceIndex;

	Handle(XCAFDoc_ColorTool) m_pColorTool;
	unordered_map<FSurface*, Quantity_Color> m_pFaceColorDefault;

};