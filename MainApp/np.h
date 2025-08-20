#pragma once

#include <vector>
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <qstring.h>
#include "../External/fKernel/include/Fkerneldefines.h"
#include "../External/fKernel/include/FKernelHeader.h"
#include "FGuideDataType.h"
#include "FInsFeature.h"
#include <Precision.hxx>
#include <TopoDS_Shape.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include "occConverter.h"

#define CROSS(dest,v1,v2)                      \
              dest.SetX(v1.Y()*v2.Z()-v1.Z()*v2.Y()); \
              dest.SetY(v1.Z()*v2.X()-v1.X()*v2.Z()); \
              dest.SetZ(v1.X()*v2.Y()-v1.Y()*v2.X());

#define DOT(v1,v2) (v1.X()*v2.X()+v1.Y()*v2.Y()+v1.Z()*v2.Z())

#define SUB(dest,v1,v2) dest.SetX(v1.X()-v2.X()); dest.SetY(v1.Y()-v2.Y()); dest.SetZ(v1.Z()-v2.Z()); 

#define ADD(dest,v1,v2) dest.SetX(v1.X()+v2.X()); dest.SetY(v1.Y()+v2.Y()); dest.SetZ(v1.Z()+v2.Z()); 

#define DSQRT(V1,V2) V1.X()*V2.X()+V1.Y()*V2.Y()+V1.Z()*V2.Z()

#define LENGTH(vec) sqrt(vec.X() * vec.X() + vec.Y() * vec.Y() + vec.Z() * vec.Z())

#define GP_PNT(tmpPnt) gp_Pnt((tmpPnt).x, (tmpPnt).y, (tmpPnt).z)

#define GP_F3DVEC(tmpPnt) F3dVector(tmpPnt.X(),tmpPnt.Y(),tmpPnt.Z())

// ���ȶ���
#define PRECISION 1.e-4

using namespace std;

struct coor
{
	int i;
	int j;
};

vector<double> linspace(double start, double end, int sec);

vector<double> reverse(vector<double>lit, vector<double>lar, vector<double>aim);//return the correspond index in lar

double MIN(vector<double> mylist);//mylist��һ��������

pair<double, int> MIN_INDEX(vector<double> mylist);//mylist��һ��������

pair<double, coor> MIN(vector<vector<double>> mylist);

double MAX(vector<double> mylist);//mylist��һ��������

pair<double, coor> MAX(vector<vector<double>> mylist);

pair<double, int> MAX_INDEX(vector<double> mylist);

double LEN(gp_Vec dest);

pair<double, int> PntToLine(double xp, double yp, double L1x, double L1y, double L2x, double L2y);

template<class _T>
vector<_T>VecDelete(vector<_T> dest, int index)
{
    vector<_T>tmp;
    for (int i = 0; i < dest.size(); i++)
    {
        if (i != index)
        {
            tmp.push_back(dest[i]);
        }
    }
    return tmp;
};

template<class _T>
vector<_T>VecDeleteSeq(vector<_T> dest, vector<int> index)
{
    //�Ƚ�Ŀ��������Ԫ��ȫ���滻Ϊ999999
    for (int i = 0; i < index.size(); i++)
    {
        dest[index[i]] = 999999;
    }
    //ɾ����ֵΪ999999��Ԫ��
    vector<_T>tmp;
    for (int i = 0; i < dest.size(); i++)
    {
        if (dest[i] != 999999)
        {
            tmp.push_back(dest[i]);
        }
    }
    return tmp;
};

vector<double>VecInsert(vector<double> dest, pair<double, int> InsAim);

bool distance(double& result, gp_Pnt orig, gp_Pnt vert0, gp_Pnt vert1, gp_Pnt vert2);

bool Edistance(double& result, gp_Pnt orig, gp_Pnt vert0, gp_Pnt vert1, gp_Pnt vert2);

// ����FSurface�����������Ϊ��Mesh��Facet����ۼ�
double FSurfaceArea(FSurface* paraFSurf);

template<class _T>
_T Average(vector<_T> theList)
{
    _T theVal = 0;

    for (auto ita = theList.begin(); ita != theList.end(); ++ita)
    {
        theVal += (*ita);
    }

    theVal /= theList.size();

    return theVal;
}

//��ȡ����
int ExNumFrQsg(QString aString);

//��ȡ������
double ExDouFrQsg(QString aString);

ON_NurbsSurface* OcctSurfToNurbs(Handle(Geom_BSplineSurface) theOcctSurf);

Handle(Geom_BSplineSurface) NurbsToOcctSurf(const ON_NurbsSurface& theSurface);

double Distance_IpPnt(FIpPoint Pnt1, FIpPoint Pnt2);

double LengthOfPath(FIpPathList ipPathList);

bool LeastSquareFitSphere(vector<gp_Pnt> PntList, gp_Pnt& theCenter, double& theRadius);

double sci2db(const std::string& strSci);

double sci2dbEX(const std::string& strSci);

FIpPntList readPoinsFromTXT(QString theFileName);

vector<gp_Pnt> readPoinsFromTXT(QString theFileName, int x_col, int y_col, int z_col, char _sep = '\t');

vector<gp_Pnt> IpPntToGpPnt(FIpPntList thePntList);

gp_Pnt CoorOrg();

void initTree_by_insfea(FInsFeature* FinsFea, QTreeWidget* theTree);

void MesgBox(QString ShowMsge);

// �������ĵİ汾
void MesgBox2(const char* _chineseMsge);

template<class _T>
// QTreeWidgetItem,ind=0
// QListWidgetItem,ind=3
inline void SetSurfToItem(FSurface* theSurf, _T* theItem, int ind = 0)
{
    theItem->setData(ind, Qt::UserRole, QVariant::fromValue((void*)(theSurf)));
}

template<class _T>
// QTreeWidgetItem,ind=0
// QListWidgetItem,ind=3
inline FSurface* GetSurfFromItem(_T* theItem, int ind = 0)
{
    return (FSurface*)(theItem->data(ind, Qt::UserRole).value<void*>());
}

int RandSeedByTimeNow();

// 2022��6,7,8,9,10,11��������ʱ��
vector<QString> RandomTime(int Month, int Count);

void ShowOcctEdge(TopoDS_Edge theEdge);

void ShowShapeEdge(TopoDS_Shape ShapeShow);

void ShowPoints(gp_Pnt fpPnt, double fpR = 1, double fpG = 0, double fpB = 0, double fpPntSize = 3);

void ShowPoints(double fpX, double fpY, double fpZ, double fpR = 1, double fpG = 0, double fpB = 0, double fpPntSize = 3);

void ShowPoints(vector<gp_Pnt> fpPnt, double fpR = 1, double fpG = 0, double fpB = 0, double fpPntSize = 3);

vector<gp_Pnt> ExWireCorners(TopoDS_Shape fpWire);

vector<gp_Pnt> ExEdgePnts(TopoDS_Edge fpEdge, bool IsReverse = false, int fpSec = 20);

template<class _T>
inline vector<_T> VecMerge(vector<_T> FirstVec, vector<_T> SecondVec)
{
    vector<_T> MergedVec;

    for (auto ita = SecondVec.begin(); ita != SecondVec.end(); ++ita)
    {
        MergedVec.push_back(*ita);
    }

    return MergedVec;
}

Standard_Real EdgeLength(const TopoDS_Edge& edge);

Handle(Geom_BSplineCurve) fit_BSplineCurve(vector<gp_Pnt> DataPntList, int CurveDeg = 2);

Standard_Real CurveCurvature(const Handle(Geom_BSplineCurve)& curve, const Standard_Real parameter);

void ShowOcctCurve(Handle(Geom_BSplineCurve) theCurve);

// ��������fpFace�ϵ�fpPnt���ĵ�λ������
gp_Vec NormalAt(TopoDS_Face fpFace, gp_Pnt fpPnt);

bool DPointToTriangle(double& result, gp_Pnt orig, gp_Pnt vert0, gp_Pnt vert1, gp_Pnt vert2, double prec = 0.05);//����㵽������Ƭ�ľ���

// ʹ��opencascade�ڲ���������һ���㵽һ����״�ľ��룬��״����ʹEdge��Face
double DistPoint2Shape(const gp_Pnt& fpPoint, const TopoDS_Shape& fpShape);

vector<gp_Pnt> mix_sampling(Handle(Geom_BSplineCurve) theCurve, int theNum, double CurWeight = 0.5);

Handle(Geom_BSplineSurface) ConvertFaceToBSplineSurface(const TopoDS_Face& face);

FSurface* BSpSurf2FSurf(Handle(Geom_BSplineSurface) fpBspSurf);

void SaveToStepFile(Handle(Geom_BSplineSurface) TheSurf, Standard_CString filename);

// �ж�ĳ�����Ƿ�����״��
bool isPointInShape(const gp_Pnt& fpPnt, const TopoDS_Shape& fpShape, double Tol = PRECISION);

// �ж�ĳ�����Ƿ�����״��
bool isPointInFace(const gp_Pnt& fpPnt, FSurface* fpShape, double Tol = PRECISION);

// �ж�ĳ�����Ƿ�����״��
bool isPointInFace(const gp_Pnt& fpPnt, const unordered_set<FSurface*>& fpShape, double Tol = PRECISION);

Handle(Geom_BSplineSurface) fit_BSplSurfLeastSquare(const vector<gp_Pnt>& fpPointsAll);

QString FileToString(QString fpFilePath);

void SurfParaSpace(Handle(Geom_BSplineSurface) theSurf, double& uStart, double& uEnd, double& vStart, double& vEnd);

bool isSuffixXXX(string _filename, string _suffix = "txt");

// �����߶�����֮���Ƿ��н���
bool isFaceIntLineseg(const TopoDS_Face& _face, const gp_Pnt& _startPnt, const gp_Pnt& _endPnt, const double& _min_dist);

// ����_startPnt��_endPnt�����ߺʹ򿪵�STEPģ���Ƿ��ཻ
bool isIntersectToModel(const gp_Pnt& _startPnt, const gp_Pnt& _endPnt, const double& _min_dist);

// ������Ľǵ������Ĳ�����Χ
void faceBndUseVert(const TopoDS_Face& _F, Standard_Real& _UMin, Standard_Real& _UMax, Standard_Real& _VMin, Standard_Real& _VMax);

// ���ɷ�Χ�� [0, 1] ֮��� Hammersley ����
vector<double> Hammersley(int _N);

// �ж� FSurface �Ƿ�Ϊ��Ӧ����
bool isSurfType(FSurface* _surf, const SURFACE_TYPE& _target);

// ��������ɾ��ָ��ֵ��Ԫ��
template<class _T>
inline void removeVal(vector<_T>& _vec, const _T& _val)
{
    // ʹ�� std::remove �����в����� value_to_remove ��Ԫ���ƶ���ǰ��
    auto new_end = std::remove(_vec.begin(), _vec.end(), _val);

    // ʹ�� vector �� erase ����ɾ���� new_end �� vec.end() ��Ԫ��
    _vec.erase(new_end, _vec.end());
}

// ��QListWidget��ɾ��ָ���������Ŀ��ʹ��ǰ���뽫FSurface*����QListWidgetItem��
// ��ŷ�ʽ: ListItem->setData(Qt::UserRole, QVariant::fromValue((void*)(_surf)));
void removeListItem(QListWidget* _widget, FSurface* _surf);

// ����С����ת��Ϊ��
double zeroLim(double _val);

// ��������㼯������
gp_Pnt massCenter(vector<gp_Pnt> _points);

// ��ƽ�����͵�TopoDS_Face����ȡ�޽�ƽ��gp_Pln
gp_Pln extractPln(const TopoDS_Face& _face);

