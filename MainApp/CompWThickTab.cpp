#include "CompWThickTab.h"
#include <QListWidget>
#include <QPushButton>
#include <QLayout>
#include <QTextEdit>
#include "FMainWindow.h"
#include "Tools/ErrCompsteTool.h"
#include "occConverter.h"
#include "np.h"
#include <Quantity_Color.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pln.hxx>
#include <gp_Trsf.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <TopoDS.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <STEPControl_StepModelType.hxx>
#include <STEPControl_Writer.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>

extern FMainWindow* gWindow;
extern QSettings* gDlgSetting;

// Function to compute the mirror point of A with respect to plane P
static gp_Pnt ComputeMirrorPoint(const gp_Pnt& A, const TopoDS_Face& P) {
	// ��ȡƽ��
	gp_Pln aPln = extractPln(P);
	gp_Trsf mirrorTransform;
	mirrorTransform.SetMirror(gp_Ax2(aPln.Location(), aPln.Axis().Direction()));
	gp_Pnt B = A.Transformed(mirrorTransform);
	return B;
}

static gp_Pnt ProjPntOnPln(const gp_Pnt& _pnt, const gp_Pln& _pln)
{
	// ��gp_Plnת��ΪHandle(Geom_Plane)
	Handle(Geom_Plane) geomPlane = new Geom_Plane(_pln);

	// ����ͶӰ��������ͶӰ
	GeomAPI_ProjectPointOnSurf projector;
	projector.Init(_pnt, geomPlane);

	if (projector.NbPoints() > 0) {
		gp_Pnt projectedPoint = projector.Point(1); // ��ȡ�����ͶӰ��
		return projectedPoint;
	}
	else {
		std::cout << "û���ҵ�ͶӰ��" << std::endl;
		return gp_Pnt();
	}

}

// Function to translate the plane P so that it passes through point B
static TopoDS_Face TranslatePlaneToPoint(const TopoDS_Face& P, const gp_Pnt& B) {
	gp_Pln plane = extractPln(P);

	// ��B���䵽P������ͶӰ��
	gp_Pnt origin = ProjPntOnPln(B, plane);
	gp_Vec translationVector(origin, B);

	gp_Trsf translationTransform;
	translationTransform.SetTranslation(translationVector);

	BRepBuilderAPI_Transform transform(translationTransform);
	transform.Perform(P, Standard_True);

	return TopoDS::Face(transform.Shape());
}

static int FSurfaceInd(FSurface* _surf)
{
	SURFACE_TYPE surfType;
	int indAll;
	int indLocal;
	QString surfName;

	gWindow->GetOccConverter()->FSurfInfo(_surf, surfType, indAll, indLocal, surfName);

	return indAll;
}

CompWThickTab::CompWThickTab(QWidget* parent): QWidget(parent)
{
	QPushButton* pCal = new QPushButton(QString::fromLocal8Bit("����"));
	QPushButton* pSaveCompsteModel = new QPushButton(QString::fromLocal8Bit("����ģ��"));

	QGridLayout* pLeftUpGLayout = new QGridLayout;
	pLeftUpGLayout->addWidget(pCal, 0, 0);
	pLeftUpGLayout->addWidget(pSaveCompsteModel, 0, 1);

	QVBoxLayout* pLeftVLayout = new QVBoxLayout;

	m_pResTextShow = new QTextEdit;
	pLeftVLayout->addLayout(pLeftUpGLayout);
	pLeftVLayout->addWidget(m_pResTextShow);

	m_pSelPlanesWidget1 = new CWTListWidget(this, 1, 1, 0, 0);
	m_pSelPlanesWidget2 = new CWTListWidget(this, 2, 0, 0, 1);

	QVBoxLayout* pRightVLayout = new QVBoxLayout;
	pRightVLayout->addWidget(new QLabel(QString::fromLocal8Bit("��һ������")));
	pRightVLayout->addWidget(m_pSelPlanesWidget1);
	pRightVLayout->addWidget(new QLabel(QString::fromLocal8Bit("�ڶ�������")));
	pRightVLayout->addWidget(m_pSelPlanesWidget2);

	QHBoxLayout* pMainHLayout = new QHBoxLayout(this);
	pMainHLayout->addLayout(pLeftVLayout);
	pMainHLayout->addLayout(pRightVLayout);

	connect(pCal, SIGNAL(clicked()), this, SLOT(onCal()));
	connect(pSaveCompsteModel, SIGNAL(clicked()), this, SLOT(onSave()));

}

CompWThickTab::~CompWThickTab()
{

}

void CompWThickTab::addPlane(FSurface* _surf, int _group)
{
	if (_group != 1 && _group != 2)
	{
		MesgBox2("�������");

		return;
	}

	std::unordered_set<FSurface*>* targetList;
	std::unordered_set<FSurface*>* anotherList;
	Quantity_Color selSurfColor;
	QListWidget* targetWidget;

	switch (_group)
	{
	case 1:
	{
		targetList = &m_SelPlanesList1;
		anotherList = &m_SelPlanesList2;
		targetWidget = m_pSelPlanesWidget1;
		selSurfColor.SetValues(Quantity_NOC_RED);

		break;
	}
	case 2:
	{
		targetList = &m_SelPlanesList2;
		anotherList = &m_SelPlanesList1;
		targetWidget = m_pSelPlanesWidget2;
		selSurfColor.SetValues(Quantity_NOC_CYAN);

		break;
	}
	default:
		break;
	}

	QString surfName = gWindow->GetOccConverter()->FSurfName(_surf);

	// ѡ��������һ�����������
	if (anotherList->count(_surf))
	{
		MesgBox2("ѡ��������һ����ѡ�е����棡");

		return;

	}

	// �ظ�ѡ��ȡ��ѡ�и���
	if (targetList->count(_surf))
	{
		gWindow->GetOccConverter()->setSurfColorDefault(_surf);
		targetList->erase(_surf);

		int itemCount = targetWidget->count();
		for (int i = 0; i < itemCount; ++i) {
			QListWidgetItem* item = targetWidget->item(i);
			if (item->text() == surfName)
			{
				targetWidget->takeItem(i);
				break;
			}
		}

	}
	// δ�ظ�ѡ�н�����ӵ��й�ϣ��
	else
	{
		const double& cR = selSurfColor.Red();
		const double& cG = selSurfColor.Green();
		const double& cB = selSurfColor.Blue();
		_surf->SetColor(cR, cG, cB);

		targetList->insert(_surf);

		// �ظ�ѡ��ȡ��ѡ�и���
		QListWidgetItem* ReListItem = new QListWidgetItem(surfName);
		//ReListItem->setData(3, QVariant::fromValue((void*)(_surf)));
		targetWidget->addItem(ReListItem);

	}

}

void CompWThickTab::setListWidgetBcgDef()
{
	m_pSelPlanesWidget1->setBcgDef();
	m_pSelPlanesWidget2->setBcgDef();

}

void CompWThickTab::selPlane1()
{
	ErrCompsteTool* compTool = new ErrCompsteTool(this, 3);
	compTool->setPlaneSelMode(1);

	gWindow->GetMainDoc()->PopTool();
	gWindow->GetMainDoc()->PushTool(compTool);

}

void CompWThickTab::selPlane2()
{
	ErrCompsteTool* compTool = new ErrCompsteTool(this, 3);
	compTool->setPlaneSelMode(2);

	gWindow->GetMainDoc()->PopTool();
	gWindow->GetMainDoc()->PushTool(compTool);

}

void CompWThickTab::onCal()
{
	if (!m_SelPlanesList1.size() || !m_SelPlanesList2.size())
	{
		MesgBox2("���ڿ�����");
		return;
	}

	// �ҵ�����ƽ������۷���
	TopoDS_Face tdsFace1 = gWindow->GetOccConverter()->FSurf2TDSFace(*m_SelPlanesList1.begin());
	TopoDS_Face tdsFace2 = gWindow->GetOccConverter()->FSurf2TDSFace(*m_SelPlanesList2.begin());

	Handle(Geom_Surface) surface = BRep_Tool::Surface(tdsFace1);
	Handle(Geom_Plane) geomPlane = Handle(Geom_Plane)::DownCast(surface);
	gp_Pln plnTheo1 = geomPlane->Pln();
	gp_Vec norm1 = plnTheo1.Axis().Direction();

	surface = BRep_Tool::Surface(tdsFace2);
	geomPlane = Handle(Geom_Plane)::DownCast(surface);
	gp_Pln plnTheo2 = geomPlane->Pln();
	gp_Vec norm2 = plnTheo2.Axis().Direction();

	// �жϷ����Ƿ��غ�
	double angle = norm1.Angle(norm2);
	if (!norm1.IsParallel(norm2, Precision::Angular()))
	{
		MesgBox2("ѡ��������治ƽ�У�");
		return;
	}

	// �ҵ�����ƽ���ϵĵ�
	const FIpPntList& theoPointsAll = gWindow->GetMainDoc()->GetIpPntList();
	const FIpPntList& measPointsAll = gWindow->GetMainDoc()->GetMesPntList();
	int nPnts = theoPointsAll.size();

	vector<gp_Pnt> pntsG1;
	vector<gp_Pnt> pntsG2;

	for (int i = 0; i < nPnts; i++)
	{
		if (isPointInFace(GP_PNT(theoPointsAll[i]), m_SelPlanesList1))
		{
			pntsG1.push_back(GP_PNT(measPointsAll[i]));
		}
		else if (isPointInFace(GP_PNT(theoPointsAll[i]), m_SelPlanesList2))
		{
			pntsG2.push_back(GP_PNT(measPointsAll[i]));
		}
		else
		{

		}

	}

	gp_Pnt mass1 = massCenter(pntsG1);
	gp_Pln pln1 = gp_Pln(mass1, norm1);

	gp_Pnt mass2 = massCenter(pntsG2);
	gp_Pln pln2 = gp_Pln(mass2, norm1);

	double distanceMeas = zeroLim(pln2.Distance(pln1.Location()));
	double distanceTheo = zeroLim(plnTheo2.Distance(plnTheo1.Location()));

	// ��ʾ�����
	QString resStr = QString::fromLocal8Bit("�������: \n%0\n\n�ں�����ֵ: \n%1\n\n�ں����ֵ: \n%2")
		// ��ʾ�������
		.arg(pntsG1.size() + pntsG2.size())

		// ��ʾ����ֵ
		.arg(distanceTheo)

		// ��ʾ����ֵ
		.arg(distanceMeas);

	m_pResTextShow->setPlainText(resStr);

	// ����������ֱ��ƫ����

	// ִ����ƫ�ƣ�ʵ���Ͼ��ǽ��о���)
	gp_Pnt mirror1 = ComputeMirrorPoint(mass1, tdsFace1);
	m_CompstePlanes1.clear();

	for (auto ita = m_SelPlanesList1.begin(); ita != m_SelPlanesList1.end(); ++ita)
	{
		TopoDS_Face cTdsFace = gWindow->GetOccConverter()->FSurf2TDSFace(*ita);
		TopoDS_Face translatedPln = TranslatePlaneToPoint(cTdsFace, mirror1);
		m_CompstePlanes1.push_back(translatedPln);

	}

	gp_Pnt mirror2 = ComputeMirrorPoint(mass2, tdsFace2);
	m_CompstePlanes2.clear();

	for (auto ita = m_SelPlanesList2.begin(); ita != m_SelPlanesList2.end(); ++ita)
	{
		TopoDS_Face cTdsFace = gWindow->GetOccConverter()->FSurf2TDSFace(*ita);
		TopoDS_Face translatedPln = TranslatePlaneToPoint(cTdsFace, mirror2);
		m_CompstePlanes2.push_back(translatedPln);

	}

}

void CompWThickTab::onSave()
{
	//�������ݵ�����
	QString defaultModelDir = gDlgSetting->value("SurfCompModelSavePath").toString();
	QString filename = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("���油����ģ���ļ�"), defaultModelDir, tr("step files (*.STEP *.step *.stp)"));

	if (!filename.isEmpty())
	{
		gDlgSetting->setValue("SurfCompModelSavePath", filename);

		// �ҵ���m_SelPlanesList1��m_SelPlanesList2��������
		unordered_set<int> allSelInds;

		QProgressDialog* progBar = new QProgressDialog;
		QFont ProgressFont;
		ProgressFont.setFamily("Microsoft YaHei");
		progBar->setFont(ProgressFont);
		progBar->show();
		progBar->setRange(0, 100);
		progBar->setValue(0);
		progBar->setCancelButton(NULL);
		progBar->setLabelText(QString::fromLocal8Bit("STEP �ļ�������"));

		for (auto ita = m_SelPlanesList1.begin(); ita != m_SelPlanesList1.end(); ++ita)
		{
			allSelInds.insert(FSurfaceInd(*ita));
		}

		progBar->setValue(10);

		for (auto ita = m_SelPlanesList2.begin(); ita != m_SelPlanesList2.end(); ++ita)
		{
			allSelInds.insert(FSurfaceInd(*ita));
		}

		progBar->setValue(20);

		vector<TopoDS_Face> tdsFaceAll = gWindow->GetOccConverter()->GetTopoDS_FaceList();

		Handle(TopTools_HSequenceOfShape) aHSequenceOfShape = new TopTools_HSequenceOfShape;
		aHSequenceOfShape->Clear();

		int nFaceSize = tdsFaceAll.size();
		for (int i = 0; i < nFaceSize; i++)
		{
			if (!allSelInds.count(i))
			{
				aHSequenceOfShape->Append(tdsFaceAll[i]);
			}
		}

		progBar->setValue(30);

		for (auto ita = m_CompstePlanes1.begin(); ita != m_CompstePlanes1.end(); ++ita)
		{
			aHSequenceOfShape->Append(*ita);
		}

		for (auto ita = m_CompstePlanes2.begin(); ita != m_CompstePlanes2.end(); ++ita)
		{
			aHSequenceOfShape->Append(*ita);
		}

		progBar->setValue(40);

		IFSelect_ReturnStatus status;
		STEPControl_StepModelType aValue = STEPControl_AsIs;
		STEPControl_Writer aWriter;

		int cProg = progBar->value();
		for (Standard_Integer i = 1; i <= aHSequenceOfShape->Length(); i++)
		{
			//ת����άģ�͵�aWriter
			status = aWriter.Transfer(aHSequenceOfShape->Value(i), aValue);
			if (status != IFSelect_RetDone)
				std::cout << "ת��ʧ��" << endl;

			progBar->setValue(cProg + (double(i) / aHSequenceOfShape->Length()) * 50);
		}

		if (aWriter.Model().IsNull())
		{
			std::cout << "���κ���״�ɱ���" << endl;
			return;
		}

		status = aWriter.Write(filename.toLocal8Bit().data());

		progBar->setValue(100);
		progBar->autoClose();

	}

}

CompWThickTab::CWTListWidget::CWTListWidget(CompWThickTab* _tab, const int& _ind, const double& _r, const double& _g, const double& _b)
{
	m_pTabWidget = _tab;
	m_nInd = _ind;

	m_bcgR = _r;
	m_bcgG = _g;
	m_bcgB = _b;

	m_bcgDefR = 1;
	m_bcgDefG = 1;
	m_bcgDefB = 1;

	setStyleSheet(getStyleSheet(m_bcgDefR, m_bcgDefG, m_bcgDefB));

}

CompWThickTab::CWTListWidget::~CWTListWidget()
{

}

void CompWThickTab::CWTListWidget::setBcgDef()
{
	setStyleSheet(getStyleSheet(m_bcgDefR, m_bcgDefG, m_bcgDefB));
}

void CompWThickTab::CWTListWidget::mousePressEvent(QMouseEvent* event)
{
	// ���û����mousePressEvent����ȷ���������¼�����
	QListWidget::mousePressEvent(event);

	m_pTabWidget->setListWidgetBcgDef();

	// �������������Ҫִ�еĲ���
	if (event->button() == Qt::LeftButton) 
	{
		setStyleSheet(getStyleSheet(m_bcgR, m_bcgG, m_bcgB));

		switch (m_nInd)
		{
		case 1:
		{
			m_pTabWidget->selPlane1();

			break;
		}
		case 2:
		{
			m_pTabWidget->selPlane2();
			break;
		}
		default:
			break;
		}

	}

}

QString CompWThickTab::CWTListWidget::getStyleSheet(double _r, double _g, double _b)
{
	int cR = 255 * _r;
	int cG = 255 * _g;
	int cB = 255 * _b;

	QString style =
		QString("QListWidget{border:1px solid black; background-color: rgba(%1, %2, %3, 50); }").arg(cR).arg(cG).arg(cB) +
		"QListWidget::Item{padding-top:12px; padding-bottom:12px; }"
		"QListWidget::Item:hover{background:skyblue; }"
		"QListWidget::item:selected{background:rgb(204,213,240); color:rgb(59,79,129); }"
		"QListWidget::item:selected:!active{border-width:0px; background:rgb(204,213,240); }"
		
		// ������
		"QScrollBar:vertical"
		"{"
		"width:22px;"
		"background:rgba(0,0,0,0%);"
		"margin:0px,0px,0px,0px;"
		"padding-top:18px;"
		"padding-bottom:18px;"
		"padding-right:5px;"
		"padding-left:5px;"
		"background:rgba(232,232,236,100%);"
		"}"
		// ���������˱����Բ
		"QScrollBar::handle:vertical"
		"{"
		"width:8px;"
		"background:rgba(0,0,0,25%);"
		"min-height:20;"
		"max-width:10;"
		"}"
		// ���ŵ��������ϵ�ʱ����ɫ����
		"QScrollBar::handle:vertical:hover"
		"{"
		"width:8px;"
		"background:rgba(0,0,0,50%);"
		"min-height:20;"
		"}"
		// ���Ӧ���������¼�ͷ�ģ�3.png���Ǽ�ͷ
		"QScrollBar::add-line:vertical"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollDown.png);"
		"subcontrol-position:bottom;"
		"}"
		// �����ϼ�ͷ
		"QScrollBar::sub-line:vertical"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollUp.png);"
		"subcontrol-position:top;"
		"}"
		// �����ŵ��¼�ͷ�ϵ�ʱ��
		"QScrollBar::add-line:vertical:hover"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollDown2.png);"
		"subcontrol-position:bottom;"
		"}"
		// �����ŵ��¼�ͷ�ϵ�ʱ��
		"QScrollBar::sub-line:vertical:hover"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollUp2.png);"
		"subcontrol-position:top;"
		"}"
		// ��������������ʱ������Ĳ��ֺ�����Ĳ���
		"QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical"
		"{"
		"background:rgba(232,232,236,100%);"
		"}"

		//����
		"QScrollBar:horizontal"
		"{"
		"height:22px;"
		"background:rgba(0,0,0,0%);"
		"margin:0px,0px,0px,0px;"
		"padding-top:5px;"
		"padding-bottom:5px;"
		"padding-right:18px;"
		"padding-left:18px;"
		"background:rgba(232,232,236,100%);"
		"}"
		// ���������˱����Բ
		"QScrollBar::handle:horizontal"
		"{"
		"height:8px;"
		"background:rgba(0,0,0,25%);"
		"min-width:10;"
		"max-height:20;"
		"}"
		// ���ŵ��������ϵ�ʱ����ɫ����
		"QScrollBar::handle:horizontal:hover"
		"{"
		"height:8px;"
		"background:rgba(0,0,0,50%);"
		"min-width:20;"
		"}"
		// �������ͷ�ģ�3.png���Ǽ�ͷ
		"QScrollBar::sub-line:horizontal"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollLeft.png);"
		"subcontrol-position:left;"
		"}"
		// �����Ҽ�ͷ
		"QScrollBar::add-line:horizontal"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollRight.png);"
		"subcontrol-position:right;"
		"}"
		// �����ŵ����ͷ�ϵ�ʱ��
		"QScrollBar::sub-line:horizontal:hover"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollLeft2.png);"
		"subcontrol-position:left;"
		"}"
		// �����ŵ��Ҽ�ͷ�ϵ�ʱ��
		"QScrollBar::add-line:horizontal:hover"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollRight2.png);"
		"subcontrol-position:right;"
		"}"
		// ��������������ʱ������Ĳ��ֺ�����Ĳ���
		"QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal"
		"{"
		"background:rgba(232,232,236,100%);"
		"}";

	return style;

}
