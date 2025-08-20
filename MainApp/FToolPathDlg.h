/*
��·�滮����
*/
#pragma once
#include "qwidget.h";
#include "MainDoc.h"
#include "../External/fKernel/include/Fkerneldefines.h"
#include "../External/fKernel/include/FKernelHeader.h"
#include <Eigen/Dense>
#include "FGuideDataType.h"
#include "MeasPt.h"
#include "qvector.h"
#include <numeric>
#include <Geom_BSplineSurface.hxx>

using namespace FT;
using namespace std;
using Eigen::MatrixXd;

class ToolSpec;
class ToolMove_thread;  //ע����������ʹ��
class PathShow;

class FToolPathDlg :public QWidget
{
	Q_OBJECT
public:
	FToolPathDlg(QWidget* parent = NULL);
	~FToolPathDlg(void);

	void CreateNURBS(FMainDoc* pDoc);
	Handle(Geom_BSplineSurface) NurbsToOcctSurf(const ON_NurbsSurface& theSurface); //ת��ΪOCCT����

	F3dVector normal_vector(F3dVector du, F3dVector dv);
	void Interpolationcurve_Calcu(double Pt, double *PtV, double *dPt, double *ddPt);
	void ParaCspline_interpolation();

	//���ֲ�
	void MPtDistTest();

	double PntK(int i,double u,double v, F3dVector du, F3dVector dv, F3dVector duu, F3dVector duv, F3dVector dvv);   //ƽ������

	//�������������Ĳ��ֲ�����//��������������NURBS������ֲ�
	void FBSampDis();	
	void SLSampDis(int i, int SampSize, int kNum, vector<vector<double>> FeaturePara, vector<vector<vector<double>>>& SampPnts,bool uorv);   //��i��NURBS���ߵĲ���

	
	//���ֲ�

	//�����ع�

	void NURBSdeform();   //ͨ��NURBS������εķ���ʵ���ع����ƶ����Ƶ�
	void NURBSPntdeform();   //����Ŀ��Ƶ��ƶ�

	//��������
	double OneRijFun(double u, double v, int u_degree, int v_degree, int CVi, int CVj, vector<double> knotUarray, vector<double> knotVarray);
	int FindSpan(int n, int p, double u, vector <double> U);
	void BasisFuns(int i, double u, int p, vector <double> U, double N[]);

	//�����ع�

	void test();
	//����·��
	void ToolParaPath();
	void ToolMoveView();
	void ToolMoveView(vector<gp_Pnt> CLPntLine, vector<gp_Vec> CLPntVecLine);

	void ToolPath();
	void DrawToolPath();

	void clsExp();  //���cls�ļ�

	vector<gp_Pnt> Togpvec(vector<FIpPoint> dest);

	ToolSpec* m_ToolSpec;
	ToolMove_thread* m_ToolMove_thread;

	friend class ToolMove_thread;    //�����߳�����Ϊ��Ԫ

	vector<gp_Pnt> getCLPntLine() { return m_CLPntLine; }
	vector<gp_Vec> getCLPntVecLine() { return m_CLPntVecLine; }

	vector<gp_Pnt> m_CLPntLine;
	vector<gp_Vec> m_CLPntVecLine;

	void SetOcctSurf(Handle(Geom_BSplineSurface) AimSurf) { pOCCT_Surface = AimSurf; }

private:

signals:
	void preper_save(FToolPathDlg*);


private slots:
	void onToolPath();
	void onCreateNURBS();
	void onMPtDist();
	void onSurfaceRecons();
	void onSelSurf();

protected:
	FSurface* pSurf;
	FSurface* pSurf1;   //NURBS�����ƶ����Ƶ��ع��������
	ON_NurbsSurface* nb_surface;
	vector <double> m_U;
	vector <double> m_V;

	F3dPoint Pnt1, Pnt2, Pnt3;    //Pnt1,Pnt2,Pnt3�ֱ�Ϊi�㵶λ��i+1�㵶λ, ���߶ε�����һ��(��Ҫƽ̨����)
	F3dPoint V1, V2;
	

	double m_uTpath, m_vTpath;
	double m_R, m_e, m_h;  //��ͷ���뾶R�������������e���Ͳ���߶��ʲ�h

	vector<vector<double>> m_Currentpathpara;
	vector<vector<double>> m_Nextpathpara;
	vector<vector<double>> m_Totalpathpara;

	vector<F3dPoint> m_CCPnt;
	vector<F3dPoint> m_CLPnt;
	vector <F3dVector> m_Nvector;       //�洢��λ�㴦������
	vector<double> m_hV, m_tPntV;
	vector<vector<double>> m_vecM;
	vector<FIpPoint> m_CCPntview;       
	vector<FIpPoint> m_CLPntview;

	PathShow* m_myToolPath;

	vector<MeasPt*> m_Pntlist;

	vector<QString> m_clsExp;

	Handle(Geom_BSplineSurface) pOCCT_Surface;

	vector<FIpPntList> m_FIpToolPathList;
	//MeasPath *m_myToolPath;
	FMainDoc*	m_pDoc;

	//���ֲ�




	//���ֲ�
private:
	//Ϊ������趨Ĭ��ֵ
	void SetDefaultVal();

	//���ڽ����Ա����
	QLineEdit* ToolRadius;
	QLineEdit* StepPres;
	QLineEdit* ResidualHeight;
	QLineEdit* SafeDis;

	QLineEdit* ToolType;
	QLineEdit* ToolDiam;
	QLineEdit* ToolLength;
	QLineEdit* ToolTaperAngle;
	QLineEdit* ToolTipAngle;
	QLineEdit* ProcessingOriginX;
	QLineEdit* ProcessingOriginY;
	QLineEdit* ProcessingOriginZ;

	QPushButton* SurfGenerate;
	QPushButton* InspPntsPlan;
	QPushButton* SurfReconstruct;
	QPushButton* MachiningPathPlan;
	QPushButton* SelSurfBtn;

	FFeatureTool* m_pCurTool;		///select toolv
};


class ToolSpec : public FEntity   //���߹����ʾ
{
public:
	ToolSpec(double x = 0, double y = 0, double z = 0, double i = 0, double j = 0, double k = 1, double r = 5, double Length = 15)
	{
		m_x = x;
		m_y = y;
		m_z = z;
		m_i = i;
		m_j = j;
		m_k = k;
		m_R = r;
		m_L = Length;
	};
	double getm_i() { return m_i; };
	double getm_j() { return m_j; };
	double getm_k() { return m_k; };
	double getm_x() { return m_x; };
	double getm_y() { return m_y; };
	double getm_z() { return m_z; };
	double getm_R() { return m_R; };
	double getm_L() { return m_L; };

	~ToolSpec() {};

	virtual void Draw(const FT::FRenderOpt& renderOpt);
	void drawSphere(double xx, double yy, double zz, double radius, double M, double N);
	void drawCylinder(double xx, double yy, double zz, double nx, double ny, double nz, double radius, double Length);
	void drawCircle(double radius, double Length);

//protected:
	double m_x, m_y, m_z;   //position of the measuring point
	double m_i, m_j, m_k;    //normal direction of the point
	double m_R,m_L;     //m_R��ͷ���߰뾶��m_L���߳���(Բ�����ǲ��ֳ���)
};

class ToolMove_thread :public QThread
{
	Q_OBJECT

public:


protected:
	void run();

private slots:
	void getaddress(FToolPathDlg* Temp) { m_FToolPathDlg = Temp; }     //��ȡ���̵߳ĵ�ַ

private:
	FToolPathDlg* m_FToolPathDlg;
	void MovePntToPnt(gp_Pnt oriPnt, gp_Vec oriVec, gp_Pnt aimPnt, gp_Vec aimVec, int theMoveTime, int Smooth);

};








