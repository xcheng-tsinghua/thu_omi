#include <math.h>
#include <conio.h>
#include "MainDoc.h"
#include "FUtilities.h"
//#include "Step\LSETP_Reader.h"
//#include "Step\LSTEPConvertor.h"
#include "FPathPlan.h"
#include "FTest.h"
#include <qpushbutton.h>
#include <qlayout.h>
#include "np.h"
#include "FMainWindow.h"
#include "FCodeWnd.h"
#include <qgroupbox.h>

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRep_Tool.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <Poly_Triangulation.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <STEPControl_Writer.hxx>
#include <TopoDS_Shape.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Pnt.hxx>
#include <vector>
#include <iostream>

#include <GeomPlate_BuildPlateSurface.hxx>
#include <GeomPlate_MakeApprox.hxx>

extern FMainWindow* gWindow;

FTest::FTest(QWidget* parent) : QWidget(parent)
{
	setWindowFlags(Qt::WindowStaysOnTopHint);

	QPushButton* pTestBtn = new QPushButton(QString::fromLocal8Bit("测试"));
	QPushButton* pCompareBtn = new QPushButton(QString::fromLocal8Bit("文件比较"));
	QPushButton* pInterpBtn = new QPushButton(QString::fromLocal8Bit("曲面插值"));

	QGroupBox* pReconstructBox = new QGroupBox;
	QGridLayout* pBoxLayout = new QGridLayout(pReconstructBox);

	m_pTol3d_Real= new QLineEdit;
	m_pNbmax_Int= new QLineEdit;
	m_pdgmax_Int= new QLineEdit;
	m_pdmax_Real= new QLineEdit;
	m_pCritOrder_Int= new QLineEdit;
	m_pContinuity_Str= new QLineEdit;
	m_pEnlargeCoeff_Real= new QLineEdit;

	m_pTol3d_Real->setText("0.00001");
	m_pNbmax_Int->setText("30");
	m_pdgmax_Int->setText("17");
	m_pdmax_Real->setText("0");
	m_pCritOrder_Int->setText("-1");
	m_pContinuity_Str->setText("GeomAbs_C2");
	m_pEnlargeCoeff_Real->setText("1.2");

	pBoxLayout->addWidget(new QLabel("m_pTol3d_Real"), 0, 0);
	pBoxLayout->addWidget(m_pTol3d_Real, 0, 1);
	pBoxLayout->addWidget(new QLabel("m_pNbmax_Int"), 1, 0);
	pBoxLayout->addWidget(m_pNbmax_Int, 1, 1);
	pBoxLayout->addWidget(new QLabel("m_pdgmax_Int"), 2, 0);
	pBoxLayout->addWidget(m_pdgmax_Int, 2, 1);
	pBoxLayout->addWidget(new QLabel("m_pdmax_Real"), 3, 0);
	pBoxLayout->addWidget(m_pdmax_Real, 3, 1);
	pBoxLayout->addWidget(new QLabel("m_pCritOrder_Int[-1, 0, 1]"), 4, 0);
	pBoxLayout->addWidget(m_pCritOrder_Int, 4, 1);
	pBoxLayout->addWidget(new QLabel("m_pContinuity_Str"), 5, 0);
	pBoxLayout->addWidget(m_pContinuity_Str, 5, 1);
	pBoxLayout->addWidget(new QLabel("m_pEnlargeCoeff_Real"), 6, 0);
	pBoxLayout->addWidget(m_pEnlargeCoeff_Real, 6, 1);

	QHBoxLayout* pMainLayout = new QHBoxLayout(this);
	pMainLayout->addStretch();
	pMainLayout->addWidget(pTestBtn);
	pMainLayout->addWidget(pCompareBtn);
	pMainLayout->addWidget(pInterpBtn);
	pMainLayout->addWidget(pReconstructBox);

	connect(pTestBtn, SIGNAL(clicked()), this, SLOT(onTest()));
	connect(pCompareBtn, SIGNAL(clicked()), this, SLOT(onFileCompare()));
	connect(pInterpBtn, SIGNAL(clicked()), this, SLOT(onInterp()));

}

FTest::~FTest()
{

}

void FTest::onTest()
{

	vector<int> asas;

	asas.push_back(0);
	asas.push_back(1);
	asas.push_back(2);
	asas.push_back(3);
	asas.push_back(4);
	asas.push_back(5);
	asas.push_back(6);

	removeVal(asas, 2);

	for (auto ita = asas.begin(); ita != asas.end(); ++ita)
	{

		cout << *ita << ",";
	}


	//if (isSuffixXXX("asaaa.txt"))
	//{
	//	MesgBox("yes");
	//}
	//else
	//{
	//	MesgBox("no");
	//}



}

void FTest::onFileCompare()
{
	auto highlightDifferences = [&](const QString& str, const QList<int>& differences)->QString
	{
		QString result;
		for (int i = 0; i < str.length(); ++i) {
			if (differences.contains(i)) {
				result += QString("<span style='color:red;'>") + str[i] + "</span>";
			}
			else {
				result += str[i];
			}
		}
		return result;
	};

	auto findDifferences = [&](const QString& str1, const QString& str2)->QList<int>
	{
		QList<int> differencePositions;
		int length = qMin(str1.length(), str2.length());

		// Compare characters up to the length of the shorter string
		for (int i = 0; i < length; ++i) {
			if (str1[i] != str2[i]) {
				differencePositions.append(i);
			}
		}

		// If strings are of different lengths, note the excess characters
		if (str1.length() != str2.length()) {
			for (int i = length; i < qMax(str1.length(), str2.length()); ++i) {
				differencePositions.append(i);
			}
		}

		return differencePositions;
	};


	QString content1 = FileToString("D:/document/ThuOMI-server/NC_Calibration/ProbeUpperNC.mpf");
	QString content2 = FileToString("D:/document/ThuOMI-server/NC_Calibration/ProbeUpperNC_CF.mpf");
	if (content1 == content2)
	{
		MesgBox("files equal");
	}
	else
	{
		MesgBox("files not equal");

		QList<int> differences = findDifferences(content1, content2);

		QString highlightedStr1 = highlightDifferences(content1, differences);
		QString highlightedStr2 = highlightDifferences(content2, differences);

		reinterpret_cast<FCodeWnd*>(gWindow->GetSubWnd()[2])->getTextEdit()->setPlainText(highlightedStr1);
	}

}

void FTest::onInterp()
{
	gWindow->GetMainDoc()->ClearDispEnts();

	vector<gp_Pnt> fpPointsAll = readPoinsFromTXT("C:\\Users\\xcheng\\Desktop\\Inspection Points.txt", 1, 2, 3);
	ShowPoints(fpPointsAll);

	GeomPlate_BuildPlateSurface plateSurface;

	for (auto ita = fpPointsAll.begin(); ita != fpPointsAll.end(); ++ita)
	{
		plateSurface.Add(new GeomPlate_PointConstraint(*ita, -1));
	}

	plateSurface.Perform();


	// 拟合曲面
	double Tol3d_Real = m_pTol3d_Real->text().toDouble();
	int Nbmax_Int = m_pNbmax_Int->text().toInt();
	int dgmax_Int = m_pdgmax_Int->text().toInt();
	double dmax_Real = m_pdmax_Real->text().toDouble();
	int CritOrder_Int = m_pCritOrder_Int->text().toInt();
	QString Continuity_Str = m_pContinuity_Str->text();
	double EnlargeCoeff_Real = m_pEnlargeCoeff_Real->text().toDouble();

	GeomAbs_Shape Continuity;
	if (Continuity_Str == "GeomAbs_C0")
	{
		Continuity = GeomAbs_C0;
	}
	else if (Continuity_Str == "GeomAbs_G1")
	{
		Continuity = GeomAbs_G1;
	}
	else if (Continuity_Str == "GeomAbs_C1")
	{
		Continuity = GeomAbs_C1;
	}
	else if (Continuity_Str == "GeomAbs_G2")
	{
		Continuity = GeomAbs_G2;
	}
	else if (Continuity_Str == "GeomAbs_C2")
	{
		Continuity = GeomAbs_C2;
	}
	else if (Continuity_Str == "GeomAbs_C3")
	{
		Continuity = GeomAbs_C3;
	}
	else if (Continuity_Str == "GeomAbs_CN")
	{
		Continuity = GeomAbs_CN;
	}
	else
	{
		MesgBox("Error Continuity");
		return;
	}

	GeomPlate_MakeApprox approx = GeomPlate_MakeApprox(
		plateSurface.Surface(), 
		Tol3d_Real, 
		Nbmax_Int, 
		dgmax_Int, 
		dmax_Real, 
		CritOrder_Int, 
		Continuity, 
		EnlargeCoeff_Real
	);

	Handle(Geom_BSplineSurface) surface = approx.Surface();

	FSurface* RecstFace = BSpSurf2FSurf(surface);
	RecstFace->SetColor(0, 1, 0);
	gWindow->GetMainDoc()->AddDispEnt(RecstFace);

	gWindow->UpdateModelView();

	//std::vector<gp_Pnt> unorderedPoints;
	//unorderedPoints.push_back(gp_Pnt(0.0, 0.0, 0.0));
	//unorderedPoints.push_back(gp_Pnt(1.0, 0.0, 0.5));
	//unorderedPoints.push_back(gp_Pnt(2.0, 0.0, 0.0));
	//unorderedPoints.push_back(gp_Pnt(0.0, 1.0, 1.0));
	//unorderedPoints.push_back(gp_Pnt(1.0, 1.0, 1.5));
	//unorderedPoints.push_back(gp_Pnt(2.0, 1.0, 1.0));
	//unorderedPoints.push_back(gp_Pnt(0.0, 2.0, 0.0));
	//unorderedPoints.push_back(gp_Pnt(1.0, 2.0, 0.5));
	//unorderedPoints.push_back(gp_Pnt(2.0, 2.0, 0.0));
	//unorderedPoints.push_back(gp_Pnt(0.0, 3.0, 1.0));
	//unorderedPoints.push_back(gp_Pnt(1.0, 3.0, 1.5));
	//unorderedPoints.push_back(gp_Pnt(2.0, 3.0, 1.0));

	//// 将点转换为TopoDS_Vertex
	//std::vector<TopoDS_Vertex> vertices;
	//for (auto ita = unorderedPoints.begin(); ita != unorderedPoints.end(); ++ita)
	//{
	//	vertices.push_back(BRepBuilderAPI_MakeVertex(*ita));
	//}

	//// 构建Delaunay三角剖分
	//TopoDS_Wire wire;
	//BRepBuilderAPI_MakeWire wireBuilder;
	//for (size_t i = 0; i < vertices.size(); ++i) {
	//	for (size_t j = i + 1; j < vertices.size(); ++j) {
	//		wireBuilder.Add(BRepBuilderAPI_MakeEdge(vertices[i], vertices[j]));
	//	}
	//}
	//wire = wireBuilder.Wire();

	//// 创建面并进行网格化
	//TopoDS_Face face = BRepBuilderAPI_MakeFace(wire);
	//BRepMesh_IncrementalMesh(face, 0.1);

	//// 提取网格点
	////TopExp_Explorer faceExplorer(face, TopAbs_FACE);
	////TopoDS_Face triangulatedFace = TopoDS::Face(faceExplorer.Current());
	//TopLoc_Location location;
	//Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, location);

	//if (!triangulation.IsNull()) {
	//	const TColgp_Array1OfPnt& nodes = triangulation->Nodes();
	//	const Poly_Array1OfTriangle& triangles = triangulation->Triangles();

	//	// 将网格点转换为二维数组
	//	const int numU = 10; // 假设为10
	//	const int numV = 10; // 假设为10
	//	TColgp_Array2OfPnt points(1, numU, 1, numV);

	//	for (int i = 1; i <= numU; ++i) {
	//		for (int j = 1; j <= numV; ++j) {
	//			// 你需要根据网格点的分布进行正确的坐标分配
	//			points.SetValue(i, j, nodes(i + (j - 1) * numU));
	//		}
	//	}

	//	// 拟合B样条曲面
	//	GeomAPI_PointsToBSplineSurface surfaceFitter(points);
	//	Handle(Geom_BSplineSurface) bsplineSurface = surfaceFitter.Surface();

	//	FSurface* RecstFace = BSpSurf2FSurf(bsplineSurface);
	//	// 显示曲面
	//	RecstFace->SetColor(0, 1, 0);
	//	gWindow->GetMainDoc()->AddDispEnt(RecstFace);

	//}
	//else 
	//{
	//	MesgBox("can not create surface");
	//}




}

void FTest::CreateArc(FMainDoc* pDoc)
{
	//auto origin = F3dPoint(0.0, 0.0, 0.0);
	//auto normal = F3dVector(0, 0, 1);
	//auto plane = FPlane(origin, normal);
	//auto circle = FCircle(plane, origin, 8);
	//auto parc = new FArc(circle, ON_Interval(3.1415/4, 5*3.1415/4));
	//parc->Update();
	//pDoc->AddDispEnt(parc);
}
void FTest::CreateBoundingBox(FMainDoc* pDoc)
{
	FBoundingBox bbox;
	bbox.SetMinPoint(F3dPoint(10,10,10));
	bbox.SetMaxPoint(F3dPoint(40,30,20));
	bbox.Print();
	//_cprintf("please input the left-bottom corner:");
	//float xx, yy, zz;
	//_cscanf("%f %f %f", &xx, &yy, &zz);	
	//bbox.SetMinPoint(F3dPoint(xx, yy, zz));	
	//_cprintf("\r\nplease input the right-top corner:");	
	//_cscanf("%f %f %f", &xx, &yy, &zz);
	//bbox.SetMaxPoint(F3dPoint(xx, yy, zz));
	//standard frame
	//Set Z axis to (1, 0, 0)
	qDebug()<<"Set Z axis to (1,0,0)";
	bbox.GetFrame()->SetAxisZ(F3dVector(1,0,0));
	bbox.Print();
	qDebug()<<"After enlarged 5 mm";
	bbox.Offset(5);
	bbox.Print();
	qDebug()<<"After enlarged (1,2,3)";
	bbox.Offset(F3dVector(1,2,3));
	bbox.Print();
	//_cprintf("Input q to exit?");
	//exit the console
	//char cmd[100];
	//int nChar = _cscanf("%s", cmd);
	//if (nChar==1 && cmd[0]=='q')
	//	FreeConsole();
	//else if (nChar==4 && cmd[0]=='e' && cmd[1]=='x' && cmd[2]=='i')
	//{
	//	FreeConsole();
	//}
}

void FTest::CreateFreeNurbsCurve(FMainDoc* pDoc)
{
	FFreeCurve* pCurve = new FFreeCurve(3, false, 3, 5);
	pCurve->Curve()->SetCV(0, F3dPoint(0, 0, 0));
	pCurve->Curve()->SetCV(1, F3dPoint(10, 0, 0));
	pCurve->Curve()->SetCV(2, F3dPoint(10, 10, 0));
	pCurve->Curve()->SetCV(3, F3dPoint(0, 10, 0));
	pCurve->Curve()->SetCV(4, F3dPoint(0, 0, 0));
	pCurve->Curve()->MakeClampedUniformKnotVector();
	pCurve->Update();
	pDoc->AddDispEnt(pCurve);
}

void FTest::CreateCircle(FMainDoc* pDoc)
{
	FCircle* pCircle = new FCircle(FPlane(F3dPoint(0,0,0), F3dVector(0, 0, 1)), 5);
	pCircle->Update();
	pDoc->AddDispEnt(pCircle);
	//FCurve* pCurve(NULL);
	//pCurve = (FCurve*)gImportFeatureFromFile(QApplication::applicationDirPath()+"/Ftlib/circle.ft");
	//pCurve->Update();
	//pDoc->AddDispEnt(pCurve);
}

void FTest::CreatePolygon(FMainDoc* pDoc)
{
	FPlane* pPlane = new FPlane(F3dPoint(1,0,0), F3dVector(1,0,0));
//	FPolygon* pPolygon = new FPolygon(*pPlane, 4, 10);
	FPolygon* pPolygon = new FPolygon();
	pPolygon->Update();
	pDoc->AddDispEnt(pPolygon);
}

void FTest::CreateCompatibleCurves(FMainDoc* pDoc)
{
	PFCurveList curves;
	//Fist nurbs curve with 8 CVs
	F3dPointList pnts;
	pnts.Append(F3dPoint(0, 0, 0));
	pnts.Append(F3dPoint(5, 0, 0));
	pnts.Append(F3dPoint(10, 0, 0));
	pnts.Append(F3dPoint(10, 5, 0));
	pnts.Append(F3dPoint(10, 10, 0));
	pnts.Append(F3dPoint(5, 10, 0));
	pnts.Append(F3dPoint(0, 10, 0));
	pnts.Append(F3dPoint(0, 5, 0));
//	pnts.Append(F3dPoint(0, 0, 0));
	FCurve* pCurve = new FCurve;
	pCurve->SetOrder(4);
	pCurve->SetDim(3);
	pCurve->SetClosed(true);
	pCurve->AddPoints(pnts);
	pCurve->SetShowUserPoint(true);
	pCurve->SetColor(1.0, 0.0, 0.0);
	pCurve->Update();
	pDoc->AddDispEnt(pCurve);
	curves.push_back(pCurve);
	//second curve with 4 CVs
	pnts.Empty();
	pnts.Append(F3dPoint(0, 0, 10));
	pnts.Append(F3dPoint(10, 0, 10));
	pnts.Append(F3dPoint(10, 10, 10));
	pnts.Append(F3dPoint(0, 10, 10));
//	pnts.Append(F3dPoint(0, 0, 10));
	pCurve = new FCurve;
	pCurve->SetOrder(4);
	pCurve->SetDim(3);
	pCurve->SetClosed(true);
	pCurve->AddPoints(pnts);
	pCurve->SetShowUserPoint(true);
	pCurve->SetColor(0.0, 1.0, 0.0);
	pCurve->Update();
	pDoc->AddDispEnt(pCurve);
	curves.push_back(pCurve);
	//make the 1st and 2nd curves compatible
	if(!gGenerateCompatibleCurves(curves))
		return ;
}
void FTest::CreateNurbsSurface(FMainDoc* pDoc)
{
	//Method1: 
	const int bIsRational = false;
	const int dim = 3;
	const int u_degree = 2;
	const int v_degree = 2;
	const int u_cv_count = 2;
	const int v_cv_count = 2;
	F3dPoint CV[u_cv_count][v_cv_count];
	FSurface* pSurf = new FSurface(3, false, u_degree, v_degree, u_cv_count, v_cv_count);
	int i, j;
	//Method1: Create a nurbs surface by setting CVs
	CV[0][0]=F3dPoint(0, 0, 0); CV[0][1] = F3dPoint(0, 10, 0);
	CV[1][0]=F3dPoint(10, 0, 0); CV[1][1] = F3dPoint(10, 10, 0);

	for ( i = 0; i < pSurf->OnNurbsSurf()->CVCount(0); i++ ) {
		for ( j = 0; j < pSurf->OnNurbsSurf()->CVCount(1); j++ ) {
			pSurf->OnNurbsSurf()->SetCV( i, j, CV[i][j] );
		}
	}
	pSurf->OnNurbsSurf()->MakeClampedUniformKnotVector(0);
	pSurf->OnNurbsSurf()->MakeClampedUniformKnotVector(1);

//	/*
	//Method2:Create nurbs surface by setting CVs and Knots
	//double u_knot[ u_cv_count + u_degree - 1 ];
	//double v_knot[ v_cv_count + v_degree - 1 ];
	//u_knot[0] = u_knot[1] = 0.0;
	//u_knot[2] = u_knot[3] = 1.0;
	//v_knot[0] = v_knot[1] = 0.0;
	//v_knot[2] = v_knot[3] = 1.0;
	//for ( i = 0; i < pSurf->OnNurbsSurf()->KnotCount(0); i++ )
	//	pSurf->OnNurbsSurf()->SetKnot( 0, i, u_knot[i] );
	//for ( j = 0; j < pSurf->OnNurbsSurf()->KnotCount(1); j++ )
	//	pSurf->OnNurbsSurf()->SetKnot( 1, j, v_knot[j] );	
	//*/	
	pSurf->Update();
	pDoc->AddDispEnt(pSurf);
}
void FTest::CreateFreeCurve(FMainDoc* pDoc)
{
	int order(4), nCV(5);
	//Clamped knot used for curve passing through the first and last CVs
	FFreeCurve* pCurve = new FFreeCurve(3, false, order, nCV);
	pCurve->Curve()->SetCV(0, F3dPoint(0,0,0));
	pCurve->Curve()->SetCV(1, F3dPoint(10,0,0));
	pCurve->Curve()->SetCV(2, F3dPoint(10,10,0));
	pCurve->Curve()->SetCV(3, F3dPoint(0,10,0));
	pCurve->Curve()->SetCV(4, F3dPoint(0,0,0));
	pCurve->Curve()->MakeClampedUniformKnotVector();
	pCurve->Update();
	pDoc->AddDispEnt(pCurve);
	//Periodic knot used for smoothed close curve
	/*pCurve = new FFreeCurve(3, false, order, nCV+order-2);
	pCurve->Curve()->SetCV(0, F3dPoint(0,0,10));
	pCurve->Curve()->SetCV(1, F3dPoint(10,0,10));
	pCurve->Curve()->SetCV(2, F3dPoint(10,10,10));
	pCurve->Curve()->SetCV(3, F3dPoint(0,10,10));
	int last = nCV-1;
	F4dPoint tCV;
	for (int i=0; i<order-1; i++)
	{
	pCurve->Curve()->GetCV(i, tCV);
	pCurve->Curve()->SetCV(last+i, tCV);
	}
	pCurve->Curve()->MakePeriodicUniformKnotVector();
	pCurve->Update();
	pDoc->AddDispEnt(pCurve);*/

	//The second way to create a freeform curve
	//F3dPointList pnts;
	//pnts.Append(F3dPoint(0, 0, 0));
	//pnts.Append(F3dPoint(10, 0, 0));
	//pnts.Append(F3dPoint(10, 10, 0));
	//pnts.Append(F3dPoint(0, 10, 0));

	//FCurve* pCurve = new FCurve;
	//pCurve->SetOrder(4);
	//pCurve->SetDim(3);
	//pCurve->SetRational(false);
	//pCurve->SetClosed(true);
	//pCurve->AddPoints(pnts);
	//pCurve->Update();
	//pDoc->AddDispEnt(pCurve);

	//Create a nurbs curve to interpolate given points
	//F3dPointList pnts;
	//double R=10;
	//const int NUM(20);
	//double step = 2*PI/NUM;
	//double angle(0);
	//for (int i=0; i<NUM; i++)
	//{
	//	angle = step*i;
	//	pnts.Append(F3dPoint(R*cos(angle), R*sin(angle), 0.0));
	//}
	//FCurve* pCurve = new FCurve;
	//pCurve->SetbValuePnt(true);
	//pCurve->SetOrder(4);
	//pCurve->SetDim(3);
	//pCurve->SetRational(false);
	//pCurve->SetClosed(true);
	//pCurve->AddPoints(pnts);
	//pCurve->Update();
//	FFreeCurve* pCurve = (FFreeCurve*)gImportFeatureFromFile(QObject::tr("LAMP_PROFILE"));
//	pCurve->Update();
//	FEntity* pEnt = pCurve->NewCopy();
////	pEnt->Update();
//	pDoc->AddDispEnt(pEnt);
}

void FTest::CreateISOCurve(FMainDoc* pDoc)
{
	//create surface
	FSphere* pSphere = new FSphere(10);
	pSphere->Update();
	//create iso curve
	FIsoCurve* pCurve = new FIsoCurve();
	pCurve->SetbUIso(true);
	pCurve->SetSurface(pSphere->Surface());
	pCurve->SetT(0.0);
	pCurve->SetStartT(0.1);
	pCurve->SetEndT(0.8);
	pCurve->Update();
	pDoc->AddDispEnt(pSphere);
	pDoc->AddDispEnt(pCurve);

	//offset the current isocurve
	FIsoCurve* pNewISO = pCurve->OffsetFtIsoCurve(0.1, 1);
	pDoc->AddDispEnt(pNewISO);
}

void FTest::CreateSurfaceCurve(FMainDoc* pDoc)
{
	//create surface
	FSphere* pSphere = new FSphere(10);
	pSphere->Update();

	//create domain curve
	F3dPointList pnts;
	double R=1;
	const int NUM(20);
	double step = 2*PI/NUM;
	double angle(0);
	for (int i=0; i<NUM; i++)
	{
		angle = step*i;
		pnts.Append(F3dPoint(R*cos(angle), R*sin(angle), 0.0));
	}
	FCurve* pCurve = new FCurve;
	pCurve->SetbValuePnt(true);
	pCurve->SetOrder(4);
	pCurve->SetDim(3);
	pCurve->SetRational(false);
	pCurve->SetClosed(true);
	pCurve->AddPoints(pnts);
	pCurve->Update();

	//create on-surface curve
	FSurfaceCurve* pEnt = new FSurfaceCurve;
	pEnt->SetDomainCurve(pCurve);
	pEnt->SetSurface(pSphere->Surface());
	pEnt->SetType(U_ISO);
	pEnt->Update();

	pDoc->AddDispEnt(pSphere);
	pDoc->AddDispEnt(pEnt);
}

void FTest::CreateFreeBody(FMainDoc* pDoc)
{
	FFreeBody* pFBody(NULL);
	//FBody* pBody(NULL);
	//FMesh* pMesh(NULL);
	//QString fname = QApplication::applicationDirPath()+"/test/ateneal.obj";
	//pMesh = FT::FileIO::ImportOBJ(fname.toLocal8Bit().data());	
	//pBody = FT::FEuler::CreateBodyFromMesh(pMesh);
	//if (pBody)
	//{
	//	pFBody = new FFreeBody(pBody);
	//	pFBody->SetTextureFile(QString("P_Red.bmp"));
	//	pFBody->Update();
	//	FGroup* pGroup = new FGroup;
	//	pGroup->AddToGroup(pFBody);
	//	pGroup->Update();

	//	FGroup* newBody = new FGroup(*pGroup);
	//	newBody->Update();
	//	pDoc->AddDispEnt(newBody);	
	//}
	//pFBody = gImportBodyFromLib(QObject::tr("claw2"));	//commented by jacky@2018/1/3
	//FForm tForm;
	////tForm.Translation(0, 10, 0);
	//tForm.Rotation(FT::PI/2, F3dVector(1,0,0), F3dPoint(0,0,0));
	//pFBody->DoTransform(tForm);
	//pFBody->Update();
	//pDoc->AddDispEnt(pFBody);
	//pDoc->UpdateAllViews();
}

void FTest::CreateWireCube(FMainDoc* pDoc, double len)
{
	FBody* pBody(NULL);
	FVertex* pA(NULL), *pNewVert(NULL);
	FEdge* pEdge(NULL);
	FFace* pFace(NULL);
	FLoop* pLoop(NULL);
	////////////////////////////Face ABCD //////////////////////////////////////////////
	//vertex A,degenerated loop, and body
	if (!FEuler::lmVLB(pNewVert, pLoop, pBody, "A"))
		return ;
	pA = pNewVert;	pNewVert->SetPoint(F3dPoint(0, 0, 0)); 
	//vertex B, edge AB
	if (!FEuler::lmVE(pNewVert->GetHEdge(), pNewVert->GetHEdge(), pEdge, pNewVert, "B"))
		return ;
	pBody->AddEdge(pEdge); pBody->AddVertex(pNewVert); 
	pNewVert->SetPoint(F3dPoint(len, 0, 0));
	//vertex C, edge BC
	if (!FEuler::lmVE(pEdge->GetSecondHEdge(), pEdge->GetFirstHEdge(), pEdge, pNewVert, "C"))
		return ;
	pBody->AddEdge(pEdge); pBody->AddVertex(pNewVert);
	pNewVert->SetPoint(F3dPoint(len, len, 0));
	//vertex D, edge CD
	if (!FEuler::lmVE(pEdge->GetSecondHEdge(), pEdge->GetFirstHEdge(), pEdge, pNewVert, "D"))
		return ;
	pBody->AddEdge(pEdge); pBody->AddVertex(pNewVert);
	pNewVert->SetPoint(F3dPoint(0, len, 0));
	//edge DA, face ABCD
	if (!FEuler::lmEF(pEdge->GetFirstHEdge(), pA->GetHEdge(),pEdge, pFace))
		return ;
	pBody->AddEdge(pEdge); pBody->AddFace(pFace);
	////////////////////////////Face ABCD //////////////////////////////////////////////

	//vertex C, edge BC
	//if (!FEuler::lmVE(pEdge->GetSecondHEdge(), pEdge->GetFirstHEdge(), pEdge, pNewVert, "C"))
	//	return NULL;
	//pBody->AddEdge(pEdge); pBody->AddVertex(pNewVert);
	//pNewVert->SetPoint(F3dPoint(len, len, 0));
	pDoc->AddDispEnt(pBody);
}

void FTest::CreateCylinder(FMainDoc* pDoc, double radius, double h)
{
	FCylinder* pEnt = new FCylinder(radius, h);
	pEnt->Update();
	pDoc->AddDispEnt(pEnt);
}

void FTest::CreateSphere(FMainDoc* pDoc, double r)
{
	FSphere* pEnt = new FSphere(r);
	pEnt->Update();
	pDoc->AddDispEnt(pEnt);
}
void FTest::CreateTorus(FMainDoc* pDoc, double majorR, double minorR)
{
	FTorus* pEnt = new FTorus(majorR, minorR);
	pEnt->Update();
	pDoc->AddDispEnt(pEnt);
}
void FTest::CreateFrame(FMainDoc* pDoc)
{
	FTransFrame* pEnt = new FTransFrame();
	pEnt->SetOrigin(F3dPoint(10, 10, 0));
	pEnt->SetRadius(3);
	pEnt->SetHandleFrame(true);
	pEnt->Update();
	pDoc->AddDispEnt(pEnt);
}

void FTest::CreateCircleHandle(FMainDoc* pDoc)
{
	FCircle* pCircle = new FCircle(FPlane(F3dPoint(0,0,0), F3dVector(0, 0, 1)), 5);
	pCircle->Update();
	pDoc->AddDispEnt(pCircle);
	F3dPoint tOrign = pCircle->PointAt(0);
	F3dVector tAxis = pCircle->plane.xaxis;
	FArrow* pArrow = new FArrow;
	pArrow->SetOrigin(tOrign);
	pArrow->SetDirection(tAxis);
	pArrow->SetLength(pCircle->RadiusX()/2);
	pArrow->Update();
	pDoc->AddDispEnt(pArrow);
}

void FTest::TestRibRecognition(FMainDoc* pDoc)
{
	//const string ab = "..\\bin\\test\\cutting_workpiece2.STEP";
	//auto a = new LSTEPConvertor(ab);
	//a->convert(pDoc);
	//a->printFaceTopoARGraph();
 //   a->holeMatching();
	//a->simpleRibMatching();
	//a->cavityRecognition();
	//a->ribRecognition();
	//a->contourRecognition();			
	////a->featureSuppression();	    
}
void FTest::TestLineMeshIntersection(FMainDoc* pDoc)
{
	ON_Line tLine(F3dPoint(-10, 0, 2), F3dPoint(10, 0, 2));
	FFreeBody* pBody = (FFreeBody*)pDoc->GetDispEnt(0);
	if(!pBody) return ;
	ON_Mesh* pMesh = pBody->Body()->Surfaces().at(0)->GetMesh()->OnMesh();
	F3dPointList resPnts;
	gCalLineMeshIntersection(pMesh, tLine.from, tLine.to, resPnts);
	for (int i=0; i<resPnts.size(); i++)
	{
		_cprintf("Points: (%f, %f, %f)\n", resPnts[i].x, resPnts[i].y, resPnts[i].z);
	}
}
void FTest::TestPathPlanning(FMainDoc* pDoc)
{
	//FPathPlan pathPlan(pDoc);
	//pathPlan.SetBackDist(6);
	//FFreeBody* pBody = (FFreeBody*)pDoc->GetDispEnt(0);
	//if(!pBody) return ;
	//ON_Mesh* pMesh = pBody->Body()->Surfaces().at(0)->GetMesh()->OnMesh();
	//pathPlan.SetObstacleMesh(pMesh);
	//pathPlan.Planning();
}
void FTest::TestFileIO()
{
	QString binPath(QApplication::applicationDirPath());
	QString fname = binPath+"/settings.psw";
	FileIO::ExportSecurity(fname.toLocal8Bit().data());
	FileIO::ImportSecurity(fname.toLocal8Bit().data());
}

void FTest::TestPointsFromFace(FMainDoc* pDoc)
{
	if(!pDoc)	return;
	int nSel = pDoc->SelectedEntCount();
	if(nSel)
		_cprintf("First selected entity is of type: %s\n", pDoc->GetSelectedEnt(0));
	else
	{
		_cprintf("No entity is selected.");
		return ;
	}
	if (pDoc->GetSelectedEnt(0)->GetEntType()!=FT_PLANAR_FACE)
	{
		cout<<"The selected surface is not a planar face."<<endl;
        return ;
	}
		
	FFace* pFace = (FFace*)pDoc->GetSelectedEnt(0);
	PFVertexList pntList;
	pFace->VerticesAround(pntList);
	int nsize = pntList.count();
	for (int i=0; i<nsize; i++)
	{
		_cprintf("Points %d: (%f, %f, %f)\n",i, pntList[i]->GetPoint().x, pntList[i]->GetPoint().y, pntList[i]->GetPoint().z);
	}
}

void FTest :: TestPointSetting(FMainDoc* pDoc)
{
	//PlaneSurfPoint planetest;
	//planetest.DrawCloseBox(pDoc);
	//planetest.DrawHaPlane(pDoc);

	//SphereSurfPoint spheretest = SphereSurfPoint();
	//spheretest.DrawHaSphere(pDoc);
}




