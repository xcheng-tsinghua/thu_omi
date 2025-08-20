#include "PointPlanDlg.h"
#include "SamplingFreeSurf.h"
#include "FPathPlanAndExeDlg.h"
#include "EntityHighlight.h"
#include "np.h"
#include "SamplingBasicFace.h"
#include "Tools/FSelectTool.h"
#include "ManualPointTool.h"
#include "FMainWindow.h"
#include "occConverter.h"

extern FMainWindow* gWindow;
extern clock_t gTStart, gTEnd;

PointPlanDlg::PointPlanDlg(QWidget* parent)/* :QWidget(parent)*/
{
	SelSurface = NULL;
	m_ArrReverse = 1.0;

	m_pCurTool = gWindow->GetCurTool();
	m_IsPlaned = false;

	initDlg();

}

PointPlanDlg::~PointPlanDlg(void)
{

}

void PointPlanDlg :: OnPointPlanQuit()
{
	this->close();
}

QLabel* PointPlanDlg::GetSelPlaneLabel()
{
	return m_pSelPlane;
}

void PointPlanDlg::setSelSurf(FSurface* _surf)
{
	SelSurface = _surf;
}

void PointPlanDlg::SetNotReverse()
{
	m_ArrReverse = 1.0;
}

void PointPlanDlg::setCBoxStatus(bool _isBspline)
{
	// �Ƚ�ѡ��״̬��ΪĬ��
	//m_pComboBox->setCurrentIndex(0);
	int nSelMethod = m_pComboBox->currentIndex();

	if (_isBspline)
	{
		if (!(nSelMethod == 1 || nSelMethod == 2 || nSelMethod == 3))
		{
			m_pComboBox->setCurrentIndex(0);
		}

		m_pComboBox->setItemData(0, QVariant(-1), Qt::UserRole - 1);
		m_pComboBox->setItemData(0, QBrush(QColor(255, 255, 255)), Qt::BackgroundRole);
		m_pComboBox->setItemData(1, QVariant(-1), Qt::UserRole - 1);
		m_pComboBox->setItemData(1, QBrush(QColor(255, 255, 255)), Qt::BackgroundRole);
		m_pComboBox->setItemData(2, QVariant(-1), Qt::UserRole - 1);
		m_pComboBox->setItemData(2, QBrush(QColor(255, 255, 255)), Qt::BackgroundRole);
		m_pComboBox->setItemData(3, QVariant(-1), Qt::UserRole - 1);
		m_pComboBox->setItemData(3, QBrush(QColor(255, 255, 255)), Qt::BackgroundRole);
		m_pComboBox->setItemData(4, QVariant(0), Qt::UserRole - 1);
		m_pComboBox->setItemData(4, QBrush(QColor(192, 192, 192)), Qt::BackgroundRole);
		m_pComboBox->setItemData(5, QVariant(0), Qt::UserRole - 1);
		m_pComboBox->setItemData(5, QBrush(QColor(192, 192, 192)), Qt::BackgroundRole);
		m_pComboBox->setItemData(6, QVariant(0), Qt::UserRole - 1);
		m_pComboBox->setItemData(6, QBrush(QColor(192, 192, 192)), Qt::BackgroundRole);

	}
	else
	{
		if (!(nSelMethod == 4 || nSelMethod == 5 || nSelMethod == 6))
		{
			m_pComboBox->setCurrentIndex(0);
		}

		m_pComboBox->setItemData(0, QVariant(-1), Qt::UserRole - 1);
		m_pComboBox->setItemData(0, QBrush(QColor(255, 255, 255)), Qt::BackgroundRole);
		m_pComboBox->setItemData(1, QVariant(0), Qt::UserRole - 1);
		m_pComboBox->setItemData(1, QBrush(QColor(192, 192, 192)), Qt::BackgroundRole);
		m_pComboBox->setItemData(2, QVariant(0), Qt::UserRole - 1);
		m_pComboBox->setItemData(2, QBrush(QColor(192, 192, 192)), Qt::BackgroundRole);
		m_pComboBox->setItemData(3, QVariant(0), Qt::UserRole - 1);
		m_pComboBox->setItemData(3, QBrush(QColor(192, 192, 192)), Qt::BackgroundRole);
		m_pComboBox->setItemData(4, QVariant(-1), Qt::UserRole - 1);
		m_pComboBox->setItemData(4, QBrush(QColor(255, 255, 255)), Qt::BackgroundRole);
		m_pComboBox->setItemData(5, QVariant(-1), Qt::UserRole - 1);
		m_pComboBox->setItemData(5, QBrush(QColor(255, 255, 255)), Qt::BackgroundRole);
		m_pComboBox->setItemData(6, QVariant(-1), Qt::UserRole - 1);
		m_pComboBox->setItemData(6, QBrush(QColor(255, 255, 255)), Qt::BackgroundRole);

	}

}

void PointPlanDlg::closeEvent(QCloseEvent* event)
{
	//�˳����߲�ʹ��Ĭ�Ϲ���
	gWindow->GetMainDoc()->PopTool();
	if (m_pCurTool)
	{
		m_pCurTool = NULL;
	}
	m_pCurTool = new EntityHighlight();
	gWindow->GetMainDoc()->PushTool(m_pCurTool);
	((EntityHighlight*)m_pCurTool)->SetSelEntType(FT_FACE);

	gWindow->ResetToDefaultCountenance();

}

void PointPlanDlg::OnSelectSurface()
{
	//setLinesDisabled();

	if (m_pCurTool)
	{
		gWindow->GetMainDoc()->PopTool();
	}

	m_pCurTool = new FSelectTool(this, gWindow->GetMainDoc());
	gWindow->GetMainDoc()->PushTool(m_pCurTool);

	((FSelectTool*)m_pCurTool)->SetSelEntType(FT_FACE);

}

void PointPlanDlg :: onAutoPointPlan()
{
	m_pSelPlaneBtn->setDisabled(false);
	//m_pComboBox->setDisabled(false);

}

void PointPlanDlg ::OnAutoSampling()
{
	gTStart = clock();

	m_IsPlaned = true;

	// ���ظ�ʹ������Ӧ���㷨��ͬһ���沼��ʱ��ɾ��֮ǰʹ������Ӧ���㷨���õĲ��
	RemoveSampledSurfPnts();

	SURFACE_TYPE tmpFSurfType;
	int FSrfIndInAll = 0;
	int FSrfIndInType = 0;
	QString tmpFSurfName;

	gWindow->GetSTEPModelConverter()->FSurfInfo(SelSurface, tmpFSurfType, FSrfIndInAll, FSrfIndInType, tmpFSurfName);
	TopoDS_Face selTDSFace = gWindow->GetOccConverter()->FSurf2TDSFace(SelSurface);

	double deviaU = m_pDeviaU->text().toDouble();
	double deviaV = m_pDeviaV->text().toDouble();

	double Retreat_U = m_pMarginalRetreat_U->text().toDouble();
	double Retreat_V = m_pMarginalRetreat_V->text().toDouble();

	int nu = m_pSection_U->text().toInt();
	int nv = m_pSection_V->text().toInt();
	int nAll = m_pNumOfPoints->text().toInt();

	vector<gp_Pnt> samples;

	// ��������
	if (tmpFSurfType == BSPLINE)
	{
		SamplingFreeSurf* m_pSfSampling = new SamplingFreeSurf;
		m_pSfSampling->setMarginalRetreat(m_pMarginalRetreatLineEdit->text().toDouble());
		m_pSfSampling->setNormReverse(m_ArrReverse);
		m_pSfSampling->onArrange(selTDSFace, m_SamplingMode, m_pSection_U->text().toInt(), m_pSection_V->text().toInt(), m_pWeightOfCurvature->text().toDouble());
		samples = m_pSfSampling->getSamplesAll();

	}
	// ��������
	else if (tmpFSurfType == CONE || tmpFSurfType == CYLINDER || tmpFSurfType == PLANE || tmpFSurfType == SPHERE)
	{
		// Hammersley
		if (m_SamplingMode == 4)
		{
			samples = SamplingBasicFace::hammersley(selTDSFace, nAll, Retreat_U, Retreat_V, deviaU, deviaV);
		}
		// ����
		else if (m_SamplingMode == 5)
		{
			samples = SamplingBasicFace::uniform(selTDSFace, nu, nv, Retreat_U, Retreat_V, deviaU, deviaV);
		}
		// ��Ե
		else if (m_SamplingMode == 6)
		{
			samples = SamplingBasicFace::marginal(selTDSFace, m_pOffsetVal->text(), m_pNeiTolVal->text());
		}
		else
		{

		}
	}
	else
	{
		MesgBox(QString::fromLocal8Bit("�ݲ�֧�ִ������沼��"));
		return;
	}

	addMesPnts(samples, selTDSFace);

	gTEnd = clock();
	double time_taken = double(gTEnd - gTStart) / CLOCKS_PER_SEC; // �����ʱ���룩
	std::cout << "��������ʱ��: " << time_taken << " ��" << std::endl;

}

void PointPlanDlg::getMethod(int i)
{
	m_SamplingMode = i;

	setLinesDisabled();
	m_pAutoSampBtn->setDisabled(false);
	m_pArrReverse->setDisabled(false);

	// ѡ�е�0�У�ȫ������ѡ
	if (i == 0)
	{
		
	}

	//ѡ����ǲ����ռ���Ȳ������ŷʽ�ռ���Ȳ���
	else if (i == 1 || i == 2)
	{
		m_pSection_U->setEnabled(true);
		m_pSection_V->setEnabled(true);
		m_pMarginalRetreat_U->setEnabled(true);
		m_pMarginalRetreat_V->setEnabled(true);

	}
	else if(i == 3)//ѡ����ǻ�ϲ���
	{
		m_pSection_U->setEnabled(true);
		m_pSection_V->setEnabled(true);
		m_pMarginalRetreat_U->setEnabled(true);
		m_pMarginalRetreat_V->setEnabled(true);
		m_pWeightOfCurvature->setEnabled(true);

	}
	else if (i == 4)//ѡ���Ĭ˹��
	{
		m_pNumOfPoints->setEnabled(true);
		m_pMarginalRetreat_U->setEnabled(true);
		m_pMarginalRetreat_V->setEnabled(true);
		m_pDeviaU->setEnabled(true);
		m_pDeviaV->setEnabled(true);

	}
	else if (i == 5)//ѡ����Ȳ���
	{
		m_pSection_U->setEnabled(true);
		m_pSection_V->setEnabled(true);
		m_pMarginalRetreat_U->setEnabled(true);
		m_pMarginalRetreat_V->setEnabled(true);
		m_pDeviaU->setEnabled(true);
		m_pDeviaV->setEnabled(true);

	}
	else if (i == 6)//ѡ���Ե����
	{
		m_pOffsetVal->setEnabled(true);
		m_pNeiTolVal->setEnabled(true);
	}
	else
	{

	}

}

void PointPlanDlg :: OnManualPointPlan()
{
	setLinesDisabled();
	m_pSelPlaneBtn->setDisabled(true);
	m_pComboBox->setDisabled(true);
	m_pAutoSampBtn->setDisabled(true);
	m_pArrReverse->setDisabled(true);

	//QMessageBox::information(this,QString::fromLocal8Bit("Info"), QString::fromLocal8Bit("�ֶ���㲼�ù����Ѽ����ֱ����ģ���ϵ�������ɣ������/ɾ�����Ҽ������"),"OK");
	if (m_pCurTool)
	{
		gWindow->GetMainDoc()->PopTool();
	}
	ManualPointTool* pTool = new ManualPointTool(gWindow->GetMainDoc());

	gWindow->GetMainDoc()->PushTool(pTool);

}

void PointPlanDlg :: OnPathPlanning()
{
	if (!gWindow->GetPathPlanAndExeDlg())
	{
		gWindow->SetPathPlanAndExeDlg(new FPathPlanAndExeDlg());
	}
	gWindow->GetPathPlanAndExeDlg()->show();
	close();
	gWindow->UpdateModelView();

}

void PointPlanDlg::OnArrReverse()
{
	m_ArrReverse *= -1.0;
	OnAutoSampling();
}

void PointPlanDlg::onClearPnts()
{
	gWindow->GetMainDoc()->clearPntsAll();

}

void PointPlanDlg::initDlg()
{
	move(1250, 150);
	setWindowIcon(QIcon("../icons/PntPlan.png"));
	this->setWindowTitle(QString::fromLocal8Bit("���滮"));
	this->setWindowFlags(Qt::WindowStaysOnTopHint);//��Զ����������֮��

	//��һ����ѡ�񲼵�ģʽ��
	QGroupBox* createFirstExclusiveGroup = new QGroupBox(QString::fromLocal8Bit("����ģʽ"));
	QGridLayout* mainGridLayout0 = new QGridLayout(createFirstExclusiveGroup);

	QPushButton* m_pStartBtn = new QPushButton(QString::fromLocal8Bit("�ֶ�����"));
	QPushButton* m_pAutoBtn = new QPushButton(QString::fromLocal8Bit("���ܲ���"));
	QPushButton* pClearBtn = new QPushButton(QString::fromLocal8Bit("��ղ��"));

	mainGridLayout0->addWidget(m_pStartBtn, 0, 0);
	mainGridLayout0->addWidget(m_pAutoBtn, 0, 1);
	mainGridLayout0->addWidget(pClearBtn, 1, 0);

	//�ڶ���������ѡȡ��
	QGroupBox* createSecondExclusiveGroup = new QGroupBox(QString::fromLocal8Bit("����ѡȡ"));
	QGridLayout* mainGridLayout1 = new QGridLayout(createSecondExclusiveGroup);

	m_pSelPlaneBtn = new QPushButton(QString::fromLocal8Bit("ѡ�������"));
	m_pSelPlane = new QLabel;

	mainGridLayout1->addWidget(m_pSelPlaneBtn, 1, 0);
	mainGridLayout1->addWidget(m_pSelPlane, 1, 1);

	//��������ȷ������������
	QGroupBox* createThirdExclusiveGroup = new QGroupBox(QString::fromLocal8Bit("��������"));
	QHBoxLayout* Hbox5 = new QHBoxLayout(createThirdExclusiveGroup);

	m_pComboBox = new QComboBox;
	m_pComboBox->addItem(QString::fromLocal8Bit("������ѡ���������"));
	m_pComboBox->addItem(QString::fromLocal8Bit("�����ռ���Ȳ���"));
	m_pComboBox->addItem(QString::fromLocal8Bit("ŷʽ�ռ���Ȳ���"));
	m_pComboBox->addItem(QString::fromLocal8Bit("��ϲ���"));
	m_pComboBox->addItem(QString::fromLocal8Bit("Hammersley����"));
	m_pComboBox->addItem(QString::fromLocal8Bit("���Ȳ���"));
	m_pComboBox->addItem(QString::fromLocal8Bit("��Ե����"));

	Hbox5->addWidget(m_pComboBox);

	//���ĸ���ȷ�������Ŀ��
	QGroupBox* createFourthExclusiveGroup = new QGroupBox(QString::fromLocal8Bit("��������"));
	QGridLayout* m_pMainGridLayout2 = new QGridLayout(createFourthExclusiveGroup);

	QLabel* label2 = new QLabel(QString::fromLocal8Bit("�������(Բ������)"));
	m_pSection_U = new QLineEdit(QString::number(2));

	QLabel* label3 = new QLabel(QString::fromLocal8Bit("�������(Բ������)"));
	m_pSection_V = new QLineEdit(QString::number(2));

	QLabel* label4 = new QLabel(QString::fromLocal8Bit("����Ȩ��[0,1]"));
	m_pWeightOfCurvature = new QLineEdit(QString::number(0.5));

	QLabel* label5 = new QLabel(QString::fromLocal8Bit("�������"));
	m_pNumOfPoints = new QLineEdit(QString::number(5));

	QLabel* m_pMarginalRetreatLabel = new QLabel(QString::fromLocal8Bit("��Ե������[0,0.5)"));
	m_pMarginalRetreatLineEdit = new QLineEdit(QString::number(0.05));

	QLabel* m_pCylinDevRLabel = new QLabel(QString::fromLocal8Bit("����ƫ����[0,1]"));
	m_pDeviaU = new QLineEdit(QString::number(0));

	QLabel* m_pCylinDevALabel = new QLabel(QString::fromLocal8Bit("����ƫ����[0,1]"));
	m_pDeviaV = new QLineEdit(QString::number(0));

	QLabel* m_pMarginalRetreatLabel_U = new QLabel(QString::fromLocal8Bit("����������[0,0.5]"));
	m_pMarginalRetreat_U = new QLineEdit(QString::number(0.05));

	QLabel* m_pMarginalRetreatLabel_V = new QLabel(QString::fromLocal8Bit("����������[0,0.5]"));
	m_pMarginalRetreat_V = new QLineEdit(QString::number(0.05));

	QLabel* m_pOffsetLabel = new QLabel(QString::fromLocal8Bit("��Ե���˾���"));
	m_pOffsetVal = new QLineEdit("-10");

	QLabel* m_pNeiTolLabel = new QLabel(QString::fromLocal8Bit("�����ݲ�"));
	m_pNeiTolVal = new QLineEdit("5.0");

	int nRow = 0;
	m_pMainGridLayout2->addWidget(label2, nRow, 0);
	m_pMainGridLayout2->addWidget(m_pSection_U, nRow++, 1);

	m_pMainGridLayout2->addWidget(label3, nRow, 0);
	m_pMainGridLayout2->addWidget(m_pSection_V, nRow++, 1);

	m_pMainGridLayout2->addWidget(label4, nRow, 0);
	m_pMainGridLayout2->addWidget(m_pWeightOfCurvature, nRow++, 1);

	m_pMainGridLayout2->addWidget(label5, nRow, 0);
	m_pMainGridLayout2->addWidget(m_pNumOfPoints, nRow++, 1);

	m_pMainGridLayout2->addWidget(m_pMarginalRetreatLabel, nRow, 0);
	m_pMainGridLayout2->addWidget(m_pMarginalRetreatLineEdit, nRow++, 1);

	m_pMainGridLayout2->addWidget(m_pCylinDevRLabel, nRow, 0);
	m_pMainGridLayout2->addWidget(m_pDeviaU, nRow++, 1);

	m_pMainGridLayout2->addWidget(m_pCylinDevALabel, nRow, 0);
	m_pMainGridLayout2->addWidget(m_pDeviaV, nRow++, 1);

	m_pMainGridLayout2->addWidget(m_pMarginalRetreatLabel_U, nRow, 0);
	m_pMainGridLayout2->addWidget(m_pMarginalRetreat_U, nRow++, 1);

	m_pMainGridLayout2->addWidget(m_pMarginalRetreatLabel_V, nRow, 0);
	m_pMainGridLayout2->addWidget(m_pMarginalRetreat_V, nRow++, 1);

	m_pMainGridLayout2->addWidget(m_pOffsetLabel, nRow, 0);
	m_pMainGridLayout2->addWidget(m_pOffsetVal, nRow++, 1);

	m_pMainGridLayout2->addWidget(m_pNeiTolLabel, nRow, 0);
	m_pMainGridLayout2->addWidget(m_pNeiTolVal, nRow++, 1);

	//��������ĸ�������ť
	QGroupBox* createFifththExclusiveGroup = new QGroupBox(QString::fromLocal8Bit("����"));
	QGridLayout* mainGridLayout4 = new QGridLayout(createFifththExclusiveGroup);

	m_pAutoSampBtn = new QPushButton(QString::fromLocal8Bit("�Զ�����"));
	m_pArrReverse = new QPushButton(QString::fromLocal8Bit("��ת����"));
	QPushButton* m_pToPath = new QPushButton(QString::fromLocal8Bit("·���滮"));
	QPushButton* pExitBtn = new QPushButton(QString::fromLocal8Bit("�˳�"));

	mainGridLayout4->addWidget(m_pAutoSampBtn, 1, 0);
	mainGridLayout4->addWidget(m_pArrReverse, 1, 1);
	mainGridLayout4->addWidget(m_pToPath, 2, 0);
	mainGridLayout4->addWidget(pExitBtn, 2, 1);

	//���ĸ���ӵ�������դ�񲼾�
	QVBoxLayout* mainGridLayout = new QVBoxLayout(this);

	mainGridLayout->addWidget(createFirstExclusiveGroup);
	mainGridLayout->addWidget(createSecondExclusiveGroup);
	mainGridLayout->addWidget(createThirdExclusiveGroup);
	mainGridLayout->addWidget(createFourthExclusiveGroup);
	mainGridLayout->addWidget(createFifththExclusiveGroup);

	setLinesDisabled();
	m_pSelPlaneBtn->setDisabled(true);
	m_pComboBox->setDisabled(true);
	m_pAutoSampBtn->setDisabled(true);
	m_pArrReverse->setDisabled(true);

	// �ۺ���
	connect(m_pComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(getMethod(int)));
	connect(pExitBtn, SIGNAL(clicked()), this, SLOT(OnPointPlanQuit()));
	connect(m_pSelPlaneBtn, SIGNAL(clicked()), this, SLOT(OnSelectSurface()));
	connect(m_pAutoBtn, SIGNAL(clicked()), this, SLOT(onAutoPointPlan()));
	connect(m_pAutoSampBtn, SIGNAL(clicked()), this, SLOT(OnAutoSampling()));
	connect(m_pStartBtn, SIGNAL(clicked()), this, SLOT(OnManualPointPlan()));
	connect(m_pToPath, SIGNAL(clicked()), this, SLOT(OnPathPlanning()));
	connect(m_pArrReverse, SIGNAL(clicked()), this, SLOT(OnArrReverse()));
	connect(pClearBtn, SIGNAL(clicked()), this, SLOT(onClearPnts()));

}

void PointPlanDlg::setLinesDisabled()
{
	m_pSection_U->setEnabled(false);
	m_pSection_V->setEnabled(false);
	m_pWeightOfCurvature->setEnabled(false);
	m_pNumOfPoints->setEnabled(false);
	m_pDeviaU->setEnabled(false);
	m_pDeviaV->setEnabled(false);
	m_pMarginalRetreatLineEdit->setEnabled(false);
	m_pMarginalRetreat_U->setEnabled(false);
	m_pMarginalRetreat_V->setEnabled(false);
	m_pOffsetVal->setEnabled(false);
	m_pNeiTolVal->setEnabled(false);

}

void PointPlanDlg::addMesPnts(const vector<gp_Pnt>& _samples, const TopoDS_Face& _cFace)
{
	FIpPntList RealIpPntLst;
	vector<MeasPt*> RealMeasPtLst;
	FIpPntList validPnts = gWindow->GetMainDoc()->GetIpPntList();

	for (auto ita = _samples.begin(); ita != _samples.end(); ++ita)
	{
		gp_Vec norm = NormalAt(_cFace, *ita);

		FIpPoint ipPnt = FIpPoint(*ita, norm);
		MeasPt* mPnt = new MeasPt(*ita, norm);

		RealIpPntLst.push_back(ipPnt);
		RealMeasPtLst.push_back(mPnt);
		gWindow->GetMainDoc()->AddDispEnt(mPnt);
		//gWindow->GetMainDoc()->getPntsShowList().push_back(mPnt);
		validPnts.push_back(ipPnt);

	}

	gWindow->GetMainDoc()->ClearIpPntList();
	gWindow->GetMainDoc()->SetIpPntList(validPnts);

	CuurSA.IpPntLst = RealIpPntLst;
	CuurSA.MeasPtLst = RealMeasPtLst;

	SampedSurf.push_back(CuurSA);

}

void PointPlanDlg::RemoveSampledSurfPnts()
{
	CuurSA.TheSurf = SelSurface;
	CuurSA.IpPntLst.clear();
	CuurSA.MeasPtLst.clear();

	//�ж�֮ǰ�Ƿ��ڴ�������������Ӧ���㷨������
	bool IsSampled = false;
	int SampledIndex = 999999;
	for (int i = 0; i < SampedSurf.size(); i++)
	{
		if (SelSurface == SampedSurf[i].TheSurf)
		{
			IsSampled = true;
			SampledIndex = i;
			break;
		}
	}

	if (IsSampled)
	{
		//ɾ��֮ǰ�Ѳ��ù���IpPnt�������ʾ��
		FIpPntList PreviousIpPnt = SampedSurf[SampledIndex].IpPntLst;
		FIpPntList AllIpPnts = gWindow->GetMainDoc()->GetIpPntList();
		FIpPntList RemovedPreviousIpPnts;

		bool IsRepated = false;
		for (int i = 0; i < AllIpPnts.size(); i++)
		{
			IsRepated = false;
			for (int j = 0; j < PreviousIpPnt.size(); j++)
			{
				if (AllIpPnts[i] == PreviousIpPnt[j])
				{
					IsRepated = true;
				}
			}

			if (!IsRepated)
			{
				RemovedPreviousIpPnts.push_back(AllIpPnts[i]);
			}
		}

		gWindow->GetMainDoc()->ClearIpPntList();
		gWindow->GetMainDoc()->SetIpPntList(RemovedPreviousIpPnts);

		//ɾ��֮ǰ�Ѳ��ù���MeasPt�������ߵ㣩
		for (int i = 0; i < SampedSurf[SampledIndex].MeasPtLst.size(); i++)
		{
			gWindow->GetMainDoc()->RemoveDispEnts(SampedSurf[SampledIndex].MeasPtLst[i]);
		}

		//ɾ��gWindow->ArrList�еĵ�
		vector<MeasPt*>tmpArrList;
		bool IsMu = false;

		for (int i = 0; i < gWindow->GetMainDoc()->getPntsShowList().size(); i++)
		{
			for (int j = 0; j < SampedSurf[SampledIndex].MeasPtLst.size(); j++)
			{
				if (SampedSurf[SampledIndex].MeasPtLst[j] == gWindow->GetMainDoc()->getPntsShowList()[i])
				{
					IsMu = true;
					break;
				}
			}
			if (!IsMu)
			{
				tmpArrList.push_back(gWindow->GetMainDoc()->getPntsShowList()[i]);
			}
			IsMu = false;

		}
		gWindow->GetMainDoc()->getPntsShowList() = tmpArrList;

		//ɾ���б����ϴε�SurfSA
		vector<SurfSA> tmpSA;
		for (int i = 0; i < SampedSurf.size(); i++)
		{
			if (i != SampledIndex)
			{
				tmpSA.push_back(SampedSurf[i]);
			}
		}
		SampedSurf = tmpSA;
	}

}
