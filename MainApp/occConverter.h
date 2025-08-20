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

	// ����STEP�ļ�����һ�ε���Ḳ��֮ǰ��ģ��
	void importStep(QString fileName);

	// ��ȡBSpline���������
	// �� GetTopoDS_FaceList() �л�ȡ TopoDS_face��GetFSurfList() �л�ȡFSurface*
	// ת��1 :Handle(Geom_Surface) = BRep_Tool::Surface(TopoDS_Face);
	// ת��2 :Handle(Geom_BSplineSurface) = GeomConvert::SurfaceToBSplineSurface(Handle(Geom_Surface));
	vector<int> GetBSplineInds() { return m_BSplineInds; }

	// ��ȡԲ׶���������
	// �� GetTopoDS_FaceList() �л�ȡ TopoDS_face��GetFSurfList() �л�ȡFSurface*
	// ת��1 :Handle(Geom_Surface) = BRep_Tool::Surface(TopoDS_Face);
	// ת��2 :Handle(Geom_ConicalSurface) = Handle(Geom_ConicalSurface)::DownCast(Handle(Geom_Surface));
	vector<int> GetConicalInds() { return m_ConicalInds; }

	// ��ȡԲ�����������
	// �� GetTopoDS_FaceList() �л�ȡ TopoDS_face��GetFSurfList() �л�ȡFSurface*
	// ת��1 :Handle(Geom_Surface) = BRep_Tool::Surface(TopoDS_Face);
	// ת��2 :Handle(Geom_CylindricalSurface) = Handle(Geom_CylindricalSurface)::DownCast(Handle(Geom_Surface));
	vector<int> GetCylindricalInds() { return m_CylindricalInds; }

	// ��ȡƽ�������
	// �� GetTopoDS_FaceList() �л�ȡ TopoDS_face��GetFSurfList() �л�ȡFSurface*
	// ת��1 :Handle(Geom_Surface) = BRep_Tool::Surface(TopoDS_Face);
	// ת��2 :Handle(Geom_Plane) = Handle(Geom_Plane)::DownCast(Handle(Geom_Surface));
	vector<int> GetPlaneInds() { return m_PlaneInds; }

	// ��ȡ���������
	// �� GetTopoDS_FaceList() �л�ȡ TopoDS_face��GetFSurfList() �л�ȡFSurface*
	// ת��1 :Handle(Geom_Surface) = BRep_Tool::Surface(TopoDS_Face);
	// ת��2 :Handle(Geom_SphericalSurface) = Handle(Geom_SphericalSurface)::DownCast(Handle(Geom_Surface));
	vector<int> GetSphericalInds() { return m_SphericalInds; }

	// ��ȡ�������������
	// �� GetTopoDS_FaceList() �л�ȡ TopoDS_face��GetFSurfList() �л�ȡFSurface*
	vector<int> GetOtherInds() { return m_OtherInds; }

	void GetMaxMinCorner(ON_3dPoint& maxCorner, ON_3dPoint& minCorner) { maxCorner = m_MaxCorner; minCorner = m_MinCorner; }
	QProgressDialog* GetProgressBar() { return m_pProgBar; }

	unordered_map<int, int> GetSameCylinderMap() { return m_SameCylinderNearby; }
	unordered_map<int, int> GetSameCylinderMapRev() { return m_SameCylinderNearbyRev; }

	// ��ȡ��������� TopoDS_Face �б�
	// Geom_Face ת����Handle(Geom_Surface) = BRep_Tool::Surface(TopoDS_Face);
	// BSplineת�� :Handle(Geom_BSplineSurface) = GeomConvert::SurfaceToBSplineSurface(Handle(Geom_Surface));
	// Բ׶ת�� :Handle(Geom_ConicalSurface) = Handle(Geom_ConicalSurface)::DownCast(Handle(Geom_Surface));
	// Բ��ת�� :Handle(Geom_CylindricalSurface) = Handle(Geom_CylindricalSurface)::DownCast(Handle(Geom_Surface));
	// ƽ��ת�� :Handle(Geom_Plane) = Handle(Geom_Plane)::DownCast(Handle(Geom_Surface));
	// ����ת�� :Handle(Geom_SphericalSurface) = Handle(Geom_SphericalSurface)::DownCast(Handle(Geom_Surface));
	vector<TopoDS_Face> GetTopoDS_FaceList() { return m_TopoDS_SurfList; }

	// ��ȡ��������� FSurface* �б�
	vector<FSurface*> GetFSurfList() { return m_FSurfList; }

	QList<QTreeWidgetItem*> getTreeList() { return m_itemList; }

	// �ж� FSurface* �����ͼ���������
	// paraSurfType:����
	// paraIndexInAllList:�� m_FSurfList �е�����
	// paraIndexInTypeList:�ڶ�Ӧ��m_XXXX_Inds�е�����
	// ���� aFSurf �� BSpline���棬m_FSurfList[9] = aFSurf��9��������� m_BSplineInds �е�����Ϊ5����m_BSplineInds[5] = 9
	// �򷵻�ֵΪ��paraSurfType = BSPLINE��paraIndexInAllList = 9��paraIndexInTypeList = 6;
	// paraSurfName:������
	// ���� aFSurf ��Բ���棬����Բ������������������Ϊ3����������Ϊ��Բ����3
	void FSurfInfo(FSurface* aFSurf, SURFACE_TYPE& paraSurfType, int& paraIndexInAllList, int& paraIndexInTypeList, QString& paraSurfName);

	// �ҵ� FSurface* ��Ӧ�� TopoDS_Face
	TopoDS_Face FSurf2TDSFace(FSurface* aFSurf);
	
	// ͨ����������������
	FSurface* GetFSurf(QString surfName);

	// �ҵ� FSurface* ��Ӧ�� Geom_BSplineSurface
	Handle(Geom_BSplineSurface) FSurf2BSpline(FSurface* aFSurf);

	// ��ȡ FSurface* ������
	QString FSurfName(FSurface* _surf);

	// ��ȡFSurface* ������
	SURFACE_TYPE FSurfType(FSurface* _surf);

	// �� FSurface* ����ɫ���ó�Ĭ����ɫ
	void setSurfColorDefault(FSurface* _surf);

	// �����������ó�Ĭ����ɫ
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

	// �����һ�ε����ģ�� - δ����
	void ClearPreviousSTEP();

	// �ҵ�����ͬһ��Բ���Ĵ�Բ���棬����2������������
	void SearchSubCylinder();

	FBody*                         m_pBody;
	QTreeWidgetItem*               m_pTree;
	QList<QTreeWidgetItem*>        m_itemList;

	//���������� m_FSurfList �е�����
	vector<int> m_BSplineInds;
	vector<int> m_ConicalInds;
	vector<int> m_CylindricalInds;
	vector<int> m_PlaneInds;
	vector<int> m_SphericalInds;
	vector<int> m_OtherInds;

	// ���ڿ����ж���������
	// key:Ŀ������������m_FSurfList�е�������value:Ŀ�����������ڶ�Ӧ��indexList�е�����
	unordered_map<int, int> m_Hashmap_BSplineInds;
	unordered_map<int, int> m_Hashmap_ConicalInds;
	unordered_map<int, int> m_Hashmap_CylindricalInds;
	unordered_map<int, int> m_Hashmap_PlaneInds;
	unordered_map<int, int> m_Hashmap_SphericalInds;
	unordered_map<int, int> m_Hashmap_OtherInds;

	// ��¼ FSurface* ������m_FSurfList ������
	unordered_map<FSurface*, int> m_HashFSurf2Ind;

	QProgressDialog* m_pProgBar;

	QString m_ImportPartName;

	// ����STEP��������� FSurface* ����
	vector<FSurface*> m_FSurfList;

	// ����STEP��������� TopoDS_Face ����
	vector<TopoDS_Face> m_TopoDS_SurfList;

	unordered_map<int, int> m_SameCylinderNearby;
	unordered_map<int, int> m_SameCylinderNearbyRev;

	ON_3dPoint m_MinCorner;
	ON_3dPoint m_MaxCorner;

	int CurrentFaceIndex;

	Handle(XCAFDoc_ColorTool) m_pColorTool;
	unordered_map<FSurface*, Quantity_Color> m_pFaceColorDefault;

};