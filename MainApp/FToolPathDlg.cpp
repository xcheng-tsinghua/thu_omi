#include "FToolPathDlg.h"
#include "../MainApp/MainDoc.h"
#include "FMainWindow.h"
#include "GLU.h"
#include <math_Gauss.hxx>
#include "Tools/ToolPathTool.h"
#include "MeasPt.h"
#include "PathShow.h"
#include <BSplCLib.hxx>

FToolPathDlg::FToolPathDlg(QWidget* parent)
{
	m_ToolMove_thread = new ToolMove_thread;
	connect(this, SIGNAL(preper_save(FToolPathDlg*)), m_ToolMove_thread, SLOT(getaddress(FToolPathDlg*)));

	m_ToolSpec = new ToolSpec(0, 0, 0, 0, 0, 1, m_R, 15);

	m_pCurTool = gWindow->GetCurTool();
	this->setWindowFlags(Qt::WindowStaysOnTopHint);//永远在其他窗口之上
	move(1250, 150);
	this->setWindowTitle(QString::fromLocal8Bit("刀路规划"));
	this->setWindowIcon(QIcon("../icons/ToolPath.png"));

	QLabel* Label_ToolRadius = new QLabel(QString::fromLocal8Bit("圆角半径"));
	ToolRadius = new QLineEdit;
	QLabel* Label_StepPres = new QLabel(QString::fromLocal8Bit("步长精度"));
	StepPres = new QLineEdit;
	QLabel* Label_ResidualHeight = new QLabel(QString::fromLocal8Bit("残余高度"));
	ResidualHeight = new QLineEdit;
	QLabel* Label_SafeDis = new QLabel(QString::fromLocal8Bit("安全距离"));
	SafeDis = new QLineEdit;

	QLabel* Label_ToolType = new QLabel(QString::fromLocal8Bit("刀具类型"));
	ToolType = new QLineEdit;
	QLabel* Label_ToolDiam = new QLabel(QString::fromLocal8Bit("刀具直径"));
	ToolDiam = new QLineEdit;
	QLabel* Label_ToolLength = new QLabel(QString::fromLocal8Bit("刀具长度"));
	ToolLength = new QLineEdit;
	QLabel* Label_ToolTaperAngle = new QLabel(QString::fromLocal8Bit("刀具锥角"));
	ToolTaperAngle = new QLineEdit;
	QLabel* Label_ToolTipAngle = new QLabel(QString::fromLocal8Bit("刀具尖角"));
	ToolTipAngle = new QLineEdit;
	QLabel* Label_ProcessingOriginX = new QLabel(QString::fromLocal8Bit("原点坐标 X"));
	ProcessingOriginX = new QLineEdit;
	QLabel* Label_ProcessingOriginY = new QLabel(QString::fromLocal8Bit("原点坐标 Y"));
	ProcessingOriginY = new QLineEdit;
	QLabel* Label_ProcessingOriginZ = new QLabel(QString::fromLocal8Bit("原点坐标 Z"));
	ProcessingOriginZ = new QLineEdit;

	SurfGenerate = new QPushButton(QString::fromLocal8Bit("生成曲面"));
	InspPntsPlan = new QPushButton(QString::fromLocal8Bit("测点规划"));
	SurfReconstruct = new QPushButton(QString::fromLocal8Bit("切削演示"));
	MachiningPathPlan = new QPushButton(QString::fromLocal8Bit("刀路规划"));
	SelSurfBtn = new QPushButton(QString::fromLocal8Bit("选择曲面"));

	QGridLayout* MainLayout = new QGridLayout(this);
	MainLayout->addWidget(SurfGenerate, 0, 0);
	MainLayout->addWidget(InspPntsPlan, 1, 0);
	MainLayout->addWidget(SurfReconstruct, 2, 0);
	MainLayout->addWidget(MachiningPathPlan, 3, 0);
	MainLayout->addWidget(SelSurfBtn, 4, 0);

	MainLayout->addWidget(Label_ToolRadius, 0, 1);
	MainLayout->addWidget(ToolRadius, 0, 2);
	MainLayout->addWidget(Label_StepPres, 1, 1);
	MainLayout->addWidget(StepPres, 1, 2);
	MainLayout->addWidget(Label_ResidualHeight, 2, 1);
	MainLayout->addWidget(ResidualHeight, 2, 2);
	MainLayout->addWidget(Label_SafeDis, 3, 1);
	MainLayout->addWidget(SafeDis, 3, 2);

	MainLayout->addWidget(Label_ToolType, 4, 1);
	MainLayout->addWidget(ToolType, 4, 2);
	MainLayout->addWidget(Label_ToolDiam, 5, 1);
	MainLayout->addWidget(ToolDiam, 5, 2);
	MainLayout->addWidget(Label_ToolLength, 6, 1);
	MainLayout->addWidget(ToolLength, 6, 2);
	MainLayout->addWidget(Label_ToolTaperAngle, 7, 1);
	MainLayout->addWidget(ToolTaperAngle, 7, 2);
	MainLayout->addWidget(Label_ToolTipAngle, 8, 1);
	MainLayout->addWidget(ToolTipAngle, 8, 2);
	MainLayout->addWidget(Label_ProcessingOriginX, 9, 1);
	MainLayout->addWidget(ProcessingOriginX, 9, 2);
	MainLayout->addWidget(Label_ProcessingOriginY, 10, 1);
	MainLayout->addWidget(ProcessingOriginY, 10, 2);
	MainLayout->addWidget(Label_ProcessingOriginZ, 11, 1);
	MainLayout->addWidget(ProcessingOriginZ, 11, 2);

	SetDefaultVal();

	connect(SurfGenerate, SIGNAL(clicked()), this, SLOT(onCreateNURBS()));
	connect(InspPntsPlan, SIGNAL(clicked()), this, SLOT(onMPtDist()));
	connect(SurfReconstruct, SIGNAL(clicked()), this, SLOT(onSurfaceRecons()));
	connect(MachiningPathPlan, SIGNAL(clicked()), this, SLOT(onToolPath()));
	connect(SelSurfBtn, SIGNAL(clicked()), this, SLOT(onSelSurf()));
}

FToolPathDlg::~FToolPathDlg(void)
{
}


/**********************************************
*函数名：onCreateNURBS()
*函数介绍：创建NURBS曲面
*输入参数：
*输出参数：
*返回值：
*调用函数：
**********************************************/
void FToolPathDlg::onCreateNURBS()
{
	gWindow->GetMainDoc();
	
	FMainDoc* pDoc = gWindow->GetMainDoc();
	CreateNURBS(pDoc);

	pOCCT_Surface = NurbsToOcctSurf(*pSurf->OnNurbsSurf());

	gWindow->UpdateModelView();
}

void FToolPathDlg::CreateNURBS(FMainDoc* pDoc)
{
	const int bIsRational = true;
	const int dim = 4;
	const int u_degree = 4;
	const int v_degree = 4;
	const int u_cv_count = 5;
	const int v_cv_count = 5;
	F3dPoint CV[u_cv_count][v_cv_count];
	pSurf = new FSurface(dim, bIsRational, u_degree, v_degree, u_cv_count, v_cv_count);
	int i, j;
	//Method1: Create a nurbs surface by setting CVs
	CV[0][0] = F3dPoint(-40, -40, 25); CV[0][1] = F3dPoint(-40, -20, 3); CV[0][2] = F3dPoint(-40, 0, -2); CV[0][3] = F3dPoint(-40, 20, 1); CV[0][4] = F3dPoint(-40, 40, 1);
	CV[1][0] = F3dPoint(-20, -40, 5);  CV[1][1] = F3dPoint(-20, -20, 20); CV[1][2] = F3dPoint(-20, 0, -16); CV[1][3] = F3dPoint(-20, 20, 1); CV[1][4] = F3dPoint(-20, 40, 6);
	CV[2][0] = F3dPoint(0, -40, 5);  CV[2][1] = F3dPoint(0, -20, 30); CV[2][2] = F3dPoint(0, 0, 0); CV[2][3] = F3dPoint(0, 20, 9); CV[2][4] = F3dPoint(0, 40, -3);
	CV[3][0] = F3dPoint(20, -40, 15);  CV[3][1] = F3dPoint(20, -20, 7); CV[3][2] = F3dPoint(20, 0, 12); CV[3][3] = F3dPoint(20, 20, -10); CV[3][4] = F3dPoint(20, 40, -15);
	CV[4][0] = F3dPoint(40, -40, 9);  CV[4][1] = F3dPoint(40, -20, -5); CV[4][2] = F3dPoint(40, 0, -4); CV[4][3] = F3dPoint(40, 20, -5); CV[4][4] = F3dPoint(40, 40, -20);

	/*CV[0][0] = F3dPoint(-5, -5, 3.125); CV[0][1] = F3dPoint(-5, -2.5, 0.375); CV[0][2] = F3dPoint(-5, 0, -0.25); CV[0][3] = F3dPoint(-5, 2.5, 0.125); CV[0][4] = F3dPoint(-5, 5, 0.125);
	CV[1][0] = F3dPoint(-2.5, -5, 0.625);  CV[1][1] = F3dPoint(-2.5, -2.5, 2.5); CV[1][2] = F3dPoint(-2.5, 0, -2); CV[1][3] = F3dPoint(-2.5, 2.5, 0.125); CV[1][4] = F3dPoint(-2.5, 5, 0.75);
	CV[2][0] = F3dPoint(0, -5, 0.625);  CV[2][1] = F3dPoint(0, -2.5, 3.75); CV[2][2] = F3dPoint(0, 0, 0); CV[2][3] = F3dPoint(0, 2.5, 1.125); CV[2][4] = F3dPoint(0, 5, -0.375);
	CV[3][0] = F3dPoint(2.5, -5, 1.875);  CV[3][1] = F3dPoint(2.5, -2.5, 0.875); CV[3][2] = F3dPoint(2.5, 0, 1.5); CV[3][3] = F3dPoint(2.5, 2.5, -1.25); CV[3][4] = F3dPoint(2.5, 5, -1.875);
	CV[4][0] = F3dPoint(5, -5, 1.125);  CV[4][1] = F3dPoint(5, -2.5, -0.625); CV[4][2] = F3dPoint(5, 0, -0.5); CV[4][3] = F3dPoint(5, 2.5, -0.625); CV[4][4] = F3dPoint(5, 5, -2.5);*/

	//CV[0][0] = F3dPoint(-5, -5, 3.125); CV[0][1] = F3dPoint(-5, -2.5, 0.375); CV[0][2] = F3dPoint(-5, 0, -0.25); CV[0][3] = F3dPoint(-5, 2.5, 0.125); CV[0][4] = F3dPoint(-5, 5, 0.125);
	//CV[1][0] = F3dPoint(-2.5, -5, 0.625);  CV[1][1] = F3dPoint(-2.5, -2.5, 2.5); CV[1][2] = F3dPoint(-2.5, 0, -4); CV[1][3] = F3dPoint(-2.5, 2.5, 0.125); CV[1][4] = F3dPoint(-2.5, 5, 0.75);
	//CV[2][0] = F3dPoint(0, -5, 0.625);  CV[2][1] = F3dPoint(0, -2.5, 5); CV[2][2] = F3dPoint(0, 0, 0); CV[2][3] = F3dPoint(0, 2.5, 1.125); CV[2][4] = F3dPoint(0, 5, -0.375);
	//CV[3][0] = F3dPoint(2.5, -5, 1.875);  CV[3][1] = F3dPoint(2.5, -2.5, 0.875); CV[3][2] = F3dPoint(2.5, 0, 1.5); CV[3][3] = F3dPoint(2.5, 2.5, -1.25); CV[3][4] = F3dPoint(2.5, 5, -1.875);
	//CV[4][0] = F3dPoint(5, -5, 1.125);  CV[4][1] = F3dPoint(5, -2.5, -0.625); CV[4][2] = F3dPoint(5, 0, -0.5); CV[4][3] = F3dPoint(5, 2.5, -0.625); CV[4][4] = F3dPoint(5, 5, -2.5);
	//表格测试复杂曲面

	//CV[0][0] = F3dPoint(-5, -5, 5); CV[0][1] = F3dPoint(-5, -2.5, 3); CV[0][2] = F3dPoint(-5, 0, 0); CV[0][3] = F3dPoint(-5, 2.5, -1); CV[0][4] = F3dPoint(-5, 5, -2);
	//CV[1][0] = F3dPoint(-2.5, -5, 2);  CV[1][1] = F3dPoint(-2.5, -2.5, 1); CV[1][2] = F3dPoint(-2.5, 0, 0); CV[1][3] = F3dPoint(-2.5, 2.5, -1); CV[1][4] = F3dPoint(-2.5, 5, -2);
	//CV[2][0] = F3dPoint(0, -5, 0);  CV[2][1] = F3dPoint(0, -2.5, 0); CV[2][2] = F3dPoint(0, 0, 0); CV[2][3] = F3dPoint(0, 2.5, -1); CV[2][4] = F3dPoint(0, 5, -2);
	//CV[3][0] = F3dPoint(2.5, -5, -1);  CV[3][1] = F3dPoint(2.5, -2.5, -2); CV[3][2] = F3dPoint(2.5, 0, -3); CV[3][3] = F3dPoint(2.5, 2.5, -3); CV[3][4] = F3dPoint(2.5, 5, -3.5);
	//CV[4][0] = F3dPoint(5, -5, -2);  CV[4][1] = F3dPoint(5, -2.5, -3); CV[4][2] = F3dPoint(5, 0, -4); CV[4][3] = F3dPoint(5, 2.5, -4); CV[4][4] = F3dPoint(5, 5, -5);
	//
	//表格测试曲面

	//CV[0][0] = F3dPoint(-10, -15, 0); CV[0][1] = F3dPoint(-10, 0, 0); CV[0][2] = F3dPoint(-10, 15, 0);
	//CV[1][0] = F3dPoint(0, -15, 0);  CV[1][1] = F3dPoint(0, 0, 0); CV[1][2] = F3dPoint(0, 15, 0);
	//CV[2][0] = F3dPoint(10, -15, 0);  CV[2][1] = F3dPoint(10, 0, 0); CV[2][2] = F3dPoint(10, 15, 0);

	for (i = 0; i < pSurf->OnNurbsSurf()->CVCount(0); i++) {
		for (j = 0; j < pSurf->OnNurbsSurf()->CVCount(1); j++) {
			pSurf->OnNurbsSurf()->SetCV(i, j, CV[i][j]);
		}
	}

	for (int i = 0;i < u_degree;i++)
		m_U.push_back(0);
	for (int i = u_degree;i < 2 * u_degree;i++)
		m_U.push_back(1);
	for (int i = u_degree;i < u_cv_count;i++)
		m_U.insert(m_U.begin() + i, (double)(((double)i - (double)u_degree + 1) / ((double)u_cv_count - (double)u_degree + 1)));

	for (int i = 0;i < v_degree;i++)
		m_V.push_back(0);
	for (int i = v_degree;i < 2 * v_degree;i++)
		m_V.push_back(1);
	for (int i = v_degree;i < v_cv_count;i++)
		m_V.insert(m_V.begin() + i, (double)(((double)i - (double)v_degree + 1) / ((double)v_cv_count - (double)v_degree + 1)));


	for (int j = 0;j < m_U.size() - 2;j++)
		pSurf->OnNurbsSurf()->SetKnot(0, j, m_U[j + 1]);

	for (int j = 0;j < m_V.size() - 2;j++)
		pSurf->OnNurbsSurf()->SetKnot(1, j, m_V[j + 1]);

	//pSurf->OnNurbsSurf()->MakeClampedUniformKnotVector(0);
	//pSurf->OnNurbsSurf()->MakeClampedUniformKnotVector(1);

	//m_u = pSurf->KnotCount(0);
	//m_v = pSurf->KnotCount(1);

	//pSurf->OnNurbsSurf()->MakePeriodicUniformKnotVector(0);
	//pSurf->OnNurbsSurf()->MakePeriodicUniformKnotVector(1);

	pSurf->Tessellator()->SetTesPrecision(1);///////////////////////////////////////////////////曲面精度
	pSurf->Update();
	nb_surface = pSurf->OnNurbsSurf();
	pDoc->AddDispEnt(pSurf);

	//清空向量，避免第二次点击创建曲面时的错误
	vector<double>().swap(m_U);
	vector<double>().swap(m_V);
}

/**********************************************
*函数名：normal_vector
*函数介绍：单位法向量计算
*输入参数：
*输出参数：
*返回值：
*调用函数：
**********************************************/
F3dVector FToolPathDlg::normal_vector(F3dVector du, F3dVector dv)
{

	F3dVector Nvector;
	double temp;

	Nvector.x = du.y*dv.z - du.z*dv.y;
	Nvector.y = du.z*dv.x - du.x*dv.z;
	Nvector.z = du.x*dv.y - du.y*dv.x;

	temp = sqrt(Nvector.x*Nvector.x + Nvector.y*Nvector.y + Nvector.z*Nvector.z);

	Nvector.x = Nvector.x / temp;
	Nvector.y = Nvector.y / temp;
	Nvector.z = Nvector.z / temp;

	return Nvector;

}

/**********************************************
*函数名：onMPtDist
*函数介绍：测点分布槽函数
*输入参数：
*输出参数：
*返回值：
*调用函数：
**********************************************/
void FToolPathDlg::onMPtDist()
{
	//MPtDistTest();//调用测点分布函数
	FBSampDis();
}

/**********************************************
*函数名：MPtDistTest
*函数介绍：测点分布方法测试函数
*输入参数：
*输出参数：
*返回值：
*调用函数：
**********************************************/
void FToolPathDlg::MPtDistTest()
{
	//vector<F3dPoint> PntKVec;  //对应点处的曲率存储
	//F3dPoint PntK;  //对应点处的曲率

	vector<double> PntKVecA, PntKVecG, PntKVecMax, PntKVecMin; //平均曲率、高斯曲率、最大主曲率、最小主曲率
	F3dVector du, dv, duu, duv, dvv;
	int m = 99, n = 99;  //在参数空间等距采样100*100
	int mAdapt, nAdapt;  //自适应布点的测点数量
	double m1 = 0, n1;
	double testMax, testMin;
	double KA, KG, KMax, KMin;  //平均曲率与高斯曲率

	for (int i = 0;i <= m;i++)
	{
		n1 = 0;
		for (int j = 0;j <= n;j++)
		{
			pSurf->OnNurbsSurf()->Ev2Der(m1, n1, Pnt1, du, dv, duu, duv, dvv, 0, 0);
			KA = PntK(1, m1, n1, du, dv, duu, duv, dvv);
			KG = PntK(2, m1, n1, du, dv, duu, duv, dvv);
			KMax = KA + sqrt(KA*KA - KG);
			KMin = KA - sqrt(KA*KA - KG);

			PntKVecA.push_back(KA);
			PntKVecG.push_back(KG);
			PntKVecMax.push_back(KMax);
			PntKVecMin.push_back(KMin);

			n1 = n1 + 1 / (double)(n+1);
		} 

		m1 = m1 + 1 / (double)(m+1);
	}
	

	testMax = testMin = PntKVecMax[0];
	for (int i = 0;i < PntKVecMax.size();i++)
	{
		if (testMax < PntKVecMax[i])
		{
			testMax = PntKVecMax[i];
		}
		if (testMin> PntKVecMax[i])
		{
			testMin = PntKVecMax[i];
		}

	}

	//首先测试最大最小主曲率的和作为评判依据



	//以上还需要规整,统一曲率取值范围

	//一条曲线分布
	double curvature;
	double uvPara;
	vector<double> curvatureList1;
	vector<double> uvParaList1;

	uvPara = 0;
	for (int i = 0;i <= m;i++)
	{
		pSurf->OnNurbsSurf()->Ev2Der(uvPara, 0, Pnt1, du, dv, duu, duv, dvv, 0, 0);
		curvature = sqrt(pow(du.y*duu.z - duu.y*du.z, 2) + pow(duu.x*du.z - du.x*duu.z, 2) + pow(du.x*duu.y - duu.x*du.y, 2)) / (pow(sqrt(du.x*du.x + du.y*du.y + du.z*du.z), 3));
		curvatureList1.push_back(curvature);
		uvParaList1.push_back(uvPara);
			   
		uvPara = uvPara + 1 / (double)(m + 1);

	}

	double sumcurvatureList;
	double meancurvatureList;

	mAdapt = 10;
	nAdapt = 10;

	sumcurvatureList = accumulate(begin(curvatureList1), end(curvatureList1), 0.0);
	//meancurvatureList = sumcurvatureList / curvatureList1.size();
	meancurvatureList = sumcurvatureList / (double)(mAdapt - 1);
	int count = 0;

	vector<double> uvList;

	for (int i = 1;i < mAdapt;i++)
	{
		double sum = 0;

		while (sum < meancurvatureList)
		{


			if (count < curvatureList1.size())
			{
				sum = sum + curvatureList1[count];
				count++;
			}
			else
			{
				uvList.push_back(uvParaList1[count]);
				break;
			}

		}

	}

	F3dVector Nvector;
	vector<F3dVector> NvectorList;
	F3dPoint MeaPntTemp;
	vector<F3dPoint> MeaPntList;
	   

	for (int i=0;i< uvList.size();i++)
	{
		pSurf->OnNurbsSurf()->Ev2Der(uvList[i], 0, MeaPntTemp, du, dv, duu, duv, dvv, 0, 0);
		MeaPntList.push_back(MeaPntTemp);
		NvectorList.push_back(normal_vector(du, dv));

	}




	////四条曲线测点分布

	//double curvature;
	//double uvPara;
	//vector<double> curvatureList1, curvatureList2, curvatureList3, curvatureList4;
	//vector<double> uvParaList1, uvParaList2, uvParaList3, uvParaList4;

	//uvPara = 0;
	//for (int i = 0;i <= m;i++)
	//{
	//	pSurf->OnNurbsSurf()->Ev2Der(0, uvPara, Pnt1, du, dv, duu, duv, dvv, 0, 0);
	//	curvature = sqrt(pow(du.y*duu.z - duu.y*du.z, 2) + pow(duu.x*du.z - du.x*duu.z, 2) + pow(du.x*duu.y - duu.x*du.y, 2)) / (pow(sqrt(du.x*du.x + du.y*du.y + du.z*du.z), 3));
	//	curvatureList1.push_back(curvature);
	//	//uvParaList1.push_back();

	//	pSurf->OnNurbsSurf()->Ev2Der(1, uvPara, Pnt1, du, dv, duu, duv, dvv, 0, 0);
	//	curvature = sqrt(pow(du.y*duu.z - duu.y*du.z, 2) + pow(duu.x*du.z - du.x*duu.z, 2) + pow(du.x*duu.y - duu.x*du.y, 2)) / (pow(sqrt(du.x*du.x + du.y*du.y + du.z*du.z), 3));
	//	curvatureList2.push_back(curvature);

	//	uvPara = uvPara + 1 / (double)(m + 1);
	//	
	//}

	//uvPara = 0;
	//for (int i = 0;i <= n;i++)
	//{
	//	pSurf->OnNurbsSurf()->Ev2Der(uvPara, 0, Pnt1, du, dv, duu, duv, dvv, 0, 0);
	//	curvature = sqrt(pow(du.y*duu.z - duu.y*du.z, 2) + pow(duu.x*du.z - du.x*duu.z, 2) + pow(du.x*duu.y - duu.x*du.y, 2)) / (pow(sqrt(du.x*du.x + du.y*du.y + du.z*du.z), 3));
	//	curvatureList3.push_back(curvature);

	//	pSurf->OnNurbsSurf()->Ev2Der(uvPara, 1, Pnt1, du, dv, duu, duv, dvv, 0, 0);
	//	curvature = sqrt(pow(du.y*duu.z - duu.y*du.z, 2) + pow(duu.x*du.z - du.x*duu.z, 2) + pow(du.x*duu.y - duu.x*du.y, 2)) / (pow(sqrt(du.x*du.x + du.y*du.y + du.z*du.z), 3));
	//	curvatureList4.push_back(curvature);

	//	uvPara = uvPara + 1 / (double)(n + 1);
	//}

	//double sumcurvatureList;
	//double meancurvatureList;
	//
	//mAdapt = 10;
	//nAdapt = 10;

	//sumcurvatureList = accumulate(begin(curvatureList1),end(curvatureList1),0.0);
	////meancurvatureList = sumcurvatureList / curvatureList1.size();
	//meancurvatureList = sumcurvatureList / (double)(mAdapt - 1);
	//int count = 0;
	//
	//vector<double> uvList;

	//for (int i=1;i< mAdapt;i++)
	//{
	//	double sum = 0;

	//	while (sum < meancurvatureList)
	//	{
	//		

	//		if (count < curvatureList1.size())
	//		{
	//			sum = sum + curvatureList1[count];
	//			count++;
	//		}
	//		else
	//		{
	//			//uvList.push_back();
	//			break;
	//		}

	//	}

	//	
	//	uvList.push_back(count);



	//}



	//显示测点
		

	m_Nvector.push_back(normal_vector(du, dv));

	for (int i = 0;i < m_CCPnt.size();i++)
	{

		MeasPt* my_measPnt = new MeasPt(m_CLPnt[i].x, m_CLPnt[i].y, m_CLPnt[i].z, m_Nvector[i].x, m_Nvector[i].y, m_Nvector[i].z);
		//MeasPt* my_measPnt = new MeasPt(m_CCPnt[i].x, m_CCPnt[i].y, m_CCPnt[i].z, m_Nvector[i].x, m_Nvector[i].y, m_Nvector[i].z);
		gWindow->GetMainDoc()->AddDispEnt(my_measPnt);
		m_Pntlist.push_back(my_measPnt);
	}




	cout<<"测试"<<endl;


	









}

/**********************************************
*函数名：PntK(int i)
*函数介绍：曲面某点处的平均曲率
*输入参数：i选择返回的曲率，i=1返回平均曲率，i=2返回高斯曲率
*输出参数：
*返回值：
*调用函数：
**********************************************/
double FToolPathDlg::PntK(int i,double u,double v, F3dVector du, F3dVector dv, F3dVector duu, F3dVector duv, F3dVector dvv)
{
	double KA,KG;   //KA为平均曲率，KG为高斯曲率
	F3dPoint pnttemp;  //暂时存储对应的点的坐标，可能不用
	F3dVector Nvector;  //存储偏导数
	double E, F, G, L, M, N;

	//pSurf->OnNurbsSurf()->Ev2Der(u, v, Pnt1, du, dv, duu, duv, dvv, 0, 0);
	E = du * du;	F = du * dv;	G = dv * dv;
	Nvector = normal_vector(du, dv);
	L = duu * Nvector;  M = duv * Nvector;  N = dvv * Nvector;

	if (i == 1)
	{
		KA = (E*N - 2 * F*M + G * L) / (E*G - F * F);
		return KA;
	}
	else if (i == 2)
	{
		KG = (L*N - M * M) / (E*G - F * F);
		return KG;
	}	
}

/**********************************************
*函数名：FBSampDis
*函数介绍：基于曲面特征的NURBS曲面测点分布
*输入参数：
*输出参数：
*返回值：
*调用函数：SLSampDis
**********************************************/
void FToolPathDlg::FBSampDis()
{
	//定义与初始化变量
	int m = 12, n = 10;   //u,v参数方向上测点的数量，后期可以改为界面输入
	int kNum = 100;    //参数平面网格细分的段数
	double minDistance; //暂存最小距离
	double minDistanceuv[2];  //最小距离时参数坐标

	F3dVector du, dv, duu, duv, dvv;
	F3dPoint Pnt1xyz;
	//double CurvatureU, CurvatureV;   //u，v方向上曲线的曲率

	vector<int> testtest(5, 4);
	testtest.resize(3);

	vector<vector<vector<double>>> FeaturePara;   //密集点处u,v方向上曲线的小段特征值存储
	FeaturePara.resize(kNum + 1);
	for (int i = 0; i < kNum + 1; i++)
	{
		FeaturePara[i].resize(kNum + 1);
	}
	for (int i = 0; i < kNum + 1; i++)
	{
		for (int j = 0; j < kNum + 1; j++)
		{
			FeaturePara[i][j].resize(2);
		}
	}

	vector<vector<vector<double>>> Curvatureuv;   //密集点处u,v方向上曲线在点上的曲率存储(kNum+1*kNum+1个，0是u方向,1是v方向)
	Curvatureuv.resize(kNum + 1);
	for (int i = 0; i < kNum + 1; i++)
	{
		Curvatureuv[i].resize(kNum + 1);
	}
	for (int i = 0; i < kNum + 1; i++)
	{
		for (int j = 0; j < kNum + 1; j++)
		{
			Curvatureuv[i][j].resize(2);
		}
	}

	vector<vector<double>> Curvatureu, arclthPPu, FeatureParau;  //密集点处u方向上曲线的小段曲率存储
	Curvatureu.resize(kNum);
	arclthPPu.resize(kNum);
	FeatureParau.resize(kNum);
	for (int i = 0; i < kNum; i++)
	{
		Curvatureu[i].resize(kNum + 1);
		arclthPPu[i].resize(kNum + 1);
		FeatureParau[i].resize(kNum + 1);
	}

	vector<vector<double>> Curvaturev, arclthPPv, FeatureParav;   //密集点处u方向上曲线的小段曲率存储
	Curvaturev.resize(kNum + 1);
	arclthPPv.resize(kNum + 1);
	FeatureParav.resize(kNum + 1);
	for (int i = 0; i < kNum + 1; i++)
	{
		Curvaturev[i].resize(kNum);
		arclthPPv[i].resize(kNum);
		FeatureParav[i].resize(kNum);
	}

	vector<vector<vector<double>>> SampPntus, SampPntvs;    //分别存储u,v方向的测点
	SampPntus.resize(m);
	for (int i = 0; i < m; i++)
	{
		SampPntus[i].resize(kNum + 1);
	}
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < kNum + 1; j++)
		{
			SampPntus[i][j].resize(2);
		}
	}
	SampPntvs.resize(kNum + 1);
	for (int i = 0; i < kNum+1; i++)
	{
		SampPntvs[i].resize(n);
	}
	for (int i = 0; i < kNum + 1; i++)
	{
		for (int j = 0; j < n; j++)
		{
			SampPntvs[i][j].resize(2);
		}
	}
	
	vector<vector<F3dPoint>> Pntsxyz;  //存储u,v两个参数方向上三维坐标点值，kNum+1维
	Pntsxyz.resize(kNum + 1);
	for (int i = 0; i < kNum + 1; i++)
	{
		Pntsxyz[i].resize(kNum + 1);
	}

	vector<vector<vector<double>>> arclthPP;  //存储u,v两个参数方向上的两点间的弧长，用直线近似
	arclthPP.resize(kNum);
	for (int i = 0; i < kNum; i++)
	{
		arclthPP[i].resize(kNum);
	}
	for (int i = 0; i < kNum; i++)
	{
		for (int j = 0; j < kNum; j++)
		{
			arclthPP[i][j].resize(2);
		}
	}

	double*** Pntuv;   //存储测点的u，v值
	Pntuv = new double** [m];
	for (int i = 0; i < m; i++)
		Pntuv[i] = new double* [n];
	for (int i = 0; i < m; i++)
		for (int j = 0;j < n;j++)
			Pntuv[i][j] = new double[2];

	double** Pntu, ** Pntv, ** uvDistance;   //分别存储垂直于参数u,v方向的多条曲线的相同点位的参数坐标,u,v曲线测点的距离
	Pntu = new double* [kNum + 1];
	Pntv = new double* [kNum + 1];
	uvDistance = new double* [kNum + 1];
	for (int i = 0; i < kNum + 1; i++)
	{
		Pntu[i] = new double[2];
		Pntv[i] = new double[2];
		uvDistance[i] = new double[kNum + 1];
	}

	vector<double> SampPnts;  //定义多条曲线上采样点的参数位置
	SampPnts.resize(kNum + 1);

	ofstream fout("testout.txt");

	//定义与初始化变量

	//细分网格上u,v方向各段特征参数计算
	//计算细密网格点的三维坐标值(用于计算近似弧长，kNum+1*kNum+1个点)
	for (int i = 0; i < kNum + 1; i++)
	{
		for (int j = 0; j < kNum + 1; j++)
		{
			pSurf->OnNurbsSurf()->Ev2Der((double)i * (1.0 / (double)kNum), (double)j * (1.0 / (double)kNum), Pnt1xyz, du, dv, duu, duv, dvv, 0, 0);
			Curvatureuv[i][j][0] = sqrt(pow(du.y * duu.z - duu.y * du.z, 2) + pow(duu.x * du.z - du.x * duu.z, 2) + pow(du.x * duu.y - duu.x * du.y, 2))
				/ (pow(sqrt(du.x * du.x + du.y * du.y + du.z * du.z), 3));
			Curvatureuv[i][j][1] = sqrt(pow(dv.y * dvv.z - dvv.y * dv.z, 2) + pow(dvv.x * dv.z - dv.x * dvv.z, 2) + pow(dv.x * dvv.y - dvv.x * dv.y, 2))
				/ (pow(sqrt(dv.x * dv.x + dv.y * dv.y + dv.z * dv.z), 3));
			Pntsxyz[i][j] = Pnt1xyz;
		}
	}	 
	 
	// u方向特征值计算(kNum+1条线上的kNum小段,每小段的特征值),u方向的kNum+1条曲线的测点分布

	//vector<double> tempSum1(kNum,0), tempSum2(kNum,0);
	double tempSumu1, tempSumu2;
	for (int j = 0; j < kNum + 1; j++)
	{
		tempSumu1 = 0;
		tempSumu2 = 0;
		//曲率、弧长计算
		for (int i = 0; i < kNum; i++)
		{			
			Curvatureu[i][j] = (1.0 / 2.0) * (Curvatureuv[i][j][0] + Curvatureuv[i + 1][j][0]);
			arclthPPu[i][j] = sqrt(pow(Pntsxyz[i][j].x - Pntsxyz[i + 1][j].x, 2) + pow(Pntsxyz[i][j].y - Pntsxyz[i + 1][j].y, 2)
				+ pow(Pntsxyz[i][j].z - Pntsxyz[i + 1][j].z, 2));
			tempSumu1 = tempSumu1 + Curvatureu[i][j];
			tempSumu2 = tempSumu2 + arclthPPu[i][j];
		}
		//曲率、弧长标准化，特征值计算(后期可以修改为权重可调节的)
		for (int i = 0; i < kNum; i++)
		{
			Curvatureu[i][j] = Curvatureu[i][j] / tempSumu1;
			arclthPPu[i][j] = arclthPPu[i][j] / tempSumu2;
			FeatureParau[i][j] = (1.0 / 2.0) * (Curvatureu[i][j] + arclthPPu[i][j]);
		}

		//计算第j条曲线的测点分布
		SLSampDis(j, m, kNum, FeatureParau, SampPntus,0);
	}

	// v方向特征值计算(kNum+1条线上的kNum小段),v方向的kNum+1条曲线的测点分布
	for (int i = 0; i < kNum + 1; i++)
	{
		tempSumu1 = 0;
		tempSumu2 = 0;		
		//曲率、弧长计算
		for (int j = 0; j < kNum; j++)
		{
			Curvaturev[i][j] = (1.0 / 2.0) * (Curvatureuv[i][j][1] + Curvatureuv[i][j + 1][1]);
			arclthPPv[i][j] = sqrt(pow(Pntsxyz[i][j].x - Pntsxyz[i][j + 1].x, 2) + pow(Pntsxyz[i][j].y - Pntsxyz[i][j + 1].y, 2)
				+ pow(Pntsxyz[i][j].z - Pntsxyz[i][j + 1].z, 2));
			tempSumu1 = tempSumu1 + Curvaturev[i][j];
			tempSumu2 = tempSumu2 + arclthPPv[i][j];
		}

		for (int j = 0; j < kNum; j++)
		{
			Curvaturev[i][j] = Curvaturev[i][j] / tempSumu1;
			arclthPPv[i][j] = arclthPPv[i][j] / tempSumu2;
			FeatureParav[i][j] = (1.0 / 2.0) * (Curvaturev[i][j] + arclthPPv[i][j]);
		}

		//计算第i条曲线的测点分布
		SLSampDis(i, n, kNum, FeatureParav, SampPntvs, 1);
	}

	//第i,j个采样点计算
	for (int i = 0;i < m;i++)
	{
		for (int j = 0;j < n;j++)
		{
			for (int k = 0;k < kNum + 1;k++)
			{
				for (int l = 0;l < kNum + 1;l++)
				{
					uvDistance[k][l] = sqrt(pow(SampPntus[i][k][0] - SampPntvs[l][j][0], 2) + pow(SampPntus[i][k][1] - SampPntvs[l][j][1], 2));
					fout << SampPntus[i][k][0] << " " << SampPntus[i][k][1] << endl;
				}
			}
			minDistance = uvDistance[0][0];
			minDistanceuv[0] = (1.0 / 2.0) * (SampPntus[i][0][0] + SampPntvs[0][j][0]);
			minDistanceuv[1] = (1.0 / 2.0) * (SampPntus[i][0][1] + SampPntvs[0][j][1]);

			for (int ku = 0;ku <= kNum;ku++)
			{
				for (int kv = 0;kv <= kNum;kv++)
				{
					if (minDistance > uvDistance[ku][kv])
					{
						minDistance = uvDistance[ku][kv];
						minDistanceuv[0] = (1.0 / 2.0) * (SampPntus[i][ku][0] + SampPntvs[kv][j][0]);
						minDistanceuv[1] = (1.0 / 2.0) * (SampPntus[i][ku][1] + SampPntvs[kv][j][1]);
					}

				}
			}
			Pntuv[i][j][0] = minDistanceuv[0];
			Pntuv[i][j][1] = minDistanceuv[1];

			//fout << Pntuv[i][j][0] << " " << Pntuv[i][j][1] << endl;
		}
	}
	
	//清除相关变量内存
	for (int i = 0; i < kNum + 1; i++)
	{
		delete[]Pntu[i];
		delete[]Pntv[i];
		delete[]uvDistance[i];
	}
	delete[]Pntu;
	delete[]Pntv;
	delete[]uvDistance;

	for (int i = 0; i < m; i++)
		for (int j = 0;j < n;j++)
			delete[]Pntuv[i][j];
	for (int i = 0; i < m; i++)
		delete[]Pntuv[i];
	delete[]Pntuv;

}

/**********************************************
*函数名：SLSampDis
*函数介绍：单个NURBS样条上采样点自适应分布，基于曲线特征的方法
*输入参数：i为第i条曲线，采样点规模SampSize，细分网格段数kNum，FeaturePara细密网格在u/v方向的特征值
* SampPnts输出曲线上第i个采样点参数,uorv=0是参数u方向的曲线
*输出参数：
*返回值：
*调用函数：
**********************************************/
void FToolPathDlg::SLSampDis(int ith, int SampSize, int kNum, vector<vector<double>> FeaturePara, vector<vector<vector<double>>>& SampPnts,bool uorv)
{
	
	double FeatureSum;
	if (uorv)
	{
		//SampPnts.resize(kNum + 1);
		//for (int i = 0;i < SampPnts.size();i++)
		//{
		//	SampPnts[i].resize(SampSize);
		//}
		//for (int i = 0;i < kNum + 1;i++)
		//{
		//	for (int j = 0;j < SampSize;j++)
		//	{
		//		SampPnts[i][j].resize(2);
		//	}
		//}

		//第一个点与最后一个点参数坐标计算
		SampPnts[ith][0][0] = (double)ith* (double)(1.0 / (double)kNum);
		SampPnts[ith][0][1] = 0;

		SampPnts[ith][SampSize - 1][0] = (double)ith* (double)(1.0 / (double)kNum);
		SampPnts[ith][SampSize - 1][1] = 1;

		//中间部位的采样点计算
		for (int j = 1;j < SampSize - 1;j++)
		{
			FeatureSum = 0;
			for (int k = 0;k < kNum;k++)
			{
				FeatureSum = FeatureSum + FeaturePara[ith][k];
				if (FeatureSum > ((double)j / (double)(SampSize - 1)))
				{
					SampPnts[ith][j][0] = (double)ith* (double)(1.0 / (double)kNum);
					SampPnts[ith][j][1] = (double)(1.0 / (double)kNum) * k;
					break;
				}
			}
		}
	}
	else
	{
		//SampPnts.resize(SampSize);
		//for (int i = 0;i < SampPnts.size();i++)
		//{
		//	SampPnts[i].resize(kNum+1);
		//}
		//for (int i = 0;i < SampPnts.size();i++)
		//{
		//	for (int j = 0;j < kNum + 1;j++)
		//	{
		//		SampPnts[i][j].resize(2);
		//	}
		//}

		//第一个点与最后一个点参数坐标计算
		SampPnts[0][ith][0] = 0;
		SampPnts[0][ith][1] = (double)ith * (double)(1.0 / (double)kNum);

		SampPnts[SampSize-1][ith][0] = 1;
		SampPnts[SampSize-1][ith][1] = (double)ith * (double)(1.0 / (double)kNum);

		//中间部位的采样点计算		
		for (int i = 1;i < SampSize-1;i++)
		{
			FeatureSum = 0;
			for (int k = 0;k < kNum;k++)
			{
				FeatureSum = FeatureSum + FeaturePara[k][ith];
				if (FeatureSum > ((double)i / (double)(SampSize - 1)))
				{
					SampPnts[i][ith][0] = (double)(1.0 / (double)kNum) * k;
					SampPnts[i][ith][1] = (double)ith * (double)(1.0 / (double)kNum);
					break;
				}
			}	
		}
	}

}

/**********************************************
*函数名：FBSampDis
*函数介绍：基于曲面特征的NURBS曲面测点分布
*输入参数：
*输出参数：
*返回值：
*调用函数：SLSampDis
**********************************************/
//void FToolPathDlg::FBSampDis()
//{
//	//定义与初始化变量
//	int m = 15, n = 10;   //u,v参数方向上测点的数量，后期可以改为界面输入
//	int kNum = 100;    //参数平面网格细分的段数
//	double minDistance; //暂存最小距离
//	double minDistanceuv[2];  //最小距离时参数坐标
//
//	F3dVector du, dv, duu, duv, dvv;
//	F3dPoint Pnt1xyz;   
//	double CurvatureU, CurvatureV;   //u，v方向上曲线的曲率
//
//	vector<vector<vector<double>>> FeaturePara;   //密集点处u,v方向上曲线的小段特征值存储
//	FeaturePara.resize(kNum+1);
//	for (int i = 0; i < kNum+1; i++)
//	{
//		FeaturePara[i].resize(kNum+1);
//	}
//	for (int i = 0; i < kNum+1; i++)
//	{
//		for (int j = 0; j < kNum+1; j++)
//		{
//			FeaturePara[i][j].resize(2);
//		}
//	}
//
//	vector<vector<vector<double>>> Curvatureuv;   //密集点处u,v方向上曲线的小段曲率存储
//	Curvatureuv.resize(kNum+1);
//	for (int i = 0; i < kNum+1; i++)
//	{
//		Curvatureuv[i].resize(kNum+1);
//	}
//	for (int i = 0; i < kNum+1; i++)
//	{
//		for (int j = 0; j < kNum+1; j++)
//		{
//			Curvatureuv[i][j].resize(2);
//		}
//	}
//
//	vector<vector<F3dPoint>> Pntsxyz;  //存储u,v两个参数方向上三维坐标点值，kNum+1维
//	Pntsxyz.resize(kNum+1);
//	for (int i = 0; i < kNum+1; i++)
//	{
//		Pntsxyz[i].resize(kNum+1);
//	}
//
//	vector<vector<vector<double>>> arclthPP;  //存储u,v两个参数方向上的两点间的弧长，用直线近似
//	arclthPP.resize(kNum);
//	for (int i = 0; i < kNum; i++)
//	{
//		arclthPP[i].resize(kNum);
//	}
//	for (int i = 0; i < kNum; i++)
//	{
//		for (int j = 0; j < kNum; j++)
//		{
//			arclthPP[i][j].resize(2);
//		}
//	}
//		
//	double*** Pntuv;   //存储测点的u，v值
//	Pntuv = new double** [m];
//	for (int i = 0; i < m; i++)
//		Pntuv[i] = new double *[n];
//	for (int i = 0; i < m; i++)
//		for (int j = 0;j < n;j++)
//			Pntuv[i][j] = new double[2];
//
//	double** Pntu, ** Pntv, ** uvDistance;   //分别存储垂直于参数u,v方向的多条曲线的相同点位的参数坐标,u,v曲线测点的距离
//	Pntu = new double* [kNum + 1];
//	Pntv = new double* [kNum + 1];
//	uvDistance = new double* [kNum + 1];
//	for (int i = 0; i < kNum + 1; i++)
//	{
//		Pntu[i] = new double[2];
//		Pntv[i] = new double[2];
//		uvDistance[i] = new double[kNum + 1];
//	}
//
//	vector<double> SampPnts;  //定义多条曲线上采样点的参数位置
//	SampPnts.resize(kNum + 1);
//
//	ofstream fout("testout.txt");
//	
//	//定义与初始化变量
//
//	//细分网格上u,v方向各段特征参数计算
//	//曲线密集分段曲率计算，
//	for (int i = 0;i < kNum+1;i++)
//	{
//		for (int j = 0;j < kNum+1;j++)
//		{
//			pSurf->OnNurbsSurf()->Ev2Der((double)i * (1.0 / (double)kNum), (double)j * (1.0 / (double)kNum), Pnt1xyz, du, dv, duu, duv, dvv, 0, 0);
//			CurvatureU = sqrt(pow(du.y * duu.z - duu.y * du.z, 2) + pow(duu.x * du.z - du.x * duu.z, 2) + pow(du.x * duu.y - duu.x * du.y, 2)) 
//				/ (pow(sqrt(du.x * du.x + du.y * du.y + du.z * du.z), 3));
//			CurvatureV = sqrt(pow(dv.y * dvv.z - dvv.y * dv.z, 2) + pow(dvv.x * dv.z - dv.x * dvv.z, 2) + pow(dv.x * dvv.y - dvv.x * dv.y, 2))
//				/ (pow(sqrt(dv.x * dv.x + dv.y * dv.y + dv.z * dv.z), 3));
//
//			Curvatureuv[i][j][0] = CurvatureU;
//			Curvatureuv[i][j][1] = CurvatureV;
//
//			Pntsxyz[i][j]=Pnt1xyz;
//		}
//	}
//
//	//u=1,v=1的边界处的点的三维坐标存储
//	//for (int i = 0;i < kNum;i++)
//	//{
//	//	pSurf->OnNurbsSurf()->Ev2Der((double)i * (1.0 / (double)kNum),1, Pnt1xyz, du, dv, duu, duv, dvv, 0, 0);
//	//	Pntsxyz[i][kNum] = Pnt1xyz;
//	//}
//
//	//for (int j = 0;j < kNum;j++)
//	//{
//	//	pSurf->OnNurbsSurf()->Ev2Der(1, (double)j * (1.0 / (double)kNum), Pnt1xyz, du, dv, duu, duv, dvv, 0, 0);
//	//	Pntsxyz[kNum][j] = Pnt1xyz;
//	//}
//
//	//pSurf->OnNurbsSurf()->Ev2Der(1, 1, Pntsxyz[kNum][kNum], du, dv, duu, duv, dvv, 0, 0);
//
//	//分段弧长计算(以两点间的直线距离近似)
//	double arclthu, arclthv;  //暂存u,v参数方向弧长值(以直线暂代)
//	for (int i = 0;i < kNum;i++)
//	{
//		for (int j = 0;j < kNum;j++)
//		{
//			arclthu = sqrt(pow(Pntsxyz[i][j].x - Pntsxyz[i + 1][j].x, 2) + pow(Pntsxyz[i][j].y - Pntsxyz[i + 1][j].y, 2)
//				+ pow(Pntsxyz[i][j].z - Pntsxyz[i + 1][j].z, 2));
//			arclthv = sqrt(pow(Pntsxyz[i][j].x - Pntsxyz[i][j + 1].x, 2) + pow(Pntsxyz[i][j].y - Pntsxyz[i][j + 1].y, 2)
//				+ pow(Pntsxyz[i][j].z - Pntsxyz[i][j + 1].z, 2));
//			arclthPP[i][j][0] = arclthu;
//			arclthPP[i][j][1] = arclthv;
//		}
//	}
//
//	//曲率、弧长标准化
//	vector<double> tempSum1, tempSum2;
//	tempSum1.resize(kNum);
//	tempSum2.resize(kNum);
//
//	for (int i = 0;i < kNum;i++)
//	{		
//		tempSum1[i] = 0;
//		tempSum2[i] = 0;
//		for (int j = 0;j < kNum;j++)
//		{
//			tempSum1[i] = tempSum1[i] + Curvatureuv[i][j][0];
//			tempSum2[i] = tempSum2[i] + arclthPP[i][j][0];
//		}
//		for (int j = 0;j < kNum;j++)
//		{
//			Curvatureuv[i][j][0] = Curvatureuv[i][j][0] / tempSum1[i];
//			arclthPP[i][j][0] = arclthPP[i][j][0] / tempSum2[i];
//		}
//	}
//
//	for (int j = 0;j < kNum;j++)
//	{
//		tempSum1[j] = 0;
//		tempSum2[j] = 0;
//		for (int i = 0;i < kNum;i++)
//		{
//			tempSum1[j] = tempSum1[j] + Curvatureuv[i][j][1];
//			tempSum2[j] = tempSum2[j] + arclthPP[i][j][1];
//		}
//		for (int i = 0;i < kNum;i++)
//		{
//			Curvatureuv[i][j][1] = Curvatureuv[i][j][1] / tempSum1[j];
//			arclthPP[i][j][1] = arclthPP[i][j][1] / tempSum2[j];
//		}
//	}
//
//	//特征参数计算(后期可以修改为权重可调节的)
//	for (int i = 0;i < kNum;i++)
//	{
//		for (int j = 0;j < kNum;j++)
//		{
//			FeaturePara[i][j][0] = (1.0 / 2.0) * (Curvatureuv[i][j][0] + arclthPP[i][j][0]);
//			FeaturePara[i][j][1] = (1.0 / 2.0) * (Curvatureuv[i][j][1] + arclthPP[i][j][1]);
//		}
//	}
//	
//	//第i,j个采样点计算
//	for (int i = 0;i < m;i++)
//	{
//		for (int j = 0;j < n;j++)
//		{
//
//			//垂直于u方向的曲线
//			for (int ku = 0;ku <= kNum;ku++)
//			{
//				Pntu[ku][0] = (double)ku*(1.0 / (double)kNum);
//				SLSampDis(j,m, kNum, FeaturePara, SampPnts,1);
//				Pntu[ku][1] = SampPnts[ku];
//			}
//
//			//垂直于v方向
//			for (int kv = 0;kv <= kNum;kv++)
//			{				
//				Pntv[kv][1] = (double)kv * (1.0 / (double)kNum);
//				SLSampDis(i,n, kNum, FeaturePara, SampPnts,0);
//				Pntv[kv][0] = SampPnts[kv];
//			}
//
//			//计算参数坐标距离，
//			for (int ku = 0;ku <= kNum;ku++)
//			{
//				for (int kv = 0;kv <= kNum;kv++)
//				{
//					uvDistance[ku][kv] = sqrt(pow(Pntu[ku][0] - Pntv[kv][0], 2) + pow(Pntu[ku][1] - Pntv[kv][1], 2));
//				}
//			}
//
//			//选择参数坐标最近的距离
//			minDistance = uvDistance[0][0];
//			minDistanceuv[0] = (1.0 / 2.0) * (Pntu[0][0] + Pntv[0][0]);
//			minDistanceuv[1] = (1.0 / 2.0) * (Pntu[0][1] + Pntv[0][1]);
//			for (int ku = 0;ku <= kNum;ku++)
//			{
//				for (int kv = 0;kv <= kNum;kv++)
//				{
//					if (minDistance > uvDistance[ku][kv])
//					{
//						minDistance = uvDistance[ku][kv];
//						minDistanceuv[0] = (1.0 / 2.0) * (Pntu[ku][0] + Pntv[kv][0]);
//						minDistanceuv[1] = (1.0 / 2.0) * (Pntu[ku][1] + Pntv[kv][1]);
//					}
//
//				}
//			}
//
//			Pntuv[i][j][0] = minDistanceuv[0];
//			Pntuv[i][j][1] = minDistanceuv[1];
//
//			fout << Pntuv[i][j][0] << " " << Pntuv[i][j][1] << endl;
//		}
//	}
//	
//	//清除相关变量内存
//	for (int i = 0; i < kNum + 1; i++)
//	{
//		delete[]Pntu[i];
//		delete[]Pntv[i];
//		delete[]uvDistance[i];
//	}
//	delete[]Pntu;
//	delete[]Pntv;
//	delete[]uvDistance;
//
//	for (int i = 0; i < m; i++)
//		for (int j = 0;j < n;j++)
//			delete[]Pntuv[i][j];
//	for (int i = 0; i < m; i++)
//		delete[]Pntuv[i];
//	delete[]Pntuv;
//
//}

/**********************************************
*函数名：NURBSdeform
*函数介绍：采用控制点移动进行NURBS曲面变形，从而进行重构的方法，采用控制点移动最少为约束
*输入参数：
*输出参数：
*返回值：
*调用函数：
**********************************************/
void FToolPathDlg::NURBSdeform()
{
	//提取曲面的相关信息
	int uCVCount = pSurf->OnNurbsSurf()->CVCount(0);
	int vCVCount = pSurf->OnNurbsSurf()->CVCount(1);

	int u_degree, v_degree;
	int KnotCountU, KnotCountV;


	u_degree = pSurf->OnNurbsSurf()->Order(0);  //返回曲面的阶数
	v_degree = pSurf->OnNurbsSurf()->Order(1);

	KnotCountU = pSurf->OnNurbsSurf()->KnotCount(0);
	KnotCountV = pSurf->OnNurbsSurf()->KnotCount(1);

	double SumPowRij = 0;

	vector<vector<F3dPoint>> CVDev;    //控制点偏离大小

	CVDev.resize(uCVCount);
	for (int k1 = 0;k1 < uCVCount;k1++)
	{
		CVDev[k1].resize(vCVCount);
	}

	vector<double> knotUarray, knotVarray;

	knotUarray.resize(KnotCountU);
	knotVarray.resize(KnotCountV);

	for (int i = 0;i < KnotCountU;i++)
	{
		knotUarray[i] = pSurf->OnNurbsSurf()->Knot(0, i);
	}
	for (int j = 0;j < KnotCountV;j++)
	{
		knotVarray[j] = pSurf->OnNurbsSurf()->Knot(1, j);
	}

	//生成测试曲面（误差曲面）利用原曲面进行对应点的法向偏置，设置采样点，首先用等参数法生成15*15个点。
	//dj=0.05cos(0.6(ui+vi)+4)

	//等参数map

	double n1 = 10, n2 = 10;  //设置uv两个方向的间隔

	vector<double> PntU, PntV;  //U,V方向的值序列
	vector<vector<F3dPoint>> PntUVSamp, PntUVSampTarget;  //采样对应点,增加误差后的点(修正目标点)
	vector<vector<ON_3dVector>> PntUVSampVec;

	PntU.resize(n1 + 1);
	PntV.resize(n2 + 1);


	for (int i = 0;i < n1 + 1;i++)
	{
		PntU[i] = i * (1 / n1);
	}

	for (int j = 0;j < n2 + 1;j++)
	{
		PntV[j] = j * (1 / n2);
	}

	PntUVSamp.resize(PntU.size());
	for (int i = 0;i < PntU.size(); i++)
	{
		PntUVSamp[i].resize(PntV.size());
	}

	PntUVSampTarget.resize(PntU.size());
	for (int i = 0;i < PntU.size(); i++)
	{
		PntUVSampTarget[i].resize(PntV.size());
	}

	PntUVSampVec.resize(PntU.size());
	for (int i = 0;i < PntU.size(); i++)
	{
		PntUVSampVec[i].resize(PntV.size());
	}

	//设置采样数，采样点
	for (int i = 0;i < PntU.size();i++)
	{
		for (int j = 0;j < PntV.size();j++)
		{
			pSurf->OnNurbsSurf()->EvNormal(PntU[i], PntV[j], PntUVSamp[i][j], PntUVSampVec[i][j]);
			PntUVSampTarget[i][j] = PntUVSamp[i][j] + 0.05*cos(0.6*(PntU[i] + PntV[j]) + 4)*PntUVSampVec[i][j];

		}
	}


	vector<vector<F3dPoint>> CVNew;  //新的控制点存储
	vector<vector<double>> Rij;

	CVNew.resize(uCVCount);
	for (int k1 = 0;k1 < uCVCount;k1++)
	{
		CVNew[k1].resize(vCVCount);
	}

	Rij.resize(uCVCount);
	for (int k1 = 0;k1 < uCVCount;k1++)
	{
		Rij[k1].resize(vCVCount);
	}

	//vector<double> SolveVector;  //待求解向量,

	int SolveVectorlenth = uCVCount * vCVCount + PntU.size()*PntV.size();  //待求方程左侧矩阵

	//SolveVector.resize(SolveVectorlenth);
	
	math_Matrix aMatrix(1, SolveVectorlenth, 1, SolveVectorlenth);   //Opencascade中关于线性方程组的求解
	vector<vector<double>> AMatrix;

	AMatrix.resize(SolveVectorlenth);
	for (int i = 0;i < SolveVectorlenth;i++)
	{
		AMatrix[i].resize(SolveVectorlenth);
	}


	aMatrix.Init(0.0);
	math_Vector b1(1, SolveVectorlenth);
	math_Vector b2(1, SolveVectorlenth);
	math_Vector b3(1, SolveVectorlenth);
	b1.Init(0.0);

	math_Vector SolveVector(1, SolveVectorlenth);
	SolveVector.Init(0.0);
			


	for (int k1 = 0;k1 < PntU.size();k1++)
	{
		for (int k2 = 0;k2 < PntV.size();k2++)
		{

			for (int k3 = 0; k3 < uCVCount;k3++)
			{
				for (int k4 = 0;k4 < vCVCount;k4++)
				{
					aMatrix(k1*PntV.size() + k2+1, k3*vCVCount + k4+1) = OneRijFun(PntU[k1], PntV[k2], u_degree, v_degree, k3, k4, knotUarray, knotVarray);  //加1是因为该矩阵是从1开始
					AMatrix[k1*PntV.size() + k2][k3*vCVCount + k4] = OneRijFun(PntU[k1], PntV[k2], u_degree, v_degree, k3, k4, knotUarray, knotVarray);
				}
			}

			b1(k1*PntV.size() + k2+1) = PntUVSampTarget[k1][k2].x - PntUVSamp[k1][k2].x;
			b2(k1*PntV.size() + k2+1) = PntUVSampTarget[k1][k2].y - PntUVSamp[k1][k2].y;
			b3(k1*PntV.size() + k2+1) = PntUVSampTarget[k1][k2].z - PntUVSamp[k1][k2].z;

		}
	}

	for (int k3 = 0; k3 < uCVCount;k3++)
	{
		for (int k4 = 0;k4 < vCVCount;k4++)
		{
			aMatrix(PntU.size()*PntV.size() + k3 * vCVCount + k4+1, k3 * vCVCount + k4+1) = 2;
			AMatrix[PntU.size()*PntV.size() + k3 * vCVCount + k4][k3 * vCVCount + k4] = 2;

			for (int k1 = 0;k1 < PntU.size();k1++)
			{
				for (int k2 = 0;k2 < PntV.size();k2++)
				{
					int test1 = PntU.size()*PntV.size() + k3 * vCVCount + k4 + 1;
					int test2 = PntU.size()*PntV.size() + k1 * PntV.size() + k2 + 1;
					
					aMatrix(PntU.size()*PntV.size() + k3 * vCVCount + k4+1, uCVCount*uCVCount + k1 * PntV.size() + k2+1) = -OneRijFun(PntU[k1], PntV[k2], u_degree, v_degree, k3, k4, knotUarray, knotVarray);
					AMatrix[PntU.size()*PntV.size() + k3 * vCVCount + k4][uCVCount*uCVCount + k1 * PntV.size() + k2] = -OneRijFun(PntU[k1], PntV[k2], u_degree, v_degree, k3, k4, knotUarray, knotVarray);
				}
			}



		}
	}

	math_Gauss  sol(aMatrix);

	if (sol.IsDone())
	{
		sol.Solve(b2, SolveVector);
	}
	else {
		try {
			sol.Solve(b2, SolveVector);
		}
		catch (Standard_Failure)
		{
			Handle(Standard_Failure) error = Standard_Failure::Caught();
			cout << error << endl;
		}
	}

	
	   	  
	cout << "算法可以的" << endl;

}

/**********************************************
*函数名：NURBSPntdeform
*函数介绍：单个点变形
*输入参数：
*输出参数：
*返回值：
*调用函数：
**********************************************/
void FToolPathDlg::NURBSPntdeform()
{

	
	
	//提取曲面的相关信息
	int uCVCount = pSurf->OnNurbsSurf()->CVCount(0);
	int vCVCount = pSurf->OnNurbsSurf()->CVCount(1);
	
	int u_degree, v_degree;
	int KnotCountU, KnotCountV;


	u_degree = pSurf->OnNurbsSurf()->Order(0);  //返回曲面的阶数
	v_degree = pSurf->OnNurbsSurf()->Order(1);
	   
	KnotCountU = pSurf->OnNurbsSurf()->KnotCount(0);
	KnotCountV = pSurf->OnNurbsSurf()->KnotCount(1);
		   
	double SumPowRij = 0;
	
	vector<vector<F3dPoint>> CVDev;
	
	CVDev.resize(uCVCount);
	for (int k1 = 0;k1 < uCVCount;k1++)
	{
		CVDev[k1].resize(vCVCount);
	}

	vector<double> knotUarray, knotVarray;

	knotUarray.resize(KnotCountU);
	knotVarray.resize(KnotCountV);

	for (int i = 0;i < KnotCountU;i++)
	{
		knotUarray[i] = pSurf->OnNurbsSurf()->Knot(0, i);
	}
	for (int j = 0;j < KnotCountV;j++)
	{
		knotVarray[j] = pSurf->OnNurbsSurf()->Knot(1, j);
	}
	
	//生成测试曲面（误差曲面）利用原曲面进行对应点的法向偏置，设置采样点，首先用等参数法生成15*15个点。
	//dj=0.05cos(0.6(ui+vi)+4)

	//等参数map

	double n1 = 14, n2 = 14;  //设置uv两个方向的间隔

	vector<double> PntU,PntV;  //U,V方向的值序列
	vector<vector<F3dPoint>> PntUVSamp, PntUVSampTarget;  //采样对应点,增加误差后的点(修正目标点)
	vector<vector<ON_3dVector>> PntUVSampVec;
	
	PntU.resize(n1 + 1);
	PntV.resize(n2 + 1);


	for (int i = 0;i < n1 + 1;i++)
	{
		PntU[i] = i * (1 / n1);
	}

	for (int j = 0;j < n1 + 1;j++)
	{
		PntV[j] = j * (1 / n2);
	}

	PntUVSamp.resize(PntU.size());
	for (int i = 0;i < PntU.size(); i++)
	{
		PntUVSamp[i].resize(PntV.size());
	}

	PntUVSampTarget.resize(PntU.size());
	for (int i = 0;i < PntU.size(); i++)
	{
		PntUVSampTarget[i].resize(PntV.size());
	}
	
	PntUVSampVec.resize(PntU.size());
	for (int i = 0;i < PntU.size(); i++)
	{
		PntUVSampVec[i].resize(PntV.size());
	}

	double errAllow = 0.01;    //设置拟合误差允许范围
	   
	for (int i = 0;i < PntU.size();i++)
	{
		for (int j = 0;j < PntV.size();j++)
		{
			pSurf->OnNurbsSurf()->EvNormal(PntU[i], PntV[j], PntUVSamp[i][j], PntUVSampVec[i][j]);
			PntUVSampTarget[i][j] = PntUVSamp[i][j] + 0.05*cos(0.6*(PntU[i] + PntV[j]) + 4)*PntUVSampVec[i][j];

		}
	}


	vector<vector<F3dPoint>> CVNew;  //新的控制点存储
	vector<vector<double>> Rij;

	CVNew.resize(uCVCount);
	for (int k1 = 0;k1 < uCVCount;k1++)
	{
		CVNew[k1].resize(vCVCount);
	}

	Rij.resize(uCVCount);
	for (int k1 = 0;k1 < uCVCount;k1++)
	{
		Rij[k1].resize(vCVCount);
	}
	
	pSurf1 = pSurf;    //初始化新曲面


	for (int i = 0;i < PntU.size();i++)
	{
		for (int j = 0;j < PntV.size();j++)
		{
			pSurf1->OnNurbsSurf()->EvNormal(PntU[i], PntV[j], PntUVSamp[i][j], PntUVSampVec[i][j]);
			if (PntUVSampTarget[i][j].DistanceTo(PntUVSamp[i][j]) > errAllow)
			{
				for (int k1 = 0; k1 < uCVCount;k1++)
				{
					for (int k2 = 0;k2 < vCVCount;k2++)
					{
						Rij[k1][k2] = OneRijFun(PntU[k1], PntV[k2], u_degree, v_degree, k1, k2, knotUarray, knotVarray); ////!!!!有问题
						SumPowRij = SumPowRij + pow(Rij[k1][k2], 2);
					}
				}

				for (int k1 = 0; k1 < uCVCount;k1++)
				{
					for (int k2 = 0;k2 < vCVCount;k2++)
					{
						CVDev[k1][k2] = (Rij[k1][k2] / SumPowRij)*(PntUVSampTarget[i][j] - PntUVSamp[i][j]);
						pSurf1->OnNurbsSurf()->GetCV(i, j, CVNew[i][j]);
						CVNew[i][j] = CVNew[i][j] + CVDev[i][j];
						pSurf1->OnNurbsSurf()->SetCV(i, j, CVNew[i][j]);
					}
				}
				
				//pSurf1 = new FSurface(dim, IsRational, u_degree, v_degree, uCVCount, vCVCount);

				i = 0, j = 0;
				
			}
			else {
			
				

			}
			
		}
	}



	//误差校核

	double test = 0,test1;
	for (int i = 0;i < PntU.size();i++)
	{
		for (int j = 0;j < PntV.size();j++)
		{
			
			test1 = PntUVSampTarget[i][j].DistanceTo(PntUVSamp[i][j]);
			if (test < test1)
			{
				test = test1;
			}

		}
	}

	cout<<"算法可以的"<<endl;

}


void LDL(int n, double *a, double *b)//LDL法，参数依次：阶数 系数矩阵a 常数矩阵b 
{
	double *U = new double[n*n];
	double *x = new double[n];
	double *y = new double[n];
	double *z = new double[n];
	double *L = new double[n*n];
	double *D = new double[n];
	for (int i = 0; i < n; i++)//用LU先算出L U
	{
		for (int j = 0; j < n; j++)
		{
			*(U + i * n + j) = 0;//暂时全部赋值为0
			if (i == j)
			{
				*(L + i * n + j) = 1;//对角线赋值为1
			}
			else
			{
				*(L + i * n + j) = 0;//其他暂时赋值为0
			}
		}
	}

	for (int k = 0; k < n; k++)//计算u和l矩阵的值
	{
		for (int j = k; j < n; j++)
		{
			*(U + k * n + j) = *(a + k * n + j);//第一行
			for (int r = 0; r < k; r++)//接下来由L的前一列算u的下一行
			{
				*(U + k * n + j) = *(U + k * n + j) - (*(L + k * n + r)*(*(U + r * n + j)));
			}
		}
		for (int i = k + 1; i < n; i++)//计算L的列
		{
			*(L + i * n + k) = *(a + i * n + k);
			for (int r = 0; r < k; r++)
			{
				*(L + i * n + k) = *(L + i * n + k) - (*(L + i * n + r)*(*(U + r * n + k)));
			}
			*(L + i * n + k) = *(L + i * n + k) / (*(U + k * n + k));
		}
	}


	for (int i = 0; i < n; i++)//把D赋值
	{
		*(D + i) = *(U + i * n + i);
	}



	for (int i = 0; i < n; i++)//由Lz=b算z
	{
		*(z + i) = *(b + i);
		for (int j = 0; j < i; j++)
		{
			*(z + i) = *(z + i) - *(L + i * n + j)*(*(z + j));

		}
	}




	for (int i = 0; i < n; i++)//算y
	{
		*(y + i) = *(z + i) / (*(D + i));
	}

	double *temp = new double[n*n];
	for (int i = 0; i < n; i++)//这里实现对L的转置
	{
		for (int j = 0; j < n; j++)
		{
			*(temp + i * n + j) = *(L + j * n + i);
		}

	}
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			*(L + i * n + j) = *(temp + i * n + j);
		}

	}
	delete[]temp;//释放

	for (int i = n - 1; i >= 0; i--)//最后算x
	{
		*(x + i) = *(y + i);
		for (int j = i + 1; j < n; j++)
		{
			*(x + i) = *(x + i) - *(L + i * n + j)*(*(x + j));
		}
	}
	for (int i = 0; i < n; i++)
	{
		cout << "解为：\n";
		cout << *(x + i) << endl;
	}
	delete[]U;
	delete[]x;
	delete[]y;
	delete[]z;
	delete[]L;
	delete[]D;
}


/**********************************************
*函数名：OneRijFun
*函数介绍：计算有理基函数Rij的值,默认权重为1(该函数作为PSO算法的适应度函数)
*输入参数：i,j分别为u,v对应的参数区间下标，p,q为次数，m=r-p-1,n=s-q-1,r为节点向量U下标的最大值(m+1个节点),s对应V
*输出参数：
*返回值：Rij
*调用函数：OneBasisFun,BasisFuns
**********************************************/
//void FClosedloopManutest::OneBasisFun(int p,int m,vector <double> U,int i,double u,double Nip)
//void FClosedloopManutest::DerOneBasisFun(int p,int m,vector <double> U,int i,double u,int n,double Ders[])
//void FClosedloopManutest::OneRijFun(int i,int j,double u,double v,int p,int q,int m,int n,int r,int s,vector <double> U,vector <double> V,double Rij)
double FToolPathDlg::OneRijFun(double u,double v, int u_degree, int v_degree,int CVi,int CVj, vector<double> knotUarray, vector<double> knotVarray)
{
	double Nup = 0.0, Nvp = 0.0, temp, Rij;

	
	int r, s, m, n, i, j;

	u_degree = u_degree - 1;  //阶数变为次数
	v_degree = v_degree - 1;  //阶数变为次数

	double *Nu = new double[u_degree + 1];
	double *Nv = new double[v_degree + 1];
	double total = 0.0;

	//参数计算

	r = knotUarray.size() - 1;
	s = knotVarray.size() - 1;

	m = r - u_degree - 1;
	n = s - v_degree - 1;

	i = FindSpan(m, u_degree, u, knotUarray);
	j = FindSpan(n, v_degree, v, knotVarray);
	//参数计算

	//OneBasisFun(M_P,r,m_U,m_i,u,Nup);
	//OneBasisFun(M_Q,s,m_V,m_j,v,Nvp);

	BasisFuns(i, u, u_degree, knotUarray, Nu);
	BasisFuns(j, v, v_degree, knotVarray, Nv);

	if (CVi > i || CVi < i - u_degree)
	{
		Nup = 0.0;
	}
	else {
		Nup = Nu[CVi - i + u_degree];
	}

	if (CVj > j || CVj < j - v_degree)
	{
		Nvp = 0.0;
	}
	else {
		Nvp = Nv[CVj - j + v_degree];
	}

	for (int i = 0;i <= u_degree;i++)
		for (int j = 0;j <= v_degree;j++)
		{
			temp = Nu[i] * Nv[j];
			total += temp;
		}
	Rij = Nup * Nvp / total;

	delete[] Nu;
	delete[] Nv;
	return Rij;
}

/**********************************************
*函数名：BasisFuns
*函数介绍：计算所有非零B样条基函数的值
*输入参数：i为节点区间下标,u为参数值,p为基函数次数,U为节点向量
*输出参数：N为非零B样条基函数的值
*返回值：
*调用函数：
**********************************************/
void FToolPathDlg::BasisFuns(int i, double u, int p, vector <double> U, double N[])
{
	N[0] = 1.0;
	double saved, temp;
	double *left = new double[p + 1];
	double *right = new double[p + 1];

	for (int j = 1;j <= p;j++)
	{
		left[j] = u - U[i + 1 - j];
		right[j] = U[i + j] - u;
		saved = 0.0;
		for (int r = 0;r < j;r++)
		{
			temp = N[r] / (right[r + 1] + left[j - r]);
			//cout<<temp<<endl;
			N[r] = saved + right[r + 1] * temp;
			//cout<<"r的值"<<r<<"N的值"<<N[r]<<endl;
			saved = left[j - r] * temp;
			//cout<<saved<<endl;
		}
		N[j] = saved;
		//cout<<"j的值"<<N[0]<<N[1]<<N[2]<<endl;
	}
	delete[] left;
	delete[] right;
}

/**********************************************
*函数名：FindSpan
*函数介绍：确定参数u所在的节点区间的下标（采用二分法）
*输入参数：n=m-p-1,p为基函数次数,u为参数值,节点向量U={u0,u1,***,um}
*输出参数：
*返回值：节点区间的下标
*调用函数：
**********************************************/
int FToolPathDlg::FindSpan(int n, int p, double u, vector <double> U)
{
	/*if(u==U[n+1]) return(n);          /*特殊情况*/
	/*int low = p;int high = n+1;       /*进行二分搜索*/
	/*int mid = (low+high)/2;           //此处需要确认定义为整形int是否合适
	while(u<U[mid]||u>=U[mid+1])
	{
		if(u<U[mid])   high = mid;
		else           low = mid;
		mid = (low+high)/2;
	}
	return(mid);*/

	int i;
	for (i = p;i < n + 1;i++)
	{
		if (u >= U[i] && u < U[i + 1])
			break;
	}
	return i;
}




/**********************************************
*函数名：onToolPath
*函数介绍：刀具路径规划的总的槽函数
*输入参数：
*输出参数：
*返回值：
*调用函数：
**********************************************/
void FToolPathDlg::onToolPath()
{
	ToolParaPath();
	//ToolPath();
	//DrawToolPath();   //绘制刀具路径
	//clsExp();
	//FBSampDis();
}

/**********************************************
*函数名：onSurfaceRecons
*函数介绍：曲面重构槽函数
*输入参数：
*输出参数：
*返回值：
*调用函数：
**********************************************/
void FToolPathDlg::onSurfaceRecons()
{
	//NURBSdeform();
	//NURBSPntdeform();
	ToolMoveView();

}

void FToolPathDlg::onSelSurf()
{
	m_pCurTool = new ToolPathTool(this, gWindow->GetMainDoc());
	gWindow->GetMainDoc()->PushTool(m_pCurTool);
	((ToolPathTool*)m_pCurTool)->SetSelEntType(FT_FACE);

}

void FToolPathDlg::SetDefaultVal()
{
	ToolRadius->setText(QString::number(5));
	StepPres->setText(QString::number(0.001));
	ResidualHeight->setText(QString::number(0.1));
	SafeDis->setText(QString::number(5));

	ToolType->setText("MILL");
	ToolDiam->setText(QString::number(10));
	ToolLength->setText(QString::number(75));
	ToolTaperAngle->setText(QString::number(0));
	ToolTipAngle->setText(QString::number(0));
	ProcessingOriginX->setText(QString::number(5));
	ProcessingOriginY->setText(QString::number(5));
	ProcessingOriginZ->setText(QString::number(5));
}

/**********************************************
*函数名：ParaCspline_interpolation
*函数介绍：参数三次B样条插值（在u,v面上的插值，用于生成连续的参数刀具路径）
*输入参数：点的坐标值，参数值
*输出参数：参数对应的点的坐标值，以及一阶导数，二阶导数
*返回值：
*调用函数：
**********************************************/
void FToolPathDlg::Interpolationcurve_Calcu(double Pt, double *PtV, double *dPt, double *ddPt)
{
	int ith = 0;

	//寻找Pt所在的段
	while (ith < m_tPntV.size() - 1)
	{
		if ((Pt >= m_tPntV[ith]) && (Pt < m_tPntV[ith + 1]))
			break;

		ith++;
	}
	//分别计算对应参数的坐标值、一阶导数值、二阶导数值
	for (int i = 0;i < m_vecM.size();i++)
	{
		PtV[i] = m_vecM[i][ith] * pow(m_tPntV[ith + 1] - Pt, 3) / (6 * m_hV[ith]) + m_vecM[i][ith + 1] * pow(Pt - m_tPntV[ith], 3) / (6 * m_hV[ith]) +
			(m_Currentpathpara[i][ith] - m_vecM[i][ith] * m_hV[ith] * m_hV[ith] / 6)*(m_tPntV[ith + 1] - Pt) / m_hV[ith] +
			(m_Currentpathpara[i][ith + 1] - m_vecM[i][ith + 1] * m_hV[ith] * m_hV[ith] / 6)*(Pt - m_tPntV[ith]) / m_hV[ith];
	}

	for (int i = 0;i < m_vecM.size();i++)
	{
		dPt[i] = -m_vecM[i][ith] * pow(m_tPntV[ith + 1] - Pt, 2) / (2 * m_hV[ith]) + m_vecM[i][ith + 1] * pow(Pt - m_tPntV[ith], 2) / (2 * m_hV[ith]) +
			(m_Currentpathpara[i][ith + 1] - m_Currentpathpara[i][ith]) / m_hV[ith] - (m_vecM[i][ith + 1] - m_vecM[i][ith])*m_hV[ith] / 6;
	}

	for (int i = 0;i < m_vecM.size();i++)
	{
		ddPt[i] = m_vecM[i][ith] * (m_tPntV[ith + 1] - Pt) / m_hV[ith] + m_vecM[i][ith + 1] * (Pt - m_tPntV[ith]) / m_hV[ith];
	}

	cout << "ith的值" << ith << "m_tPntV[ith]的值" << m_tPntV[ith] << "m_tPntV[ith+1]的值" << m_tPntV[ith + 1] << endl;
}

/**********************************************
*函数名：ParaCspline_interpolation
*函数介绍：参数三次B样条插值（在u,v面上的插值，用于生成连续的参数刀具路径）
*输入参数：点的坐标值，参数值
*输出参数：参数对应的点的坐标值，以及一阶导数，二阶导数
*返回值：
*调用函数：
**********************************************/
void FToolPathDlg::ParaCspline_interpolation()
{
	double Pntdistance, PntdistanceTotal;
	vector<double> PntdistanceV;

	double tPnt;
	vector<double> uV, lambdaV;

	vector<vector<double>> dV, IntervariableV;
	vector<double> MdiagnalV, BetaV;

	MdiagnalV.resize(m_Currentpathpara[0].size());

	dV.resize(m_Currentpathpara.size());

	IntervariableV.resize(m_Currentpathpara.size());

	tPnt = 0;
	PntdistanceTotal = 0;
	Pntdistance = 0;
	//参数化方法选定t，采用累积弦长参数化方法

	for (int i = 0;i < m_Currentpathpara[0].size() - 1;i++)
	{
		for (int j = 0;j < m_Currentpathpara.size();j++)
		{
			Pntdistance += pow(m_Currentpathpara[j][i] - m_Currentpathpara[j][i + 1], 2);
		}
		Pntdistance = sqrt(Pntdistance);
		PntdistanceTotal += Pntdistance;
		PntdistanceV.push_back(Pntdistance);
		Pntdistance = 0;

	}

	//cout<<"MdiagnalV"<<MdiagnalV.size()<<endl;

	for (int i = 0;i < PntdistanceV.size();i++)
	{
		tPnt += PntdistanceV[i] / PntdistanceTotal;
		m_tPntV.push_back(tPnt);
	}

	m_tPntV.insert(m_tPntV.begin(), 0);
	//PntdistanceV.insert(PntdistanceV.begin(),0);

	//自然边界条件的插值,两端点的导数值为df1,df2设置为0;

	//求0-n-1个参数区间长度h的值
	for (int j = 0;j < m_Currentpathpara[0].size() - 1;j++)
	{
		m_hV.push_back(m_tPntV[j + 1] - m_tPntV[j]);
	}

	//求1-n个u的值
	//求0-n-1个lambda的值

	lambdaV.push_back(0);
	for (int j = 1;j < m_Currentpathpara[0].size() - 1;j++)
	{
		uV.push_back(m_hV[j - 1] / (m_hV[j - 1] + m_hV[j]));
		lambdaV.push_back(1 - uV.back());
	}
	//uV.erase(uV.begin());
	uV.push_back(0);

	//求方程组右侧的值dV
	for (int j = 0;j < m_Currentpathpara.size();j++)
	{
		dV[j].push_back(0);

	}

	for (int R = 0;R < m_Currentpathpara.size();R++)
	{

		for (int L = 1;L < m_Currentpathpara[0].size() - 1;L++)
		{
			dV[R].push_back(6 * ((m_Currentpathpara[R][L + 1] - m_Currentpathpara[0][L]) - (m_Currentpathpara[R][L] - m_Currentpathpara[0][L - 1])) / (m_hV[L - 1] + m_hV[L]));
		}

	}

	for (int j = 0;j < m_Currentpathpara.size();j++)
	{
		dV[j].push_back(0);

	}

	//给主向量的每个元素赋值2
	for (int j = 0;j < MdiagnalV.size();j++)
	{
		MdiagnalV[j] = 2;
	}

	//追赶法求m_vecM

	BetaV.push_back(lambdaV[0] / MdiagnalV[0]);

	for (int j = 1;j < m_Currentpathpara[0].size() - 1;j++)
	{
		BetaV.push_back(lambdaV[j - 1] / (MdiagnalV[j] - uV[j - 1] * BetaV.back()));
	}

	//解中间方程（追的过程）
	for (int j = 0;j < m_Currentpathpara.size();j++)
	{
		IntervariableV[j].push_back(dV[j][0] / MdiagnalV[0]);
	}

	for (int j = 0;j < m_Currentpathpara.size();j++)
	{
		for (int i = 1;i < m_Currentpathpara[0].size();i++)
		{
			IntervariableV[j].push_back((dV[j][i] - uV[i - 1] * IntervariableV[j].back()) / (MdiagnalV[i] - uV[i - 1] * BetaV[i - 1]));
		}
	}

	//解最终方程（赶的过程）
	for (int j = 0;j < m_Currentpathpara.size();j++)
	{
		m_vecM[j].push_back(IntervariableV[j].back());
	}

	for (int j = 0;j < m_Currentpathpara.size();j++)
	{
		for (int i = 0;i < m_Currentpathpara[0].size() - 1;i++)
		{
			m_vecM[j].insert(m_vecM[j].begin(), IntervariableV[j][m_Currentpathpara[0].size() - i - 2] - BetaV[m_Currentpathpara[0].size() - i - 2] * m_vecM[j][0]);
		}
	}


	cout << m_vecM[0][1] << endl;

}

/**********************************************
*函数名：ToolParaPath()
*函数介绍：参数线方法刀具路径规划
*输入参数：
*输出参数：
*返回值：
*调用函数：
**********************************************/
void FToolPathDlg::ToolParaPath()
{
	//定义与初始化变量

	
	int kNum = 100;  //网格密化为kNum段(后期可按照精度需要定制该值)
	vector<double> uSigPara;    //单条路径的参数值
	vector<double> uPara,vPara;   //u，v方向的参数值

	double Step_size, Parameter_increment;
	double E, F, G, L, M, N;
	gp_Vec Su, Sv, Suu, Suv, Svv, Nvector;
	gp_Pnt TempPnt;
	double base1Q, base2Q;
	double a, g;
	double dut = 1.0, dvt = 0.0;   //u,v分别对t的导数 本算法因为每条路径都在u方向上 因此初值不需要修改
	double curvature;
	double CurvatureRadius;
	double SafeDistance;
	double Der_v;   //v方向偏置的参数距离
	vector<double> tempDer_v;
	vector<vector<vector<double>>> uvPntPara;
	vector<vector<gp_Pnt>> CCPnt, CLPnt;  //分别为刀触点、刀位点、刀具方向向量
	vector<vector<gp_Vec>> CLPntVec;
	

	m_R = ToolRadius->text().toDouble();
	m_e = StepPres->text().toDouble();
	m_h = ResidualHeight->text().toDouble();
	SafeDistance = SafeDis->text().toDouble();   //超出曲面后的刀路延伸参考距离

	ofstream fout("testPath.txt");
	//定义与初始化变量
	//test();
	//u方向进行路径规划(后期可以修改为可选择起始位置)
	//确定v方向各条线的参数
	vPara.push_back(0);  //以v=0开始路径规划
	
	while (1)
	{
		//当参数v越过参数边界的界限，进行v=1
		if (vPara.back() > 1)
		{
			vPara.back() = 1;
			break;
		}
		

		tempDer_v.swap(vector<double>());
		for (int i = 0;i < kNum + 1;i++)
		{
			pOCCT_Surface->D2((double)i * (double)(1.0 / (double)kNum), vPara.back(), TempPnt, Su, Sv, Suu, Svv, Suv);

			//pSurf->OnNurbsSurf()->Ev2Der((double)i*(double)(1.0/(double)kNum), vPara.back(), TempPnt, du, dv, duu, duv, dvv, 0, 0);
			//curvature = sqrt(pow(du.y * duu.z - duu.y * du.z, 2) + pow(duu.x * du.z - du.x * duu.z, 2) + pow(du.x * duu.y - duu.x * du.y, 2)) 
			//	/ (pow(sqrt(du.x * du.x + du.y * du.y + du.z * du.z), 3));
			/*curvature = sqrt(pow(dv.y * dvv.z - dvv.y * dv.z, 2) + pow(dvv.x * dv.z - dv.x * dvv.z, 2) + pow(dv.x * dvv.y - dvv.x * dv.y, 2))
				/ (pow(sqrt(dv.x * dv.x + dv.y * dv.y + dv.z * dv.z), 3));*/

			E = Su * Su;	F = Su * Sv;	G = Sv * Sv;
			base1Q = E * dut * dut + 2 * F * dut * dvt + G * dvt * dvt;

			//du,dv
			Nvector.SetX(Su.Y() * Sv.Z() - Sv.Y() * Su.Z());
			Nvector.SetY(Su.Z() * Sv.X() - Sv.Z() * Su.X());
			Nvector.SetZ(Su.X() * Sv.Y() - Sv.X() * Su.Y());

			Nvector.Normalize();

			//Nvector = Nvector / (sqrt(pow(Nvector.x, 2) + pow(Nvector.y, 2) + pow(Nvector.z, 2)));

			L = Suu * Nvector;  M = Suv * Nvector;  N = Svv * Nvector;
			base2Q = L * dut * dut + 2 * M * dut * dvt + N * dvt * dvt;

			CurvatureRadius = abs(base1Q / base2Q);

			if (base2Q == 0)
			{
				a = sqrt(pow(m_R, 2) - pow(m_R - m_h, 2));
				g = 2 * a;
			}
			else if (base2Q < 0)
			{
				a = sqrt(pow(m_R, 2) - pow(m_R - m_h, 2));
				for (int i = 0;i < 5;i++)               //文献中说实践表明5次之内就会收敛
				{
					a = sqrt(pow(m_R, 2) - pow(sqrt(pow(CurvatureRadius + m_R, 2) - pow(a, 2)) - CurvatureRadius - m_h, 2));
				}
				g = 2 * a / (1 + m_R / CurvatureRadius);

			}
			else
			{
				a = sqrt(pow(m_R, 2) - pow(m_R - m_h, 2));
				for (int i = 0;i < 5;i++)               //文献中说实践表明5次之内就会收敛
				{
					a = sqrt(pow(m_R, 2) - pow(sqrt(pow(CurvatureRadius - m_R, 2) - pow(a, 2)) - CurvatureRadius + m_h, 2));
				}
				g = 2 * a / (1 - m_R / CurvatureRadius);
			}

			Der_v = g * (E * dut + F * dvt) / (sqrt(E * G - F * F) * sqrt(E * dut * dut + 2 * F * dut * dvt + G * dvt * dvt));
			tempDer_v.push_back(Der_v);

		}

		//求Der_v的最小值作为行距
		vector<double>::iterator minDer_v = min_element(tempDer_v.begin(), tempDer_v.end());
		vPara.push_back(*minDer_v + vPara.back());
	}

	uvPntPara.resize(vPara.size());

	//各条路径离散化
	for (int j = 0;j < vPara.size();j++)
	{
		uSigPara.swap(vector<double>());
		uSigPara.push_back(0);
		while (1)
		{
			if (uSigPara.back() > 1)
			{
				uSigPara.back() = 1;
				break;
			}			
			
			//pSurf->OnNurbsSurf()->Ev2Der(uSigPara.back(), vPara[j], TempPnt, du, dv, duu, duv, dvv, 0, 0);
			pOCCT_Surface->D2(uSigPara.back(), vPara[j], TempPnt, Su, Sv, Suu, Svv, Suv);

			curvature = sqrt(pow(Su.Y() * Suu.Z() - Suu.Y() * Su.Z(), 2) + pow(Suu.X() * Su.Z() - Su.X() * Suu.Z(), 2) + pow(Su.X() * Suu.Y() - Suu.X() * Su.Y(), 2))
				/ (pow(sqrt(Su.X() * Su.X() + Su.Y() * Su.Y() + Su.Z() * Su.Z()), 3));
			Step_size = 2 * sqrt(2 * m_e / curvature - m_e * m_e);
			Parameter_increment = Step_size / sqrt(Su.X()* Su.X()+ Su.Y()* Su.Y()+ Su.Z()* Su.Z())+ uSigPara.back();   //获得单个的点
			uSigPara.push_back(Parameter_increment);

		}
		
		uvPntPara[j].resize(uSigPara.size());
		for (int kk = 0;kk < uSigPara.size();kk++)
		{
			uvPntPara[j][kk].resize(2);
			uvPntPara[j][kk][0] = uSigPara[kk];
			uvPntPara[j][kk][1] = vPara[j];
		}
	}

	//刀位点变换与输出
	CCPnt.resize(uvPntPara.size());  //刀触点长度
	CLPnt.resize(uvPntPara.size());  //刀位点长度
	CLPntVec.resize(uvPntPara.size());

	for (int j = 0;j < vPara.size();j++)
	{
		CCPnt[j].resize(uvPntPara[j].size());
		CLPnt[j].resize(uvPntPara[j].size());
		CLPntVec[j].resize(uvPntPara[j].size());

		for (int i = 0;i < uvPntPara[j].size();i++)
		{
			fout << uvPntPara[j][i][0] << " " << uvPntPara[j][i][1] << endl;		

			pOCCT_Surface->D2(uvPntPara[j][i][0], uvPntPara[j][i][1], TempPnt, Su, Sv, Suu, Svv, Suv);

			CCPnt[j][i] = TempPnt;

			Nvector.SetX(Su.Y()* Sv.Z() - Sv.Y() * Su.Z());
			Nvector.SetY(Su.Z()* Sv.X() - Sv.Z() * Su.X());
			Nvector.SetZ(Su.X()* Sv.Y() - Sv.X() * Su.Y());
			Nvector.Normalize();

			CLPntVec[j][i].SetX(Nvector.X());
			CLPntVec[j][i].SetY(Nvector.Y());
			CLPntVec[j][i].SetZ(Nvector.Z());

			//法向偏置CC点一个球头刀半径得到CL点
			CLPnt[j][i].SetX(CCPnt[j][i].X() + m_R * Nvector.X());
			CLPnt[j][i].SetY(CCPnt[j][i].Y() + m_R * Nvector.Y());
			CLPnt[j][i].SetZ(CCPnt[j][i].Z() + m_R * Nvector.Z());

		}
	}

	//CL路径生成(为刀位点集添加空余行程刀位点)
	gp_Pnt AuxPnt;
	gp_Vec TempXYZ;

	for (int j = 0;j < CLPnt.size();j++)
	{
		//每条路径空行程刀位确定(路径首点)
		TempXYZ.SetX(CLPnt[j][0].X() - CLPnt[j][1].X());
		TempXYZ.SetY(CLPnt[j][0].Y() - CLPnt[j][1].Y());
		TempXYZ.SetZ(CLPnt[j][0].Z() - CLPnt[j][1].Z());
		TempXYZ.Normalize();

		AuxPnt.SetX(CLPnt[j][0].X() + SafeDistance * TempXYZ.X());
		AuxPnt.SetY(CLPnt[j][0].Y() + SafeDistance * TempXYZ.Y());
		AuxPnt.SetZ(CLPnt[j][0].Z() + SafeDistance * TempXYZ.Z());

		CLPnt[j].insert(CLPnt[j].begin(), AuxPnt);

		TempXYZ.SetX(CLPntVec[j][0].X());
		TempXYZ.SetY(CLPntVec[j][0].Y());
		TempXYZ.SetZ(CLPntVec[j][0].Z());

		CLPntVec[j].insert(CLPntVec[j].begin(), TempXYZ);  //路径空行程时与初始切入工件刀具方向相同

		//每条路径空行程刀位确定(路径末点)
		TempXYZ.SetX(CLPnt[j][CLPnt[j].size() - 1].X() - CLPnt[j][CLPnt[j].size() - 2].X());
		TempXYZ.SetY(CLPnt[j][CLPnt[j].size() - 1].Y() - CLPnt[j][CLPnt[j].size() - 2].Y());
		TempXYZ.SetZ(CLPnt[j][CLPnt[j].size() - 1].Z() - CLPnt[j][CLPnt[j].size() - 2].Z());
		TempXYZ.Normalize();

		AuxPnt.SetX(CLPnt[j][CLPnt[j].size() - 1].X() + SafeDistance * TempXYZ.X());
		AuxPnt.SetY(CLPnt[j][CLPnt[j].size() - 1].Y() + SafeDistance * TempXYZ.Y());
		AuxPnt.SetZ(CLPnt[j][CLPnt[j].size() - 1].Z() + SafeDistance * TempXYZ.Z());

		CLPnt[j].insert(CLPnt[j].end(), AuxPnt);

		TempXYZ.SetX(CLPntVec[j][CLPntVec[j].size() - 1].X());
		TempXYZ.SetY(CLPntVec[j][CLPntVec[j].size() - 1].Y());
		TempXYZ.SetZ(CLPntVec[j][CLPntVec[j].size() - 1].Z());

		CLPntVec[j].insert(CLPntVec[j].end(), TempXYZ);  //路径空行程时与切出工件刀具方向相同
	
	}

	//转为1维向量的刀具路径("之"字型路线进给)
	vector<gp_Pnt> CLPntLine;
	vector<gp_Vec> CLPntVecLine;

	for (int j = 0;j < CLPnt.size();j++)
	{
		if (j % 2 == 0) //第奇数条刀路
		{
			for (int i = 0;i < CLPnt[j].size();i++)
			{
				CLPntLine.push_back(CLPnt[j][i]);
				CLPntVecLine.push_back(CLPntVec[j][i]);
			}
		}
		else
		{
			vector<gp_Pnt> TempPnt(CLPnt[j].size());
			vector<gp_Vec> TempPntVec(CLPntVec[j].size());
			reverse_copy(CLPnt[j].begin(), CLPnt[j].end(), TempPnt.begin());
			reverse_copy(CLPntVec[j].begin(), CLPntVec[j].end(), TempPntVec.begin());
			for (int i = 0;i < CLPnt[j].size();i++)
			{
				CLPntLine.push_back(TempPnt[i]);
				CLPntVecLine.push_back(TempPntVec[i]);
			}

		}		

	}

	//绘制刀具路径
	FIpPoint ViewPnt;
	//vector<FIpPoint> CLPntview;
	PathShow* ToolPathView;

	//for (int i = 0;i < CLPntLine.size();i++)
	//{
	//	ViewPnt.x = CLPntLine[i].X();
	//	ViewPnt.y = CLPntLine[i].Y();
	//	ViewPnt.z = CLPntLine[i].Z();
	//	
	//	CLPntview.push_back(ViewPnt);
	//}

	//ToolPathView = new PathShow(ToQvec(CLPntview));
	ToolPathView = new PathShow(CLPntLine);
	//ToolPathView->SetPathNum(1);
	gWindow->GetMainDoc()->AddDispEnt(ToolPathView);

	//刀具移动仿真显示
	//ToolMoveView(CLPntLine, CLPntVecLine);
	m_CLPntLine.resize(CLPntLine.size());
	m_CLPntVecLine.resize(CLPntVecLine.size());
	for (int i = 0;i < CLPntLine.size();i++)
	{
		m_CLPntLine[i] = CLPntLine[i];
		m_CLPntVecLine[i] = CLPntVecLine[i];
	}

	//gWindow->GetMainDoc()->AddDispEnt(ToolView);

	//后期可以重新设计刀轴方向
	


}

void FToolPathDlg::ToolMoveView()
{
	//m_ToolSpec = new ToolSpec(0,0,0,0,0,1, m_R,15);

	//m_ToolSpec->Update();
	//gWindow->UpdateModelView();
	emit preper_save(this);

	gWindow->GetMainDoc()->AddDispEnt(m_ToolSpec);

	//ToolMove_thread* ToolMovethread = new ToolMove_thread;
	m_ToolMove_thread->start();

}

/**********************************************
*函数名：ToolMoveView(vector<gp_Pnt> CLPntLine, vector<gp_Vec> CLPntVecLine)
*函数介绍：刀具移动仿真
*输入参数：
*输出参数：
*返回值：
*调用函数：
**********************************************/
void FToolPathDlg::ToolMoveView(vector<gp_Pnt> CLPntLine, vector<gp_Vec> CLPntVecLine)
{
	
	vector<ToolSpec*> ToolViewList;
	for (int i = 0;i < CLPntLine.size();i++)
	{
		ToolSpec* ToolView = new ToolSpec(CLPntLine[i].X(), CLPntLine[i].Y(), CLPntLine[i].Z(),
			CLPntVecLine[i].X(), CLPntVecLine[i].Y(), CLPntVecLine[i].Z(), m_R, 15);
		ToolViewList.push_back(ToolView);
	}	
	
	for (int i = 0;i < ToolViewList.size();i++)
	{
		gWindow->GetMainDoc()->AddDispEnt(ToolViewList[i]);
		gWindow->UpdateModelView();
		Sleep(50);
		gWindow->GetMainDoc()->RemoveDispEnts(ToolViewList[i]);
		gWindow->UpdateModelView();
	}

}

/**********************************************
*函数名：NurbsToOcctSurf(const ON_NurbsSurface& theSurface)
*函数介绍：ON_NurbsSurface转化为OCCT曲面
*输入参数：const ON_NurbsSurface& theSurface
*输出参数：
*返回值：Handle(Geom_BSplineSurface)
*调用函数：
**********************************************/
Handle(Geom_BSplineSurface) FToolPathDlg::NurbsToOcctSurf(const ON_NurbsSurface& theSurface)
{
	TColgp_Array2OfPnt aPoles(1, theSurface.CVCount(0), 1, theSurface.CVCount(1));
	TColStd_Array2OfReal aWeights(1, theSurface.CVCount(0), 1, theSurface.CVCount(1));

	TColStd_Array1OfReal aUKnotSequence(1, theSurface.KnotCount(0) + 2);
	TColStd_Array1OfReal aVKnotSequence(1, theSurface.KnotCount(1) + 2);

	bool IsRational = theSurface.IsRational();
	bool IsUPeriodic = (theSurface.IsPeriodic(0)) ? true : false;
	bool IsVPeriodic = (theSurface.IsPeriodic(1)) ? true : false;

	// control point and its weight.
	for (int i = 0; i < theSurface.CVCount(0); ++i)
	{
		for (int j = 0; j < theSurface.CVCount(1); ++j)
		{
			if (IsRational)
			{
				ON_4dPoint aPole;

				theSurface.GetCV(i, j, aPole);

				aPoles.SetValue(i + 1, j + 1, gp_Pnt(aPole.x / aPole.w, aPole.y / aPole.w, aPole.z / aPole.w));
				aWeights.SetValue(i + 1, j + 1, aPole.w);
			}
			else
			{
				ON_3dPoint aPole;

				theSurface.GetCV(i, j, aPole);

				aPoles.SetValue(i + 1, j + 1, gp_Pnt(aPole.x, aPole.y, aPole.z));
			}

		}
	}

	// Knot vector and its multiplicity.
	for (int i = 0; i < theSurface.KnotCount(0); ++i)
	{
		aUKnotSequence.SetValue(i + 2, theSurface.Knot(0, i));
	}

	aUKnotSequence.SetValue(aUKnotSequence.Lower(), theSurface.Knot(0, 0));
	aUKnotSequence.SetValue(aUKnotSequence.Upper(), theSurface.Knot(0, theSurface.KnotCount(0) - 1));

	TColStd_Array1OfReal aUKnots(1, BSplCLib::KnotsLength(aUKnotSequence, IsUPeriodic));
	TColStd_Array1OfInteger aUMults(1, aUKnots.Upper());

	BSplCLib::Knots(aUKnotSequence, aUKnots, aUMults);

	for (int j = 0; j < theSurface.KnotCount(1); ++j)
	{
		aVKnotSequence.SetValue(j + 2, theSurface.Knot(1, j));
	}

	aVKnotSequence.SetValue(aVKnotSequence.Lower(), theSurface.Knot(1, 0));
	aVKnotSequence.SetValue(aVKnotSequence.Upper(), theSurface.Knot(1, theSurface.KnotCount(1) - 1));

	TColStd_Array1OfReal aVKnots(1, BSplCLib::KnotsLength(aVKnotSequence, IsVPeriodic));
	TColStd_Array1OfInteger aVMults(1, aVKnots.Upper());

	BSplCLib::Knots(aVKnotSequence, aVKnots, aVMults);

	Handle_Geom_BSplineSurface aBSplineSurface = new Geom_BSplineSurface(aPoles,
		aWeights, aUKnots, aVKnots, aUMults, aVMults,
		theSurface.Degree(0), theSurface.Degree(1),
		IsUPeriodic, IsVPeriodic);

	return aBSplineSurface;
}



//临时测试函数
void FToolPathDlg::test()
{
	
	int a[2] = { 86,72 };
	int b[7] = { 1,2,9,4,5,2,7 };
	vector<int> tttt(a, a + 2);
	vector<int> test(b, b + 7);

	vector<int> result(7);

	
	reverse_copy(test.begin(), test.end(), result.begin());


	

	//d= test.end();

}


/**********************************************
*函数名：ToolPath
*函数介绍：等残高刀具路径规划方法
*输入参数：
*输出参数：
*返回值：
*调用函数：
**********************************************/
void FToolPathDlg::ToolPath()
{
	double Step_size, Parameter_increment;
	double curvature;
	//vector<double> curvatureV;

	double E, F, G, L, M, N;
	F3dVector du, dv, duu, duv, dvv, Nvector;

	double base1Q, base2Q;
	double a, g;
	double Der_u, Der_v;
	double SafeDistance;
	//int ith=0;   //ith为刀路的条数
	vector<int> ithSize;  //第i行刀具路径上刀位点的数量

	//vector<vector<double>> Nextpath_para;
	//vector<vector<double>> Totalpath_para;

	vector<double> tincrementV, tPathV;    ////tPathV存储当前样条的t值
	vector<double> tV;    //存储当前样条的t值

	m_Currentpathpara.resize(2);
	m_Nextpathpara.resize(2);
	m_Totalpathpara.resize(2);
	//Totalpath_para.resize(2);

	double dut, dvt;   //u,v分别对t的导数(需要赋值)

	//m_e = 0.01;  //步长残高允差
	//m_h = 0.1;   //残余高度允差
	//m_R = 4;     //球头刀具半径设置

	m_R = ToolRadius->text().toDouble();
	m_e = StepPres->text().toDouble();
	m_h = ResidualHeight->text().toDouble();
	SafeDistance = SafeDis->text().toDouble();   //超出曲面后的刀路延伸参考距离

	m_uTpath = 0.0;
	m_vTpath = 0.0;
	dut = 1.0;	dvt = 0.0;    //第一条路径求导的初始化（选u方向的曲线，即v=0的那条曲线）
	double t, t_increment;
	double Pt[2], dPt[2], ddPt[2];  //u,v平面三次样条的点、一阶导数、二阶导数
	vector<vector<double>> PtV, dPtV, ddPtV;
	PtV.resize(2);
	dPtV.resize(2);
	ddPtV.resize(2);
	double CurvatureRadius;
	F3dVector Sd, Sdd;

	m_CCPnt.swap(vector<F3dPoint>());
	m_CLPnt.swap(vector<F3dPoint>());
	m_Nvector.swap(vector<F3dVector>());
	m_CLPntview.swap(vector<FIpPoint>());
	m_Pntlist.swap(vector<MeasPt*>());
	m_Currentpathpara[0].swap(vector<double>());
	m_Currentpathpara[1].swap(vector<double>());
	m_Nextpathpara[0].swap(vector<double>());
	m_Nextpathpara[1].swap(vector<double>());
	m_Totalpathpara[0].swap(vector<double>());
	m_Totalpathpara[1].swap(vector<double>());

	//以上为初始化部分


	//第一条路径规划（选u方向的曲线，即v=0的那条曲线）

	do {
		pSurf->OnNurbsSurf()->Ev2Der(m_uTpath, m_vTpath, Pnt1, du, dv, duu, duv, dvv, 0, 0);

		curvature = sqrt(pow(du.y*duu.z - duu.y*du.z, 2) + pow(duu.x*du.z - du.x*duu.z, 2) + pow(du.x*duu.y - duu.x*du.y, 2)) / (pow(sqrt(du.x*du.x + du.y*du.y + du.z*du.z), 3));


		Step_size = 2 * sqrt(2 * m_e / curvature - m_e * m_e);

		Parameter_increment = Step_size / du.Length();

		m_Currentpathpara[0].push_back(m_uTpath);
		m_Currentpathpara[1].push_back(m_vTpath);

		//Totalpath_para[0].push_back(m_uTpath);
		//Totalpath_para[1].push_back(m_vTpath);

		m_CCPnt.push_back(pSurf->OnNurbsSurf()->PointAt(m_uTpath, m_vTpath));
		m_Nvector.push_back(normal_vector(du, dv));
		
		m_uTpath = m_uTpath + Parameter_increment;
	} while (m_uTpath <= 1);

	ithSize.push_back(m_Currentpathpara[0].size());   //将第i条路径的长度存储
	m_Totalpathpara[0].insert(m_Totalpathpara[0].end(), m_Currentpathpara[0].begin(), m_Currentpathpara[0].end());
	m_Totalpathpara[1].insert(m_Totalpathpara[1].end(), m_Currentpathpara[1].begin(), m_Currentpathpara[1].end());

	//下一条路径离散点
	for (int i = 0;i < m_Currentpathpara[0].size();i++)
	{
		m_uTpath = m_Currentpathpara[0][i];
		m_vTpath = m_Currentpathpara[1][i];

		pSurf->OnNurbsSurf()->Ev2Der(m_uTpath, m_vTpath, Pnt1, du, dv, duu, duv, dvv, 0, 0);
		E = du * du;	F = du * dv;	G = dv * dv;
		base1Q = E * dut*dut + 2 * F*dut*dvt + G * dvt*dvt;

		//du,dv
		Nvector.x = du.y*dv.z - dv.y*du.z;
		Nvector.y = du.z*dv.x - du.x*dv.z;
		Nvector.z = du.x*dv.y - dv.x*du.y;

		Nvector = Nvector / (sqrt(pow(Nvector.x, 2) + pow(Nvector.y, 2) + pow(Nvector.z, 2)));

		L = duu * Nvector;  M = duv * Nvector;  N = dvv * Nvector;
		base2Q = L * dut*dut + 2 * M*dut*dvt + N * dvt*dvt;

		if (base2Q == 0)
		{
			a = sqrt(pow(m_R, 2) - pow(m_R - m_h, 2));
			g = 2 * a;
		}
		else if (base2Q < 0)
		{
			a = sqrt(pow(m_R, 2) - pow(m_R - m_h, 2));
			for (int i = 0;i < 5;i++)               //文献中说实践表明5次之内就会收敛
			{
				a = sqrt(pow(m_R, 2) - pow(sqrt(pow(1 / curvature + m_R, 2) - pow(a, 2)) - 1 / curvature - m_h, 2));
			}
			g = 2 * a / (1 + m_R * curvature);

		}
		else
		{
			a = sqrt(pow(m_R, 2) - pow(m_R - m_h, 2));
			for (int i = 0;i < 5;i++)               //文献中说实践表明5次之内就会收敛
			{
				a = sqrt(pow(m_R, 2) - pow(sqrt(pow(1 / curvature - m_R, 2) - pow(a, 2)) - 1 / curvature + m_h, 2));
			}
			g = 2 * a / (1 - m_R * curvature);
		}

		//下一条路径离散点
		Der_u = g * (F*dut + G * dvt) / (sqrt(E*G - F * F)*sqrt(E*dut*dut + 2 * F*dut*dvt + G * dvt*dvt));
		Der_v = g * (E*dut + F * dvt) / (sqrt(E*G - F * F)*sqrt(E*dut*dut + 2 * F*dut*dvt + G * dvt*dvt));     //有一个负号的问题，后面再处理

		m_Nextpathpara[0].push_back(m_Currentpathpara[0][i] + Der_u);
		m_Nextpathpara[1].push_back(m_Currentpathpara[1][i] + Der_v);

	}

	//m_Currentpathpara[0].swap(vector<double>());
	//m_Currentpathpara[1].swap(vector<double>());

	//for (int i = 0;i < m_Nextpathpara[0].size();i++)
	//{
	//	m_Currentpathpara[0].push_back(m_Nextpathpara[0][i]);
	//	m_Currentpathpara[1].push_back(m_Nextpathpara[1][i]);

	//}

	//m_Nextpathpara[0].swap(vector<double>());
	//m_Nextpathpara[1].swap(vector<double>());


	/*
	//修改
	//首个点的确定
	for(int j=0;j<m_Currentpathpara[0].size();j++)
	{
		m_uTpath = m_Currentpathpara[0][j];
		m_vTpath = m_Currentpathpara[1][j];

		if((m_uTpath>0)||(m_vTpath>0))
		{


		}else if(m_uTpath<0)
		{

		}else if(m_uTpath<0)
		{

		}else{}


	}

	//末尾点的确定
	*/


	/*大段注释：
	》》该注释之前的内容为选取了以v=0的那条曲线作为初始路径，基于此路径的内容确定了第二条路径的离散点的值，
	后续需要对前面的内容进行修改以能够让用户对第一条路径进行选取。
	》》该注释之后的内容为一个大循环，过程为三次样条拟合离散的点，然后重新离散生成当前路径，依据离散点继续
	得到下一条路径离散点的值，判断下一条路径离散点的值是否都超出边界，如果都超出，则停止循环，如果不超出，继续循环。
	*/

	int count = 0;

	while (1)
	{

		count++;
		if (count > 4)
			break;

		//整理点，判断是否越界，然后对点进行延伸

		m_Currentpathpara[0].swap(vector<double>());
		m_Currentpathpara[1].swap(vector<double>());

		for (int i = 0;i < m_Nextpathpara[0].size();i++)
		{
			m_Currentpathpara[0].push_back(m_Nextpathpara[0][i]);
			m_Currentpathpara[1].push_back(m_Nextpathpara[1][i]);

		}

		m_Nextpathpara[0].swap(vector<double>());
		m_Nextpathpara[1].swap(vector<double>());




		//修剪或延伸下一条路径
		//增删u在0附近的点
		if (m_Currentpathpara[0][0] >= 0)
		{
			m_Currentpathpara[0].insert(m_Currentpathpara[0].begin(), 0);
			m_Currentpathpara[1].insert(m_Currentpathpara[1].begin(), m_Currentpathpara[1][0]);			
		}
		else
		{
			for (int i = 0;i < m_Currentpathpara[0].size() - 1;i++)
			{
				if ((m_Currentpathpara[0][i] < 0) && (m_Currentpathpara[0][i + 1] >= 0))
				{
					m_Currentpathpara[0].erase(m_Currentpathpara[0].begin(), m_Currentpathpara[0].begin() + i);
					m_Currentpathpara[1].erase(m_Currentpathpara[1].begin(), m_Currentpathpara[1].begin() + i);
					m_Currentpathpara[0][0] = 0;
					break;

				}
				else
				{
					cout << "有未知问题" << endl;
					break;
				}
			}
		}


		//增删u在1附近的点
		if (m_Currentpathpara[0][m_Currentpathpara[0].size() - 1] <= 1)
		{
			m_Currentpathpara[0].push_back(1);
			m_Currentpathpara[1].push_back(m_Currentpathpara[1][m_Currentpathpara[1].size() - 1]);
		}
		else
		{
			for (int i = m_Currentpathpara[0].size() - 1;i >= 0;i--)
			{
				if ((m_Currentpathpara[0][i] > 1) && (m_Currentpathpara[0][i - 1] <= 1))
				{
					m_Currentpathpara[0].erase(m_Currentpathpara[0].end() - i, m_Currentpathpara[0].end());
					m_Currentpathpara[1].erase(m_Currentpathpara[1].end() - i, m_Currentpathpara[1].end());
					m_Currentpathpara[0][m_Currentpathpara[0].size() - 1] = 1;
					break;
				}
				else
				{
					cout << "有未知问题" << endl;
					break;
				}
			}
		}

		//删除u方向排布不规范导致不能较好插值的点，使其按大小排列,以及处理超出域的问题
		for (int j = 0;j < m_Currentpathpara[0].size()-1;j++)
		{
			if (m_Currentpathpara[0][j]>= m_Currentpathpara[0][j+1])
			{
				m_Currentpathpara[0].erase(m_Currentpathpara[0].begin() + j + 1);
				m_Currentpathpara[1].erase(m_Currentpathpara[1].begin() + j + 1);
			}

		}
			   		 	  	  	 

		
		//if (m_Currentpathpara[0].back() < 1.0)
		//{
		//	m_Currentpathpara[0].push_back(1);
		//	m_Currentpathpara[1].push_back(m_Currentpathpara[1].back());
		//}
		//else
		//{

		//}
			   		 
		/*
		if(m_Currentpathpara[0][0]<=1-m_Currentpathpara[1][0])
		{
			m_Currentpathpara[0].insert(m_Currentpathpara[0].begin(),0);
			m_Currentpathpara[1].insert(m_Currentpathpara[1].begin(),m_Currentpathpara[1][0]);
		}else{
			m_Currentpathpara[0].insert(m_Currentpathpara[0].begin(),m_Currentpathpara[0][0]);
			m_Currentpathpara[1].insert(m_Currentpathpara[1].begin(),1);
		}
		*/

		//以上还需要进一步整理,细化

		for (int j = 0;j < m_Currentpathpara[0].size();j++)
		{
			if ((m_Currentpathpara[0][j] < 0 || m_Currentpathpara[0][j]>1) || (m_Currentpathpara[1][j] < 0 || m_Currentpathpara[1][j]>1))
			{
				m_Currentpathpara[0].erase(m_Currentpathpara[0].begin() + j);
				m_Currentpathpara[1].erase(m_Currentpathpara[1].begin() + j);
				j--;
			}

		}

		//if (m_Currentpathpara[0].size() < 20)
		//	break;



		//下一条路径拟合
		m_tPntV.swap(vector<double>());
		m_vecM.resize(m_Currentpathpara.size());
		m_vecM[0].swap(vector<double>());
		m_vecM[1].swap(vector<double>());

		m_hV.swap(vector<double>());
		ParaCspline_interpolation();

		//重新生成离散路径
		//清空当前路径内存
		//m_Currentpathpara[0].swap(vector<double>());
		//m_Currentpathpara[1].swap(vector<double>());
		
		//插值后的路径规划（插值曲线的离散）
			   
		//Interpolationcurve_Calcu(0.1,Pt,dPt,ddPt);  //调试测试

		//删除u方向排布不规范导致不能较好插值的点，使其按大小排列,以及处理超出域的问题

		t = 0;
		PtV[0].swap(vector<double>());
		PtV[1].swap(vector<double>());

		dPtV[0].swap(vector<double>());
		dPtV[1].swap(vector<double>());

		ddPtV[0].swap(vector<double>());
		ddPtV[1].swap(vector<double>());

		//curvatureV.swap(vector<double>());

		do {
			Interpolationcurve_Calcu(t, Pt, dPt, ddPt);
			tPathV.push_back(t);

			PtV[0].push_back(Pt[0]);
			PtV[1].push_back(Pt[1]);

			dPtV[0].push_back(dPt[0]);
			dPtV[1].push_back(dPt[1]);

			ddPtV[0].push_back(ddPt[0]);
			ddPtV[1].push_back(ddPt[1]);

			pSurf->OnNurbsSurf()->Ev2Der(Pt[0], Pt[1], Pnt1, du, dv, duu, duv, dvv, 0, 0);
			Sd = dPt[0] * du + dPt[1] * dv;
			Sdd = (duu*dPt[0] + duv * dPt[1])*dPt[0] + du * ddPt[0] + (duv*dPt[0] + dvv * dPt[1])*dPt[1] + dv * ddPt[1];

			curvature = sqrt(pow(Sd.y*Sdd.z - Sdd.y*Sd.z, 2) + pow(Sdd.x*Sd.z - Sd.x*Sdd.z, 2) + pow(Sd.x*Sdd.y - Sdd.x*Sd.y, 2)) / (pow(sqrt(Sd.x*Sd.x + Sd.y*Sd.y + Sd.z*Sd.z), 3));
			//curvatureV.push_back(1/curvature);   //曲率半径向量
			Step_size = 2 * sqrt(2 * m_e / curvature - m_e * m_e);
			Parameter_increment = Step_size / Sd.Length();

			m_Nextpathpara[0].push_back(Pt[0]);
			m_Nextpathpara[1].push_back(Pt[1]);

			//Totalpath_para[0].push_back(Pt[0]);
			//Totalpath_para[1].push_back(Pt[1]);

			m_CCPnt.push_back(pSurf->OnNurbsSurf()->PointAt(Pt[0], Pt[1]));
			m_Nvector.push_back(normal_vector(du, dv));

			t = t + Parameter_increment;

		} while (t <= 1);

		m_Currentpathpara[0].swap(vector<double>());
		m_Currentpathpara[1].swap(vector<double>());

		for (int i = 0;i < m_Nextpathpara[0].size();i++)
		{
			m_Currentpathpara[0].push_back(m_Nextpathpara[0][i]);
			m_Currentpathpara[1].push_back(m_Nextpathpara[1][i]);

		}
		
		ithSize.push_back(m_Currentpathpara[0].size());   //将第i条路径的长度存储
		m_Totalpathpara[0].insert(m_Totalpathpara[0].end(), m_Currentpathpara[0].begin(), m_Currentpathpara[0].end());
		m_Totalpathpara[1].insert(m_Totalpathpara[1].end(), m_Currentpathpara[1].begin(), m_Currentpathpara[1].end());

		m_Nextpathpara[0].swap(vector<double>());
		m_Nextpathpara[1].swap(vector<double>());


		//下一条路径离散点
		for (int i = 0;i < m_Currentpathpara[0].size();i++)
		{
			pSurf->OnNurbsSurf()->Ev2Der(PtV[0][i], PtV[1][i], Pnt1, du, dv, duu, duv, dvv, 0, 0);
			//Interpolationcurve_Calcu(tPathV[i],Pt,dPt,ddPt);

			dut = dPtV[0][i];
			dvt = dPtV[1][i];

			E = du * du;	F = du * dv;	G = dv * dv;
			base1Q = E * dut*dut + 2 * F*dut*dvt + G * dvt*dvt;

			//du,dv
			Nvector.x = du.y*dv.z - dv.y*du.z;
			Nvector.y = du.z*dv.x - du.x*dv.z;
			Nvector.z = du.x*dv.y - dv.x*du.y;

			Nvector = Nvector / (sqrt(pow(Nvector.x, 2) + pow(Nvector.y, 2) + pow(Nvector.z, 2)));

			L = duu * Nvector;  M = duv * Nvector;  N = dvv * Nvector;
			base2Q = L * dut*dut + 2 * M*dut*dvt + N * dvt*dvt;

			CurvatureRadius = abs(base1Q / base2Q);

			if (base2Q == 0)
			{
				a = sqrt(pow(m_R, 2) - pow(m_R - m_h, 2));
				g = 2 * a;
			}
			else if (base2Q < 0)
			{
				a = sqrt(pow(m_R, 2) - pow(m_R - m_h, 2));
				for (int i = 0;i < 5;i++)               //文献中说实践表明5次之内就会收敛
				{
					a = sqrt(pow(m_R, 2) - pow(sqrt(pow(CurvatureRadius + m_R, 2) - pow(a, 2)) - CurvatureRadius - m_h, 2));
				}
				g = 2 * a / (1 + m_R / CurvatureRadius);

			}
			else
			{
				a = sqrt(pow(m_R, 2) - pow(m_R - m_h, 2));
				for (int i = 0;i < 5;i++)               //文献中说实践表明5次之内就会收敛
				{
					a = sqrt(pow(m_R, 2) - pow(sqrt(pow(CurvatureRadius - m_R, 2) - pow(a, 2)) - CurvatureRadius + m_h, 2));
				}
				g = 2 * a / (1 - m_R / CurvatureRadius);
			}

			//下一条路径离散点
			Der_u = -g * (F*dut + G * dvt) / (sqrt(E*G - F * F)*sqrt(E*dut*dut + 2 * F*dut*dvt + G * dvt*dvt));
			Der_v = g * (E*dut + F * dvt) / (sqrt(E*G - F * F)*sqrt(E*dut*dut + 2 * F*dut*dvt + G * dvt*dvt));     //有一个负号的问题，后面再处理

			m_Nextpathpara[0].push_back(m_Currentpathpara[0][i] + Der_u);
			m_Nextpathpara[1].push_back(m_Currentpathpara[1][i] + Der_v);

		}

		//m_Currentpathpara[0].swap(vector<double>());
		//m_Currentpathpara[1].swap(vector<double>());

		//for (int i = 0;i < m_Nextpathpara[0].size();i++)
		//{
		//	m_Currentpathpara[0].push_back(m_Nextpathpara[0][i]);
		//	m_Currentpathpara[1].push_back(m_Nextpathpara[1][i]);

		//}

		//m_Nextpathpara[0].swap(vector<double>());
		//m_Nextpathpara[1].swap(vector<double>());

	}
	   
	//转为CL刀路
	
	
	for (int k = 0;k < m_CCPnt.size();k++)
	{
		//m_CLPnt[k].x = m_CCPnt[k].x + m_R * m_Nvector[k].x;
		//m_CLPnt[k].y = m_CCPnt[k].y + m_R * m_Nvector[k].y;
		//m_CLPnt[k].z = m_CCPnt[k].z + m_R * m_Nvector[k].z;
		m_CLPnt.push_back(m_CCPnt[k] + m_R * m_Nvector[k]);
	}

	//本部分插入刀位非切削部分的刀位控制点并显示
	int tempcount = 0;

	for (int i = 0;i < ithSize.size();i++)
	{
		//m_Totalpathpara
		//m_CCPnt
		//m_Nvector

		double temp1;
		F3dPoint FirstPnt,LastPnt;
		FIpPoint temp;   //中间量

		vector<vector<double>> tempTotalpathpara;
		vector<F3dPoint> tempCCPnt, tempCLPnt;
		vector <F3dVector> tempNvector;

		tempTotalpathpara.resize(2);
		tempTotalpathpara[0].swap(vector<double>());
		tempTotalpathpara[1].swap(vector<double>());
		tempCCPnt.swap(vector<F3dPoint>());
		tempCLPnt.swap(vector<F3dPoint>());
		tempNvector.swap(vector <F3dVector>());		

		if (i % 2 == 1)   //刀路反向
		{
			for (int j = 0;j < ithSize[i];j++)
			{
				tempTotalpathpara[0].push_back(m_Totalpathpara[0][tempcount + ithSize[i] - j - 1]);
				tempTotalpathpara[1].push_back(m_Totalpathpara[0][tempcount + ithSize[i] - j - 1]);
				tempCCPnt.push_back(m_CCPnt[tempcount + ithSize[i] - j - 1]);
				tempNvector.push_back(m_Nvector[tempcount + ithSize[i] - j - 1]);
				tempCLPnt.push_back(m_CLPnt[tempcount + ithSize[i] - j - 1]);
			}
		}
		else
		{	
			//tempTotalpathpara[0].assign(m_Totalpathpara[0].begin() + tempcount, m_Totalpathpara[0].begin() + tempcount + ithSize[i] - 1);

			for (int j = 0;j < ithSize[i];j++)
			{
				tempTotalpathpara[0].push_back(m_Totalpathpara[0][tempcount+j]);
				tempTotalpathpara[1].push_back(m_Totalpathpara[0][tempcount + j]);
				tempCCPnt.push_back(m_CCPnt[tempcount + j]);
				tempNvector.push_back(m_Nvector[tempcount + j]);
				tempCLPnt.push_back(m_CLPnt[tempcount + j]);
			}
		}
		
		//temp1 = sqrt(pow(tempCLPnt[1].x - tempCLPnt[0].x, 2) + pow(tempCLPnt[1].y - tempCLPnt[0].y, 2) + pow(tempCLPnt[1].z - tempCLPnt[0].z, 2));
		//FirstPnt.x = (SafeDistance / temp1)*(tempCLPnt[0].x - tempCLPnt[1].x);
		//FirstPnt.y = (SafeDistance / temp1)*(tempCLPnt[0].y - tempCLPnt[1].y);
		//FirstPnt.z = (SafeDistance / temp1)*(tempCLPnt[0].z - tempCLPnt[1].z);
		//tempCLPnt.insert(tempCLPnt.begin(), FirstPnt);

		//LastPnt.x = (SafeDistance / temp1)*(tempCLPnt[tempCLPnt.size() - 1].x - tempCLPnt[tempCLPnt.size() - 2].x);
		//LastPnt.y = (SafeDistance / temp1)*(tempCLPnt[tempCLPnt.size() - 1].y - tempCLPnt[tempCLPnt.size() - 2].y);
		//LastPnt.z = (SafeDistance / temp1)*(tempCLPnt[tempCLPnt.size() - 1].z - tempCLPnt[tempCLPnt.size() - 2].z);
		//tempCLPnt.push_back(LastPnt);
		
		//m_CLPntview.insert(m_CLPntview.end(), tempCLPnt.begin(), tempCLPnt.end());
		for (int k = 0;k < tempCLPnt.size();k++)
		{
			temp.x = tempCLPnt[k].x;
			temp.y = tempCLPnt[k].y;
			temp.z = tempCLPnt[k].z;

			m_CLPntview.push_back(temp);
		}

	
		tempcount = tempcount + ithSize[i];	
	}
	   
	

	
	//for (int i = 0;i < m_CCPnt.size();i++)
	//{
	//	
	//	MeasPt* my_measPnt = new MeasPt(m_CLPnt[i].x, m_CLPnt[i].y, m_CLPnt[i].z, m_Nvector[i].x, m_Nvector[i].y, m_Nvector[i].z);
	//	//MeasPt* my_measPnt = new MeasPt(m_CCPnt[i].x, m_CCPnt[i].y, m_CCPnt[i].z, m_Nvector[i].x, m_Nvector[i].y, m_Nvector[i].z);
	//	gWindow->GetMainDoc() ->AddDispEnt(my_measPnt);
	//	m_Pntlist.push_back(my_measPnt);
	//}

	

	   	  
	//m_myToolPath[i] = new MeasPath(m_FIpToolPathList[i]);
	//m_myToolPath[i]->SetPathNum(i);
	//gWindow->GetMainDoc()->AddDispEnt(m_myToolPath[i]);


	/*
	m_CCPntview.resize(m_CCPnt.size());
	for (int i = 0;i < m_CCPnt.size();i++)
	{
		m_CCPntview[i].x = m_CCPnt[i].x;
		m_CCPntview[i].y = m_CCPnt[i].y;
		m_CCPntview[i].z = m_CCPnt[i].z;
	}*/
	   	  
}

/**********************************************
*函数名：DrawToolPath
*函数介绍：绘制刀具路径
*输入参数：
*输出参数：
*返回值：
*调用函数：
**********************************************/
void FToolPathDlg::DrawToolPath()
{
	//int i = 1;
	//vector<FIpPntList> DirIpPathList = m_CLPntview;
	
	//m_myToolPath = new MeasPath(ToQvec(m_CLPntview));
	m_myToolPath = new PathShow(Togpvec(m_CLPntview));
	//m_myToolPath->SetPathNum(1);
	gWindow->GetMainDoc()->AddDispEnt(m_myToolPath);
	
	//vector<FIpPntList> DirIpPathList = m_CLPntview->GetPathList();

	cout << "keyi" << endl;

}

/**********************************************
*函数名：ToQvec
*函数介绍：向量类型转化
*输入参数：
*输出参数：
*返回值：
*调用函数：
**********************************************/
vector<gp_Pnt> FToolPathDlg::Togpvec(vector<FIpPoint> dest)
{
	vector<gp_Pnt> tmp;
	
	for (int i = 0;i < dest.size();i++)
	{
		tmp.push_back(gp_Pnt(dest[i].x, dest[i].y, dest[i].z));
	}

	return tmp;
}

/**********************************************
*函数名：clsExp()
*函数介绍：输出cls文件
*输入参数：
*输出参数：
*返回值：
*调用函数：
**********************************************/
void FToolPathDlg::clsExp()
{
	m_clsExp.swap(vector<QString>());
	QString tempLineText,tempSec;




	m_clsExp.push_back("TOOL PATH/VARIABLE_CONTOUR,TOOL,D10R5-2");
		
	tempLineText = "TLDATA/" + ToolType->text() + tempLineText + "," + ToolDiam->text() + "," +
		ToolRadius->text() + "," + ToolLength->text() + "," + ToolTaperAngle->text() + "," + ToolTipAngle->text();
	m_clsExp.push_back(tempLineText);

	tempLineText = "MSYS/" + ProcessingOriginX->text() + "," + ProcessingOriginY->text() + "," + ProcessingOriginZ->text()
		+ "1,0,0,0,1,0";
	m_clsExp.push_back(tempLineText);

	m_clsExp.push_back("RAPID");






	ofstream inFile;
	inFile.open("ToolPath.cls", ios::trunc);

	for (int i = 0;i < m_clsExp.size();i++)
	{
		inFile << m_clsExp[i].toStdString() << "\n";
			
	}
	inFile.close();//关闭文件
	m_clsExp.swap(vector<QString>());




}

//刀具绘制函数
void ToolSpec::Draw(const FT::FRenderOpt& renderOpt)
{

	//绘制球头刀的球头部分
	//球心坐标为（x，y，z），球的半径为radius，M，N分别表示球体的横纵向被分成多少份
	int M = 36, N = 18;

	drawSphere(m_x, m_y, m_z, m_R, M, N);

	drawCylinder(m_x, m_y, m_z, m_i, m_j, m_k, m_R, m_L);

	//drawCircle(m_x, m_y, m_z, m_i, m_j, m_k, m_R, m_L);

}


void ToolSpec::drawSphere(double xx, double yy, double zz, double radius, double M, double N)
{
	double step_z = PI / M;
	double step_xy = 2 * PI / N;
	double x[4], y[4], z[4];
	double angle_z,angle_xy;
	//glBindTexture(GL_TEXTURE_2D, texture[0]);

	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
	for (int i = 0; i < M; i++)
	{
		angle_z = i * step_z;
		for (int j = 0;j < N;j++)
		{
			angle_xy = j * step_xy;
			x[0] = radius * sin(angle_z) * cos(angle_xy);
			y[0] = radius * sin(angle_z) * sin(angle_xy);
			z[0] = radius * cos(angle_z);

			x[1] = radius * sin(angle_z + step_z) * cos(angle_xy);
			y[1] = radius * sin(angle_z + step_z) * sin(angle_xy);
			z[1] = radius * cos(angle_z + step_z);

			x[2] = radius * sin(angle_z + step_z) * cos(angle_xy + step_xy);
			y[2] = radius * sin(angle_z + step_z) * sin(angle_xy + step_xy);
			z[2] = radius * cos(angle_z + step_z);

			x[3] = radius * sin(angle_z) * cos(angle_xy + step_xy);
			y[3] = radius * sin(angle_z) * sin(angle_xy + step_xy);
			z[3] = radius * cos(angle_z);

			for (int k = 0; k < 4; k++)
			{
				//glColor3f(sin(angle_z), cos(angle_z), tan(angle_z));
				glVertex3f(xx + x[k], yy + y[k], zz + z[k]);
			}
		}
	}
	glEnd();
}


void ToolSpec::drawCylinder(double xx, double yy, double zz, double nx, double ny, double nz, double radius, double Length)
{	
	////计算旋转轴线(Z向与方向向量的叉乘)

	double CrossMx = -ny, CrossMy = nx, CrossMz = 0;
	//标准化
	CrossMx = CrossMx / sqrt(CrossMx * CrossMx + CrossMy * CrossMy + CrossMz * CrossMz);
	CrossMy = CrossMy / sqrt(CrossMx * CrossMx + CrossMy * CrossMy + CrossMz * CrossMz);
	CrossMz = CrossMz / sqrt(CrossMx * CrossMx + CrossMy * CrossMy + CrossMz * CrossMz);

	double angle = acos(nz) * 180.0 / 3.14159;

	glTranslatef(xx, yy, zz);
	glRotatef(angle, CrossMx, CrossMy, CrossMz);
	
	glBegin(GL_QUAD_STRIP);   //连续填充四边形串
	for (int i = 0; i <= 360; i += 15)
	{
		double p = i * 3.14 / 180;
		
		glVertex3f(radius * sin(p), radius * cos(p), Length);
		glVertex3f(radius * sin(p), radius * cos(p),0);
	}
	glEnd();
	//bottom circle
	//glColor3f(1, 0, 0);
	//drawCircle(xx, yy, zz, nx, ny, nz, radius);
	
	//top circle
	drawCircle(radius, Length);
	//glRotatef()

	//glTranslatef(xx, yy, zz + Length);	
	
	//glColor3f(0, 1, 0);
	

}

//绘制刀具的底面顶面圆
void ToolSpec::drawCircle(double radius, double Length)
{
	////计算旋转轴线(Z向与方向向量的叉乘)

	//double CrossMx = -ny, CrossMy = nx, CrossMz = 0;
	////标准化
	//CrossMx = CrossMx / sqrt(CrossMx * CrossMx + CrossMy * CrossMy + CrossMz * CrossMz);
	//CrossMy = CrossMy / sqrt(CrossMx * CrossMx + CrossMy * CrossMy + CrossMz * CrossMz);
	//CrossMz = CrossMz / sqrt(CrossMx * CrossMx + CrossMy * CrossMy + CrossMz * CrossMz);

	
	glColor3f(0, 0, 1);
	//glTranslatef(0, 0, Length);
	//glTranslatef(xx, yy, zz);
	//glRotatef(40, CrossMx, CrossMy, CrossMz);
	glBegin(GL_TRIANGLE_FAN);     //扇形连续填充三角形串
	//glVertex3f(xx, yy, zz);
	for (int i = 0; i <= 360; i += 15)
	{
		double p = i * 3.14 / 180;
		//glColor3f(sin(p), cos(p), tan(p));
		
		glVertex3f(radius * sin(p), radius * cos(p), Length);
	}
	glEnd();
}



void ToolMove_thread::run()
{

	//FIpPathList CurrPathList = gWindow->GetMainDoc()->GetPathList();
	//FIpPntList PathPntList;
	vector<gp_Pnt> CLPntLine;
	vector<gp_Vec> CLPntVecLine;
	CLPntLine.resize(m_FToolPathDlg->getCLPntLine().size());
	CLPntVecLine.resize(m_FToolPathDlg->getCLPntVecLine().size());

	for (int i = 0;i < m_FToolPathDlg->m_CLPntLine.size();i++)
	{
		CLPntLine[i] = m_FToolPathDlg->m_CLPntLine[i];
		CLPntVecLine[i] = m_FToolPathDlg->m_CLPntVecLine[i];
	}

	//vector<gp_Pnt> CLPntLine = m_FToolPathDlg->m_CLPntLine;
	//vector<gp_Vec> CLPntVecLine = m_FToolPathDlg->m_CLPntVecLine;

	m_FToolPathDlg->m_ToolSpec->m_R = m_FToolPathDlg->m_R;

	for (int i = 0;i < CLPntLine.size();i++)
	{
		/*m_FToolPathDlg->m_ToolSpec = new ToolSpec(CLPntLine[i].X(), CLPntLine[i].Y(), CLPntLine[i].Z(), 
			CLPntVecLine[i].X(), CLPntVecLine[i].Y(), CLPntVecLine[i].Z(), 5, 15);*/
		m_FToolPathDlg->m_ToolSpec->m_x = CLPntLine[i].X();
		m_FToolPathDlg->m_ToolSpec->m_y = CLPntLine[i].Y();
		m_FToolPathDlg->m_ToolSpec->m_z = CLPntLine[i].Z();

		m_FToolPathDlg->m_ToolSpec->m_i = CLPntVecLine[i].X();
		m_FToolPathDlg->m_ToolSpec->m_j = CLPntVecLine[i].Y();
		m_FToolPathDlg->m_ToolSpec->m_k = CLPntVecLine[i].Z();

		gWindow->UpdateModelView();
		Sleep(100);
	}







}


void ToolMove_thread::MovePntToPnt(gp_Pnt oriPnt, gp_Vec oriVec, gp_Pnt aimPnt, gp_Vec aimVec, int theMoveTime = 1, int Smooth = 100)
{





}
