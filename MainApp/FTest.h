/*	
\brief	Test various new classes. 
\author: Jacky
\date    2014-05
*/
#ifndef F_TEST_H
#define F_TEST_H

#include "../External/fKernel/include/Fkerneldefines.h"
#include "../External/fKernel/include/FKernelHeader.h"
#include <qwidget.h>

//////////////////////////////////////////////////////////////////////////
using namespace FT;

class FMainDoc;
class QLineEdit;

class FTest : public QWidget
{
	Q_OBJECT
public:
	FTest(QWidget* parent = NULL);
	~FTest();

private slots:
	void onTest();
	void onFileCompare();
	void onInterp();

public:
	void	CreateArc(FMainDoc* pDoc);
	void	CreateBoundingBox(FMainDoc* pDoc);
	void	CreateFreeNurbsCurve(FMainDoc* pDoc);
	void	CreateFreeCurve(FMainDoc* pDoc);
	void	CreateNurbsSurface(FMainDoc* pDoc);
	void	CreateCompatibleCurves(FMainDoc* pDoc);
	void	CreateSurfaceCurve(FMainDoc* pDoc);
	void	CreateISOCurve(FMainDoc* pDoc);
	void	CreateCircle(FMainDoc* pDoc);
	void	CreatePolygon(FMainDoc* pDoc);

	void	CreateFreeBody(FMainDoc* pDoc);
	void	CreateWireCube(FMainDoc* pDoc, double len);	//Make a wire cube with Euler operators.
	void	CreateSolidCube(FMainDoc* pDoc, double len);
	void	CreateCylinder(FMainDoc* pDoc, double radius, double h);
	void	CreateSphere(FMainDoc* pDoc, double r);
	void 	CreateTorus(FMainDoc* pDoc, double majorR, double minorR);

	void	CreateFrame(FMainDoc* pDoc);
	void	CreateFramePattern(FMainDoc* pDoc);
	void	CreateCircleHandle(FMainDoc* pDoc);

	void	CreateSLCFile(FMainDoc* pDoc);
	void	CreateTextFeature(FMainDoc* pDoc);

	void	TestFileIO();
	void	ReadStepFile(FMainDoc* pDoc);
	void	TestRibRecognition(FMainDoc*pDoc);
	void	TestPathPlanning(FMainDoc* pDoc);
	void	TestLineMeshIntersection(FMainDoc* pDoc);
	void	TestPointsFromFace(FMainDoc* pDoc);
	void    TestPointSetting(FMainDoc* pDoc);

private:
	QLineEdit* m_pTol3d_Real;
	QLineEdit* m_pNbmax_Int;
	QLineEdit* m_pdgmax_Int;
	QLineEdit* m_pdmax_Real;
	QLineEdit* m_pCritOrder_Int;
	QLineEdit* m_pContinuity_Str;
	QLineEdit* m_pEnlargeCoeff_Real;

};

#endif