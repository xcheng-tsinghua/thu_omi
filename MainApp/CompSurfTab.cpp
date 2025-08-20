#include "CompSurfTab.h"
#include "MainDoc.h"
#include "FMainWindow.h"
#include "occConverter.h"
#include <GeomAPI_PointsToBSpline.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <GeomAPI_IntCS.hxx>
#include <Geom_OffsetSurface.hxx>
#include <iostream>
#include "Tools/ErrCompsteTool.h"
#include "np.h"
#include <TopoDS_Face.hxx>
#include <GeomPlate_BuildPlateSurface.hxx>
#include <GeomPlate_MakeApprox.hxx>
#include <Geom_Line.hxx>
#include "ErrCompsteDlg.h"
#include <TopTools_HSequenceOfShape.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <STEPControl_StepModelType.hxx>
#include <STEPControl_Writer.hxx>

extern FMainWindow* gWindow;
extern QSettings* gDlgSetting;

CompSurfTab::CompSurfTab(QWidget* parent) : QWidget(parent)
{
	// ��ʼ������
	m_pRecstBSplSurf = NULL;
	m_pRecstFace = NULL;
	m_pCompsedBSplSurf = NULL;
	m_pCompsedFace = NULL;
	m_pSelBSplSurf = NULL;
	m_pSelFSurf = NULL;
	m_ErrEvaluSecUV = 10;
	m_is_out_compsurf_only = true;

	QPushButton* pFaceSelBnt = new QPushButton(QString::fromLocal8Bit("ѡ��ӹ���"));
	m_pSelFaceNameLabel = new QLabel(QString::fromLocal8Bit("δѡ��"));
	QHBoxLayout* SelFaceLayout = new QHBoxLayout;
	SelFaceLayout->addWidget(pFaceSelBnt);
	SelFaceLayout->addWidget(m_pSelFaceNameLabel);

	QPushButton* pRecstMeasFaceBtn = new QPushButton(QString::fromLocal8Bit("�ع�������"));

	QLabel* pMachiningAllowanceLabel = new QLabel(QString::fromLocal8Bit("�ӹ�����"));
	m_pMachiningAllowance = new QLineEdit(gDlgSetting->value("MachiningAllowance").toString());
	QHBoxLayout* AllowanceLayout = new QHBoxLayout;
	AllowanceLayout->addWidget(pMachiningAllowanceLabel);
	AllowanceLayout->addWidget(m_pMachiningAllowance);

	QPushButton* pFaceCompseBtn = new QPushButton(QString::fromLocal8Bit("���ɲ�����"));
	QPushButton* pSaveBtn = new QPushButton(QString::fromLocal8Bit("���油��ģ��"));

	QVBoxLayout* MainVBoxLayout = new QVBoxLayout(this);
	MainVBoxLayout->addStretch();
	MainVBoxLayout->addLayout(SelFaceLayout);
	MainVBoxLayout->addStretch();
	MainVBoxLayout->addWidget(pRecstMeasFaceBtn);
	MainVBoxLayout->addStretch();
	MainVBoxLayout->addLayout(AllowanceLayout);
	MainVBoxLayout->addStretch();
	MainVBoxLayout->addWidget(pFaceCompseBtn);
	MainVBoxLayout->addStretch();
	MainVBoxLayout->addWidget(pSaveBtn);
	MainVBoxLayout->addStretch();

	connect(pFaceSelBnt, SIGNAL(clicked()), this, SLOT(onFaceSel()));
	connect(pRecstMeasFaceBtn, SIGNAL(clicked()), this, SLOT(onRecstMeasFace()));
	connect(pFaceCompseBtn, SIGNAL(clicked()), this, SLOT(onGenCompadSurf()));
	connect(pSaveBtn, SIGNAL(clicked()), this, SLOT(onSaveCompensatedModel()));

}

CompSurfTab::~CompSurfTab(void)
{

}

void CompSurfTab::setSurface(FSurface* _surf)
{
	Handle(Geom_BSplineSurface) SelBSplineSurf = gWindow->GetSTEPModelConverter()->FSurf2BSpline(_surf);
	m_pSelBSplSurf = SelBSplineSurf;
	m_pSelFSurf = _surf;

	gWindow->ResetToDefaultCountenance();

	_surf->SetColor(1, 0, 0);

	SURFACE_TYPE tmpType;
	int indAllList;
	int indTypeList;
	QString SurfName;
	gWindow->GetSTEPModelConverter()->FSurfInfo(_surf, tmpType, indAllList, indTypeList, SurfName);

	m_pSelFaceNameLabel->setText(SurfName);

	gWindow->UpdateModelView();

}

void CompSurfTab::onRecstMeasFace()
{
	// ֱ�ӴӲ����ȡ���
	FIpPntList measPntAll = gWindow->GetMainDoc()->GetMesPntList();

	if (measPntAll.size() == 0)
	{
		MesgBox(QString::fromLocal8Bit("δ����ʵ�������ݣ�"));
		return;
	}

	// �ҵ��������ϵĵ�
	vector<gp_Pnt> facePnts;
	TopoDS_Face selTDSFace = gWindow->GetSTEPModelConverter()->FSurf2TDSFace(m_pSelFSurf);

	for (auto ita = measPntAll.begin(); ita != measPntAll.end(); ++ita)
	{
		if (isPointInShape(GP_PNT((*ita)), selTDSFace)) facePnts.push_back(GP_PNT((*ita)));
	}

	if (facePnts.size() <= 3)
	{
		MesgBox(QString::fromLocal8Bit("��Ч����С��3���޷����������ع�"));
		return;
	}

	// �ع�����
	m_pRecstBSplSurf = fit_BSplSurfLeastSquare(facePnts);

	if (!m_pRecstBSplSurf)
	{
		MesgBox(QString::fromLocal8Bit("�ع�ʧ��"));
	}
	else
	{
		if (!m_pRecstFace) gWindow->GetMainDoc()->RemoveDispEnts(m_pRecstFace, true);
		m_pRecstFace = BSpSurf2FSurf(m_pRecstBSplSurf);

		// ��ʾ����
		m_pRecstFace->SetColor(0, 1, 0);
		gWindow->GetMainDoc()->AddDispEnt(m_pRecstFace);
	}

}

void CompSurfTab::onGenCompadSurf()
{
	double machiningAllowance = m_pMachiningAllowance->text().toDouble();
	gDlgSetting->setValue("MachiningAllowance", machiningAllowance);

	gp_Dir zBack(0, 0, 1.0);
	TColgp_Array2OfPnt pnts_MirrorSurfConstruct(1, m_ErrEvaluSecUV, 1, m_ErrEvaluSecUV);

	double uStart = 0, uEnd = 0, vStart = 0, vEnd = 0;
	SurfParaSpace(m_pSelBSplSurf, uStart, uEnd, vStart, vEnd);
	vector<double> uParas = linspace(uStart, uEnd, m_ErrEvaluSecUV);
	vector<double> vParas = linspace(vStart, vEnd, m_ErrEvaluSecUV);

	for (int i = 0; i < m_ErrEvaluSecUV; ++i)//�����뾫�ӹ������Ͼ��ȷֲ���
	{
		for (int j = 0; j < m_ErrEvaluSecUV; ++j)
		{
			// ���������ϲ�����
			double c_parau = uParas[i];
			double c_parav = vParas[j];
			gp_Pnt pnt_SelSurf = m_pSelBSplSurf->Value(c_parau, c_parav);

			// �����������ع�����
			Handle(Geom_Line)intersecLine = new Geom_Line(pnt_SelSurf.XYZ() - gp_Vec(0, 0, 10000).XYZ(), zBack);
			GeomAPI_IntCS intersecRes = GeomAPI_IntCS(intersecLine, m_pRecstBSplSurf);

			if (intersecRes.IsDone() && intersecRes.NbPoints() > 0)
			{
				gp_Pnt intersecPnt_RecstSurf = intersecRes.Point(1);

				// �Խ���Ϊ���ģ������ع����ϵĲ����㣬���ڼӹ������Ĵ��ڣ�������� Z Ӧ�ü��ϼӹ�����
				double machiningErr = intersecPnt_RecstSurf.Z() - (pnt_SelSurf.Z() + machiningAllowance);
				double mirroredPntZ = pnt_SelSurf.Z() - machiningErr;
				gp_Pnt pnt_MirrorSurf = gp_Pnt(intersecPnt_RecstSurf.X(), intersecPnt_RecstSurf.Y(), mirroredPntZ);

				pnts_MirrorSurfConstruct.SetValue(i + 1, j + 1, pnt_MirrorSurf);

			}
			else
			{
				continue;
			}

		}

	}

	m_pCompsedBSplSurf = GeomAPI_PointsToBSplineSurface(pnts_MirrorSurfConstruct, 2, 2, GeomAbs_C2, 0.0000001);

	if (!m_pCompsedFace) gWindow->GetMainDoc()->RemoveDispEnts(m_pCompsedFace, true);
	m_pCompsedFace = BSpSurf2FSurf(m_pCompsedBSplSurf);
	gWindow->GetMainDoc()->AddDispEnt(m_pCompsedFace);

}

void CompSurfTab::onSaveCompensatedModel()
{
	// ��������ת��Ϊ TopoDS_Face
	TopoDS_Face comp_tds_face = BRepBuilderAPI_MakeFace(m_pCompsedBSplSurf, Precision::Confusion());

	Handle(TopTools_HSequenceOfShape) aHSequenceOfShape = new TopTools_HSequenceOfShape;
	aHSequenceOfShape->Clear();

	// �����浥�������������
	if (m_is_out_compsurf_only)
	{
		aHSequenceOfShape->Append(comp_tds_face);
	}
	// ����ȫ�����棨�����������ģ���滻Ϊʵ��ģ�ͣ�
	else
	{
		// �ҵ�����ѡ����֮���������
		SURFACE_TYPE surfType;
		int indAll;
		int indLocal;
		QString surfName;

		gWindow->GetOccConverter()->FSurfInfo(m_pSelFSurf, surfType, indAll, indLocal, surfName);
		vector<TopoDS_Face> tdsFaceAll = gWindow->GetOccConverter()->GetTopoDS_FaceList();

		int nFaceSize = tdsFaceAll.size();
		for (int i = 0; i < nFaceSize; i++)
		{
			if (i == indAll)
			{
				aHSequenceOfShape->Append(comp_tds_face);
			}
			else
			{
				aHSequenceOfShape->Append(tdsFaceAll[i]);
			}
		}

	}

	IFSelect_ReturnStatus status;
	STEPControl_StepModelType aValue = STEPControl_AsIs;
	STEPControl_Writer aWriter;

	for (Standard_Integer i = 1; i <= aHSequenceOfShape->Length(); i++)
	{
		//ת����άģ�͵�aWriter
		status = aWriter.Transfer(aHSequenceOfShape->Value(i), aValue);
		if (status != IFSelect_RetDone)
			std::cout << "ת��ʧ��" << endl;
	}

	if (aWriter.Model().IsNull())
	{
		std::cout << "���κ���״�ɱ���" << endl;
		return;
	}

	//�������ݵ�����
	
	QString defaultModelDir = gDlgSetting->value("SurfCompModelSavePath").toString();
	QString filename = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("ѡ��ģ���ļ�"), defaultModelDir, tr("Mesh/Point files (*.obj *.stl *.step *.mda *.stp)"));

	if (!filename.isEmpty())
	{
		gDlgSetting->setValue("SurfCompModelSavePath", filename);
		status = aWriter.Write(filename.toLocal8Bit().data());
	}

}

void CompSurfTab::onFaceSel()
{
	ErrCompsteTool* compTool = new ErrCompsteTool(this, 1);

	gWindow->GetMainDoc()->PopTool();
	gWindow->GetMainDoc()->PushTool(compTool);

}





