#include "occConverter.h"

//header from OCCT
#include <Aspect_Handle.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <Poly_Triangulation.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomPlate_BuildPlateSurface.hxx>
#include <GeomPlate_PointConstraint.hxx>
#include <GeomPlate_MakeApprox.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Message_Messenger.hxx>
#include <Message_PrinterOStream.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <ShapeFix_Shape.hxx>
#include <StepData_StepModel.hxx>
#include <ShapeFix_Wireframe.hxx>
#include <Standard_Type.hxx>
#include <GeomConvert.hxx>
#include <Geom2dConvert.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TopLoc_Location.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TShort_Array1OfShortReal.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelSequence.hxx>
#include <Transfer_TransientProcess.hxx>
#include <ShapeAnalysis.hxx>
#include <WNT_Window.hxx>
#include <XCAFDoc_Location.hxx>
#include <XSControl_WorkSession.hxx>
#include <BRepTools.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <XCAFApp_Application.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <TDocStd_Document.hxx>
#include <Geom_Line.hxx>
#include <TDataStd_Name.hxx>

//headers from upper class
#include "FMainWindow.h"
#include "MainDoc.h"
#include "EntityHighlight.h"
#include <functional>
#include "np.h"

occConverter::occConverter()
{
	gWindow->onFileNew();

	m_pBody = new FBody();
	m_pTree = new QTreeWidgetItem();
	m_pTree->setHidden(false);

	m_pProgBar = new QProgressDialog;
	m_pProgBar->setMinimumWidth(500);
	m_pProgBar->setWindowTitle(QString::fromLocal8Bit("导入STEP文件"));
	m_pProgBar->setWindowIcon(QIcon("../icons/ImportModelProgress.png"));
	QFont ProgressFont;
	ProgressFont.setFamily("Microsoft YaHei");
	m_pProgBar->setFont(ProgressFont);
	m_pProgBar->setCancelButton(NULL);

	FFeatureTool* m_pCurTool = gWindow->GetCurTool();
	if (gWindow->GetCurTool())
	{
		gWindow->GetMainDoc()->PopTool();
		m_pCurTool = NULL;
	}
	m_pCurTool = new EntityHighlight();
	gWindow->GetMainDoc()->PushTool(m_pCurTool);
	((EntityHighlight*)m_pCurTool)->SetSelEntType(FT_FACE);

	m_MinCorner = ON_3dPoint(0, 0, 0);
	m_MaxCorner = ON_3dPoint(0, 0, 0);
	CurrentFaceIndex = 0;

}

occConverter::~occConverter()
{

}

void occConverter::FSurfInfo(FSurface* aFSurf, SURFACE_TYPE& paraSurfType, int& paraIndexInAllList, int& paraIndexInTypeList, QString& paraSurfName)
{
	paraSurfName.clear();

	int SearchedSurfInd = m_HashFSurf2Ind[aFSurf];

	//int nSize = m_FSurfList.size();
	//int SearchedSurfInd = -1;
	//for (int i = 0; i < nSize; i++)
	//{
	//	if (aFSurf == m_FSurfList[i])
	//	{
	//		SearchedSurfInd = i;
	//		break;
	//	}
	//}

	paraIndexInAllList = SearchedSurfInd;

	if (SearchedSurfInd == -1) _DEBUG_ERROR("未找到该曲面");

	if (m_Hashmap_BSplineInds.count(SearchedSurfInd))
	{
		paraSurfType = BSPLINE;
		paraIndexInTypeList = m_Hashmap_BSplineInds[SearchedSurfInd];

		paraSurfName = QString::fromLocal8Bit("自由曲面") + QString::number(paraIndexInTypeList);
	}
	else if (m_Hashmap_ConicalInds.count(SearchedSurfInd))
	{
		paraSurfType = CONE;
		paraIndexInTypeList = m_Hashmap_ConicalInds[SearchedSurfInd];

		paraSurfName = QString::fromLocal8Bit("圆锥面") + QString::number(paraIndexInTypeList);
	}
	else if (m_Hashmap_CylindricalInds.count(SearchedSurfInd))
	{
		paraSurfType = CYLINDER;
		paraIndexInTypeList = m_Hashmap_CylindricalInds[SearchedSurfInd];

		paraSurfName = QString::fromLocal8Bit("圆柱面") + QString::number(paraIndexInTypeList);
	}
	else if (m_Hashmap_PlaneInds.count(SearchedSurfInd))
	{
		paraSurfType = PLANE;
		paraIndexInTypeList = m_Hashmap_PlaneInds[SearchedSurfInd];

		paraSurfName = QString::fromLocal8Bit("平面") + QString::number(paraIndexInTypeList);
	}
	else if (m_Hashmap_SphericalInds.count(SearchedSurfInd))
	{
		paraSurfType = SPHERE;
		paraIndexInTypeList = m_Hashmap_SphericalInds[SearchedSurfInd];

		paraSurfName = QString::fromLocal8Bit("球面") + QString::number(paraIndexInTypeList);
	}
	else if (m_Hashmap_OtherInds.count(SearchedSurfInd))
	{
		paraSurfType = OTHER_SURF;
		paraIndexInTypeList = m_Hashmap_OtherInds[SearchedSurfInd];

		paraSurfName = QString::fromLocal8Bit("其它型面") + QString::number(paraIndexInTypeList);
	}
	else _DEBUG_ERROR("未找到该索引");

}

TopoDS_Face occConverter::FSurf2TDSFace(FSurface* aFSurf)
{
	int FSurfInd = -1;
	try
	{
		FSurfInd = m_HashFSurf2Ind[aFSurf];
	}
	catch (...)
	{
		MesgBox(QString::fromLocal8Bit("无法找到该 FSurface*"));
		//cout << "无法找到该 FSurface*" << endl;
		return TopoDS_Face();
	}

	return m_TopoDS_SurfList[FSurfInd];
}

FSurface* occConverter::GetFSurf(QString surfName)
{
	SURFACE_TYPE tmpType = OTHER_SURF;

	if (surfName.contains(QString::fromLocal8Bit("自由曲面")))
	{
		tmpType= BSPLINE;
	}
	else if (surfName.contains(QString::fromLocal8Bit("圆锥面")))
	{
		tmpType = CONE;
	}
	else if (surfName.contains(QString::fromLocal8Bit("圆柱面")))
	{
		tmpType = CYLINDER;
	}
	else if (surfName.contains(QString::fromLocal8Bit("平面")))
	{
		tmpType = PLANE;
	}
	else if (surfName.contains(QString::fromLocal8Bit("球面")))
	{
		tmpType = SPHERE;
	}
	else if (surfName.contains(QString::fromLocal8Bit("其它型面")))
	{
		tmpType = OTHER_SURF;
	}
	else
	{
		QMessageBox::warning(0, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("错误的曲面名"), "OK");
		return nullptr;
	}

	int local_index = ExNumFrQsg(surfName);

	switch (tmpType)
	{
	case BSPLINE:return m_FSurfList[m_BSplineInds[local_index]];
		break;
	case CONE:return m_FSurfList[m_ConicalInds[local_index]];
		break;
	case CYLINDER:return m_FSurfList[m_CylindricalInds[local_index]];
		break;
	case PLANE:return m_FSurfList[m_PlaneInds[local_index]];
		break;
	case SPHERE:return m_FSurfList[m_SphericalInds[local_index]];
		break;
	case OTHER_SURF:return m_FSurfList[m_OtherInds[local_index]];
		break;
	default:
		break;
	}

	return nullptr;
}

Handle(Geom_BSplineSurface) occConverter::FSurf2BSpline(FSurface* aFSurf)
{
	int FSurfInd = -1;
	try
	{
		FSurfInd = m_HashFSurf2Ind[aFSurf];
	}
	catch (...)
	{
		cout << "无法找到该 FSurface*" << endl;
		return NULL;
	}

	TopoDS_Face SearchedFace = m_TopoDS_SurfList[FSurfInd];

	Handle(Geom_BSplineSurface) transed = ConvertFaceToBSplineSurface(SearchedFace);

	return transed;

}

QString occConverter::FSurfName(FSurface* _surf)
{
	QString paraSurfName;

	int SearchedSurfInd = m_HashFSurf2Ind[_surf];
	int paraIndexInTypeList = 0;

	if (m_Hashmap_BSplineInds.count(SearchedSurfInd))
	{
		paraIndexInTypeList = m_Hashmap_BSplineInds[SearchedSurfInd];

		paraSurfName = QString::fromLocal8Bit("自由曲面") + QString::number(paraIndexInTypeList);
	}
	else if (m_Hashmap_ConicalInds.count(SearchedSurfInd))
	{
		paraIndexInTypeList = m_Hashmap_ConicalInds[SearchedSurfInd];

		paraSurfName = QString::fromLocal8Bit("圆锥面") + QString::number(paraIndexInTypeList);
	}
	else if (m_Hashmap_CylindricalInds.count(SearchedSurfInd))
	{
		paraIndexInTypeList = m_Hashmap_CylindricalInds[SearchedSurfInd];

		paraSurfName = QString::fromLocal8Bit("圆柱面") + QString::number(paraIndexInTypeList);
	}
	else if (m_Hashmap_PlaneInds.count(SearchedSurfInd))
	{
		paraIndexInTypeList = m_Hashmap_PlaneInds[SearchedSurfInd];

		paraSurfName = QString::fromLocal8Bit("平面") + QString::number(paraIndexInTypeList);
	}
	else if (m_Hashmap_SphericalInds.count(SearchedSurfInd))
	{
		paraIndexInTypeList = m_Hashmap_SphericalInds[SearchedSurfInd];

		paraSurfName = QString::fromLocal8Bit("球面") + QString::number(paraIndexInTypeList);
	}
	else if (m_Hashmap_OtherInds.count(SearchedSurfInd))
	{
		paraIndexInTypeList = m_Hashmap_OtherInds[SearchedSurfInd];

		paraSurfName = QString::fromLocal8Bit("其它型面") + QString::number(paraIndexInTypeList);
	}
	else _DEBUG_ERROR("未找到该索引");

	return paraSurfName;

}

SURFACE_TYPE occConverter::FSurfType(FSurface* _surf)
{
	int SearchedSurfInd = m_HashFSurf2Ind[_surf];
	int paraIndexInTypeList = 0;

	SURFACE_TYPE surfType = OTHER_SURF;

	if (m_Hashmap_BSplineInds.count(SearchedSurfInd))
	{
		surfType = BSPLINE;
	}
	else if (m_Hashmap_ConicalInds.count(SearchedSurfInd))
	{
		surfType = CONE;
	}
	else if (m_Hashmap_CylindricalInds.count(SearchedSurfInd))
	{
		surfType = CYLINDER;
	}
	else if (m_Hashmap_PlaneInds.count(SearchedSurfInd))
	{
		surfType = PLANE;
	}
	else if (m_Hashmap_SphericalInds.count(SearchedSurfInd))
	{
		surfType = SPHERE;
	}
	else if (m_Hashmap_OtherInds.count(SearchedSurfInd))
	{
		surfType = OTHER_SURF;
	}
	else _DEBUG_ERROR("未找到该索引");

	return surfType;

}

void occConverter::setSurfColorDefault(FSurface* _surf)
{
	// 初始默认值: (0.8, 0.835, 0.941)

	Quantity_Color faceColor = m_pFaceColorDefault[_surf];

	double cR = faceColor.Red();
	double cG = faceColor.Green();
	double cB = faceColor.Blue();

	_surf->SetColor(cR, cG, cB);

}

void occConverter::setAllSurfColorDefault()
{
	for (auto ita = m_pFaceColorDefault.begin(); ita != m_pFaceColorDefault.end(); ++ita)
	{
		Quantity_Color faceColor = (*ita).second;

		double cR = faceColor.Red();
		double cG = faceColor.Green();
		double cB = faceColor.Blue();

		(*ita).first->SetColor(cR, cG, cB);

	}

}

void occConverter::faceConvert(TColgp_SequenceOfPnt aPoints,Handle_Geom_Surface aSurf)
{
	//Transfer from Geom_Surface to BSplineSurface

	Handle_Geom_BSplineSurface aBSplineSurface = GeomConvert::SurfaceToBSplineSurface(aSurf);

	_ASSERTE(!aBSplineSurface.IsNull());

	//Get the parameter to form an ON_NurbsSurface surface
	int m_dim = 3;
	bool isUPeriodic = aBSplineSurface->IsUPeriodic();
	bool isVPeriodic = aBSplineSurface->IsVPeriodic();
	int m_is_rat = (aBSplineSurface->IsURational() || aBSplineSurface->IsVRational()) ? 1 : 0;
	int m_order[2] = { aBSplineSurface->UDegree() + 1,  aBSplineSurface->VDegree() + 1 };
	int m_cv_count[2] = { aBSplineSurface->NbUPoles() + (isUPeriodic ? 1 : 0),  aBSplineSurface->NbVPoles() + (isVPeriodic ? 1 : 0) };

	//create an ON_NurbsSurface
	auto pOnSurf =  ON_NurbsSurface::New(m_dim, m_is_rat, m_order[0], m_order[1], m_cv_count[0], m_cv_count[1]);

	//create a new FSurface
	FSurface* pSurf = new FSurface();

	//set initial nurbs surface
	pSurf->SetNurbsSurf(pOnSurf);

	// editing knotsU
	int nbUknots = aBSplineSurface->NbUKnots();
	TColStd_Array1OfReal OCCknotsU(1, aBSplineSurface->NbUKnots());
	aBSplineSurface->UKnots(OCCknotsU);
	int indexU = 0;
	for (int i = 1; i <= nbUknots; i++)
	{
		int uMulti = aBSplineSurface->UMultiplicity(i);
		if (!isUPeriodic && (i == 1 || i == nbUknots)) {
			uMulti--;
		}
		while (uMulti > 0) {
			pSurf->SetKnot(0,indexU++, OCCknotsU.Value(i));
			uMulti--;
		}
	}

	// editing knotsV
	int nbVknots = aBSplineSurface->NbVKnots();
	TColStd_Array1OfReal OCCknotsV(1, aBSplineSurface->NbVKnots());
	aBSplineSurface->VKnots(OCCknotsV);
	int indexV = 0;
	for (int i = 1; i <= nbVknots; i++)
	{
		int vMulti = aBSplineSurface->VMultiplicity(i);
		if (!isVPeriodic && (i == 1 || i == nbVknots)) {
			vMulti--;
		}
		while (vMulti > 0) {
			pSurf->SetKnot(1, indexV++, OCCknotsV.Value(i));
			vMulti--;
		}
	}

	// editing CVs
	TColgp_Array2OfPnt poles(1, aBSplineSurface->NbUPoles(),1, aBSplineSurface->NbVPoles());
	aBSplineSurface->Poles(poles);
	for (int i = 1; i <= m_cv_count[0]; i++) {
		for (int j = 1; j <= m_cv_count[1]; j++) {
			auto pt = poles.Value(i, j);
			double w = aBSplineSurface->Weight(i, j);
			int beginPosition = (i - 1) * pSurf->OnNurbsSurf()->m_cv_stride[0] + (j - 1) * pSurf->OnNurbsSurf()->m_cv_stride[1];
			pSurf->OnNurbsSurf()->m_cv[beginPosition + 0] = pt.X() * w;
			pSurf->OnNurbsSurf()->m_cv[beginPosition + 1] = pt.Y() * w;
			pSurf->OnNurbsSurf()->m_cv[beginPosition + 2] = pt.Z() * w;
			if (m_is_rat) {
				pSurf->OnNurbsSurf()->m_cv[beginPosition + 3] = w;
			}
		}
	}


	//create a new FFace and put the face into face list
	FFace* pFace = new FFace();
	m_pBody->AddFace(pFace);
	
	//set the Surface with face
	pSurf->Tessellator()->SetTesPrecision(5);
	pSurf->Update();
	pFace->FFace::SetSurface(pSurf);

	//output for debug
	//cout << "The name of added entity is " << pSurf << endl;

}

void occConverter::convert(TopoDS_Shape aShape)
{
	//run the shape_Fix function to fix shape and meet the occt shape requirement
	Handle(ShapeFix_Shape) sfs = new ShapeFix_Shape;
	sfs->Init(aShape);
	sfs->Perform();

	//getting the result 
	if (sfs->Status(ShapeExtend_DONE)) {
		cout << "Shape was fixed" << endl;
	}
	TopoDS_Shape aResult = sfs->Shape();
	if (aResult.IsNull())
		return;

	Standard_Real aDeflection = 0.1;

	// removes all the triangulations of the faces ,
	//and all the polygons on the triangulations of the edges
	BRepTools::Clean(aResult);
	// adds a triangulation of the shape aShape with the deflection aDeflection
	BRepMesh_IncrementalMesh(aResult, aDeflection);

	Standard_Integer aIndex = 1, nbNodes = 0;

	TColgp_SequenceOfPnt aPoints, aPoints1;

	// triangulation
	for (TopExp_Explorer aExpFace(aResult, TopAbs_FACE); aExpFace.More(); aExpFace.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(aExpFace.Current());
		TopLoc_Location aLocation;

		// takes the triangulation of the face aFace
		Handle_Poly_Triangulation aTr = BRep_Tool::Triangulation(aFace, aLocation);

		if (!aTr.IsNull())
		{
			// takes the array of nodes for this triangulation
			const TColgp_Array1OfPnt& aNodes = aTr->Nodes();
			nbNodes = aNodes.Length();
			for (Standard_Integer i = 1; i <= nbNodes; i++)
			{
				// create seguence of node points in absolute coordinate system
				gp_Pnt aPnt = aNodes(i).Transformed(aLocation);
				aPoints.Append(aPnt);
				aPoints1.Append(aPnt);
			}
		}

		//check outer boundary
		if (!ShapeAnalysis::IsOuterBound(aFace)) {
			cout << "Face has not outer boundary" << endl;
		}

		Standard_Real Umin,Umax,Vmin,Vmax;
		ShapeAnalysis::GetFaceUVBounds(aFace, Umin, Umax, Vmin, Vmax);
		cout << "The face bounds are " << Umin << ", " << Umax << ", " << Vmin << ", " << Vmax << endl;

		//convert TopoDS_Face to Geom_Surface
		Handle_Geom_Surface  currentSur;
		currentSur = BRep_Tool::Surface(aFace);
		/*Standard_Real U1, U2, V1, V2;
		currentSur->Bounds(U1,U2,V1,V2);
		cout << "The surface bounds are " << U1 << ", " << U2 << ", " << V1 << ", " << V2 << endl;*/

		// only finite surface can be transferred into BSplineSurface, 
		// thus, using Geom_RectangularTrimmedSurface() to trim the infinite surface into finite surface
		Handle_Standard_Type type = currentSur->DynamicType();
		if (type == STANDARD_TYPE(Geom_Plane))
		{
			Handle(Geom_RectangularTrimmedSurface) aSurface =
				new Geom_RectangularTrimmedSurface(currentSur, Umin, Umax, Vmin, Vmax, Standard_True, Standard_True);
			faceConvert(aPoints, aSurface);
		}
		else if (type == STANDARD_TYPE(Geom_CylindricalSurface) || type == STANDARD_TYPE(Geom_ConicalSurface))
		{
			Handle(Geom_RectangularTrimmedSurface) aSurface =
				new Geom_RectangularTrimmedSurface(currentSur, 0, 2 * M_PI, -10000, 10000, Standard_True, Standard_True);
			faceConvert(aPoints, aSurface);
		}
		else
		{
			faceConvert(aPoints, currentSur);
		}

		// clear aPoints for next cycle
		aPoints.Clear();
	}
}

ON_Mesh* occConverter::meshConvert(TopoDS_Face aFace,TopLoc_Location mLoc)
{
	//check the face status
	if (aFace.IsNull()) {
		std::cout << "Face is Null" << endl;
		return NULL;
	}

	bool reversed = (aFace.Orientation() == TopAbs_REVERSED);

	//setting triangulation in occt
	TopLoc_Location L;
	Handle(Poly_Triangulation) facing = BRep_Tool::Triangulation(aFace, L);

	//check mesh status
	if (facing.IsNull()) {
		std::cout << "No triangulation created" << endl;
		return NULL;
	}

	//getting the number of triangles, nodes(vertices) & whether the face has normals
	int faceNum = facing->NbTriangles();
	int vertexNum = facing->NbNodes();
	bool hasNorm = facing->HasNormals();

	//cout << "面数：" << faceNum << endl;
	//cout << "点数：" << vertexNum << endl;

	//situation that the triangluation has normals
	if (hasNorm == true)
	{
		//according to the info above to generate an ON_Mesh object
		ON_Mesh* pMesh = new ON_Mesh(faceNum, vertexNum, hasNorm, false);

		//getting the nodes list
		TColgp_Array1OfPnt tPnts(1, vertexNum);
		tPnts = facing->Nodes();
		gp_Trsf aTrsf = L.Transformation();

		//transfer nodes in occt into vertices in openNurbs
		for (int i = 1; i <= vertexNum; i++)
		{
			gp_Pnt pnt = tPnts.Value(i).Transformed(aTrsf);

			double x = pnt.X();
			double y = pnt.Y();
			double z = pnt.Z();
			/*double x = tPnts.Value(i).X();
			double y = tPnts.Value(i).Y();
			double z = tPnts.Value(i).Z();*/

			ON_3dPoint point = ON_3dPoint(x, y, z);

			SetMinMaxPnt(m_MinCorner, point);
			SetMinMaxPnt(m_MaxCorner, point, false);

			pMesh->SetVertex(i - 1, point);

		}

		//getting the triangle list (poly_lists)
		Poly_Array1OfTriangle tTri(1, faceNum);
		tTri = facing->Triangles();

		//transfer triangles in occt into triangles in openNurbs
		for (int j = 1; j <= faceNum; j++)
		{
			int n1, n2, n3;
			tTri.Value(j).Get(n1, n2, n3);

			pMesh->SetTriangle(j - 1, n1 - 1, n2 - 1, n3 - 1);
		}

		//getting the normal talbe if normal exists
		if (facing->HasNormals())
		{
			const TShort_Array1OfShortReal& tNor = facing->Normals();

			//transfer node normals into openNurbs

			for (int i = 0; i < vertexNum; i++)
			{
				const Standard_ShortReal& nx = tNor(i * 3 + 1);
				const Standard_ShortReal& ny = tNor(i * 3 + 2);
				const Standard_ShortReal& nz = tNor(i * 3 + 3);

				double NX, NY, NZ;
				NX = nx; NY = ny; NZ = nz;

				ON_3dVector vec = ON_3dVector(NX, NY, NZ);

				pMesh->SetVertexNormal(i, vec);
			}

		}
		return pMesh;
	}

	//situation that the normals need to be calculated
	else
	{
		//according to the info above to generate an ON_Mesh object
		ON_Mesh* pMesh = new ON_Mesh(faceNum, vertexNum, true, false);

		//getting the nodes list
		TColgp_Array1OfPnt tPnts(1, vertexNum);
		tPnts = facing->Nodes();
		gp_Trsf aTrsf = L.Transformation();

		//transfer nodes in occt into vertices in openNurbs
		for (int i = 1; i <= vertexNum; i++)
		{
			gp_Pnt pnt = tPnts.Value(i).Transformed(aTrsf);

			double x = pnt.X();
			double y = pnt.Y();
			double z = pnt.Z();
			/*double x = tPnts.Value(i).X();
			double y = tPnts.Value(i).Y();
			double z = tPnts.Value(i).Z();*/

			ON_3dPoint point = ON_3dPoint(x, y, z);

			SetMinMaxPnt(m_MinCorner, point);
			SetMinMaxPnt(m_MaxCorner, point, false);

			pMesh->SetVertex(i - 1, point);

		}

		//getting the triangle list (poly_lists)
		Poly_Array1OfTriangle tTri(1, faceNum);
		tTri = facing->Triangles();

		//transfer triangles in occt into triangles in openNurbs
		for (int j = 1; j <= faceNum; j++)
		{
			int n1, n2, n3;
			tTri.Value(j).Get(n1, n2, n3);

			if(reversed)
			{
				pMesh->SetTriangle(j - 1, n2 - 1, n1 - 1, n3 - 1);
			}

			else
			{
				pMesh->SetTriangle(j - 1, n1 - 1, n2 - 1, n3 - 1);
			}

			
		}

		//calculating the node normals
		Handle_Geom_Surface surface = BRep_Tool::Surface(aFace);
		gp_Vec normal;

		//vertex normal calculating method for bounded surfaces
		if (facing->HasUVNodes()) {

			const TColgp_Array1OfPnt2d& uvnodes = facing->UVNodes();
			for (int i = 0; i < vertexNum; i++) {

				const gp_Pnt2d& uv = uvnodes(i + 1);
				double fU = uv.X();
				double fV = uv.Y();

				GeomLProp_SLProps faceprop(surface, fU, fV, 2, gp::Resolution());
				if (!faceprop.IsNormalDefined())
				{
					continue;
				}
				normal = faceprop.Normal();

				if (normal.SquareMagnitude() > 1.0e-10) {
					normal.Normalize();
				}

				/*gp_Vec uTan, vTan;
				gp_Vec checkVec = gp_Vec(0, 0, 0);
				gp_Pnt pnt3D;

				surface->D1(fU,fV,pnt3D,uTan,vTan);

				if (uTan.IsEqual(checkVec, gp::Resolution(), gp::Resolution()) || vTan.IsEqual(checkVec, gp::Resolution(), gp::Resolution()))
					cout << "Undefined surface is detected! Please check the model property!"<<endl;
					continue;

				uTan.Normalize(); vTan.Normalize();

				gp_Dir normal = uTan.Crossed(vTan);
				*/

				double nx, ny, nz;

				if (reversed)
				{
					normal *= -1.0;
				}

				nx = normal.X(); ny = normal.Y(); nz = normal.Z();

				ON_3dVector vec = ON_3dVector(nx, ny, nz);
				pMesh->SetVertexNormal(i, vec);
			}
		}

		//vertex normal calculating method for elementary surfaces
		else {
			for (int i = 0; i < vertexNum; i++) {

				const gp_Pnt& vertex = tPnts(i + 1);
				GeomAPI_ProjectPointOnSurf SrfProp(vertex, surface);

				Standard_Real fU, fV;
				SrfProp.Parameters(1, fU, fV);

				GeomLProp_SLProps faceprop(surface, fU, fV, 2, gp::Resolution());
				

				if (!faceprop.IsNormalDefined())
				{
					continue;
				}

				normal = faceprop.Normal();
				if (normal.SquareMagnitude() > 1.0e-10) {
					normal.Normalize();
				}

				if (reversed)
				{
					normal *= -1.0;
				}
				
				double nx, ny, nz;

				nx = normal.X(); ny = normal.Y(); nz = normal.Z();
				ON_3dVector vec = ON_3dVector(nx, ny, nz);
				pMesh->SetVertexNormal(i, vec);

			}
		}
		return pMesh;
	}
}

void occConverter::load(QTreeWidgetItem* topLevelItem, Handle(XCAFDoc_ShapeTool) aMainTool)
{
	TDF_LabelSequence aSeq;
	aMainTool->GetFreeShapes(aSeq);
	//std::cout << "The number of free shapes is: " << aSeq.Length()<<endl;
	TDF_LabelSequence compSeq;
	
	int scdsd = aSeq.Length();
	for (int i = 1; i <= aSeq.Length(); i++)
	{
		if (!aMainTool->GetComponents(aSeq.Value(i), compSeq))
		{
			//std::cout << "The import item is not an assembly" << endl;
			TopLoc_Location loc = aMainTool->GetLocation(aSeq.Value(i));
			//QString partName;
			getName(aSeq.Value(i), m_ImportPartName);
			m_pTree->setText(0, m_ImportPartName);
			m_pTree->setIcon(0, QIcon("../icons/Part.png"));
			QList<QTreeWidgetItem*> faceTree = loadPart(aSeq.Value(i), aMainTool, loc);

			m_pTree->addChildren(faceTree);
			m_itemList.append(m_pTree);
			continue;
		}

		QTreeWidgetItem* compWidget = new QTreeWidgetItem();
		QString compName;
		getName(aSeq.Value(i), compName);
		compWidget->setText(0, compName);
		TopoDS_Shape topShape = aMainTool->GetShape(aSeq.Value(i));
		TopLoc_Location loc = topShape.Location();
		QList<QTreeWidgetItem*> compItems = loadAssembly(aSeq.Value(i), aMainTool,loc);
		compWidget->addChildren(compItems);
		m_itemList.append(compWidget);

	}

}

QList<QTreeWidgetItem*> occConverter::loadAssembly(TDF_Label aLabel, Handle(XCAFDoc_ShapeTool) aMainTool, TopLoc_Location aLoc)
{
	QList<QTreeWidgetItem*> assemList;
	TDF_LabelSequence seqComp;
	Handle(XCAFDoc_Location) hLoc;
	TopLoc_Location compLoc;
	if (aMainTool->GetComponents(aLabel, seqComp))
	{
		Standard_Integer nbComp = seqComp.Length();
		//std::cout << "The number of components is: " << nbComp << endl;
		for (int i = 1; i <= seqComp.Length(); i++)
		{
			TDF_Label compLabel = seqComp.Value(i);
			QTreeWidgetItem* compWidget = new QTreeWidgetItem();
			QString compName;
			getName(compLabel, compName);
			compWidget->setText(0, compName);

			TDF_Label refLabel;
			bool ref;
			if (aMainTool->IsReference(compLabel) && aMainTool->GetReferredShape(compLabel, refLabel))
			{
				ref = true;
			}

			if (compLabel.FindAttribute(XCAFDoc_Location::GetID(), hLoc))
			{
				if (ref)
				{
					compLoc = aLoc * hLoc->Get();
					QList<QTreeWidgetItem*> nextLevelItems = loadAssembly(refLabel, aMainTool, compLoc);
					compWidget->addChildren(nextLevelItems);
					assemList.append(compWidget);
				}
			}
		}
	}
	else
	{
		QTreeWidgetItem* partWidget = new QTreeWidgetItem();
		QString partName;
		getName(aLabel, partName);
		partWidget->setText(0, partName);
		QList<QTreeWidgetItem*> part = loadPart(aLabel, aMainTool, aLoc);
		partWidget->addChildren(part);
		assemList.append(partWidget);
	}

	return assemList;
}

void occConverter::importStep(QString fileName)
{
	ClearPreviousSTEP();

	bool IsInvalid = fileName.contains(QRegExp("[\\x4e00-\\x9fa5]+"));
	if (IsInvalid)
	{
		QMessageBox::warning(0, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("无法读取含有中文路径的文件"), "OK");
		return;
	}

	//ProgressBar
	m_pProgBar->show();
	m_pProgBar->setLabelText(QString::fromLocal8Bit("初始化CAF_Reader"));
	m_pProgBar->setValue(10);

	// initialize step CAF reader for documentation structure
	STEPCAFControl_Reader cafReader;

	//ProgressBar
	m_pProgBar->setLabelText(QString::fromLocal8Bit("配置CAF_Reader"));
	m_pProgBar->setValue(20);

	// configure the CAF reader and documentation type
	cafReader.SetNameMode(true);
	Handle(TDocStd_Document) aDoc;
	Handle(XCAFApp_Application) anApp = XCAFApp_Application::GetApplication();
	anApp->NewDocument("MDTV-XCAF", aDoc);

	//ProgressBar
	m_pProgBar->setLabelText(QString::fromLocal8Bit("读取文件结构"));
	m_pProgBar->setValue(30);

	// reading documentation sturcture and transfer data into doc created before
	IFSelect_ReturnStatus read_status = cafReader.ReadFile(fileName.toLocal8Bit().data());
	cafReader.Transfer(aDoc);
	TDF_Label rootLabel = aDoc->Main();

	//ProgressBar
	m_pProgBar->setLabelText(QString::fromLocal8Bit("获取形状特征"));
	m_pProgBar->setValue(30);

	// create a shape tool to access the children label
	Handle(XCAFDoc_ShapeTool) shapeTool = XCAFDoc_DocumentTool::ShapeTool(rootLabel);
	m_pColorTool = XCAFDoc_DocumentTool::ColorTool(rootLabel);

	//ProgressBar
	m_pProgBar->setLabelText(QString::fromLocal8Bit("转化曲面网格"));
	m_pProgBar->setValue(50);
	
	// start loading the structure
	load(m_pTree, shapeTool);

	// 寻找次圆柱面
	SearchSubCylinder();
	
	//ProgressBar
	m_pProgBar->setLabelText(QString::fromLocal8Bit("STEP解析完成"));
	m_pProgBar->setValue(90);

	m_pProgBar->setLabelText(QString::fromLocal8Bit("完成"));
	m_pProgBar->setValue(100);
	m_pProgBar->autoClose();
	
	// print the displayed entities 
	int eCount = gWindow->GetMainDoc()->DispEntCount();
	for (int i = 0; i < eCount; i++)
	{
		//cout << "The display entity is: " << gWindow->GetMainDoc()->GetDispEnt(i) << endl;
	}

	gWindow->ResetToDefaultCountenance();

}

void occConverter::SearchSubCylinder()
{
	// 判断Edge是否相同
	function<bool(TopoDS_Edge, TopoDS_Edge)> IsEdgeTheSame = [=](TopoDS_Edge Edge1, TopoDS_Edge Edge2)->bool
	{
		double CurveStart1 = 0, CurveEnd1 = 0;

		BRep_Tool::Range(Edge1, CurveStart1, CurveEnd1);

		Handle(Geom_Curve) Curve1 = BRep_Tool::Curve(Edge1, CurveStart1, CurveEnd1);

		double CurveStart2 = 0, CurveEnd2 = 0;

		BRep_Tool::Range(Edge2, CurveStart2, CurveEnd2);

		Handle(Geom_Curve) Curve2 = BRep_Tool::Curve(Edge2, CurveStart2, CurveEnd2);

		if (Curve1.IsNull() || Curve2.IsNull())
		{
			return false;
		}

		gp_Pnt PtStartCurve1 = Curve1->Value(CurveStart1);
		gp_Pnt PtEndCurve1 = Curve1->Value(CurveEnd1);
		gp_Pnt PtMidCurve1 = Curve1->Value((CurveStart1 + CurveEnd1) / 2.0);

		gp_Pnt PtStartCurve2 = Curve2->Value(CurveStart2);
		gp_Pnt PtEndCurve2 = Curve2->Value(CurveEnd2);
		gp_Pnt PtMidCurve2 = Curve2->Value((CurveStart2 + CurveEnd2) / 2.0);

		bool IsStartOverlap = false;
		bool IsEndOverlap = false;
		bool IsMidOverlap = false;

		if (abs(PtStartCurve1.Distance(PtStartCurve2)) < 1e-6 || abs(PtStartCurve1.Distance(PtEndCurve2)) < 1e-6)
		{
			IsStartOverlap = true;
		}

		if (abs(PtEndCurve1.Distance(PtStartCurve2)) < 1e-6 || abs(PtEndCurve1.Distance(PtEndCurve2)) < 1e-6)
		{
			IsEndOverlap = true;
		}

		if (abs(PtMidCurve1.Distance(PtMidCurve2)) < 1e-6)
		{
			IsMidOverlap = true;
		}

		return IsStartOverlap && IsEndOverlap && IsMidOverlap;

	};

	// 判断Edge是否为直线
	function<bool(TopoDS_Edge)> IsEdgeTheLine = [](TopoDS_Edge the_edge)->bool
	{
		double curve_start = 0, curve_end = 0;
		BRep_Tool::Range(the_edge, curve_start, curve_end);
		Handle(Geom_Curve) the_curve = BRep_Tool::Curve(the_edge, curve_start, curve_end);

		if (the_curve->DynamicType() == STANDARD_TYPE(Geom_Line))
		{
			return true;
		}
		else
		{
			return false;
		}

	};

	// 判断是否为圆柱面
	function<bool(TopoDS_Face)> IsFaceCylinder = [](TopoDS_Face the_face)->bool
	{
		Handle(Geom_Surface) the_surf = BRep_Tool::Surface(the_face);

		if (the_surf->DynamicType() == STANDARD_TYPE(Geom_CylindricalSurface))
		{
			return true;
		}
		else
		{
			return false;
		}

	};

	// 判断是否在同一个圆柱
	function<bool(TopoDS_Face, TopoDS_Face)> IsCylinderTheSame = [](TopoDS_Face aFace, TopoDS_Face bFace)->bool
	{
		Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
		Handle(Geom_Surface) bSurf = BRep_Tool::Surface(bFace);

		Handle(Geom_CylindricalSurface) aCylsurf = Handle(Geom_CylindricalSurface)::DownCast(aSurf);
		Handle(Geom_CylindricalSurface) bCylsurf = Handle(Geom_CylindricalSurface)::DownCast(bSurf);

		gp_Pnt locA = aCylsurf->Axis().Location();
		gp_Dir dirA = aCylsurf->Axis().Direction();

		gp_Pnt locB = bCylsurf->Axis().Location();
		gp_Dir dirB = bCylsurf->Axis().Direction();

		bool IsAxisParallel = dirA.IsParallel(dirB, 0.001);

		bool IsAxisParallel2 = false;
		// 如果起点重合
		if (locA.Distance(locB) < 0.001)
		{
			IsAxisParallel2 = true;
		}
		else
		{
			gp_Dir testDir = gp_Dir(locA.X() - locB.X(), locA.Y() - locB.Y(), locA.Z() - locB.Z());

			IsAxisParallel2 = dirA.IsParallel(testDir, 0.001);
		}

		bool IsRadiusSame = (aCylsurf->Radius() == bCylsurf->Radius());

		if (IsAxisParallel && IsAxisParallel2 && IsRadiusSame)
		{
			return true;
		}
		else
		{
			return false;
		}

	};

	int nSize = m_TopoDS_SurfList.size();

	for (int i = 0; i < nSize - 1; i++)
	{
		// 先判断是不是圆柱面
		if (!IsFaceCylinder(m_TopoDS_SurfList[i])) continue;

		for (int j = i + 1; j < nSize; j++)
		{
			// 先判断是不是圆柱面
			if (!IsFaceCylinder(m_TopoDS_SurfList[j])) continue;

			// 先判断圆柱面轴线是否重合、直径是否相等
			if (!IsCylinderTheSame(m_TopoDS_SurfList[i], m_TopoDS_SurfList[j])) continue;

			vector<bool> AllLineIsOverlap;
			unordered_set<bool> IsAllLineOverlap;

			// 先考察index=i的面的所有边
			for (TopExp_Explorer aExpEdge(m_TopoDS_SurfList[i], TopAbs_EDGE); aExpEdge.More(); aExpEdge.Next())
			{
				TopoDS_Edge first_edge = TopoDS::Edge(aExpEdge.Current());

				// 如果是直线，则有可能是圆柱面邻接边线
				if (IsEdgeTheLine(first_edge))
				{
					bool IsThisEdgeOberlap = false;

					for (TopExp_Explorer aExpEdge2(m_TopoDS_SurfList[j], TopAbs_EDGE); aExpEdge2.More(); aExpEdge2.Next())
					{
						TopoDS_Edge second_edge = TopoDS::Edge(aExpEdge2.Current());

						if (IsEdgeTheLine(second_edge))
						{
							if (IsEdgeTheSame(first_edge, second_edge))
							{
								IsThisEdgeOberlap = true;

								break;
							}
						}
					}

					IsAllLineOverlap.insert(IsThisEdgeOberlap);
				}
			}

			// 如果所有直线边都有对应重叠边，则break，否则继续下一次循环
			
			if (IsAllLineOverlap.count(true))
			{
				m_SameCylinderNearby[i] = j;
				m_SameCylinderNearbyRev[j] = i;

				//cout << "找到一对同圆柱面" << endl;
			}
		}
	}

	//// 将j合并到i，并删除j
	//unordered_set<int> SecondIndex;

	//for (auto ita = MergeCylinderInds.begin(); ita != MergeCylinderInds.end(); ++ita)
	//{
	//	// 合并Mesh
	//	MeshList[(*ita).first]->Append(*(MeshList[(*ita).second]));

	//	SecondIndex.insert((*ita).second);
	//}

	//// 在MeshList和m_OcctSurfList中删除j
	//vector<ON_Mesh*> tmpMeshList;
	//vector<TopoDS_Face> tmpOcctSurfList;

	//for (int i = 0; i < nSize; i++)
	//{
	//	if (!SecondIndex.count(i))
	//	{
	//		tmpMeshList.push_back(MeshList[i]);

	//		tmpOcctSurfList.push_back(m_OcctSurfList[i]);
	//	}
	//}

	//MeshList = tmpMeshList;

	//m_OcctSurfList = tmpOcctSurfList;
}

QList<QTreeWidgetItem*> occConverter::loadPart(TDF_Label pLabel, Handle(XCAFDoc_ShapeTool) aMainTool, TopLoc_Location pLoc)
{
	TopoDS_Shape part;

	aMainTool->GetShape(pLabel, part);

	if (!pLoc.IsIdentity())
	{
		part = part.Moved(pLoc);
		//aMainTool->UpdateAssemblies();
	}

	// removes all the triangulations of the faces ,
	//and all the polygons on the triangulations of the edges
	BRepTools::Clean(part);

	// adds a new triangulation of the shape aShape with the deflection aDeflection
	Standard_Real aDeflection = 0.1;//初始0.1，数值越小Mesh精度越高
	BRepMesh_IncrementalMesh(part, aDeflection);

	QList<QTreeWidgetItem*> fList;

	initPartTree(fList);

	for (TopExp_Explorer aExpFace(part, TopAbs_FACE); aExpFace.More(); aExpFace.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(aExpFace.Current());

		ON_Mesh* pON_Mesh = this->meshConvert(aFace, pLoc);

		if (pON_Mesh == NULL) continue;
		if (!pON_Mesh->IsValid()) continue;

		// 提取颜色
		Quantity_Color faceColor;
		if (!m_pColorTool->GetColor(aFace, XCAFDoc_ColorSurf, faceColor))
		{
			faceColor.SetValues(Quantity_NOC_GRAY);
			//cout << "未提取到颜色" << faceColor.Red() << "," << faceColor.Green() << "," << faceColor.Blue() << endl;
		}
		else
		{
			//cout << "提取到颜色：" << faceColor.Red() << "," << faceColor.Green() << "," << faceColor.Blue() << endl;
		}

		////////
		FMesh* pMesh = new FMesh();
		pMesh->SetOnMesh(pON_Mesh);
		FSurface* pSurf = new FSurface(pMesh);

		pSurf->Update();

		// get the surface underlying the face
		Handle(Geom_Surface)  currentSur = BRep_Tool::Surface(aFace);

		SURFACE_TYPE flag = addFaceToTree(currentSur, fList, pSurf);

		FFace* pFace = new FFace();
		m_pBody->AddFace(pFace);

		pFace->SetSurface(pSurf);

		gWindow->GetMainDoc()->AddDispEnt(pFace);

		switch (flag)
		{
		case BSPLINE:m_BSplineInds.push_back(CurrentFaceIndex); m_Hashmap_BSplineInds[CurrentFaceIndex] = m_BSplineInds.size() - 1;
			break;
		case CONE:m_ConicalInds.push_back(CurrentFaceIndex); m_Hashmap_ConicalInds[CurrentFaceIndex] = m_ConicalInds.size() - 1;
			break;
		case CYLINDER:m_CylindricalInds.push_back(CurrentFaceIndex); m_Hashmap_CylindricalInds[CurrentFaceIndex] = m_CylindricalInds.size() - 1;
			break;
		case PLANE:m_PlaneInds.push_back(CurrentFaceIndex); m_Hashmap_PlaneInds[CurrentFaceIndex] = m_PlaneInds.size() - 1;
			break;
		case SPHERE:m_SphericalInds.push_back(CurrentFaceIndex); m_Hashmap_SphericalInds[CurrentFaceIndex] = m_SphericalInds.size() - 1;
			break;
		case OTHER_SURF:m_OtherInds.push_back(CurrentFaceIndex); m_Hashmap_OtherInds[CurrentFaceIndex] = m_OtherInds.size() - 1;
			break;
		default:m_OtherInds.push_back(CurrentFaceIndex); m_Hashmap_OtherInds[CurrentFaceIndex] = m_OtherInds.size() - 1;
			break;
		}

		// 保存邻接图，无邻接点
		gWindow->GetMainDoc()->GetSurfNearbyGraph().push_back(new SurfNearbyGraph_Node(CurrentFaceIndex));

		m_TopoDS_SurfList.push_back(aFace);
		m_FSurfList.push_back(pSurf);
		m_HashFSurf2Ind[pSurf] = m_FSurfList.size() - 1;
		m_pFaceColorDefault[pSurf] = faceColor;

		++CurrentFaceIndex;
	}

	return fList;
}


void occConverter::getName(TDF_Label label, QString& name)
{
	Handle(TDataStd_Name) aName;
	if (!label.FindAttribute(TDataStd_Name::GetID(), aName)) {
		//std::cout << "WARNING! The assembly do not have a name! A default name has been assigned." << endl;
		name = QString::fromLocal8Bit("Undefined_name_entity");
	}
	else
	{
		label.FindAttribute(TDataStd_Name::GetID(), aName);
		// transfer the occt string to C String
		TCollection_ExtendedString extstr = aName->Get();
		char* str = new char[extstr.LengthOfCString() + 1];
		extstr.ToUTF8CString(str);
		name = QString::fromStdString(str);
		//std::cout << "The name is: " << name.toLocal8Bit().data() << endl;
		delete[] str;

		//bool IsInvalid = name.contains(QRegExp("[\\x4e00-\\x9fa5]+"));
		//if (IsInvalid)
		//{
		//	QMessageBox::warning(0, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("零件名中包含中文字符，可能导致公差计算失败"), "OK");
		//}
	}
}


void occConverter::initPartTree(QList<QTreeWidgetItem*>& faceList)
{
	/// 1：BSpline Surface
	/// 2：圆锥面
	/// 3：圆柱面
	/// 4：球面
	/// 5：平面
	/// 6：其它型面
	QVariant SurfDynamicType;

	QTreeWidgetItem* BezierSurface = new QTreeWidgetItem();
	BezierSurface->setText(0, "Bezier Surface");

	QTreeWidgetItem* BSplineSurface = new QTreeWidgetItem();
	BSplineSurface->setText(0, QString::fromLocal8Bit("自由曲面"));
	BSplineSurface->setIcon(0, QIcon("../icons/BsplineSurface.png"));
	SurfDynamicType.setValue(1);
	BSplineSurface->setData(0, Qt::UserRole, SurfDynamicType);

	QTreeWidgetItem* ConicalSurface = new QTreeWidgetItem();
	ConicalSurface->setText(0, QString::fromLocal8Bit("圆锥面"));
	ConicalSurface->setIcon(0, QIcon("../icons/ConicalSurface.png"));
	SurfDynamicType.setValue(2);
	ConicalSurface->setData(0, Qt::UserRole, SurfDynamicType);

	QTreeWidgetItem* CylindricalSurface = new QTreeWidgetItem();
	CylindricalSurface->setText(0, QString::fromLocal8Bit("圆柱面"));
	CylindricalSurface->setIcon(0, QIcon("../icons/CylindricalSurface.png"));
	SurfDynamicType.setValue(3);
	CylindricalSurface->setData(0, Qt::UserRole, SurfDynamicType);

	QTreeWidgetItem* SphericalSurface = new QTreeWidgetItem();
	SphericalSurface->setText(0, QString::fromLocal8Bit("球面"));
	SphericalSurface->setIcon(0, QIcon("../icons/SphericalSurface.png"));
	SurfDynamicType.setValue(4);
	SphericalSurface->setData(0, Qt::UserRole, SurfDynamicType);

	QTreeWidgetItem* Plane = new QTreeWidgetItem();
	Plane->setText(0, QString::fromLocal8Bit("平面"));
	Plane->setIcon(0, QIcon("../icons/PlaneSurface.png"));
	SurfDynamicType.setValue(5);
	Plane->setData(0, Qt::UserRole, SurfDynamicType);

	QTreeWidgetItem* ToroidalSurface = new QTreeWidgetItem();
	ToroidalSurface->setText(0, "Toroidal Surface");

	QTreeWidgetItem* OffsetSurface = new QTreeWidgetItem();
	OffsetSurface->setText(0, "Offset Surface");

	QTreeWidgetItem* SurfaceOfLinearExtrusion = new QTreeWidgetItem();
	SurfaceOfLinearExtrusion->setText(0, "Linear Extrusion Surface");

	QTreeWidgetItem* SurfaceOfRevolution = new QTreeWidgetItem();
	SurfaceOfRevolution->setText(0, "Revolution Surface");

	QTreeWidgetItem* PlateSurface = new QTreeWidgetItem();
	PlateSurface->setText(0, "Plate Surface");

	QTreeWidgetItem* otherSurface = new QTreeWidgetItem();
	otherSurface->setText(0, QString::fromLocal8Bit("其它型面"));
	otherSurface->setIcon(0, QIcon("../icons/OtherSurface.png"));
	SurfDynamicType.setValue(6);
	otherSurface->setData(0, Qt::UserRole, SurfDynamicType);

	//faceList.append(BezierSurface);
	faceList.append(BSplineSurface);
	faceList.append(ConicalSurface);
	faceList.append(CylindricalSurface);
	faceList.append(Plane);
	faceList.append(SphericalSurface);
	faceList.append(otherSurface);
	//faceList.append(ToroidalSurface);
	//faceList.append(OffsetSurface);
	//faceList.append(SurfaceOfLinearExtrusion);
	//faceList.append(SurfaceOfRevolution);
	//faceList.append(PlateSurface);
}


SURFACE_TYPE occConverter::addFaceToTree(Handle(Geom_Surface) surf, QList<QTreeWidgetItem*>& faceList, FSurface* CurSurf)
{
	Handle_Standard_Type type = surf->DynamicType();
	
	if(type == STANDARD_TYPE(Geom_BSplineSurface))
	{
		QTreeWidgetItem* bspSurface = new QTreeWidgetItem();
		bspSurface->setData(0, Qt::UserRole, QVariant::fromValue((void*)(CurSurf)));
		QString text = QString::fromLocal8Bit("B样条曲面") + QString::number(m_BSplineInds.size());
		bspSurface->setText(0, text);
		faceList.at(0)->addChild(bspSurface);

		return BSPLINE;
	}

	else if(type == STANDARD_TYPE(Geom_ConicalSurface))
	{
		QTreeWidgetItem* conSurface = new QTreeWidgetItem();
		conSurface->setData(0, Qt::UserRole, QVariant::fromValue((void*)(CurSurf)));
		QString text = QString::fromLocal8Bit("圆锥面") + QString::number(m_ConicalInds.size());
		conSurface->setText(0, text);
		faceList.at(1)->addChild(conSurface);

		return CONE;
	}

	else if(type == STANDARD_TYPE(Geom_CylindricalSurface))
	{
		QTreeWidgetItem* cySurface = new QTreeWidgetItem();
		cySurface->setData(0, Qt::UserRole, QVariant::fromValue((void*)(CurSurf)));
		QString text = QString::fromLocal8Bit("圆柱面") + QString::number(m_CylindricalInds.size());
		cySurface->setText(0, text);
		faceList.at(2)->addChild(cySurface);

		return CYLINDER;
	}

	else if (type == STANDARD_TYPE(Geom_Plane))
	{
		QTreeWidgetItem* plSurface = new QTreeWidgetItem();
		plSurface->setData(0, Qt::UserRole, QVariant::fromValue((void*)(CurSurf)));
		QString text = QString::fromLocal8Bit("平面") + QString::number(m_PlaneInds.size());
		plSurface->setText(0, text);
		faceList.at(3)->addChild(plSurface);

		return PLANE;
	}

	else if (type == STANDARD_TYPE(Geom_SphericalSurface))
	{
		QTreeWidgetItem* spSurface = new QTreeWidgetItem();
		spSurface->setData(0, Qt::UserRole, QVariant::fromValue((void*)(CurSurf)));
		QString text = QString::fromLocal8Bit("球面") + QString::number(m_SphericalInds.size());
		spSurface->setText(0, text);
		faceList.at(4)->addChild(spSurface);

		return SPHERE;
	}

	else
	{
		QTreeWidgetItem* otSurface = new QTreeWidgetItem();
		otSurface->setData(0, Qt::UserRole, QVariant::fromValue((void*)(CurSurf)));
		QString text = QString::fromLocal8Bit("其他型面") + QString::number(m_OtherInds.size());
		otSurface->setText(0, text);
		faceList.at(5)->addChild(otSurface);

		return OTHER_SURF;
	}
}

void occConverter::SetMinMaxPnt(ON_3dPoint& TarPnt, ON_3dPoint CompPnt, bool IsMin)
{
	if (IsMin)
	{
		if (CompPnt.x < TarPnt.x)
		{
			TarPnt.x = CompPnt.x;
		}
		if (CompPnt.y < TarPnt.y)
		{
			TarPnt.y = CompPnt.y;
		}
		if (CompPnt.z < TarPnt.z)
		{
			TarPnt.z = CompPnt.z;
		}
	}
	else
	{
		if (CompPnt.x > TarPnt.x)
		{
			TarPnt.x = CompPnt.x;
		}
		if (CompPnt.y > TarPnt.y)
		{
			TarPnt.y = CompPnt.y;
		}
		if (CompPnt.z > TarPnt.z)
		{
			TarPnt.z = CompPnt.z;
		}
	}
}

void occConverter::ClearPreviousSTEP()
{
	// 重置当前面索引
	CurrentFaceIndex = 0;

}
