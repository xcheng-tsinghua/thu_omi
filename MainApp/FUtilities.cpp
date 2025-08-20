#include <math.h>
#include "TriTriIntersect.h"
#include "../External/fkernel/Include/DataStructure/FBody.h"
#include "FGuideDataType.h"
#include "FMainWindow.h"
#include "FUtilities.h"

bool gSetHintMessage(QString msgStr)
{
	QLabel* hintLabel = gWindow->GetHintLabel();
	if(hintLabel)
	{
		hintLabel->setStyleSheet(
			"color:white;");

		hintLabel->setText(msgStr);
		hintLabel->setToolTip(msgStr);
		return true;
	}
	return false;
}

bool gIsDirExist( QString fullPath )
{
	QDir dir(fullPath);
	if(dir.exists())
	{
		return true;
	}
	else
	{
		bool ok = dir.mkdir(fullPath);//只创建一级子目录，即必须保证上级目录存在
		return ok;
	}
}

bool gIsFileExist( QString fullFileName )
{
	QFileInfo fileInfo(fullFileName);
	if(fileInfo.isFile())
	{
		return true;
	}
	return false;
}

bool gCalLineMeshIntersection(const ON_Mesh *pMesh,F3dPoint sPnt, F3dPoint ePnt, F3dPointList& resPnts, double lTol/*=0*/, double rTol/*=0*/)
{
	F3dPointList allPnts;
	const ON_Mesh* pOnMesh = pMesh;
	if (NULL == pOnMesh)	return false;
	ON_BoundingBox box = pOnMesh->BoundingBox();
	F3dVector rayDir = ePnt-sPnt;
	double rayPnt[3]={sPnt.x, sPnt.y, sPnt.z};
	ON_3fPoint onV[3];
	double v0[3],v1[3],v2[3], cc[3]; 
	double u(0), v(0), t(0);  
	int nFace = pOnMesh->FaceCount();
	//step1. intersection points between the ray and mesh
	for(int j=0; j<nFace; j++)
	{
		const ON_MeshFace& face = pOnMesh->m_F[j];
		onV[0] = pOnMesh->m_V[face.vi[0]];
		onV[1] = pOnMesh->m_V[face.vi[1]];
		onV[2] = pOnMesh->m_V[face.vi[2]];
		v0[0] = onV[0].x;	v0[1] = onV[0].y;	v0[2] = onV[0].z;
		v1[0] = onV[1].x;	v1[1] = onV[1].y;	v1[2] = onV[1].z;
		v2[0] = onV[2].x;	v2[1] = onV[2].y;	v2[2] = onV[2].z;
		if (intersect_triangle3(rayPnt,rayDir,v0,v1,v2,&t,&u,&v)) 
		{
			for(int i=0;i<3;i++)
			{
				cc[i] = (1-u-v)*v0[i]+u*v1[i]+v*v2[i];
			}
			allPnts.Append(F3dPoint(cc[0], cc[1], cc[2]));
		}
	}
	if (allPnts.size() < 1)	return false;
	//step2. Judge whether a point in the line and return the nearest intersection point
	ON_Line tLine(sPnt, ePnt);
	t=0;
	for (int i=0; i<allPnts.size(); i++)
	{
		tLine.ClosestPointTo(allPnts[i], &t);
		if (t>lTol && t<(1-rTol))
			resPnts.Append(allPnts[i]);
	}
	if (resPnts.size()>0)
		return true;
	else
		return false;
}

bool gCalLineMeshIntersection(const ON_Mesh *pMesh,F3dPoint sPnt, F3dPoint ePnt, F3dPointList& resPnts, vector<int>& interIndex, double lTol/*=0*/, double rTol/*=0*/)
{
	F3dPointList allPnts;
	const ON_Mesh* pOnMesh = pMesh;
	if (NULL == pOnMesh)	return false;
	ON_BoundingBox box = pOnMesh->BoundingBox();
	F3dVector rayDir = ePnt-sPnt;
	double rayPnt[3]={sPnt.x, sPnt.y, sPnt.z};
	ON_3fPoint onV[3];
	double v0[3],v1[3],v2[3], cc[3]; 
	double u(0), v(0), t(0);  
	int nFace = pOnMesh->FaceCount();
	pair<int,F3dPoint> dp;
	vector<pair<int,F3dPoint>> dv;
	//step1. intersection points between the ray and mesh
	for(int j=0; j<nFace; j++)
	{
		const ON_MeshFace& face = pOnMesh->m_F[j];
		onV[0] = pOnMesh->m_V[face.vi[0]];
		onV[1] = pOnMesh->m_V[face.vi[1]];
		onV[2] = pOnMesh->m_V[face.vi[2]];
		v0[0] = onV[0].x;	v0[1] = onV[0].y;	v0[2] = onV[0].z;
		v1[0] = onV[1].x;	v1[1] = onV[1].y;	v1[2] = onV[1].z;
		v2[0] = onV[2].x;	v2[1] = onV[2].y;	v2[2] = onV[2].z;
		if (intersect_triangle3(rayPnt,rayDir,v0,v1,v2,&t,&u,&v)) 
		{
			for(int i=0;i<3;i++)
			{
				cc[i] = (1-u-v)*v0[i]+u*v1[i]+v*v2[i];
			}
			allPnts.Append(F3dPoint(cc[0], cc[1], cc[2]));
			dp.first = j;
			dp.second = F3dPoint(cc[0], cc[1], cc[2]);
			dv.push_back(dp);
		}
	}
	if (allPnts.size() < 1)	return false;
	//step2. Judge whether a point in the line and return the nearest intersection point
	ON_Line tLine(sPnt, ePnt);
	t=0;
	for (int i=0; i<allPnts.size(); i++)
	{
		tLine.ClosestPointTo(allPnts[i], &t);
		if (t>lTol && t<(1-rTol))
			resPnts.Append(allPnts[i]);
	}
	
	if (resPnts.size()>0)
	{
		for(int j = 0; j < resPnts.size(); j++)
		{
			for(int k = 0; k < dv.size(); k++)
			{
				if(resPnts[j] == dv[k].second)
					interIndex.push_back(dv[k].first);
			}
		}
		return true;
	}
	else
		return false;
}

bool gCalPlaneLineIntersect(FPlane* pPlane, ON_Line* pLine, double& u)
{
	F3dVector dir = pLine->Direction();
	double denom =	pPlane->plane_equation.x*dir.x +
		pPlane->plane_equation.y*dir.y + 
		pPlane->plane_equation.z*dir.z;
	if(fabs(denom)<EPSILON)		return false;
	u = -(pPlane->plane_equation.d+pPlane->plane_equation.x*pLine->from.x+
		pPlane->plane_equation.y*pLine->from.y+pPlane->plane_equation.z*pLine->from.z)/denom;
	return true;
}

bool gCalLineMeshIntersection_LYM(const ON_Mesh *pMesh,F3dPoint sPnt, F3dPoint ePnt, F3dPointList& resPnts, double lTol/*=0*/, double rTol/*=0*/,double offset)
{
	F3dPointList allPnts;
	const ON_Mesh* pOnMesh = pMesh;   //get geometry mesh
	if (NULL == pOnMesh)	return false;    //if there is no mesh, return false
	ON_BoundingBox box = pOnMesh->BoundingBox();   //bounding box??
	F3dVector rayDir = ePnt-sPnt;    //direction of the line
	double rayPnt[3]={sPnt.x, sPnt.y, sPnt.z};    //start point
	ON_3fPoint onV[3];    //vertex
	ON_3fPoint onN[3];    //normal 
	double v0[3],v1[3],v2[3], cc[3]; 
	double u(0), v(0), t(0);  
	int nFace = pOnMesh->FaceCount();    //get number of faces
	//step1. intersection points between the ray and mesh
	for(int j=0; j<nFace; j++)
	{
		const ON_MeshFace& face = pOnMesh->m_F[j];
		onV[0] = pOnMesh->m_V[face.vi[0]];  //vertex
		onV[1] = pOnMesh->m_V[face.vi[1]];
		onV[2] = pOnMesh->m_V[face.vi[2]];

		onN[0]=pOnMesh->m_N[face.vi[0]];  //normal
		onN[1]=pOnMesh->m_N[face.vi[1]];
		onN[2]=pOnMesh->m_N[face.vi[2]];

		//double vecsize=sqrt(onN[0].x*onN[0].x+onN[0].y*onN[0].y+onN[0].z*onN[0].z);
		v0[0] = onV[0].x+onN[0].x*offset;	
		v0[1] = onV[0].y+onN[0].y*offset;	
		v0[2] = onV[0].z+onN[0].z*offset;
		//cout<<onN[0].x<<"  "<<onN[0].y<<"  "<<onN[0].z<<endl;
		//cout<<offset<<endl;
		//vecsize=sqrt(onN[1].x*onN[1].x+onN[1].y*onN[1].y+onN[1].z*onN[1].z);
		v1[0] = onV[1].x+onN[1].x*offset;	
		v1[1] = onV[1].y+onN[1].y*offset;	
		v1[2] = onV[1].z+onN[1].z*offset;

		//vecsize=sqrt(onN[2].x*onN[2].x+onN[2].y*onN[2].y+onN[2].z*onN[2].z);
		v2[0] = onV[2].x+onN[2].x*offset;	
		v2[1] = onV[2].y+onN[2].y*offset;	
		v2[2] = onV[2].z+onN[2].z*offset;

		if (intersect_triangle3(rayPnt,rayDir,v0,v1,v2,&t,&u,&v)) 
		{
			for(int i=0;i<3;i++)
			{
				cc[i] = (1-u-v)*v0[i]+u*v1[i]+v*v2[i];
			}
			allPnts.Append(F3dPoint(cc[0], cc[1], cc[2]));
		}
	}
	if (allPnts.size() < 1)	return false;
	//step2. Judge whether a point in the line and return the nearest intersection point
	ON_Line tLine(sPnt, ePnt);
	t=0;
	for (int i=0; i<allPnts.size(); i++)
	{
		tLine.ClosestPointTo(allPnts[i], &t);
		if (t>lTol && t<(1-rTol))
			resPnts.Append(allPnts[i]);
	}
	if (resPnts.size()>0)
		return true;
	else
		return false;
}

