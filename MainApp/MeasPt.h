/*
������ʾ�ĵ�����
*/
#pragma once
#include "../External/fkernel/Include/FEntity.h"
#include "FGuideDataType.h"

class MeasPt : public FEntity
{
public:
	MeasPt(double x = 0, double y = 0, double z = 0, double i = 0, double j = 0, double k = 0, double normMag = 8);
	MeasPt(gp_Pnt _loc, gp_Vec _dir);
	MeasPt(FIpPoint _loc, double normMag = 8);
	MeasPt(double x, double y, double z, double i, double j, double k, double r, double g, double b, double normMag);
	~MeasPt();

	void setColor(double _r, double _g, double _b);
	void setPointSize(double _pointSize = 8);
	void setNormWidth(double _normWidth = 1);

	double& getm_i() { return m_i; };
	double& getm_j() { return m_j; };
	double& getm_k() { return m_k; };
	double& getm_x() { return m_x; };
	double& getm_y() { return m_y; };
	double& getm_z() { return m_z; };
	double& getm_NormMag() { return m_NormMagnificent; };

	virtual void Draw(const FT::FRenderOpt& renderOpt);

protected:
	// ����
	double m_x, m_y, m_z;

	// ����
	double m_i, m_j, m_k;

	// ��ɫ
	double m_r, m_g, m_b;

	// �����ӳ�����
	double m_NormMagnificent;

	// ���С
	double m_PointSize;

	// �����߿�
	double m_NormWidth;

};