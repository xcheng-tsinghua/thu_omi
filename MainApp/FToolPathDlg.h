/*
刀路规划窗口
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
class ToolMove_thread;  //注意先声明后使用
class PathShow;

class FToolPathDlg :public QWidget
{
	Q_OBJECT
public:
	FToolPathDlg(QWidget* parent = NULL);
	~FToolPathDlg(void);

	void CreateNURBS(FMainDoc* pDoc);
	Handle(Geom_BSplineSurface) NurbsToOcctSurf(const ON_NurbsSurface& theSurface); //转化为OCCT曲面

	F3dVector normal_vector(F3dVector du, F3dVector dv);
	void Interpolationcurve_Calcu(double Pt, double *PtV, double *dPt, double *ddPt);
	void ParaCspline_interpolation();

	//测点分布
	void MPtDistTest();

	double PntK(int i,double u,double v, F3dVector du, F3dVector dv, F3dVector duu, F3dVector duv, F3dVector dvv);   //平均曲率

	//基于曲面特征的测点分布方法//基于曲面特征的NURBS曲面测点分布
	void FBSampDis();	
	void SLSampDis(int i, int SampSize, int kNum, vector<vector<double>> FeaturePara, vector<vector<vector<double>>>& SampPnts,bool uorv);   //第i条NURBS曲线的采样

	
	//测点分布

	//曲面重构

	void NURBSdeform();   //通过NURBS曲面变形的方法实现重构，移动控制点
	void NURBSPntdeform();   //单点的控制点移动

	//基本方程
	double OneRijFun(double u, double v, int u_degree, int v_degree, int CVi, int CVj, vector<double> knotUarray, vector<double> knotVarray);
	int FindSpan(int n, int p, double u, vector <double> U);
	void BasisFuns(int i, double u, int p, vector <double> U, double N[]);

	//曲面重构

	void test();
	//刀具路径
	void ToolParaPath();
	void ToolMoveView();
	void ToolMoveView(vector<gp_Pnt> CLPntLine, vector<gp_Vec> CLPntVecLine);

	void ToolPath();
	void DrawToolPath();

	void clsExp();  //输出cls文件

	vector<gp_Pnt> Togpvec(vector<FIpPoint> dest);

	ToolSpec* m_ToolSpec;
	ToolMove_thread* m_ToolMove_thread;

	friend class ToolMove_thread;    //将子线程设置为友元

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
	FSurface* pSurf1;   //NURBS曲面移动控制点重构后的曲面
	ON_NurbsSurface* nb_surface;
	vector <double> m_U;
	vector <double> m_V;

	F3dPoint Pnt1, Pnt2, Pnt3;    //Pnt1,Pnt2,Pnt3分别为i点刀位，i+1点刀位, 曲线段的任意一点(需要平台计算)
	F3dPoint V1, V2;
	

	double m_uTpath, m_vTpath;
	double m_R, m_e, m_h;  //球头刀半径R，弓高允许误差e，和残余高度允差h

	vector<vector<double>> m_Currentpathpara;
	vector<vector<double>> m_Nextpathpara;
	vector<vector<double>> m_Totalpathpara;

	vector<F3dPoint> m_CCPnt;
	vector<F3dPoint> m_CLPnt;
	vector <F3dVector> m_Nvector;       //存储刀位点处法向量
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

	//测点分布




	//测点分布
private:
	//为输入框设定默认值
	void SetDefaultVal();

	//窗口界面成员变量
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


class ToolSpec : public FEntity   //刀具规格显示
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
	double m_R,m_L;     //m_R球头刀具半径，m_L刀具长度(圆柱的那部分长度)
};

class ToolMove_thread :public QThread
{
	Q_OBJECT

public:


protected:
	void run();

private slots:
	void getaddress(FToolPathDlg* Temp) { m_FToolPathDlg = Temp; }     //获取主线程的地址

private:
	FToolPathDlg* m_FToolPathDlg;
	void MovePntToPnt(gp_Pnt oriPnt, gp_Vec oriVec, gp_Pnt aimPnt, gp_Vec aimVec, int theMoveTime, int Smooth);

};








