/*	
\brief	Some commonly used functions are collected here. 
\author: Jacky
\date    2014-04
*/
#ifndef F_UTILITIES_H
#define F_UTILITIES_H

#include "../External/fkernel/Include/FKernelDefines.h"
#include <utility>
#include <vector>

using namespace std;

class FMainWindow;

namespace FT
{
	class FBody;
	class FMesh;
}
class ON_Line;
class ON_Mesh;
extern FMainWindow*			gWindow;
extern QString				gAppPath;
extern QSettings*			gAppSetting;

/*!
\brief	Display given hint message to status bar.
\param	[i]msgStr	given hint message.
\return	true if successful.*/
bool	gSetHintMessage(QString msgStr);
bool    gIsDirExist(QString fullPath);
bool    gIsFileExist(QString fullFileName);
/*!
\brief	Find the intersection points between the line and pMesh.
\param	[i]pMesh	triangular mesh model.
\param	[i]sPnt		start point of the line.
\param	[i]ePnt		end point of the line.
\param	[o]resPnts	intersection points.
\param	[i]lTol/rTol greater than 1-tol is considered as collision default is 0.
\return	true if intersected; false otherwise.*/
bool gCalLineMeshIntersection(const ON_Mesh *pMesh,F3dPoint sPnt, F3dPoint ePnt, F3dPointList& resPnts, double lTol=0, double rTol=0);
bool gCalLineMeshIntersection(const ON_Mesh *pMesh,F3dPoint sPnt, F3dPoint ePnt, F3dPointList& resPnts, vector<int>& interIdx, double lTol=0, double rTol=0);
bool gCalLineMeshIntersection_LYM(const ON_Mesh *pMesh,F3dPoint sPnt, F3dPoint ePnt, F3dPointList& resPnts, double lTol/*=0*/, double rTol/*=0*/,double offset);
/*!	
\brief	Calculate the intersection between the line and the plane.
\param	[i]pPlane	given plane 
\param	[i]pLine	given line
\param	[o]u		pLine->PointAt(u) is the intersection point, if any
\return	true if there has an intersection point.		*/
extern bool	  gCalPlaneLineIntersect(FPlane* pPlane, ON_Line* pLine, double& u);
#endif