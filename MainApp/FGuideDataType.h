/*
brief this class is transfered from ThuOMI v1.0
*/

#ifndef _FGUILDDATATYPE_H_
#define _FGUILDDATATYPE_H_

#include <QVector>
#include <QVector3D>
#include "../External/fkernel/Include/FKernelDefines.h"
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

class FIpPoint
{
public:
	FIpPoint();           // protected constructor used by dynamic creation
	FIpPoint(double _x, double _y, double _z);
	FIpPoint(gp_Pnt _loc, gp_Vec _dir);

	bool		isVaild() const;
	F3dPoint	Point(){return F3dPoint(x, y, z);}
	double		DistanceTo(FIpPoint aPnt) { return sqrt((x - aPnt.x) * (x - aPnt.x) + (y - aPnt.y) * (y - aPnt.y) + (z - aPnt.z) * (z - aPnt.z)); }
	double		DistanceTo(F3dPoint aPnt) { return sqrt((x - aPnt.x) * (x - aPnt.x) + (y - aPnt.y) * (y - aPnt.y) + (z - aPnt.z) * (z - aPnt.z)); }
	double		DistanceTo(gp_Pnt aPnt) { return sqrt((x - aPnt.X()) * (x - aPnt.X()) + (y - aPnt.Y()) * (y - aPnt.Y()) + (z - aPnt.Z()) * (z - aPnt.Z())); }

	bool operator == (FIpPoint aIpPnt)
	{
		return (this->x == aIpPnt.x) && (this->y == aIpPnt.y) && (this->z == aIpPnt.z) && (this->i == aIpPnt.i) && (this->j == aIpPnt.j) && (this->k == aIpPnt.k);
	}

	// Attributes
	double x, y, z;		///theoretic point coordinates
	double i, j, k;		///normal data of theoretic point
	double mx, my, mz;	///measure point, not used in this version
	double r;		///???
	double a;		///???
	double cot;		///???
	double err, uperror, downerror;		///point/upper/down error
	double deg;		///theta, used in inspection points of calibration
	double degz;	///fai, used in inspection points of calibration
	int		num;		//点序号
	int		symbol;		//指导文件-点信息中第7个数值
	int		flag;		///1: inspection point; 0: intermediate point

	int flag_lym;    //是否为回退点.
	double meas_dx, meas_dy, meas_dz;    //测量方向
	double safe_px, safe_py, safe_pz;     // safe point. by lym
	double probe_dx,probe_dy,probe_dz;   //测头方向

	int BType[6];    //回退类型
	double Safelen[6];    //回退安全点的距离

	int EndLabel;   //端点标签
	int GroupNum;   //组号

};
typedef QVector<FIpPoint>	FIpPntList;

class FIpPath
{
public:
	explicit FIpPath();
	/////////////////////////////// Public methods //////////////////////////////////
	int			GetSize()  { return m_pathPntList.size(); }
	FIpPntList& GetPathPnts()  { return m_pathPntList; }
	FIpPoint&	GetStartPnt() 	{ return m_pathPntList[0]; }
	FIpPoint&	GetEndPnt() 	{ return m_pathPntList[m_pathPntList.size()-1]; }
	FIpPoint&	GetPoint(int _index){if(_index>=0 && _index<m_pathPntList.size()) return m_pathPntList[_index];}
	void		AddPnt( double x, double y, double z ){m_pathPntList.push_back(FIpPoint(x, y, z));}
	void		AddPnt( const FIpPoint& p ){m_pathPntList.push_back(p);}

private:
	FIpPntList	 m_pathPntList;
};

typedef QVector<FIpPath> FIpPathList;

#endif
