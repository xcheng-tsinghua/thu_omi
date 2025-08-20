/*	
Description:
	This class generates a collision-free inspection path,stored in FDoc::m_ipPathList
	given a set of inspection points.
Usage:
	FPathPlane pathPlane;
	pathPlan.SetObstackMesh(pMesh);
	pathPlan.SetBackDist(4);	//default 6mm
	pathPlan.SetIpPoints(pntlist);
	pathPlan.Planning();
Copyright 
	\author: Jacky, L.,ZENG
	\date    2018-05
*/
#ifndef	F_PATHPLAN_H
#define	F_PATHPLAN_H
#pragma once

#include <windows.h>
#include <math.h>
#include "FUtilities.h"
#include "FGuideDataType.h"

using namespace FT;
class FMainDoc;
class FPathControl;
class ON_Mesh;
class safebox;
class MeasPt;
class FPathPlan

{
public:
	FPathPlan();
	FPathPlan(FMainDoc* pDoc);
	~FPathPlan();
	//accessing methods
	double		GetBackDist(){return m_backDist;}
	void		SetBackDist(double dist){m_backDist=dist;}
	double		GetSafeDist(){return m_safeDist;}
	void		SetSaftDist(double dist){m_safeDist=dist;}
	//ON_Mesh*	GetObstacleMesh(){return m_pObstacleMesh;}
	//void		SetObstacleMesh(ON_Mesh* pMesh){m_pObstacleMesh=pMesh;}
	void		SetIpPntList(FIpPntList& pntlist){m_ipPntList=pntlist;}
	//public methods
	void		Init();
	void		ClearPnts();

	bool		Planning(FIntList& orders, FIpPathList& ipPathList);

	bool Planning_FDA(FIntList& orders,FIpPntList& tIpPntList);  //ʹ��TSP�㷨
	
	// ���������˳��Ĺ滮
	bool PathPlanGivenOrder(FIntList& orders, FIpPntList& tIpPntList);

	void SetOffset(double offset){m_offset=offset;};  //����meshƫ����
	void SetSafeBox(safebox *pt){SF_Box=pt;};  //���ð�ȫ����
	void MeasPt_Push(MeasPt *pt){m_pt.push_back(pt);};    //added by lym

	//ֵΪ0�����٣�ֵΪ1�����
	void setPathPlanType(int _type);



protected:
	void		WriteTSPProblemFile();	//write m_ipPntList to TSP
	bool		BasicPath(FIntList& basicPath);	//[o] basic path not consider back points and collision
	void		ComputeNoCollisionPath(FIpPntList& ipPntList, FIpPathList& pathlist);	//[o]collision-free path

protected:
	FIpPntList		m_ipPntList;	///inspection points
	FMainDoc*		m_pDoc;
	FPathControl*	m_FPathControl;	///temporary used
	double			m_backDist;		///distance used to compute back points, default 6mm
	double			m_safeDist;		///distance between safe plane and top face of bbox,default 20mm
	//ON_Mesh*		m_pObstacleMesh;///used to check collision for generated paths
	double m_offset;     //����meshƫ����
	safebox * SF_Box;   //��ȫ����
	vector <MeasPt *>  m_pt;

	//ֵΪ0�����٣�ֵΪ1�����
	int TypeOfPathPlan;

};

#endif 