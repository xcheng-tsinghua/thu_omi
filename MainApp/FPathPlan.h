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

	bool Planning_FDA(FIntList& orders,FIpPntList& tIpPntList);  //使用TSP算法
	
	// 给定测点检测顺序的规划
	bool PathPlanGivenOrder(FIntList& orders, FIpPntList& tIpPntList);

	void SetOffset(double offset){m_offset=offset;};  //设置mesh偏移量
	void SetSafeBox(safebox *pt){SF_Box=pt;};  //设置安全盒子
	void MeasPt_Push(MeasPt *pt){m_pt.push_back(pt);};    //added by lym

	//值为0，快速，值为1，最短
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
	double m_offset;     //工件mesh偏移量
	safebox * SF_Box;   //安全盒子
	vector <MeasPt *>  m_pt;

	//值为0，快速，值为1，最短
	int TypeOfPathPlan;

};

#endif 