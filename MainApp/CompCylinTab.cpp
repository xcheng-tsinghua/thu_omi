#include "CompCylinTab.h"
#include <QListWidget>
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include "ErrCompsteDlg.h"
#include "FMainWindow.h"
#include "Tools/ErrCompsteTool.h"
#include "occConverter.h"
#include "np.h"
#include <Eigen\Dense>
#include <BRep_Tool.hxx>
#include <gp_Pln.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <BRepIntCurveSurface_Inter.hxx>

using namespace Eigen;
extern FMainWindow* gWindow;

CompCylinTab::CompCylinTab(QWidget* parent): QWidget(parent)
{
	// ��ʼ������
	m_pBasePlane = NULL;
	m_SelMode = 0;

	// ��ʼ������
	QPushButton* pCalBtn = new QPushButton(QString::fromLocal8Bit("����"));
	QPushButton* pSaveBtn = new QPushButton(QString::fromLocal8Bit("����"));

	QVBoxLayout* pLeftGLayout = new QVBoxLayout;
	QGridLayout* pLeftUpGLayout = new QGridLayout;
	pLeftUpGLayout->addWidget(pCalBtn, 0, 0);
	pLeftUpGLayout->addWidget(pSaveBtn, 0, 1);

	m_pResTextShow = new QTextEdit;
	pLeftGLayout->addLayout(pLeftUpGLayout);
	pLeftGLayout->addWidget(m_pResTextShow);

	m_pSurfListWidget = new CCYListWidget(this, 1, 1, 0, 0);
	m_pSurfListWidget->setFrameStyle(QFrame::Panel | QFrame::Raised);

	m_pBasePlaneListWidget = new CCYListWidget(this, 2, 0, 0, 1);
	m_pBasePlaneListWidget->setFrameStyle(QFrame::Panel | QFrame::Raised);

	QVBoxLayout* pRightVLayout = new QVBoxLayout;
	pRightVLayout->addWidget(new QLabel(QString::fromLocal8Bit("����")));
	pRightVLayout->addWidget(m_pSurfListWidget);
	pRightVLayout->addWidget(new QLabel(QString::fromLocal8Bit("����")));
	pRightVLayout->addWidget(m_pBasePlaneListWidget);

	QHBoxLayout* pMainHLayout = new QHBoxLayout(this);

	pMainHLayout->addLayout(pLeftGLayout);
	pMainHLayout->addLayout(pRightVLayout);

	connect(pCalBtn, SIGNAL(clicked()), this, SLOT(onCal()));

}

CompCylinTab::~CompCylinTab()
{

}

QListWidget* CompCylinTab::getListWidget()
{
	return m_pSurfListWidget;
}

void CompCylinTab::addSurf(FSurface* _surf)
{
	switch (m_SelMode)
	{
	case 1:
	{

		if (!isSurfType(_surf, CYLINDER))
		{
			MesgBox2("��ѡ��Բ����");
			return;
		}

		QString surfName = gWindow->GetOccConverter()->FSurfName(_surf);

		// �ظ�ѡ��ȡ��ѡ�и���
		if (m_SelSurf.count(_surf))
		{
			// ��ɫ�ָ���Ĭ��
			gWindow->GetOccConverter()->setSurfColorDefault(_surf);

			// ��ϣ����ɾ������Ŀ
			m_SelSurf.erase(_surf);

			// �б������ɾ������Ŀ
			removeListItem(m_pSurfListWidget, _surf);

		}
		// δ�ظ�ѡ�н�����ӵ��й�ϣ��
		else
		{
			_surf->SetColor(1, 0, 0);

			m_SelSurf.insert(_surf);

			// �����Ŀ���б����
			QListWidgetItem* ReListItem = new QListWidgetItem(surfName);
			ReListItem->setData(Qt::UserRole, QVariant::fromValue((void*)(_surf)));
			m_pSurfListWidget->addItem(ReListItem);

		}

		break;
	}
	case 2:
	{
		if (!isSurfType(_surf, PLANE))
		{
			MesgBox2("��ѡ��ƽ��");
			return;
		}

		if (m_pBasePlane)
		{
			gWindow->GetOccConverter()->setSurfColorDefault(m_pBasePlane);
		}

		QString surfName = gWindow->GetOccConverter()->FSurfName(_surf);

		m_pBasePlane = _surf;

		m_pBasePlaneListWidget->clear();

		_surf->SetColor(0, 1, 1);

		// �����Ŀ���б����
		QListWidgetItem* ReListItem = new QListWidgetItem(surfName);
		ReListItem->setData(Qt::UserRole, QVariant::fromValue((void*)(_surf)));
		m_pBasePlaneListWidget->addItem(ReListItem);

		break;
	}
	default:
		break;
	}

}

void CompCylinTab::setListWidgetBcgDef()
{
	m_pSurfListWidget->setBcgDef();
	m_pBasePlaneListWidget->setBcgDef();
}

void CompCylinTab::selCylin()
{
	m_SelMode = 1;

	toolProcess();

}

void CompCylinTab::selBasePlane()
{
	m_SelMode = 2;

	toolProcess();

}

void CompCylinTab::onCal()
{
	if (!m_pBasePlane || !m_SelSurf.size())
	{
		MesgBox2("δѡ��׻���棡");
		return;
	}

	// �ж�ѡ�е�Բ�������������Ƿ��غ�
	{

	}

	// ����ȫ����
	const FIpPntList& theoPointsAll = gWindow->GetMainDoc()->GetIpPntList();
	const FIpPntList& measPointsAll = gWindow->GetMainDoc()->GetMesPntList();

	m_MeasPnts.clear();
	int nPnts = theoPointsAll.size();
	for (int i = 0; i < nPnts; i++)
	{
		if (isPointInFace(GP_PNT(theoPointsAll[i]), m_SelSurf))
		{
			m_MeasPnts.push_back(GP_PNT(measPointsAll[i]));
		}
	}

	if (!m_MeasPnts.size())
	{
		MesgBox2("�޲�㣡");
		return;
	}

	// ��ȡ��������
	TopoDS_Face tdsFace = gWindow->GetOccConverter()->FSurf2TDSFace(*m_SelSurf.begin());
	Handle(Geom_CylindricalSurface) geom_Cylin = Handle(Geom_CylindricalSurface)::DownCast(BRep_Tool::Surface(tdsFace));
	gp_Dir cylinDir = geom_Cylin->Axis().Direction();

	// ��ȡ����
	TopoDS_Face selTDSFace = gWindow->GetOccConverter()->FSurf2TDSFace(m_pBasePlane);
	Handle(Geom_Surface) surface = BRep_Tool::Surface(selTDSFace);
	Handle(Geom_Plane) geomPlane = Handle(Geom_Plane)::DownCast(surface);
	gp_Pln basePln = geomPlane->Pln();

	// ��ȡ���۶�λ��
	gp_Lin screenNorm = gp_Lin(geom_Cylin->Axis());

	IntAna_IntConicQuad intersector;
	intersector.Perform(screenNorm, basePln, Precision::Confusion());
	gp_Pnt baseTheo = intersector.Point(1);

	// ����������߻���
	gp_Pnt baseMeas = fitCylinAxis(m_MeasPnts, cylinDir);
	screenNorm = gp_Lin(baseMeas, geom_Cylin->Axis().Direction());
	intersector.Perform(screenNorm, basePln, Precision::Confusion());
	baseMeas = intersector.Point(1);

	double measX = zeroLim(baseMeas.X());
	double measY = zeroLim(baseMeas.Y());
	double measZ = zeroLim(baseMeas.Z());

	double theoX = zeroLim(baseTheo.X());
	double theoY = zeroLim(baseTheo.Y());
	double theoZ = zeroLim(baseTheo.Z());

	QString resStr = QString::fromLocal8Bit("Բ������������: \n%0\n\n���߷�������ֵ: \nX: %1, Y: %2, Z: %3\n\n��λ������ֵ: \nX: %4, Y: %5, Z: %6\n\n��λ�����ֵ: \nX: %7, Y: %8, Z: %9\n\n��λ�����ֵ: \nX: %10, Y: %11, Z: %12")
		// ��ʾ�������
		.arg(m_MeasPnts.size())

		// ��ʾ��������ֵ
		.arg(cylinDir.X()).arg(cylinDir.Y()).arg(cylinDir.Z())

		// ��ʾ����ֵ
		.arg(theoX).arg(theoY).arg(theoZ)

		// ��ʾ����ֵ
		.arg(measX).arg(measY).arg(measZ)

		// ��ʾ����ֵ
		.arg(theoX - measX).arg(measY - theoY).arg(measZ - theoZ);

	m_pResTextShow->setPlainText(resStr);


}

gp_Pnt CompCylinTab::fitCylinAxis(vector<gp_Pnt> _points, gp_Vec _axisDir)
{
	// Number of points
	int n = _points.size();

	// Check if there are enough points
	if (n < 2) {
		throw std::runtime_error("Not enough points to fit a cylinder");
	}

	// Convert the direction vector to an Eigen vector
	Eigen::Vector3d axis(_axisDir.X(), _axisDir.Y(), _axisDir.Z());

	// Compute the centroid of the points
	Eigen::Vector3d centroid(0.0, 0.0, 0.0);
	for (int i = 0; i < n; i++)
	{
		centroid += Eigen::Vector3d(_points[i].X(), _points[i].Y(), _points[i].Z());
	}
	centroid /= static_cast<double>(n);

	// Project the points onto a plane orthogonal to the cylinder axis
	std::vector<Eigen::Vector3d> projectedPoints;
	projectedPoints.reserve(n);
	for (int i = 0; i < n; i++)
	{
		Eigen::Vector3d point(_points[i].X(), _points[i].Y(), _points[i].Z());
		Eigen::Vector3d projection = point - axis.dot(point - centroid) * axis;
		projectedPoints.push_back(projection);
	}

	// Compute the mean of the projected points
	Eigen::Vector3d meanProjected(0.0, 0.0, 0.0);
	for (int i = 0; i < n; i++)
	{
		meanProjected += projectedPoints[i];
	}
	meanProjected /= static_cast<double>(n);

	// Calculate the base point as the projection of the mean of the projected points back onto the cylinder axis
	Eigen::Vector3d basePoint = centroid + axis.dot(meanProjected - centroid) * axis;

	// Set the base point
	gp_Pnt basePnt = gp_Pnt(basePoint.x(), basePoint.y(), basePoint.z());

	return basePnt;

}

void CompCylinTab::toolProcess()
{
	ErrCompsteTool* compTool = new ErrCompsteTool(this, 2);

	gWindow->GetMainDoc()->PopTool();
	gWindow->GetMainDoc()->PushTool(compTool);

}

CompCylinTab::CCYListWidget::CCYListWidget(CompCylinTab* _tab, const int& _ind, const double& _r, const double& _g, const double& _b)
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

CompCylinTab::CCYListWidget::~CCYListWidget()
{

}

void CompCylinTab::CCYListWidget::setBcgDef()
{
	setStyleSheet(getStyleSheet(m_bcgDefR, m_bcgDefG, m_bcgDefB));
}

void CompCylinTab::CCYListWidget::mousePressEvent(QMouseEvent* event)
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
			m_pTabWidget->selCylin();

			break;
		}
		case 2:
		{
			m_pTabWidget->selBasePlane();
			break;
		}
		default:
			break;
		}
	}
}

QString CompCylinTab::CCYListWidget::getStyleSheet(double _r, double _g, double _b)
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
