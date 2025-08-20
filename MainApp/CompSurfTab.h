/*
ʹ�þ�������ķ���ʵ�ּӹ�����
���ۣ�����
ʵ�֣��Ų���
�޸ģ�����
*/

#pragma once
#include <qwidget.h>
#include "../External/fKernel/include/Fkerneldefines.h"
#include <Geom_BSplineSurface.hxx>
#include <numeric>

class CompSurfTab :public QWidget
{
	Q_OBJECT
public:
	CompSurfTab(QWidget* parent = NULL);
	~CompSurfTab(void);

	struct ConPnt  //�ع���
	{
		double uPara, vPara, ErrorDist;
		gp_Pnt MirrorInsec, ErrorInsec;
		gp_Vec Nvector;
		ConPnt(double p1, double p2) : uPara(p1), vPara(p2) {}
		ConPnt() : uPara(0), vPara(0) {}
	};

	void setSurface(FSurface* _surf);

private slots:
	void onRecstMeasFace();

	// ���������������
	void onGenCompadSurf();

	void onSaveCompensatedModel();
	void onFaceSel();

private:
	// ����ѡ�񹤾�
	//FFeatureTool* m_pFaceSelTool;

	// ��ʾѡ���������
	QLabel* m_pSelFaceNameLabel;

	//���ڽ����Ա����
	QLineEdit* m_pMachiningAllowance;

	// �ɱ�����ع���õ�����
	Handle(Geom_BSplineSurface) m_pRecstBSplSurf;
	FSurface* m_pRecstFace;

	// �����������
	Handle(Geom_BSplineSurface) m_pCompsedBSplSurf;
	FSurface* m_pCompsedFace;

	// ����ѡ������
	Handle(Geom_BSplineSurface) m_pSelBSplSurf;
	FSurface* m_pSelFSurf;

	// ���쾵�����棨�������棩ʱ�ɵ�ķֶ���
	int m_ErrEvaluSecUV;

	// ���油��ģ��ʱ���Ƿ�����������ĵ�������
	// == false ʱ���ȫ���棨�����������ģ���滻Ϊʵ��ģ�ͣ�
	bool m_is_out_compsurf_only;


};




