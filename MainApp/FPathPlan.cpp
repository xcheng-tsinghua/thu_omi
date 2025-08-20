//#include <conio.h>
//#include "MainDoc.h"
#include "PathPlan/Reader/io.h"
#include "PathPlan/TSPSOLVER/LKH/SRC/LKHSolver.h"
#include "../External/fkernel/Include/DataStructure/FMesh.h"
#include "../External/fkernel/Include/Features/FPlane.h"
#include "FGuideDataType.h"
#include "FPathPlan.h"
#include "FMainWindow.h"
#include "np.h"
#include "occConverter.h"
//#include "FPathPlanAndExeDlg.h"

extern FMainWindow* gWindow;
//using namespace std;

void FPathPlan::Init()
{
	m_backDist = 6;		//mm
	m_safeDist = 20;	//mm
	m_is_fast_path_plan = false;
	//m_pObstacleMesh = NULL;
	m_pDoc = NULL;
	m_is_fast_collision_judge = true;
	mp_progress = NULL;

}

FPathPlan::FPathPlan()
{
	Init();
}

FPathPlan::FPathPlan(FMainDoc* pDoc){
	Init();
	m_pDoc = pDoc;
//	m_ipPntList = m_pDoc->GetIpPntList();
}

FPathPlan::~FPathPlan()
{
	
}

void FPathPlan::ClearPnts()
{
	m_ipPntList.clear();
}

bool FPathPlan::Planning(FIntList& orders, FIpPathList& ipPathList)
{
	// ��������0ʱ��ִ�й滮
	if (m_ipPntList.size() == 0) return false;

	//step1. write m_ipPntList to problem file 
	WriteTSPProblemFile();

	//step2. basic path without interference check
	if(!BasicPath(orders)) return false;

	// progress
	mp_progress->setValue(30);

	//step3. compute collision-free path
	FIpPntList tIpPntList;
	for (int i = 0; i < orders.size(); i++)
	{
		//index of basic path start from 1
		tIpPntList.push_back(m_ipPntList[orders[i] - 1]);
	}

	//if (m_pObstacleMesh)
	if (gWindow->GetOccConverter())
	{
		if (m_is_fast_collision_judge)
		{
			//cout << "---------ʹ�ÿ��ٸ������㷨-------" << endl;
			ComputeNoCollisionPathPrev(tIpPntList, ipPathList);
		}
		else
		{
			//cout << "---------ʹ�þ�ȷ�������㷨-------" << endl;
			ComputeNoCollisionPath(tIpPntList, ipPathList);
		}
		
	}
	else
	{
		MesgBox(QString::fromLocal8Bit("δ����ģ��"));
		return false;

		FIpPath tPath;
		for (int i=0; i<orders.size(); i++)
		{
			tPath.AddPnt(m_ipPntList[orders[i] - 1]);
		}
		m_pDoc->ClearIpPathList();
		m_pDoc->AddIpPath(tPath);
	}

	return true;
}

bool FPathPlan::Planning_FDA(FIntList& orders, FIpPntList & IpPntList)   //added by lym.
{
	FIpPntList IN_Path;
	if (m_ipPntList.size()==0)   //if there are no points, return false
		return false;
	if (m_ipPntList.size()>2)   //if there are more than three points, use the TSP algorithm
	{
		//step1. write m_ipPntList to problem file 
		WriteTSPProblemFile();
		//step2. basic path without interference check

		if(!BasicPath(orders))
			return false;

		//step3. compute collision-free path

		FIpPntList TP_PntList;
		for (int i=0; i<orders.size(); i++)	//index of basic path start from 1
			TP_PntList.push_back(m_ipPntList[orders[i]-1]);

		for (int i=0;i<TP_PntList.size();i++)
		{
			IpPntList.push_back(TP_PntList[i]);
		}	
		return true;
	}
	else if(m_ipPntList.size()==2)   //if there are only two points, 
	{
		IpPntList.push_back(m_ipPntList[0]);
		IpPntList.push_back(m_ipPntList[1]);
		return true;
	}
	else if(m_ipPntList.size()==1)  //if there is only one point.
	{
		IpPntList.push_back(m_ipPntList[0]);
		return true;
	}

}

bool FPathPlan::PathPlanGivenOrder(FIntList& orders, FIpPntList& IpPntList)
{
	FIpPntList IN_Path;
	if (m_ipPntList.size() == 0)   //if there are no points, return false
		return false;
	if (m_ipPntList.size() > 2)   //if there are more than three points, use the TSP algorithm
	{
		//step1. write m_ipPntList to problem file 
		WriteTSPProblemFile();

		//step3. compute collision-free path

		FIpPntList TP_PntList;
		for (int i = 0; i < orders.size(); i++)	//index of basic path start from 1
			TP_PntList.push_back(m_ipPntList[orders[i] - 1]);

		for (int i = 0; i < TP_PntList.size(); i++)
		{
			IpPntList.push_back(TP_PntList[i]);
		}
		return true;
	}
	else if (m_ipPntList.size() == 2)   //if there are only two points, 
	{
		IpPntList.push_back(m_ipPntList[0]);
		IpPntList.push_back(m_ipPntList[1]);
		return true;
	}
	else if (m_ipPntList.size() == 1)  //if there is only one point.
	{
		IpPntList.push_back(m_ipPntList[0]);
		return true;
	}
}

void FPathPlan::ComputeNoCollisionPath(FIpPntList& ipPntList, FIpPathList& pathlist)
{
	if (!gWindow->GetOccConverter()) return;

	int nsize = ipPntList.size();
	double prob_radius = 0.5 * gWindow->GetMainDoc()->getProbeDia();  // ����챦ʯ��뾶

	if (abs(prob_radius) > 100)
	{
		QMessageBox::warning(QApplication::activeWindow(), QString::fromLocal8Bit("���󣡣���"), QString::fromLocal8Bit("δ�趨����챦ʯ��ֱ��������"), QMessageBox::Ok | QMessageBox::Cancel | QMessageBox::Abort);

		double probe_diameter = QInputDialog::getDouble(0, QString::fromLocal8Bit("�������챦ʯ��ֱ��"), QString::fromLocal8Bit("����챦ʯ��ֱ��:"), 6, 0, 100, 5);
		gWindow->GetMainDoc()->setProbeDia(probe_diameter);

		prob_radius = probe_diameter * 0.5;
	}

	// ��Ҫ�ж���ײ���ܴ����� n + (n - 1)
	int cProgress = mp_progress->value();
	double cColli = 0;
	double nColliJudge = nsize + (nsize - 1);

	//step1. compute back points
	FIpPntList backStartList;
	FIpPntList backPntList;
	double xx(0), yy(0), zz(0);
	double xs(0), ys(0), zs(0);
	for (int i=0; i<nsize; i++)
	{
		xs = ipPntList[i].x + prob_radius * ipPntList[i].i;
		ys = ipPntList[i].y + prob_radius * ipPntList[i].j;
		zs = ipPntList[i].z + prob_radius * ipPntList[i].k;
		backStartList.push_back(FIpPoint(xs, ys, zs));

		xx = ipPntList[i].x + m_backDist * ipPntList[i].i;
		yy = ipPntList[i].y + m_backDist * ipPntList[i].j;
		zz = ipPntList[i].z + m_backDist * ipPntList[i].k;
		backPntList.push_back(FIpPoint(xx, yy, zz));
	}

	//step2. �����˹������Ƿ�ᷢ����ײ
	F3dPoint sPnt, ePnt;
	F3dPointList resPnts;
	for (int i=0; i<nsize; i++)
	{
		resPnts.clear();
		sPnt.Set(backStartList[i].x, backStartList[i].y, backStartList[i].z);
		ePnt.Set(backPntList[i].x, backPntList[i].y, backPntList[i].z);

		// �����ƫ��0.1mm����ֹ��Ϊ����㴦·����ģ���ཻ
		F3dVector sToEnd = ePnt - sPnt;
		sToEnd /= sToEnd.Length();
		sPnt = sPnt + 0.1 * sToEnd;

		if (isIntersectToModel(GP_PNT(sPnt), GP_PNT(ePnt), prob_radius))
		{
			QMessageBox::warning(QApplication::activeWindow(), QObject::tr("Caution"), QObject::tr("Back point collide with part!"));
			return;
		}

		// progress
		mp_progress->setValue(int(cProgress + (++cColli / nColliJudge) * 60));

	}

	//step3. check and handle collision of neighboring inspection points
	//F3dPoint tPnt = m_pObstacleMesh->BoundingBox().m_max;	//when the part is import, it must be upward
	F3dPoint minCorner(0, 0, 0);
	F3dPoint tPnt(0, 0, 0);
	gWindow->GetOccConverter()->GetMaxMinCorner(tPnt, minCorner);
	tPnt.z += m_safeDist;	//safe plane is 10mm higher than top face of bounding box/////////////////
	FPlane safePlane(tPnt, F3dVector(0, 0, 1));//////////////////////////////////////
	F3dPoint tPnt1, tPnt2;
	ON_Line tLine;
	double u(0);	//parameter value of line-plane intersection

	for (int i = 0; i < nsize - 1; i++)
	{
		resPnts.clear();
		tPnt1.Set(backPntList[i].x, backPntList[i].y, backPntList[i].z);
		tPnt2.Set(backPntList[i + 1].x, backPntList[i + 1].y, backPntList[i + 1].z);
		//if (gCalLineMeshIntersection(m_pObstacleMesh, tPnt1, tPnt2, resPnts, 0.1,0.1))
		if (isIntersectToModel(GP_PNT(tPnt1), GP_PNT(tPnt2), prob_radius))
		{
			FIpPath tPath;

			if (i == 0)//added by chengxi���ȰѴӵ�һ����Ļ��˵㵽��һ�����·�����룬��֤���һ����ʱ�����߷���ӽ�
			{
				tLine.from = ON_3dPoint(backPntList[0].x, backPntList[0].y, backPntList[0].z);
				tLine.to = ON_3dPoint(backPntList[0].x, backPntList[0].y, backPntList[0].z) + F3dPoint(0, 0, 10);
				gCalPlaneLineIntersect(&safePlane, &tLine, u);
				tPnt = tLine.PointAt(u);

				tPath.AddPnt(FIpPoint(tPnt.x, tPnt.y, tPnt.z));//���ƶ�����ȫƽ�棬���ƶ�����һ����Ļ��˵�
				tPath.AddPnt(backPntList[0]);
			}

			tLine.from = tPnt1;	tLine.to = tPnt1 + F3dPoint(0,0,10);///////////////////////·����������ֻ��ת����ͬһ����ȫƽ��tLine.from = tPnt1;	tLine.to = tPnt1 + F3dPoint(0,0,10);
			tPath.AddPnt(ipPntList[i]); 
			tPath.AddPnt(backPntList[i]);	
			gCalPlaneLineIntersect(&safePlane, &tLine, u);
			tPnt = tLine.PointAt(u);
			tPath.AddPnt(FIpPoint(tPnt.x, tPnt.y, tPnt.z)); 
			tLine.from = tPnt2;	tLine.to = tPnt2 + F3dPoint(0,0,10);////////////////////////////////
			gCalPlaneLineIntersect(&safePlane, &tLine, u);
			tPnt = tLine.PointAt(u);			
			tPath.AddPnt(FIpPoint(tPnt.x, tPnt.y, tPnt.z));
			tPath.AddPnt(backPntList[i+1]);
			tPath.AddPnt(ipPntList[i+1]);
			pathlist.push_back(tPath);
		}
		else
		{
			FIpPath tPath;

			if (i == 0)//added by chengxi���ȰѴӵ�һ����Ļ��˵㵽��һ�����·�����룬��֤���һ����ʱ�����߷���ӽ�
			{
				tLine.from = ON_3dPoint(backPntList[0].x, backPntList[0].y, backPntList[0].z);
				tLine.to = ON_3dPoint(backPntList[0].x, backPntList[0].y, backPntList[0].z) + F3dPoint(0, 0, 10);
				gCalPlaneLineIntersect(&safePlane, &tLine, u);
				tPnt = tLine.PointAt(u);

				tPath.AddPnt(FIpPoint(tPnt.x, tPnt.y, tPnt.z));//���ƶ�����ȫƽ�棬���ƶ�����һ����Ļ��˵�
				tPath.AddPnt(backPntList[0]);
			}

			tPath.AddPnt(ipPntList[i]); 
			tPath.AddPnt(backPntList[i]);
			tPath.AddPnt(backPntList[i+1]);
			tPath.AddPnt(ipPntList[i+1]);
			pathlist.push_back(tPath);
		}

		// progress
		mp_progress->setValue(int(cProgress + (++cColli / nColliJudge) * 60));

	}

}

void FPathPlan::ComputeNoCollisionPathPrev(FIpPntList& ipPntList, FIpPathList& pathlist)
{
	ON_Mesh* m_pObstacleMesh = gWindow->GetOccConverter()->GetModelMesh();

	if (!m_pObstacleMesh) return;
	int nsize = ipPntList.size();

	// ��Ҫ�ж���ײ���ܴ����� n + (n - 1)
	int cProgress = mp_progress->value();
	double cColli = 0;
	double nColliJudge = nsize + (nsize - 1);

	//step1. ������˵�
	FIpPntList backPntList;
	double xx(0), yy(0), zz(0);
	for (int i = 0; i < nsize; i++)
	{
		xx = ipPntList[i].x + m_backDist * ipPntList[i].i;
		yy = ipPntList[i].y + m_backDist * ipPntList[i].j;
		zz = ipPntList[i].z + m_backDist * ipPntList[i].k;
		backPntList.push_back(FIpPoint(xx, yy, zz));
	}

	//step2. �����˹������Ƿ��и���
	F3dPoint sPnt, ePnt;
	F3dPointList resPnts;
	for (int i = 0; i < nsize; i++)
	{
		resPnts.clear();
		sPnt.Set(ipPntList[i].x, ipPntList[i].y, ipPntList[i].z);
		ePnt.Set(backPntList[i].x, backPntList[i].y, backPntList[i].z);
		if (gCalLineMeshIntersection(m_pObstacleMesh, sPnt, ePnt, resPnts, 0.1, 0.0))
		{
			QMessageBox::warning(QApplication::activeWindow(), QObject::tr("Caution"), QObject::tr("Back point collide with part!"));
			return;
		}

		// progress
		mp_progress->setValue(int(cProgress + (++cColli / nColliJudge) * 60));

	}

	//step3. ���ʵ��·����ģ�͵ĸ��棬������
	F3dPoint tPnt = m_pObstacleMesh->BoundingBox().m_max;	//when the part is import, it must be upward
	tPnt.z += m_safeDist;	//safe plane is 10mm higher than top face of bounding box/////////////////
	FPlane safePlane(tPnt, F3dVector(0, 0, 1));//////////////////////////////////////
	F3dPoint tPnt1, tPnt2;
	ON_Line tLine;
	double u(0);	//parameter value of line-plane intersection	
	for (int i = 0; i < nsize - 1; i++)
	{
		resPnts.clear();
		tPnt1.Set(backPntList[i].x, backPntList[i].y, backPntList[i].z);
		tPnt2.Set(backPntList[i + 1].x, backPntList[i + 1].y, backPntList[i + 1].z);
		if (gCalLineMeshIntersection(m_pObstacleMesh, tPnt1, tPnt2, resPnts, 0.1, 0.1))
		{
			FIpPath tPath;

			if (i == 0)//added by chengxi���ȰѴӵ�һ����Ļ��˵㵽��һ�����·�����룬��֤���һ����ʱ�����߷���ӽ�
			{
				tLine.from = ON_3dPoint(backPntList[0].x, backPntList[0].y, backPntList[0].z);
				tLine.to = ON_3dPoint(backPntList[0].x, backPntList[0].y, backPntList[0].z) + F3dPoint(0, 0, 10);
				gCalPlaneLineIntersect(&safePlane, &tLine, u);
				tPnt = tLine.PointAt(u);

				tPath.AddPnt(FIpPoint(tPnt.x, tPnt.y, tPnt.z));//���ƶ�����ȫƽ�棬���ƶ�����һ����Ļ��˵�
				tPath.AddPnt(backPntList[0]);
			}

			tLine.from = tPnt1;	tLine.to = tPnt1 + F3dPoint(0, 0, 10);///////////////////////·����������ֻ��ת����ͬһ����ȫƽ��tLine.from = tPnt1;	tLine.to = tPnt1 + F3dPoint(0,0,10);
			tPath.AddPnt(ipPntList[i]);
			tPath.AddPnt(backPntList[i]);
			gCalPlaneLineIntersect(&safePlane, &tLine, u);
			tPnt = tLine.PointAt(u);
			tPath.AddPnt(FIpPoint(tPnt.x, tPnt.y, tPnt.z));
			tLine.from = tPnt2;	tLine.to = tPnt2 + F3dPoint(0, 0, 10);////////////////////////////////
			gCalPlaneLineIntersect(&safePlane, &tLine, u);
			tPnt = tLine.PointAt(u);
			tPath.AddPnt(FIpPoint(tPnt.x, tPnt.y, tPnt.z));
			tPath.AddPnt(backPntList[i + 1]);
			tPath.AddPnt(ipPntList[i + 1]);
			pathlist.push_back(tPath);
		}
		else
		{
			FIpPath tPath;

			if (i == 0)//added by chengxi���ȰѴӵ�һ����Ļ��˵㵽��һ�����·�����룬��֤���һ����ʱ�����߷���ӽ�
			{
				tLine.from = ON_3dPoint(backPntList[0].x, backPntList[0].y, backPntList[0].z);
				tLine.to = ON_3dPoint(backPntList[0].x, backPntList[0].y, backPntList[0].z) + F3dPoint(0, 0, 10);
				gCalPlaneLineIntersect(&safePlane, &tLine, u);
				tPnt = tLine.PointAt(u);

				tPath.AddPnt(FIpPoint(tPnt.x, tPnt.y, tPnt.z));//���ƶ�����ȫƽ�棬���ƶ�����һ����Ļ��˵�
				tPath.AddPnt(backPntList[0]);
			}

			tPath.AddPnt(ipPntList[i]);
			tPath.AddPnt(backPntList[i]);
			tPath.AddPnt(backPntList[i + 1]);
			tPath.AddPnt(ipPntList[i + 1]);
			pathlist.push_back(tPath);
		}

		// progress
		mp_progress->setValue(int(cProgress + (++cColli / nColliJudge) * 60));

	}

}

void FPathPlan::SetIsFastPathPlan(bool _is_fast)
{
	m_is_fast_path_plan = _is_fast;
}

void FPathPlan::SetIsFaseColliJudge(bool _is_fast)
{
	m_is_fast_collision_judge = _is_fast;
}

void FPathPlan::SetProgress(QProgressBar* _progress)
{
	mp_progress = _progress;
}

void FPathPlan::WriteTSPProblemFile()
{
	QString str = QApplication::applicationDirPath() + QString("/data/FreeForm_ICD_1.icd");
	char fff[100]; 
	strcpy(fff, str.toLocal8Bit().data());
	norm = LTP_EUCLIDEAN_3D;      
	Base = 1;
	Backd = 6;                                           
	IOExplicit = 0;
	EUC3D = 1;	
	SetFileName("Freefrom_ICD_1.icd");
	int ncount = m_ipPntList.size();
//	IOWriteTSPLIB(LKHProblemFileName, IONumPoint);
	int i;
	FILE *fout;
	char *fname = LKHProblemFileName;
	if ((fout = fopen(fname, "w")) == (FILE *)NULL) {
		perror(fname);
		fprintf(stderr,"Unable to open %s for output\n", fname);
		goto CLEANUP;
	}
	fprintf(fout, "NAME: part%d\n", ncount);
	fprintf(fout, "TYPE: TSP\n");
	fprintf(fout, "COMMENT: Generated by WriteTSPProblem\n");
	fprintf(fout, "DIMENSION: %d\n", ncount);
	fprintf(fout, "EDGE_WEIGHT_TYPE: ");
	switch (norm) {
	case LTP_MAXNORM:
		fprintf(fout, "MAX_2D\n");
		break;
	case LTP_MANNORM:
		fprintf(fout, "MAN_2D\n");
		break;
	case LTP_EUCLIDEAN_CEIL:
		fprintf(fout, "CEIL_2D\n");
		break;
	case LTP_EUCLIDEAN:
		fprintf(fout, "EUC_2D\n");
		break;
	case LTP_EUCLIDEAN_3D:
		fprintf(fout, "EUC_3D\n");
		break;
	case LTP_USER:
		fprintf(fout, "USER\n");
		fprintf(stderr, "warning: Norm not a supported in TSPLIB\n");
		goto CLEANUP;
	case LTP_ATT:
		fprintf(fout, "ATT\n");
		break;
	case LTP_GEOGRAPHIC:
		fprintf(fout, "GEO\n");
		break;
	case LTP_GEOM:
		fprintf(fout, "GEOM\n");
		break;
	case LTP_MATRIXNORM:
		fprintf(fout, "EXPLICIT\n");
		break;
	case LTP_DSJRANDNORM:
		fprintf(fout, "DSJ_RANDOM\n");
		fprintf(stderr, "Warning: Norm not a supported in TSPLIB\n");
		goto CLEANUP;
	case LTP_CRYSTAL:
		fprintf(fout, "CRYSTAL\n");
		fprintf(stderr, "Warning: Crystal is not supported in TSPLIB\n");
		goto CLEANUP;
	case LTP_RHMAP1:
	case LTP_RHMAP2:
	case LTP_RHMAP3:
	case LTP_RHMAP4:
	case LTP_RHMAP5:
		fprintf(fout, "RHMAPx\n");
		fprintf(stderr, "Warning: Norm not a supported in TSPLIB\n");
		goto CLEANUP;
	default:
		fprintf(stderr, "Unknown NORM\n");
		goto CLEANUP;
	}

	switch (norm & LTP_NORM_SIZE_BITS) {
	case LTP_D2_NORM_SIZE:
		fprintf(fout, "NODE_COORD_SECTION\n");
		for (i = 0; i < ncount; i++) {
			fprintf(fout, "%d %f %f\n", i + 1, m_ipPntList[i].x, m_ipPntList[i].y);
		}
		break;
	case LTP_D3_NORM_SIZE:
		fprintf(fout, "NODE_COORD_SECTION\n");
		for (i = 0; i < ncount; i++) {
			fprintf(fout, "%d %f %f %f\n", i + 1, m_ipPntList[i].x, m_ipPntList[i].y, m_ipPntList[i].z);
		}
		break;
	case LTP_MATRIX_NORM_SIZE:
		fprintf(stderr, "��֧����\" MATRIX \"��ʽ���\n");/* ������Ҫ�����Ծ�������ķ�ʽ*/
		break;
	default:
		fprintf(stderr, "δ֪ NORM_SIZE\n");
		goto CLEANUP;
	}

CLEANUP:
	fprintf(fout, "EOF");
	if (fout != (FILE *)NULL) 
		fclose(fout);
}

bool FPathPlan::BasicPath(FIntList& basicPath)
{
	// ����С�ڵ���2ʱʹ�����·���������
	bool is_short = gWindow->GetMainDoc()->GetMesPntList().size() >= 2 && !m_is_fast_path_plan;

	if (m_is_fast_path_plan)
	{
		FastPathPlan:
		//cout << "---------ʹ�ÿ���·���滮�㷨-------" << endl;

		IONumPoint = m_ipPntList.size();
		for (int i = 0; i < IONumPoint/*-1*/; i++)
		{
			basicPath.push_back(i + 1);
			//basicPath.push_back(IONumPoint - i);
		}
		return true;

	}
	else
	{
		//cout << "---------ʹ�þ�ȷ·���滮�㷨-------" << endl;
		try
		{
			char* LKHPara = "lkh_PART1_para.para";
			char* CLKHParar = "clkh_PART1_para.para";
			IONumPoint = m_ipPntList.size();
			IOWriteTSPParameter(LKHPara);
			IODistance = IORoundDistance;
			LKHmain(1, LKHPara);
			IOReadTour(LKHTourFileName);
			for (int i = 0; i < IONumPoint/*-1*/; i++)
			{
				basicPath.push_back(tour[i]);
				//basicPath.push_back(i+1);
			}
			IOFreeMemory();
			return true;
		}
		catch (...)
		{
			goto FastPathPlan;
		}

	}

}