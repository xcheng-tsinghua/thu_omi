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
	this->setWindowFlags(Qt::WindowStaysOnTopHint);//��Զ����������֮��
	move(1250, 150);
	this->setWindowTitle(QString::fromLocal8Bit("��·�滮"));
	this->setWindowIcon(QIcon("../icons/ToolPath.png"));

	QLabel* Label_ToolRadius = new QLabel(QString::fromLocal8Bit("Բ�ǰ뾶"));
	ToolRadius = new QLineEdit;
	QLabel* Label_StepPres = new QLabel(QString::fromLocal8Bit("��������"));
	StepPres = new QLineEdit;
	QLabel* Label_ResidualHeight = new QLabel(QString::fromLocal8Bit("����߶�"));
	ResidualHeight = new QLineEdit;
	QLabel* Label_SafeDis = new QLabel(QString::fromLocal8Bit("��ȫ����"));
	SafeDis = new QLineEdit;

	QLabel* Label_ToolType = new QLabel(QString::fromLocal8Bit("��������"));
	ToolType = new QLineEdit;
	QLabel* Label_ToolDiam = new QLabel(QString::fromLocal8Bit("����ֱ��"));
	ToolDiam = new QLineEdit;
	QLabel* Label_ToolLength = new QLabel(QString::fromLocal8Bit("���߳���"));
	ToolLength = new QLineEdit;
	QLabel* Label_ToolTaperAngle = new QLabel(QString::fromLocal8Bit("����׶��"));
	ToolTaperAngle = new QLineEdit;
	QLabel* Label_ToolTipAngle = new QLabel(QString::fromLocal8Bit("���߼��"));
	ToolTipAngle = new QLineEdit;
	QLabel* Label_ProcessingOriginX = new QLabel(QString::fromLocal8Bit("ԭ������ X"));
	ProcessingOriginX = new QLineEdit;
	QLabel* Label_ProcessingOriginY = new QLabel(QString::fromLocal8Bit("ԭ������ Y"));
	ProcessingOriginY = new QLineEdit;
	QLabel* Label_ProcessingOriginZ = new QLabel(QString::fromLocal8Bit("ԭ������ Z"));
	ProcessingOriginZ = new QLineEdit;

	SurfGenerate = new QPushButton(QString::fromLocal8Bit("��������"));
	InspPntsPlan = new QPushButton(QString::fromLocal8Bit("���滮"));
	SurfReconstruct = new QPushButton(QString::fromLocal8Bit("������ʾ"));
	MachiningPathPlan = new QPushButton(QString::fromLocal8Bit("��·�滮"));
	SelSurfBtn = new QPushButton(QString::fromLocal8Bit("ѡ������"));

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
*��������onCreateNURBS()
*�������ܣ�����NURBS����
*���������
*���������
*����ֵ��
*���ú�����
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
	//�����Ը�������

	//CV[0][0] = F3dPoint(-5, -5, 5); CV[0][1] = F3dPoint(-5, -2.5, 3); CV[0][2] = F3dPoint(-5, 0, 0); CV[0][3] = F3dPoint(-5, 2.5, -1); CV[0][4] = F3dPoint(-5, 5, -2);
	//CV[1][0] = F3dPoint(-2.5, -5, 2);  CV[1][1] = F3dPoint(-2.5, -2.5, 1); CV[1][2] = F3dPoint(-2.5, 0, 0); CV[1][3] = F3dPoint(-2.5, 2.5, -1); CV[1][4] = F3dPoint(-2.5, 5, -2);
	//CV[2][0] = F3dPoint(0, -5, 0);  CV[2][1] = F3dPoint(0, -2.5, 0); CV[2][2] = F3dPoint(0, 0, 0); CV[2][3] = F3dPoint(0, 2.5, -1); CV[2][4] = F3dPoint(0, 5, -2);
	//CV[3][0] = F3dPoint(2.5, -5, -1);  CV[3][1] = F3dPoint(2.5, -2.5, -2); CV[3][2] = F3dPoint(2.5, 0, -3); CV[3][3] = F3dPoint(2.5, 2.5, -3); CV[3][4] = F3dPoint(2.5, 5, -3.5);
	//CV[4][0] = F3dPoint(5, -5, -2);  CV[4][1] = F3dPoint(5, -2.5, -3); CV[4][2] = F3dPoint(5, 0, -4); CV[4][3] = F3dPoint(5, 2.5, -4); CV[4][4] = F3dPoint(5, 5, -5);
	//
	//����������

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

	pSurf->Tessellator()->SetTesPrecision(1);///////////////////////////////////////////////////���澫��
	pSurf->Update();
	nb_surface = pSurf->OnNurbsSurf();
	pDoc->AddDispEnt(pSurf);

	//�������������ڶ��ε����������ʱ�Ĵ���
	vector<double>().swap(m_U);
	vector<double>().swap(m_V);
}

/**********************************************
*��������normal_vector
*�������ܣ���λ����������
*���������
*���������
*����ֵ��
*���ú�����
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
*��������onMPtDist
*�������ܣ����ֲ��ۺ���
*���������
*���������
*����ֵ��
*���ú�����
**********************************************/
void FToolPathDlg::onMPtDist()
{
	//MPtDistTest();//���ò��ֲ�����
	FBSampDis();
}

/**********************************************
*��������MPtDistTest
*�������ܣ����ֲ��������Ժ���
*���������
*���������
*����ֵ��
*���ú�����
**********************************************/
void FToolPathDlg::MPtDistTest()
{
	//vector<F3dPoint> PntKVec;  //��Ӧ�㴦�����ʴ洢
	//F3dPoint PntK;  //��Ӧ�㴦������

	vector<double> PntKVecA, PntKVecG, PntKVecMax, PntKVecMin; //ƽ�����ʡ���˹���ʡ���������ʡ���С������
	F3dVector du, dv, duu, duv, dvv;
	int m = 99, n = 99;  //�ڲ����ռ�Ⱦ����100*100
	int mAdapt, nAdapt;  //����Ӧ����Ĳ������
	double m1 = 0, n1;
	double testMax, testMin;
	double KA, KG, KMax, KMin;  //ƽ���������˹����

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

	//���Ȳ��������С�����ʵĺ���Ϊ��������



	//���ϻ���Ҫ����,ͳһ����ȡֵ��Χ

	//һ�����߷ֲ�
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




	////�������߲��ֲ�

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



	//��ʾ���
		

	m_Nvector.push_back(normal_vector(du, dv));

	for (int i = 0;i < m_CCPnt.size();i++)
	{

		MeasPt* my_measPnt = new MeasPt(m_CLPnt[i].x, m_CLPnt[i].y, m_CLPnt[i].z, m_Nvector[i].x, m_Nvector[i].y, m_Nvector[i].z);
		//MeasPt* my_measPnt = new MeasPt(m_CCPnt[i].x, m_CCPnt[i].y, m_CCPnt[i].z, m_Nvector[i].x, m_Nvector[i].y, m_Nvector[i].z);
		gWindow->GetMainDoc()->AddDispEnt(my_measPnt);
		m_Pntlist.push_back(my_measPnt);
	}




	cout<<"����"<<endl;


	









}

/**********************************************
*��������PntK(int i)
*�������ܣ�����ĳ�㴦��ƽ������
*���������iѡ�񷵻ص����ʣ�i=1����ƽ�����ʣ�i=2���ظ�˹����
*���������
*����ֵ��
*���ú�����
**********************************************/
double FToolPathDlg::PntK(int i,double u,double v, F3dVector du, F3dVector dv, F3dVector duu, F3dVector duv, F3dVector dvv)
{
	double KA,KG;   //KAΪƽ�����ʣ�KGΪ��˹����
	F3dPoint pnttemp;  //��ʱ�洢��Ӧ�ĵ�����꣬���ܲ���
	F3dVector Nvector;  //�洢ƫ����
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
*��������FBSampDis
*�������ܣ���������������NURBS������ֲ�
*���������
*���������
*����ֵ��
*���ú�����SLSampDis
**********************************************/
void FToolPathDlg::FBSampDis()
{
	//�������ʼ������
	int m = 12, n = 10;   //u,v���������ϲ������������ڿ��Ը�Ϊ��������
	int kNum = 100;    //����ƽ������ϸ�ֵĶ���
	double minDistance; //�ݴ���С����
	double minDistanceuv[2];  //��С����ʱ��������

	F3dVector du, dv, duu, duv, dvv;
	F3dPoint Pnt1xyz;
	//double CurvatureU, CurvatureV;   //u��v���������ߵ�����

	vector<int> testtest(5, 4);
	testtest.resize(3);

	vector<vector<vector<double>>> FeaturePara;   //�ܼ��㴦u,v���������ߵ�С������ֵ�洢
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

	vector<vector<vector<double>>> Curvatureuv;   //�ܼ��㴦u,v�����������ڵ��ϵ����ʴ洢(kNum+1*kNum+1����0��u����,1��v����)
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

	vector<vector<double>> Curvatureu, arclthPPu, FeatureParau;  //�ܼ��㴦u���������ߵ�С�����ʴ洢
	Curvatureu.resize(kNum);
	arclthPPu.resize(kNum);
	FeatureParau.resize(kNum);
	for (int i = 0; i < kNum; i++)
	{
		Curvatureu[i].resize(kNum + 1);
		arclthPPu[i].resize(kNum + 1);
		FeatureParau[i].resize(kNum + 1);
	}

	vector<vector<double>> Curvaturev, arclthPPv, FeatureParav;   //�ܼ��㴦u���������ߵ�С�����ʴ洢
	Curvaturev.resize(kNum + 1);
	arclthPPv.resize(kNum + 1);
	FeatureParav.resize(kNum + 1);
	for (int i = 0; i < kNum + 1; i++)
	{
		Curvaturev[i].resize(kNum);
		arclthPPv[i].resize(kNum);
		FeatureParav[i].resize(kNum);
	}

	vector<vector<vector<double>>> SampPntus, SampPntvs;    //�ֱ�洢u,v����Ĳ��
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
	
	vector<vector<F3dPoint>> Pntsxyz;  //�洢u,v����������������ά�����ֵ��kNum+1ά
	Pntsxyz.resize(kNum + 1);
	for (int i = 0; i < kNum + 1; i++)
	{
		Pntsxyz[i].resize(kNum + 1);
	}

	vector<vector<vector<double>>> arclthPP;  //�洢u,v�������������ϵ������Ļ�������ֱ�߽���
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

	double*** Pntuv;   //�洢����u��vֵ
	Pntuv = new double** [m];
	for (int i = 0; i < m; i++)
		Pntuv[i] = new double* [n];
	for (int i = 0; i < m; i++)
		for (int j = 0;j < n;j++)
			Pntuv[i][j] = new double[2];

	double** Pntu, ** Pntv, ** uvDistance;   //�ֱ�洢��ֱ�ڲ���u,v����Ķ������ߵ���ͬ��λ�Ĳ�������,u,v���߲��ľ���
	Pntu = new double* [kNum + 1];
	Pntv = new double* [kNum + 1];
	uvDistance = new double* [kNum + 1];
	for (int i = 0; i < kNum + 1; i++)
	{
		Pntu[i] = new double[2];
		Pntv[i] = new double[2];
		uvDistance[i] = new double[kNum + 1];
	}

	vector<double> SampPnts;  //������������ϲ�����Ĳ���λ��
	SampPnts.resize(kNum + 1);

	ofstream fout("testout.txt");

	//�������ʼ������

	//ϸ��������u,v�������������������
	//����ϸ����������ά����ֵ(���ڼ�����ƻ�����kNum+1*kNum+1����)
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
	 
	// u��������ֵ����(kNum+1�����ϵ�kNumС��,ÿС�ε�����ֵ),u�����kNum+1�����ߵĲ��ֲ�

	//vector<double> tempSum1(kNum,0), tempSum2(kNum,0);
	double tempSumu1, tempSumu2;
	for (int j = 0; j < kNum + 1; j++)
	{
		tempSumu1 = 0;
		tempSumu2 = 0;
		//���ʡ���������
		for (int i = 0; i < kNum; i++)
		{			
			Curvatureu[i][j] = (1.0 / 2.0) * (Curvatureuv[i][j][0] + Curvatureuv[i + 1][j][0]);
			arclthPPu[i][j] = sqrt(pow(Pntsxyz[i][j].x - Pntsxyz[i + 1][j].x, 2) + pow(Pntsxyz[i][j].y - Pntsxyz[i + 1][j].y, 2)
				+ pow(Pntsxyz[i][j].z - Pntsxyz[i + 1][j].z, 2));
			tempSumu1 = tempSumu1 + Curvatureu[i][j];
			tempSumu2 = tempSumu2 + arclthPPu[i][j];
		}
		//���ʡ�������׼��������ֵ����(���ڿ����޸�ΪȨ�ؿɵ��ڵ�)
		for (int i = 0; i < kNum; i++)
		{
			Curvatureu[i][j] = Curvatureu[i][j] / tempSumu1;
			arclthPPu[i][j] = arclthPPu[i][j] / tempSumu2;
			FeatureParau[i][j] = (1.0 / 2.0) * (Curvatureu[i][j] + arclthPPu[i][j]);
		}

		//�����j�����ߵĲ��ֲ�
		SLSampDis(j, m, kNum, FeatureParau, SampPntus,0);
	}

	// v��������ֵ����(kNum+1�����ϵ�kNumС��),v�����kNum+1�����ߵĲ��ֲ�
	for (int i = 0; i < kNum + 1; i++)
	{
		tempSumu1 = 0;
		tempSumu2 = 0;		
		//���ʡ���������
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

		//�����i�����ߵĲ��ֲ�
		SLSampDis(i, n, kNum, FeatureParav, SampPntvs, 1);
	}

	//��i,j�����������
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
	
	//�����ر����ڴ�
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
*��������SLSampDis
*�������ܣ�����NURBS�����ϲ���������Ӧ�ֲ����������������ķ���
*���������iΪ��i�����ߣ��������ģSampSize��ϸ���������kNum��FeatureParaϸ��������u/v���������ֵ
* SampPnts��������ϵ�i�����������,uorv=0�ǲ���u���������
*���������
*����ֵ��
*���ú�����
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

		//��һ���������һ��������������
		SampPnts[ith][0][0] = (double)ith* (double)(1.0 / (double)kNum);
		SampPnts[ith][0][1] = 0;

		SampPnts[ith][SampSize - 1][0] = (double)ith* (double)(1.0 / (double)kNum);
		SampPnts[ith][SampSize - 1][1] = 1;

		//�м䲿λ�Ĳ��������
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

		//��һ���������һ��������������
		SampPnts[0][ith][0] = 0;
		SampPnts[0][ith][1] = (double)ith * (double)(1.0 / (double)kNum);

		SampPnts[SampSize-1][ith][0] = 1;
		SampPnts[SampSize-1][ith][1] = (double)ith * (double)(1.0 / (double)kNum);

		//�м䲿λ�Ĳ��������		
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
*��������FBSampDis
*�������ܣ���������������NURBS������ֲ�
*���������
*���������
*����ֵ��
*���ú�����SLSampDis
**********************************************/
//void FToolPathDlg::FBSampDis()
//{
//	//�������ʼ������
//	int m = 15, n = 10;   //u,v���������ϲ������������ڿ��Ը�Ϊ��������
//	int kNum = 100;    //����ƽ������ϸ�ֵĶ���
//	double minDistance; //�ݴ���С����
//	double minDistanceuv[2];  //��С����ʱ��������
//
//	F3dVector du, dv, duu, duv, dvv;
//	F3dPoint Pnt1xyz;   
//	double CurvatureU, CurvatureV;   //u��v���������ߵ�����
//
//	vector<vector<vector<double>>> FeaturePara;   //�ܼ��㴦u,v���������ߵ�С������ֵ�洢
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
//	vector<vector<vector<double>>> Curvatureuv;   //�ܼ��㴦u,v���������ߵ�С�����ʴ洢
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
//	vector<vector<F3dPoint>> Pntsxyz;  //�洢u,v����������������ά�����ֵ��kNum+1ά
//	Pntsxyz.resize(kNum+1);
//	for (int i = 0; i < kNum+1; i++)
//	{
//		Pntsxyz[i].resize(kNum+1);
//	}
//
//	vector<vector<vector<double>>> arclthPP;  //�洢u,v�������������ϵ������Ļ�������ֱ�߽���
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
//	double*** Pntuv;   //�洢����u��vֵ
//	Pntuv = new double** [m];
//	for (int i = 0; i < m; i++)
//		Pntuv[i] = new double *[n];
//	for (int i = 0; i < m; i++)
//		for (int j = 0;j < n;j++)
//			Pntuv[i][j] = new double[2];
//
//	double** Pntu, ** Pntv, ** uvDistance;   //�ֱ�洢��ֱ�ڲ���u,v����Ķ������ߵ���ͬ��λ�Ĳ�������,u,v���߲��ľ���
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
//	vector<double> SampPnts;  //������������ϲ�����Ĳ���λ��
//	SampPnts.resize(kNum + 1);
//
//	ofstream fout("testout.txt");
//	
//	//�������ʼ������
//
//	//ϸ��������u,v�������������������
//	//�����ܼ��ֶ����ʼ��㣬
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
//	//u=1,v=1�ı߽紦�ĵ����ά����洢
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
//	//�ֶλ�������(��������ֱ�߾������)
//	double arclthu, arclthv;  //�ݴ�u,v�������򻡳�ֵ(��ֱ���ݴ�)
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
//	//���ʡ�������׼��
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
//	//������������(���ڿ����޸�ΪȨ�ؿɵ��ڵ�)
//	for (int i = 0;i < kNum;i++)
//	{
//		for (int j = 0;j < kNum;j++)
//		{
//			FeaturePara[i][j][0] = (1.0 / 2.0) * (Curvatureuv[i][j][0] + arclthPP[i][j][0]);
//			FeaturePara[i][j][1] = (1.0 / 2.0) * (Curvatureuv[i][j][1] + arclthPP[i][j][1]);
//		}
//	}
//	
//	//��i,j�����������
//	for (int i = 0;i < m;i++)
//	{
//		for (int j = 0;j < n;j++)
//		{
//
//			//��ֱ��u���������
//			for (int ku = 0;ku <= kNum;ku++)
//			{
//				Pntu[ku][0] = (double)ku*(1.0 / (double)kNum);
//				SLSampDis(j,m, kNum, FeaturePara, SampPnts,1);
//				Pntu[ku][1] = SampPnts[ku];
//			}
//
//			//��ֱ��v����
//			for (int kv = 0;kv <= kNum;kv++)
//			{				
//				Pntv[kv][1] = (double)kv * (1.0 / (double)kNum);
//				SLSampDis(i,n, kNum, FeaturePara, SampPnts,0);
//				Pntv[kv][0] = SampPnts[kv];
//			}
//
//			//�������������룬
//			for (int ku = 0;ku <= kNum;ku++)
//			{
//				for (int kv = 0;kv <= kNum;kv++)
//				{
//					uvDistance[ku][kv] = sqrt(pow(Pntu[ku][0] - Pntv[kv][0], 2) + pow(Pntu[ku][1] - Pntv[kv][1], 2));
//				}
//			}
//
//			//ѡ�������������ľ���
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
//	//�����ر����ڴ�
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
*��������NURBSdeform
*�������ܣ����ÿ��Ƶ��ƶ�����NURBS������Σ��Ӷ������ع��ķ��������ÿ��Ƶ��ƶ�����ΪԼ��
*���������
*���������
*����ֵ��
*���ú�����
**********************************************/
void FToolPathDlg::NURBSdeform()
{
	//��ȡ����������Ϣ
	int uCVCount = pSurf->OnNurbsSurf()->CVCount(0);
	int vCVCount = pSurf->OnNurbsSurf()->CVCount(1);

	int u_degree, v_degree;
	int KnotCountU, KnotCountV;


	u_degree = pSurf->OnNurbsSurf()->Order(0);  //��������Ľ���
	v_degree = pSurf->OnNurbsSurf()->Order(1);

	KnotCountU = pSurf->OnNurbsSurf()->KnotCount(0);
	KnotCountV = pSurf->OnNurbsSurf()->KnotCount(1);

	double SumPowRij = 0;

	vector<vector<F3dPoint>> CVDev;    //���Ƶ�ƫ���С

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

	//���ɲ������棨������棩����ԭ������ж�Ӧ��ķ���ƫ�ã����ò����㣬�����õȲ���������15*15���㡣
	//dj=0.05cos(0.6(ui+vi)+4)

	//�Ȳ���map

	double n1 = 10, n2 = 10;  //����uv��������ļ��

	vector<double> PntU, PntV;  //U,V�����ֵ����
	vector<vector<F3dPoint>> PntUVSamp, PntUVSampTarget;  //������Ӧ��,��������ĵ�(����Ŀ���)
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

	//���ò�������������
	for (int i = 0;i < PntU.size();i++)
	{
		for (int j = 0;j < PntV.size();j++)
		{
			pSurf->OnNurbsSurf()->EvNormal(PntU[i], PntV[j], PntUVSamp[i][j], PntUVSampVec[i][j]);
			PntUVSampTarget[i][j] = PntUVSamp[i][j] + 0.05*cos(0.6*(PntU[i] + PntV[j]) + 4)*PntUVSampVec[i][j];

		}
	}


	vector<vector<F3dPoint>> CVNew;  //�µĿ��Ƶ�洢
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

	//vector<double> SolveVector;  //���������,

	int SolveVectorlenth = uCVCount * vCVCount + PntU.size()*PntV.size();  //���󷽳�������

	//SolveVector.resize(SolveVectorlenth);
	
	math_Matrix aMatrix(1, SolveVectorlenth, 1, SolveVectorlenth);   //Opencascade�й������Է���������
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
					aMatrix(k1*PntV.size() + k2+1, k3*vCVCount + k4+1) = OneRijFun(PntU[k1], PntV[k2], u_degree, v_degree, k3, k4, knotUarray, knotVarray);  //��1����Ϊ�þ����Ǵ�1��ʼ
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

	
	   	  
	cout << "�㷨���Ե�" << endl;

}

/**********************************************
*��������NURBSPntdeform
*�������ܣ����������
*���������
*���������
*����ֵ��
*���ú�����
**********************************************/
void FToolPathDlg::NURBSPntdeform()
{

	
	
	//��ȡ����������Ϣ
	int uCVCount = pSurf->OnNurbsSurf()->CVCount(0);
	int vCVCount = pSurf->OnNurbsSurf()->CVCount(1);
	
	int u_degree, v_degree;
	int KnotCountU, KnotCountV;


	u_degree = pSurf->OnNurbsSurf()->Order(0);  //��������Ľ���
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
	
	//���ɲ������棨������棩����ԭ������ж�Ӧ��ķ���ƫ�ã����ò����㣬�����õȲ���������15*15���㡣
	//dj=0.05cos(0.6(ui+vi)+4)

	//�Ȳ���map

	double n1 = 14, n2 = 14;  //����uv��������ļ��

	vector<double> PntU,PntV;  //U,V�����ֵ����
	vector<vector<F3dPoint>> PntUVSamp, PntUVSampTarget;  //������Ӧ��,��������ĵ�(����Ŀ���)
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

	double errAllow = 0.01;    //��������������Χ
	   
	for (int i = 0;i < PntU.size();i++)
	{
		for (int j = 0;j < PntV.size();j++)
		{
			pSurf->OnNurbsSurf()->EvNormal(PntU[i], PntV[j], PntUVSamp[i][j], PntUVSampVec[i][j]);
			PntUVSampTarget[i][j] = PntUVSamp[i][j] + 0.05*cos(0.6*(PntU[i] + PntV[j]) + 4)*PntUVSampVec[i][j];

		}
	}


	vector<vector<F3dPoint>> CVNew;  //�µĿ��Ƶ�洢
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
	
	pSurf1 = pSurf;    //��ʼ��������


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
						Rij[k1][k2] = OneRijFun(PntU[k1], PntV[k2], u_degree, v_degree, k1, k2, knotUarray, knotVarray); ////!!!!������
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



	//���У��

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

	cout<<"�㷨���Ե�"<<endl;

}


void LDL(int n, double *a, double *b)//LDL�����������Σ����� ϵ������a ��������b 
{
	double *U = new double[n*n];
	double *x = new double[n];
	double *y = new double[n];
	double *z = new double[n];
	double *L = new double[n*n];
	double *D = new double[n];
	for (int i = 0; i < n; i++)//��LU�����L U
	{
		for (int j = 0; j < n; j++)
		{
			*(U + i * n + j) = 0;//��ʱȫ����ֵΪ0
			if (i == j)
			{
				*(L + i * n + j) = 1;//�Խ��߸�ֵΪ1
			}
			else
			{
				*(L + i * n + j) = 0;//������ʱ��ֵΪ0
			}
		}
	}

	for (int k = 0; k < n; k++)//����u��l�����ֵ
	{
		for (int j = k; j < n; j++)
		{
			*(U + k * n + j) = *(a + k * n + j);//��һ��
			for (int r = 0; r < k; r++)//��������L��ǰһ����u����һ��
			{
				*(U + k * n + j) = *(U + k * n + j) - (*(L + k * n + r)*(*(U + r * n + j)));
			}
		}
		for (int i = k + 1; i < n; i++)//����L����
		{
			*(L + i * n + k) = *(a + i * n + k);
			for (int r = 0; r < k; r++)
			{
				*(L + i * n + k) = *(L + i * n + k) - (*(L + i * n + r)*(*(U + r * n + k)));
			}
			*(L + i * n + k) = *(L + i * n + k) / (*(U + k * n + k));
		}
	}


	for (int i = 0; i < n; i++)//��D��ֵ
	{
		*(D + i) = *(U + i * n + i);
	}



	for (int i = 0; i < n; i++)//��Lz=b��z
	{
		*(z + i) = *(b + i);
		for (int j = 0; j < i; j++)
		{
			*(z + i) = *(z + i) - *(L + i * n + j)*(*(z + j));

		}
	}




	for (int i = 0; i < n; i++)//��y
	{
		*(y + i) = *(z + i) / (*(D + i));
	}

	double *temp = new double[n*n];
	for (int i = 0; i < n; i++)//����ʵ�ֶ�L��ת��
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
	delete[]temp;//�ͷ�

	for (int i = n - 1; i >= 0; i--)//�����x
	{
		*(x + i) = *(y + i);
		for (int j = i + 1; j < n; j++)
		{
			*(x + i) = *(x + i) - *(L + i * n + j)*(*(x + j));
		}
	}
	for (int i = 0; i < n; i++)
	{
		cout << "��Ϊ��\n";
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
*��������OneRijFun
*�������ܣ��������������Rij��ֵ,Ĭ��Ȩ��Ϊ1(�ú�����ΪPSO�㷨����Ӧ�Ⱥ���)
*���������i,j�ֱ�Ϊu,v��Ӧ�Ĳ��������±꣬p,qΪ������m=r-p-1,n=s-q-1,rΪ�ڵ�����U�±�����ֵ(m+1���ڵ�),s��ӦV
*���������
*����ֵ��Rij
*���ú�����OneBasisFun,BasisFuns
**********************************************/
//void FClosedloopManutest::OneBasisFun(int p,int m,vector <double> U,int i,double u,double Nip)
//void FClosedloopManutest::DerOneBasisFun(int p,int m,vector <double> U,int i,double u,int n,double Ders[])
//void FClosedloopManutest::OneRijFun(int i,int j,double u,double v,int p,int q,int m,int n,int r,int s,vector <double> U,vector <double> V,double Rij)
double FToolPathDlg::OneRijFun(double u,double v, int u_degree, int v_degree,int CVi,int CVj, vector<double> knotUarray, vector<double> knotVarray)
{
	double Nup = 0.0, Nvp = 0.0, temp, Rij;

	
	int r, s, m, n, i, j;

	u_degree = u_degree - 1;  //������Ϊ����
	v_degree = v_degree - 1;  //������Ϊ����

	double *Nu = new double[u_degree + 1];
	double *Nv = new double[v_degree + 1];
	double total = 0.0;

	//��������

	r = knotUarray.size() - 1;
	s = knotVarray.size() - 1;

	m = r - u_degree - 1;
	n = s - v_degree - 1;

	i = FindSpan(m, u_degree, u, knotUarray);
	j = FindSpan(n, v_degree, v, knotVarray);
	//��������

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
*��������BasisFuns
*�������ܣ��������з���B������������ֵ
*���������iΪ�ڵ������±�,uΪ����ֵ,pΪ����������,UΪ�ڵ�����
*���������NΪ����B������������ֵ
*����ֵ��
*���ú�����
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
			//cout<<"r��ֵ"<<r<<"N��ֵ"<<N[r]<<endl;
			saved = left[j - r] * temp;
			//cout<<saved<<endl;
		}
		N[j] = saved;
		//cout<<"j��ֵ"<<N[0]<<N[1]<<N[2]<<endl;
	}
	delete[] left;
	delete[] right;
}

/**********************************************
*��������FindSpan
*�������ܣ�ȷ������u���ڵĽڵ�������±꣨���ö��ַ���
*���������n=m-p-1,pΪ����������,uΪ����ֵ,�ڵ�����U={u0,u1,***,um}
*���������
*����ֵ���ڵ�������±�
*���ú�����
**********************************************/
int FToolPathDlg::FindSpan(int n, int p, double u, vector <double> U)
{
	/*if(u==U[n+1]) return(n);          /*�������*/
	/*int low = p;int high = n+1;       /*���ж�������*/
	/*int mid = (low+high)/2;           //�˴���Ҫȷ�϶���Ϊ����int�Ƿ����
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
*��������onToolPath
*�������ܣ�����·���滮���ܵĲۺ���
*���������
*���������
*����ֵ��
*���ú�����
**********************************************/
void FToolPathDlg::onToolPath()
{
	ToolParaPath();
	//ToolPath();
	//DrawToolPath();   //���Ƶ���·��
	//clsExp();
	//FBSampDis();
}

/**********************************************
*��������onSurfaceRecons
*�������ܣ������ع��ۺ���
*���������
*���������
*����ֵ��
*���ú�����
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
*��������ParaCspline_interpolation
*�������ܣ���������B������ֵ����u,v���ϵĲ�ֵ���������������Ĳ�������·����
*����������������ֵ������ֵ
*���������������Ӧ�ĵ������ֵ���Լ�һ�׵��������׵���
*����ֵ��
*���ú�����
**********************************************/
void FToolPathDlg::Interpolationcurve_Calcu(double Pt, double *PtV, double *dPt, double *ddPt)
{
	int ith = 0;

	//Ѱ��Pt���ڵĶ�
	while (ith < m_tPntV.size() - 1)
	{
		if ((Pt >= m_tPntV[ith]) && (Pt < m_tPntV[ith + 1]))
			break;

		ith++;
	}
	//�ֱ�����Ӧ����������ֵ��һ�׵���ֵ�����׵���ֵ
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

	cout << "ith��ֵ" << ith << "m_tPntV[ith]��ֵ" << m_tPntV[ith] << "m_tPntV[ith+1]��ֵ" << m_tPntV[ith + 1] << endl;
}

/**********************************************
*��������ParaCspline_interpolation
*�������ܣ���������B������ֵ����u,v���ϵĲ�ֵ���������������Ĳ�������·����
*����������������ֵ������ֵ
*���������������Ӧ�ĵ������ֵ���Լ�һ�׵��������׵���
*����ֵ��
*���ú�����
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
	//����������ѡ��t�������ۻ��ҳ�����������

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

	//��Ȼ�߽������Ĳ�ֵ,���˵�ĵ���ֵΪdf1,df2����Ϊ0;

	//��0-n-1���������䳤��h��ֵ
	for (int j = 0;j < m_Currentpathpara[0].size() - 1;j++)
	{
		m_hV.push_back(m_tPntV[j + 1] - m_tPntV[j]);
	}

	//��1-n��u��ֵ
	//��0-n-1��lambda��ֵ

	lambdaV.push_back(0);
	for (int j = 1;j < m_Currentpathpara[0].size() - 1;j++)
	{
		uV.push_back(m_hV[j - 1] / (m_hV[j - 1] + m_hV[j]));
		lambdaV.push_back(1 - uV.back());
	}
	//uV.erase(uV.begin());
	uV.push_back(0);

	//�󷽳����Ҳ��ֵdV
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

	//����������ÿ��Ԫ�ظ�ֵ2
	for (int j = 0;j < MdiagnalV.size();j++)
	{
		MdiagnalV[j] = 2;
	}

	//׷�Ϸ���m_vecM

	BetaV.push_back(lambdaV[0] / MdiagnalV[0]);

	for (int j = 1;j < m_Currentpathpara[0].size() - 1;j++)
	{
		BetaV.push_back(lambdaV[j - 1] / (MdiagnalV[j] - uV[j - 1] * BetaV.back()));
	}

	//���м䷽�̣�׷�Ĺ��̣�
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

	//�����շ��̣��ϵĹ��̣�
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
*��������ToolParaPath()
*�������ܣ������߷�������·���滮
*���������
*���������
*����ֵ��
*���ú�����
**********************************************/
void FToolPathDlg::ToolParaPath()
{
	//�������ʼ������

	
	int kNum = 100;  //�����ܻ�ΪkNum��(���ڿɰ��վ�����Ҫ���Ƹ�ֵ)
	vector<double> uSigPara;    //����·���Ĳ���ֵ
	vector<double> uPara,vPara;   //u��v����Ĳ���ֵ

	double Step_size, Parameter_increment;
	double E, F, G, L, M, N;
	gp_Vec Su, Sv, Suu, Suv, Svv, Nvector;
	gp_Pnt TempPnt;
	double base1Q, base2Q;
	double a, g;
	double dut = 1.0, dvt = 0.0;   //u,v�ֱ��t�ĵ��� ���㷨��Ϊÿ��·������u������ ��˳�ֵ����Ҫ�޸�
	double curvature;
	double CurvatureRadius;
	double SafeDistance;
	double Der_v;   //v����ƫ�õĲ�������
	vector<double> tempDer_v;
	vector<vector<vector<double>>> uvPntPara;
	vector<vector<gp_Pnt>> CCPnt, CLPnt;  //�ֱ�Ϊ�����㡢��λ�㡢���߷�������
	vector<vector<gp_Vec>> CLPntVec;
	

	m_R = ToolRadius->text().toDouble();
	m_e = StepPres->text().toDouble();
	m_h = ResidualHeight->text().toDouble();
	SafeDistance = SafeDis->text().toDouble();   //���������ĵ�·����ο�����

	ofstream fout("testPath.txt");
	//�������ʼ������
	//test();
	//u�������·���滮(���ڿ����޸�Ϊ��ѡ����ʼλ��)
	//ȷ��v��������ߵĲ���
	vPara.push_back(0);  //��v=0��ʼ·���滮
	
	while (1)
	{
		//������vԽ�������߽�Ľ��ޣ�����v=1
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
				for (int i = 0;i < 5;i++)               //������˵ʵ������5��֮�ھͻ�����
				{
					a = sqrt(pow(m_R, 2) - pow(sqrt(pow(CurvatureRadius + m_R, 2) - pow(a, 2)) - CurvatureRadius - m_h, 2));
				}
				g = 2 * a / (1 + m_R / CurvatureRadius);

			}
			else
			{
				a = sqrt(pow(m_R, 2) - pow(m_R - m_h, 2));
				for (int i = 0;i < 5;i++)               //������˵ʵ������5��֮�ھͻ�����
				{
					a = sqrt(pow(m_R, 2) - pow(sqrt(pow(CurvatureRadius - m_R, 2) - pow(a, 2)) - CurvatureRadius + m_h, 2));
				}
				g = 2 * a / (1 - m_R / CurvatureRadius);
			}

			Der_v = g * (E * dut + F * dvt) / (sqrt(E * G - F * F) * sqrt(E * dut * dut + 2 * F * dut * dvt + G * dvt * dvt));
			tempDer_v.push_back(Der_v);

		}

		//��Der_v����Сֵ��Ϊ�о�
		vector<double>::iterator minDer_v = min_element(tempDer_v.begin(), tempDer_v.end());
		vPara.push_back(*minDer_v + vPara.back());
	}

	uvPntPara.resize(vPara.size());

	//����·����ɢ��
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
			Parameter_increment = Step_size / sqrt(Su.X()* Su.X()+ Su.Y()* Su.Y()+ Su.Z()* Su.Z())+ uSigPara.back();   //��õ����ĵ�
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

	//��λ��任�����
	CCPnt.resize(uvPntPara.size());  //�����㳤��
	CLPnt.resize(uvPntPara.size());  //��λ�㳤��
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

			//����ƫ��CC��һ����ͷ���뾶�õ�CL��
			CLPnt[j][i].SetX(CCPnt[j][i].X() + m_R * Nvector.X());
			CLPnt[j][i].SetY(CCPnt[j][i].Y() + m_R * Nvector.Y());
			CLPnt[j][i].SetZ(CCPnt[j][i].Z() + m_R * Nvector.Z());

		}
	}

	//CL·������(Ϊ��λ�㼯��ӿ����г̵�λ��)
	gp_Pnt AuxPnt;
	gp_Vec TempXYZ;

	for (int j = 0;j < CLPnt.size();j++)
	{
		//ÿ��·�����г̵�λȷ��(·���׵�)
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

		CLPntVec[j].insert(CLPntVec[j].begin(), TempXYZ);  //·�����г�ʱ���ʼ���빤�����߷�����ͬ

		//ÿ��·�����г̵�λȷ��(·��ĩ��)
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

		CLPntVec[j].insert(CLPntVec[j].end(), TempXYZ);  //·�����г�ʱ���г��������߷�����ͬ
	
	}

	//תΪ1ά�����ĵ���·��("֮"����·�߽���)
	vector<gp_Pnt> CLPntLine;
	vector<gp_Vec> CLPntVecLine;

	for (int j = 0;j < CLPnt.size();j++)
	{
		if (j % 2 == 0) //����������·
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

	//���Ƶ���·��
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

	//�����ƶ�������ʾ
	//ToolMoveView(CLPntLine, CLPntVecLine);
	m_CLPntLine.resize(CLPntLine.size());
	m_CLPntVecLine.resize(CLPntVecLine.size());
	for (int i = 0;i < CLPntLine.size();i++)
	{
		m_CLPntLine[i] = CLPntLine[i];
		m_CLPntVecLine[i] = CLPntVecLine[i];
	}

	//gWindow->GetMainDoc()->AddDispEnt(ToolView);

	//���ڿ���������Ƶ��᷽��
	


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
*��������ToolMoveView(vector<gp_Pnt> CLPntLine, vector<gp_Vec> CLPntVecLine)
*�������ܣ������ƶ�����
*���������
*���������
*����ֵ��
*���ú�����
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
*��������NurbsToOcctSurf(const ON_NurbsSurface& theSurface)
*�������ܣ�ON_NurbsSurfaceת��ΪOCCT����
*���������const ON_NurbsSurface& theSurface
*���������
*����ֵ��Handle(Geom_BSplineSurface)
*���ú�����
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



//��ʱ���Ժ���
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
*��������ToolPath
*�������ܣ��Ȳиߵ���·���滮����
*���������
*���������
*����ֵ��
*���ú�����
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
	//int ith=0;   //ithΪ��·������
	vector<int> ithSize;  //��i�е���·���ϵ�λ�������

	//vector<vector<double>> Nextpath_para;
	//vector<vector<double>> Totalpath_para;

	vector<double> tincrementV, tPathV;    ////tPathV�洢��ǰ������tֵ
	vector<double> tV;    //�洢��ǰ������tֵ

	m_Currentpathpara.resize(2);
	m_Nextpathpara.resize(2);
	m_Totalpathpara.resize(2);
	//Totalpath_para.resize(2);

	double dut, dvt;   //u,v�ֱ��t�ĵ���(��Ҫ��ֵ)

	//m_e = 0.01;  //�����и��ʲ�
	//m_h = 0.1;   //����߶��ʲ�
	//m_R = 4;     //��ͷ���߰뾶����

	m_R = ToolRadius->text().toDouble();
	m_e = StepPres->text().toDouble();
	m_h = ResidualHeight->text().toDouble();
	SafeDistance = SafeDis->text().toDouble();   //���������ĵ�·����ο�����

	m_uTpath = 0.0;
	m_vTpath = 0.0;
	dut = 1.0;	dvt = 0.0;    //��һ��·���󵼵ĳ�ʼ����ѡu��������ߣ���v=0���������ߣ�
	double t, t_increment;
	double Pt[2], dPt[2], ddPt[2];  //u,vƽ�����������ĵ㡢һ�׵��������׵���
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

	//����Ϊ��ʼ������


	//��һ��·���滮��ѡu��������ߣ���v=0���������ߣ�

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

	ithSize.push_back(m_Currentpathpara[0].size());   //����i��·���ĳ��ȴ洢
	m_Totalpathpara[0].insert(m_Totalpathpara[0].end(), m_Currentpathpara[0].begin(), m_Currentpathpara[0].end());
	m_Totalpathpara[1].insert(m_Totalpathpara[1].end(), m_Currentpathpara[1].begin(), m_Currentpathpara[1].end());

	//��һ��·����ɢ��
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
			for (int i = 0;i < 5;i++)               //������˵ʵ������5��֮�ھͻ�����
			{
				a = sqrt(pow(m_R, 2) - pow(sqrt(pow(1 / curvature + m_R, 2) - pow(a, 2)) - 1 / curvature - m_h, 2));
			}
			g = 2 * a / (1 + m_R * curvature);

		}
		else
		{
			a = sqrt(pow(m_R, 2) - pow(m_R - m_h, 2));
			for (int i = 0;i < 5;i++)               //������˵ʵ������5��֮�ھͻ�����
			{
				a = sqrt(pow(m_R, 2) - pow(sqrt(pow(1 / curvature - m_R, 2) - pow(a, 2)) - 1 / curvature + m_h, 2));
			}
			g = 2 * a / (1 - m_R * curvature);
		}

		//��һ��·����ɢ��
		Der_u = g * (F*dut + G * dvt) / (sqrt(E*G - F * F)*sqrt(E*dut*dut + 2 * F*dut*dvt + G * dvt*dvt));
		Der_v = g * (E*dut + F * dvt) / (sqrt(E*G - F * F)*sqrt(E*dut*dut + 2 * F*dut*dvt + G * dvt*dvt));     //��һ�����ŵ����⣬�����ٴ���

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
	//�޸�
	//�׸����ȷ��
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

	//ĩβ���ȷ��
	*/


	/*���ע�ͣ�
	������ע��֮ǰ������Ϊѡȡ����v=0������������Ϊ��ʼ·�������ڴ�·��������ȷ���˵ڶ���·������ɢ���ֵ��
	������Ҫ��ǰ������ݽ����޸����ܹ����û��Ե�һ��·������ѡȡ��
	������ע��֮�������Ϊһ����ѭ��������Ϊ�������������ɢ�ĵ㣬Ȼ��������ɢ���ɵ�ǰ·����������ɢ�����
	�õ���һ��·����ɢ���ֵ���ж���һ��·����ɢ���ֵ�Ƿ񶼳����߽磬�������������ֹͣѭ�������������������ѭ����
	*/

	int count = 0;

	while (1)
	{

		count++;
		if (count > 4)
			break;

		//����㣬�ж��Ƿ�Խ�磬Ȼ��Ե��������

		m_Currentpathpara[0].swap(vector<double>());
		m_Currentpathpara[1].swap(vector<double>());

		for (int i = 0;i < m_Nextpathpara[0].size();i++)
		{
			m_Currentpathpara[0].push_back(m_Nextpathpara[0][i]);
			m_Currentpathpara[1].push_back(m_Nextpathpara[1][i]);

		}

		m_Nextpathpara[0].swap(vector<double>());
		m_Nextpathpara[1].swap(vector<double>());




		//�޼���������һ��·��
		//��ɾu��0�����ĵ�
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
					cout << "��δ֪����" << endl;
					break;
				}
			}
		}


		//��ɾu��1�����ĵ�
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
					cout << "��δ֪����" << endl;
					break;
				}
			}
		}

		//ɾ��u�����Ų����淶���²��ܽϺò�ֵ�ĵ㣬ʹ�䰴��С����,�Լ��������������
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

		//���ϻ���Ҫ��һ������,ϸ��

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



		//��һ��·�����
		m_tPntV.swap(vector<double>());
		m_vecM.resize(m_Currentpathpara.size());
		m_vecM[0].swap(vector<double>());
		m_vecM[1].swap(vector<double>());

		m_hV.swap(vector<double>());
		ParaCspline_interpolation();

		//����������ɢ·��
		//��յ�ǰ·���ڴ�
		//m_Currentpathpara[0].swap(vector<double>());
		//m_Currentpathpara[1].swap(vector<double>());
		
		//��ֵ���·���滮����ֵ���ߵ���ɢ��
			   
		//Interpolationcurve_Calcu(0.1,Pt,dPt,ddPt);  //���Բ���

		//ɾ��u�����Ų����淶���²��ܽϺò�ֵ�ĵ㣬ʹ�䰴��С����,�Լ��������������

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
			//curvatureV.push_back(1/curvature);   //���ʰ뾶����
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
		
		ithSize.push_back(m_Currentpathpara[0].size());   //����i��·���ĳ��ȴ洢
		m_Totalpathpara[0].insert(m_Totalpathpara[0].end(), m_Currentpathpara[0].begin(), m_Currentpathpara[0].end());
		m_Totalpathpara[1].insert(m_Totalpathpara[1].end(), m_Currentpathpara[1].begin(), m_Currentpathpara[1].end());

		m_Nextpathpara[0].swap(vector<double>());
		m_Nextpathpara[1].swap(vector<double>());


		//��һ��·����ɢ��
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
				for (int i = 0;i < 5;i++)               //������˵ʵ������5��֮�ھͻ�����
				{
					a = sqrt(pow(m_R, 2) - pow(sqrt(pow(CurvatureRadius + m_R, 2) - pow(a, 2)) - CurvatureRadius - m_h, 2));
				}
				g = 2 * a / (1 + m_R / CurvatureRadius);

			}
			else
			{
				a = sqrt(pow(m_R, 2) - pow(m_R - m_h, 2));
				for (int i = 0;i < 5;i++)               //������˵ʵ������5��֮�ھͻ�����
				{
					a = sqrt(pow(m_R, 2) - pow(sqrt(pow(CurvatureRadius - m_R, 2) - pow(a, 2)) - CurvatureRadius + m_h, 2));
				}
				g = 2 * a / (1 - m_R / CurvatureRadius);
			}

			//��һ��·����ɢ��
			Der_u = -g * (F*dut + G * dvt) / (sqrt(E*G - F * F)*sqrt(E*dut*dut + 2 * F*dut*dvt + G * dvt*dvt));
			Der_v = g * (E*dut + F * dvt) / (sqrt(E*G - F * F)*sqrt(E*dut*dut + 2 * F*dut*dvt + G * dvt*dvt));     //��һ�����ŵ����⣬�����ٴ���

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
	   
	//תΪCL��·
	
	
	for (int k = 0;k < m_CCPnt.size();k++)
	{
		//m_CLPnt[k].x = m_CCPnt[k].x + m_R * m_Nvector[k].x;
		//m_CLPnt[k].y = m_CCPnt[k].y + m_R * m_Nvector[k].y;
		//m_CLPnt[k].z = m_CCPnt[k].z + m_R * m_Nvector[k].z;
		m_CLPnt.push_back(m_CCPnt[k] + m_R * m_Nvector[k]);
	}

	//�����ֲ��뵶λ���������ֵĵ�λ���Ƶ㲢��ʾ
	int tempcount = 0;

	for (int i = 0;i < ithSize.size();i++)
	{
		//m_Totalpathpara
		//m_CCPnt
		//m_Nvector

		double temp1;
		F3dPoint FirstPnt,LastPnt;
		FIpPoint temp;   //�м���

		vector<vector<double>> tempTotalpathpara;
		vector<F3dPoint> tempCCPnt, tempCLPnt;
		vector <F3dVector> tempNvector;

		tempTotalpathpara.resize(2);
		tempTotalpathpara[0].swap(vector<double>());
		tempTotalpathpara[1].swap(vector<double>());
		tempCCPnt.swap(vector<F3dPoint>());
		tempCLPnt.swap(vector<F3dPoint>());
		tempNvector.swap(vector <F3dVector>());		

		if (i % 2 == 1)   //��·����
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
*��������DrawToolPath
*�������ܣ����Ƶ���·��
*���������
*���������
*����ֵ��
*���ú�����
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
*��������ToQvec
*�������ܣ���������ת��
*���������
*���������
*����ֵ��
*���ú�����
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
*��������clsExp()
*�������ܣ����cls�ļ�
*���������
*���������
*����ֵ��
*���ú�����
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
	inFile.close();//�ر��ļ�
	m_clsExp.swap(vector<QString>());




}

//���߻��ƺ���
void ToolSpec::Draw(const FT::FRenderOpt& renderOpt)
{

	//������ͷ������ͷ����
	//��������Ϊ��x��y��z������İ뾶Ϊradius��M��N�ֱ��ʾ����ĺ����򱻷ֳɶ��ٷ�
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
	////������ת����(Z���뷽�������Ĳ��)

	double CrossMx = -ny, CrossMy = nx, CrossMz = 0;
	//��׼��
	CrossMx = CrossMx / sqrt(CrossMx * CrossMx + CrossMy * CrossMy + CrossMz * CrossMz);
	CrossMy = CrossMy / sqrt(CrossMx * CrossMx + CrossMy * CrossMy + CrossMz * CrossMz);
	CrossMz = CrossMz / sqrt(CrossMx * CrossMx + CrossMy * CrossMy + CrossMz * CrossMz);

	double angle = acos(nz) * 180.0 / 3.14159;

	glTranslatef(xx, yy, zz);
	glRotatef(angle, CrossMx, CrossMy, CrossMz);
	
	glBegin(GL_QUAD_STRIP);   //��������ı��δ�
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

//���Ƶ��ߵĵ��涥��Բ
void ToolSpec::drawCircle(double radius, double Length)
{
	////������ת����(Z���뷽�������Ĳ��)

	//double CrossMx = -ny, CrossMy = nx, CrossMz = 0;
	////��׼��
	//CrossMx = CrossMx / sqrt(CrossMx * CrossMx + CrossMy * CrossMy + CrossMz * CrossMz);
	//CrossMy = CrossMy / sqrt(CrossMx * CrossMx + CrossMy * CrossMy + CrossMz * CrossMz);
	//CrossMz = CrossMz / sqrt(CrossMx * CrossMx + CrossMy * CrossMy + CrossMz * CrossMz);

	
	glColor3f(0, 0, 1);
	//glTranslatef(0, 0, Length);
	//glTranslatef(xx, yy, zz);
	//glRotatef(40, CrossMx, CrossMy, CrossMz);
	glBegin(GL_TRIANGLE_FAN);     //����������������δ�
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
