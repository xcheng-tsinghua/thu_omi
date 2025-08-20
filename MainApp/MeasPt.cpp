#include "MeasPt.h"
#include "FMainWindow.h"

extern FMainWindow* gWindow;

MeasPt::MeasPt(double x, double y, double z, double i, double j, double k, double normMag)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_i = i;
	m_j = j;
	m_k = k;
	m_NormMagnificent = normMag;

	gWindow->GetMainDoc()->getPntsShowColorDefault(m_r, m_g, m_b);

	//m_r = 0;
	//m_g = 0;
	//m_b = 1;

	m_PointSize = 8;
	m_NormWidth = 1;

}

MeasPt::MeasPt(gp_Pnt _loc, gp_Vec _dir)
{
	m_x = _loc.X();
	m_y = _loc.Y();
	m_z = _loc.Z();
	m_i = _dir.X();
	m_j = _dir.Y();
	m_k = _dir.Z();
	m_NormMagnificent = 8;

	gWindow->GetMainDoc()->getPntsShowColorDefault(m_r, m_g, m_b);

	//m_r = 0;
	//m_g = 0;
	//m_b = 1;

	m_PointSize = 8;
	m_NormWidth = 1;
}

MeasPt::MeasPt(FIpPoint _loc, double normMag)
{
	m_x = _loc.x;
	m_y = _loc.y;
	m_z = _loc.z;
	m_i = _loc.i;
	m_j = _loc.j;
	m_k = _loc.k;

	m_r = 0;
	m_g = 0;
	m_b = 1;

	m_NormMagnificent = normMag;

	m_PointSize = 8;
	m_NormWidth = 1;
}

MeasPt::MeasPt(double x, double y, double z, double i, double j, double k, double r, double g, double b, double normMag)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_i = i;
	m_j = j;
	m_k = k;
	m_r = r;
	m_g = g;
	m_b = b;

	m_NormMagnificent = normMag;

	m_PointSize = 8;
	m_NormWidth = 1;
}

MeasPt::~MeasPt()
{

}

void MeasPt::setColor(double _r, double _g, double _b)
{
	m_r = _r;
	m_g = _g;
	m_b = _b;

}

void MeasPt::setPointSize(double _pointSize)
{
	m_PointSize = _pointSize;
}

void MeasPt::setNormWidth(double _normWidth)
{
	m_NormWidth = _normWidth;
}

void MeasPt::Draw(const FT::FRenderOpt& renderOpt)
{
	glColor3f(m_r, m_g, m_b);
	glPointSize(m_PointSize);

	// »­²âµã
	glDisable(GL_LIGHTING);
	glBegin(GL_POINTS);
	glVertex3d(m_x, m_y, m_z);
	glEnd();

	// »­·¨Ïß
	glLineWidth(m_NormWidth);
	glBegin(GL_LINE_STRIP);
	glVertex3d(m_x, m_y, m_z);

	glVertex3d(m_x + m_i * m_NormMagnificent, m_y + m_j * m_NormMagnificent, m_z + m_k * m_NormMagnificent);
	glEnd();

}
