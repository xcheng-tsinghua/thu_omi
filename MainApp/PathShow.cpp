#include "PathShow.h"

PathShow::PathShow()
{

}

PathShow::PathShow(vector<gp_Pnt> _pathPoints, const double& _r, const double& _g, const double& _b, const double& _width)
{
	m_PathPoints = _pathPoints;

	m_R = _r;
	m_G = _g;
	m_B = _b;

	m_LineWidth = _width;

}

PathShow::~PathShow()
{

}

void PathShow::setPathPnts(const vector<gp_Pnt>& _pathPoints)
{
	m_PathPoints = _pathPoints;
}

void PathShow::setRGB(const double& _r, const double& _g, const double& _b)
{
	m_R = _r;
	m_G = _g;
	m_B = _b;
}

void PathShow::setLineWidth(double _width)
{
	m_LineWidth = _width;
}

void PathShow::clearPathPoints()
{
	m_PathPoints.clear();
}

void PathShow::appendPnt(const gp_Pnt& _pnt)
{
	m_PathPoints.push_back(_pnt);
}

void PathShow::Draw(const FT::FRenderOpt& renderOpt)
{
	glEnable(GL_LIGHTING);
	glColor3f(m_R, m_G, m_B);
	glLineWidth(m_LineWidth);

	glBegin(GL_LINE_STRIP);

	int pointsSize = m_PathPoints.size();
	for (int i = 0; i < pointsSize; i++)
	{
		glVertex3d(m_PathPoints[i].X(), m_PathPoints[i].Y(), m_PathPoints[i].Z());
	}

	glEnd();
	glDisable(GL_LIGHTING);

}
