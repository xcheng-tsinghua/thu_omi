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

	void SetIsFastPathPlan(bool _is_fast);
	void SetIsFaseColliJudge(bool _is_fast);

	void SetProgress(QProgressBar* _progress);

protected:
	void		WriteTSPProblemFile();	//write m_ipPntList to TSP
	bool		BasicPath(FIntList& basicPath);	//[o] basic path not consider back points and collision
	void		ComputeNoCollisionPath(FIpPntList& ipPntList, FIpPathList& pathlist);  // 改进后的算法，更精确，并且是红宝石球移动过程的圆管与模型求交
	void		ComputeNoCollisionPathPrev(FIpPntList& ipPntList, FIpPathList& pathlist); // 先前的原始方式，速度快，但是可能出现小bug，例如路径中某段在某个面中时，不会判定为干涉

protected:
	// 是否回退过程中干涉
	//bool IsRetreatColli(const FIpPntList& ipPntList);

	FIpPntList		m_ipPntList;	///inspection points
	FMainDoc*		m_pDoc;
	FPathControl*	m_FPathControl;	///temporary used
	double			m_backDist;		///distance used to compute back points, default 6mm
	double			m_safeDist;		///distance between safe plane and top face of bbox,default 20mm
	//ON_Mesh*		m_pObstacleMesh;///used to check collision for generated paths
	double m_offset;     //工件mesh偏移量
	safebox * SF_Box;   //安全盒子
	vector <MeasPt *>  m_pt;

	// 是否使用快速路径规划算法
	bool m_is_fast_path_plan;

	// 是否使用快速干涉检测算法
	bool m_is_fast_collision_judge;

	QProgressBar* mp_progress;

};

#endif 