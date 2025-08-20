#include "FInsFeatureDlg.h"
#include "FMainWindow.h"
#include "FInsFeaTreeWnd.h"
#include "AccuEvaluWnd.h"
#include "Tools/InsFeaTool.h"
#include "../External/fkernel/Include/DataStructure/FBody.h"
#include "occConverter.h"
#include "EntityHighlight.h"
#include "np.h"
#include <functional>
#include <Quantity_Color.hxx>

extern int gInsFeaIndex;
extern clock_t gTStart, gTEnd;

FInsFeatureDlg::FInsFeatureDlg(QWidget* parent)
{
	m_pCurTool = gWindow->GetCurTool();

	this->setWindowIcon(QIcon("../icons/NewFeat.png"));
	this->setFixedSize(690, 485);
	this->setWindowTitle(QString::fromLocal8Bit("���ɼ������"));
	this->setWindowFlags(Qt::WindowStaysOnTopHint);
	this->move(1200, 100);

	// �����Ҽ��˵�
	m_pPopMenu = QSharedPointer<QMenu>(new QMenu());
	m_pActionDelete = new QAction(QString::fromLocal8Bit("�Ƴ�����"), this);
	m_pDelAllSurf = new QAction(QString::fromLocal8Bit("�Ƴ���������"), this);

	m_pActionDelete->setIcon(QIcon("../icons/Delete.png"));
	m_pDelAllSurf->setIcon(QIcon("../icons/DelAllSurf.png"));

	ClickedFaceListWidget = 0;

	SelType = UNKNOW;
	DirSelState = 0;

	ToleranceValue = 0;
	UpDeviationValue = 0;
	DownDeviationValue = 0;

	// ��ʼ����������ؼ�
	pToleranceValueEditLine = new QLineEdit(this);
	pUpDeviationEditLine = new QLineEdit(this);
	pDownDeviationEditLine = new QLineEdit(this);

	pToleranceValueEditLine->move(10000, 10000);
	pUpDeviationEditLine->move(10000, 10000);
	pDownDeviationEditLine->move(10000, 10000);

	pToleranceValueEditLine->setText(QString::number(0.05));
	pUpDeviationEditLine->setText(QString::number(0.05));
	pDownDeviationEditLine->setText(QString::number(0.05));

	pToleranceValueLabel = new QLabel(QString::fromLocal8Bit("����Ҫ��/mm"));
	pUpDeviationLabel = new QLabel(QString::fromLocal8Bit("��ƫ��/mm��+"));
	pDownDeviationLabel = new QLabel(QString::fromLocal8Bit("��ƫ��/mm��-"));

	/////////////////////////////////////////������
	QVBoxLayout* MainVBoxLayout = new QVBoxLayout(this);

	/////////////////////////////////////////���Ͻ�ѡ���������Ӵ���
	QGroupBox* ToleranceTypeGroup = new QGroupBox(QString::fromLocal8Bit("�������"));
	ToleranceTypeGroup->setFixedSize(334, 270);

	//���Ҫ�󲼾�
	InsRequestGridLayout = new QGridLayout;

	//���Ҫ�����Ͱ�ť
	QRadioButton* pParallelismBox = new QRadioButton(QString::fromLocal8Bit("ƽ�ж�"));
	QRadioButton* pRoundnessBox = new QRadioButton(QString::fromLocal8Bit("Բ��"));
	QRadioButton* pDistanceBox = new QRadioButton(QString::fromLocal8Bit("����"));
	QRadioButton* pVerticalityBox = new QRadioButton(QString::fromLocal8Bit("��ֱ��"));
	QRadioButton* pDiameterBox = new QRadioButton(QString::fromLocal8Bit("ֱ��"));
	QRadioButton* pAxialityBox = new QRadioButton(QString::fromLocal8Bit("ͬ���"));
	QRadioButton* pFlatnessBox = new QRadioButton(QString::fromLocal8Bit("ƽ���"));
	QRadioButton* pProfileBox = new QRadioButton(QString::fromLocal8Bit("������"));
	QRadioButton* pPositionBox = new QRadioButton(QString::fromLocal8Bit("λ�ö�"));

	//���Ҫ����뷽�����Ͱ�ť
	pDis_Stra = new QRadioButton(QString::fromLocal8Bit("ֱ�߾���"));
	pDis_X = new QRadioButton(QString::fromLocal8Bit("X�����"));
	pDis_Y = new QRadioButton(QString::fromLocal8Bit("Y�����"));
	pDis_Z = new QRadioButton(QString::fromLocal8Bit("Z�����"));

	InsRequestGridLayout->addWidget(pParallelismBox, 0, 0);
	InsRequestGridLayout->addWidget(pRoundnessBox, 1, 0);
	InsRequestGridLayout->addWidget(pDistanceBox, 2, 0);
	InsRequestGridLayout->addWidget(pVerticalityBox, 3, 0);
	InsRequestGridLayout->addWidget(pDiameterBox, 4, 0);
	InsRequestGridLayout->addWidget(pAxialityBox, 5, 0);
	InsRequestGridLayout->addWidget(pFlatnessBox, 6, 0);
	InsRequestGridLayout->addWidget(pProfileBox, 7, 0);
	InsRequestGridLayout->addWidget(pPositionBox, 8, 0);

	QButtonGroup* m_pButtonGroup = new QButtonGroup;
	m_pButtonGroup->addButton(pParallelismBox);
	m_pButtonGroup->addButton(pRoundnessBox);
	m_pButtonGroup->addButton(pDistanceBox);
	m_pButtonGroup->addButton(pVerticalityBox);
	m_pButtonGroup->addButton(pDiameterBox);
	m_pButtonGroup->addButton(pAxialityBox);
	m_pButtonGroup->addButton(pFlatnessBox);
	m_pButtonGroup->addButton(pProfileBox);
	m_pButtonGroup->addButton(pPositionBox);

	QButtonGroup* m_pDirSelGroup = new QButtonGroup;
	m_pDirSelGroup->addButton(pDis_Stra);
	m_pDirSelGroup->addButton(pDis_X);
	m_pDirSelGroup->addButton(pDis_Y);
	m_pDirSelGroup->addButton(pDis_Z);

	ToleranceTypeGroup->setLayout(InsRequestGridLayout);

	/////////////////////////////////////////���½�����������Ӵ���
	InsParameterGroup = new QGroupBox(QString::fromLocal8Bit("������"));
	InsParameterGroup->setFixedHeight(150);
	InsParameterGridLayout = new QGridLayout;/////////////�������б���

	InsParameterGroup->setLayout(InsParameterGridLayout);

	/////////////////////////////////////////�Ҳ�ѡ�������б��Ӵ���
	QGroupBox* FaceListExclusiveGroup = new QGroupBox(QString::fromLocal8Bit("�����б�"));
	FaceListExclusiveGroup->setFixedWidth(200 / 0.618);

	//�����б���
	FaceListGridLayout = new QGridLayout;

	//�����б�
	FaceList1 = new IFDListWidget(this, 1, 1, 0, 0);
	FaceList1->setFrameStyle(QFrame::Panel | QFrame::Raised);

	FaceList2 = new IFDListWidget(this, 2, 0, 0, 1);//////////////////�����б�
	FaceList2->setFrameStyle(QFrame::Panel | QFrame::Raised);

	FaceListExclusiveGroup->setLayout(FaceListGridLayout);

	//��ര�ڲ���
	QVBoxLayout* LeftVBoxLayout = new QVBoxLayout;
	LeftVBoxLayout->addWidget(ToleranceTypeGroup);
	LeftVBoxLayout->addWidget(InsParameterGroup);

	//�ϲര�ڲ���
	QHBoxLayout* UpLayout = new QHBoxLayout;
	UpLayout->addLayout(LeftVBoxLayout);
	UpLayout->addWidget(FaceListExclusiveGroup);

	/////////////////////////////////////////���·����ɼ��������ȡ����ť
	QPushButton* pGenerateInsFeature = new QPushButton(QString::fromLocal8Bit("���ɼ������"), this);
	QPushButton* pExit = new QPushButton(QString::fromLocal8Bit("�˳�"), this);

	QHBoxLayout* DownButtonLayout = new QHBoxLayout;

	// ���Ҷ���
	DownButtonLayout->addStretch();

	DownButtonLayout->addWidget(pGenerateInsFeature,0);
	DownButtonLayout->addWidget(pExit, 0);

	/////////////////////////////////////////���ؼ������������
	MainVBoxLayout->addLayout(UpLayout);
	MainVBoxLayout->addLayout(DownButtonLayout);

	// ���ܺ���
	connect(FaceList1, SIGNAL(currentRowChanged(int)), this, SLOT(ChooseTargetFace(int)));
	connect(m_pButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));
	connect(m_pDirSelGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onDirSelButtonClicked(QAbstractButton*)));
	connect(pGenerateInsFeature, SIGNAL(clicked()), this, SLOT(GenerateInsFeature()));
	connect(pExit, SIGNAL(clicked()), this, SLOT(ExitGenerateInsFeature()));

	// �����Ҽ��˵�����
	connect(m_pDelAllSurf, SIGNAL(triggered()), this, SLOT(onRemoveAllSurf()));
	connect(m_pActionDelete, SIGNAL(triggered()),this, SLOT(onDeleteFace()));

}

FInsFeatureDlg::~FInsFeatureDlg()
{

}

void FInsFeatureDlg::ChooseTargetFace(int i)
{

}

void FInsFeatureDlg::showInsParameterList()
{
	hideValueInputLabelLine();

	////////////////////////��ʼ������
	switch (SelType)
	{
		case PARALLELISM:
		{
			pToleranceValueLabel->setText(QString::fromLocal8Bit("ƽ�жȹ���ֵ/mm"));

			showToleranceValueInputLabelLine();

			break;
		}
		case ROUNDNESS:
		{
			pToleranceValueLabel->setText(QString::fromLocal8Bit("Բ�ȹ���ֵ/mm"));

			showToleranceValueInputLabelLine();

			break;
		}
		case DISTANCE:
		{
			pToleranceValueLabel->setText(QString::fromLocal8Bit("�����׼ֵ/mm"));

			showDimensionValueInputLabelLine();

			break;
		}
		case VERTICALITY:
		{
			pToleranceValueLabel->setText(QString::fromLocal8Bit("��ֱ�ȹ���ֵ/mm"));

			showToleranceValueInputLabelLine();

			break;
		}
		case DIAMETER:
		{
			pToleranceValueLabel->setText(QString::fromLocal8Bit("ֱ����׼ֵ/mm"));

			showDimensionValueInputLabelLine();

			break;
		}
		case AUXIALITY:
		{
			pToleranceValueLabel->setText(QString::fromLocal8Bit("ͬ��ȹ���ֵ/mm"));

			showToleranceValueInputLabelLine();

			break;
		}
		case FLATNESS:
		{
			pToleranceValueLabel->setText(QString::fromLocal8Bit("ƽ��ȹ���ֵ/mm"));

			showToleranceValueInputLabelLine();

			break;
		}

		case PROFILE:
		{
			pToleranceValueLabel->setText(QString::fromLocal8Bit("�����ȹ���ֵ/mm"));

			showToleranceValueInputLabelLine();

			break;


		}

		case POSITION:
		{
			pToleranceValueLabel->setText(QString::fromLocal8Bit("λ�öȹ���ֵ/mm"));

			showToleranceValueInputLabelLine();

			break;

		}

		default:
			break;
	}

	InsParameterGridLayout->update();

}

void FInsFeatureDlg::GenerateInsFeature()
{
	ToleranceValue = pToleranceValueEditLine->text().toDouble();
	UpDeviationValue = pUpDeviationEditLine->text().toDouble();
	DownDeviationValue = pDownDeviationEditLine->text().toDouble();

	vector<FSurface*> surfListVec1 = vector<FSurface*>(m_HashSurfList1.begin(), m_HashSurfList1.end());
	vector<FSurface*> surfListVec2 = vector<FSurface*>(m_HashSurfList2.begin(), m_HashSurfList2.end());
	FInsFeature* pInsFeature = new FInsFeature(surfListVec1, surfListVec2, SelType, ToleranceValue, UpDeviationValue, DownDeviationValue, DirSelState, gInsFeaIndex);

	initTree_by_insfea(pInsFeature, static_cast<FInsFeaTreeWnd*>(gWindow->GetSubWnd()[6])->GetTree());

	initTree_by_insfea(pInsFeature, static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->GetTree());

	gWindow->GetMainDoc()->GetInsFeaList().push_back(pInsFeature);
	
	gInsFeaIndex += 1;
}

void FInsFeatureDlg::ExitGenerateInsFeature()
{
	if (m_pCurTool)
	{
		gWindow->GetMainDoc()->PopTool();
		m_pCurTool = NULL;

		m_pCurTool = new EntityHighlight();
		gWindow->GetMainDoc()->PushTool(m_pCurTool);
		((EntityHighlight*)m_pCurTool)->SetSelEntType(FT_FACE);
	}

	close();

	//�˳����ں���������ɫ��ΪĬ��
	gWindow->ResetToDefaultCountenance();

	gWindow->UpdateModelView();
	//�˳����Ƴ������б���������
	onRemoveAllSurf();

}

void FInsFeatureDlg::onDeleteFace()
{
	QListWidget* pListWidget = NULL;
	unordered_set<FSurface*>* pHashList;

	if (ClickedFaceListWidget == 1)
	{
		pListWidget = FaceList1;
		pHashList = &m_HashSurfList1;

	}
	else if (ClickedFaceListWidget == 2)
	{
		pListWidget = FaceList2;
		pHashList = &m_HashSurfList2;

	}
	else
	{
		return;
	}

	// ��ȡ����
	int row = pListWidget->currentRow();
	QListWidgetItem* cItem = pListWidget->item(row);
	FSurface* cSurf = (FSurface*)cItem->data(Qt::UserRole).value<void*>();

	// ���������ɾ������Ŀ
	pListWidget->takeItem(row);
	delete cItem;

	// �ָ���������ɫ
	gWindow->GetOccConverter()->setSurfColorDefault(cSurf);

	// �ӹ�ϣ����ɾ������Ŀ
	pHashList->erase(cSurf);

}

void FInsFeatureDlg::onAIS_FaceSel()
{
	//�Ȼָ�������ɫ
	//gWindow->ResetToDefaultCountenance();

	gWindow->UpdateModelView();
	if (m_pCurTool)
	{
		gWindow->GetMainDoc()->PopTool();
		m_pCurTool = NULL;
	}
	m_pCurTool = new InsFeaTool(this, gWindow->GetMainDoc());
	gWindow->GetMainDoc()->PushTool(m_pCurTool);
	((InsFeaTool*)m_pCurTool)->SetSelEntType(FT_FACE);

}

void FInsFeatureDlg::onRemoveAllSurf()
{
	QListWidget* pListWidget = NULL;
	unordered_set<FSurface*>* pHashList;

	if (ClickedFaceListWidget == 1)
	{
		pListWidget = FaceList1;
		pHashList = &m_HashSurfList1;

	}
	else if (ClickedFaceListWidget == 2)
	{
		pListWidget = FaceList2;
		pHashList = &m_HashSurfList2;

	}
	else
	{
		return;
	}

	// �ָ�������ɫ
	for (auto ita = pHashList->begin(); ita != pHashList->end(); ++ita)
	{
		gWindow->GetOccConverter()->setSurfColorDefault(*ita);
	}

	// ��ն�Ӧ��ϣ������Ŀ
	pHashList->clear();
	pListWidget->clear();

	// ˢ��ģ�ʹ�����ʾ
	gWindow->UpdateModelView();

}

void FInsFeatureDlg::onButtonClicked(QAbstractButton* button)
{
	gWindow->GetMainDoc()->PopTool();
	this->setListWidgetBcgDef();
	// ��������б����ʾ����
	m_HashSurfList1.clear();
	m_HashSurfList2.clear();
	FaceList1->clear();
	FaceList2->clear();

	FaceListGridLayout->removeWidget(FaceList1);
	FaceListGridLayout->removeWidget(FaceList2);
	FaceList2->move(9999, 9999);
	
	FaceListGridLayout->addWidget(FaceList1);
	gWindow->ResetToDefaultCountenance();
	gWindow->UpdateModelView();

	if (button->text() == QString::fromLocal8Bit("ƽ�ж�"))
	{
		//QMessageBox::information(0, QString::fromLocal8Bit("��ܰ��ʾ"), QString::fromLocal8Bit("�����Ҫ������Բ������֮���ƽ�жȣ��뽻��ѡȡ�ֱ����������ǵİ�Բ���档"), "OK");
		SelType = PARALLELISM;

		DirSelState = 0;

		hideXYZSel();

		FaceListGridLayout->addWidget(FaceList2);
	}
	else if (button->text() == QString::fromLocal8Bit("Բ��"))
	{
		SelType = ROUNDNESS;

		DirSelState = 0;

		hideXYZSel();
	}
	else if (button->text() == QString::fromLocal8Bit("����"))
	{
		SelType = DISTANCE;

		pDis_Stra->setChecked(true);
		DirSelState = 0;

		showXYZSel();

		FaceListGridLayout->addWidget(FaceList2);

	}
	else if (button->text() == QString::fromLocal8Bit("��ֱ��"))
	{
		SelType = VERTICALITY;

		DirSelState = 0;

		hideXYZSel();

		FaceListGridLayout->addWidget(FaceList2);
	}
	else if (button->text() == QString::fromLocal8Bit("ֱ��"))
	{
		SelType = DIAMETER;

		DirSelState = 0;

		hideXYZSel();
	}
	else if (button->text() == QString::fromLocal8Bit("ͬ���"))
	{
		SelType = AUXIALITY;

		DirSelState = 0;

		hideXYZSel();

		FaceListGridLayout->addWidget(FaceList2);

	}
	else if (button->text() == QString::fromLocal8Bit("ƽ���"))
	{
		SelType = FLATNESS;

		DirSelState = 0;

		hideXYZSel();
	}

	else if (button->text() == QString::fromLocal8Bit("������"))
	{
		SelType = PROFILE;

		DirSelState = 0;

		hideXYZSel();
	}

	else if (button->text() == QString::fromLocal8Bit("λ�ö�"))
	{
		SelType = POSITION;

		DirSelState = 0;

		hideXYZSel();
	}

	else
	{
		QMessageBox::warning(0, QString::fromLocal8Bit("��������"),
			QString::fromLocal8Bit("ϵͳ����"));
	}

	showInsParameterList();

}

void FInsFeatureDlg::onDirSelButtonClicked(QAbstractButton* button)
{
	if (button->text() == QString::fromLocal8Bit("ֱ�߾���"))
	{
		DirSelState = 0;
	}
	else if (button->text() == QString::fromLocal8Bit("X�����"))
	{
		DirSelState = 1;
	}
	else if (button->text() == QString::fromLocal8Bit("Y�����"))
	{
		DirSelState = 2;
	}
	else if (button->text() == QString::fromLocal8Bit("Z�����"))
	{
		DirSelState = 3;
	}
	else
	{
		QMessageBox::warning(0, QString::fromLocal8Bit("��������"),
			QString::fromLocal8Bit("ϵͳ����"));
	}
}

void FInsFeatureDlg::setListWidgetBcgDef()
{
	FaceList1->setBcgDef();
	FaceList2->setBcgDef();
}

void FInsFeatureDlg::faceSelG1()
{
	ClickedFaceListWidget = 1;
	onAIS_FaceSel();

}

void FInsFeatureDlg::faceSelG2()
{
	ClickedFaceListWidget = 2;
	onAIS_FaceSel();

}

void FInsFeatureDlg::closeEvent(QCloseEvent* event)
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

	//�˳����ں���������ɫ��ΪĬ��
	gWindow->ResetToDefaultCountenance();

	gWindow->UpdateModelView();
	//�˳����Ƴ������б���������
	ClickedFaceListWidget = 1;
	onRemoveAllSurf();

	ClickedFaceListWidget = 2;
	onRemoveAllSurf();

}

void FInsFeatureDlg::hideXYZSel()
{
	InsRequestGridLayout->removeWidget(pDis_Stra);
	InsRequestGridLayout->removeWidget(pDis_X);
	InsRequestGridLayout->removeWidget(pDis_Y);
	InsRequestGridLayout->removeWidget(pDis_Z);
	pDis_Stra->move(9999, 9999);
	pDis_X->move(9999, 9999);
	pDis_Y->move(9999, 9999);
	pDis_Z->move(9999, 9999);

	FaceListGridLayout->removeWidget(FaceList2);
}

void FInsFeatureDlg::showXYZSel()
{
	InsRequestGridLayout->addWidget(pDis_Stra, 0, 1);
	InsRequestGridLayout->addWidget(pDis_X, 1, 1);
	InsRequestGridLayout->addWidget(pDis_Y, 2, 1);
	InsRequestGridLayout->addWidget(pDis_Z, 3, 1);

	FaceListGridLayout->removeWidget(FaceList2);

}

void FInsFeatureDlg::hideValueInputLabelLine()
{
	InsParameterGridLayout->removeWidget(pToleranceValueLabel);
	InsParameterGridLayout->removeWidget(pUpDeviationLabel);
	InsParameterGridLayout->removeWidget(pDownDeviationLabel);

	InsParameterGridLayout->removeWidget(pUpDeviationEditLine);
	InsParameterGridLayout->removeWidget(pDownDeviationEditLine);
	InsParameterGridLayout->removeWidget(pToleranceValueEditLine);
}

void FInsFeatureDlg::showDimensionValueInputLabelLine()
{
	InsParameterGridLayout->addWidget(pToleranceValueLabel, 0, 0);
	InsParameterGridLayout->addWidget(pToleranceValueEditLine, 0, 1);
	InsParameterGridLayout->addWidget(pUpDeviationLabel, 1, 0);
	InsParameterGridLayout->addWidget(pUpDeviationEditLine, 1, 1);
	InsParameterGridLayout->addWidget(pDownDeviationLabel, 2, 0);
	InsParameterGridLayout->addWidget(pDownDeviationEditLine, 2, 1);
}

void FInsFeatureDlg::showToleranceValueInputLabelLine()
{
	InsParameterGridLayout->addWidget(pToleranceValueLabel, 0, 0);
	InsParameterGridLayout->addWidget(pToleranceValueEditLine, 0, 1);

	pUpDeviationLabel->move(10000, 10000);
	pUpDeviationEditLine->move(10000, 10000);
	pDownDeviationLabel->move(10000, 10000);
	pDownDeviationEditLine->move(10000, 10000);
}

void FInsFeatureDlg::contextMenuEvent(QContextMenuEvent* event)
{
	QPoint point = QCursor::pos();
	QPoint localPos = FaceList1->viewport()->mapFromGlobal(point);

	function<void(QListWidgetItem*, int)> ft = [&](QListWidgetItem* tmpItem, int axis)->void
	{
		//������ѡ���item�Ƿ�׼ȷ
		if (tmpItem)
		{
			std::cout << tmpItem->text().toLocal8Bit().data() << std::endl;
			std::cout << tmpItem->type() << std::endl;
		}

		if (tmpItem)
		{
			m_pPopMenu->clear();
			m_pPopMenu->addAction(m_pActionDelete);
			m_pPopMenu->addAction(m_pDelAllSurf);
			m_pPopMenu->exec(QCursor::pos());
		}
		else
		{
		}
	};

	// �Ҽ����ڵ�һ��FaceListWidget
	if (FaceList1->geometry().contains(localPos))
	{
		QListWidgetItem* pItem = FaceList1->itemAt(localPos);
		m_pTmpItem = pItem;

		ClickedFaceListWidget = 1;

		ft(pItem, 1);
	}
	// �Ҽ����ڵڶ���FaceListWidget
	else if(FaceList2->geometry().contains(localPos))
	{
		QListWidgetItem* pItem = FaceList2->itemAt(FaceList2->viewport()->mapFromGlobal(point));
		m_pTmpItem = pItem;

		ClickedFaceListWidget = 2;

		ft(pItem, 2);

	}

}

void FInsFeatureDlg::addSurf(FSurface* _surf)
{
	QListWidget* pWidget = NULL;
	unordered_set<FSurface*>* surfHashList;
	Quantity_Color highlightColor;

	if (ClickedFaceListWidget == 1)
	{
		pWidget = FaceList1;
		surfHashList = &m_HashSurfList1;
		highlightColor.SetValues(Quantity_NOC_RED);
	}
	else if (ClickedFaceListWidget == 2)
	{
		pWidget = FaceList2;
		surfHashList = &m_HashSurfList2;
		highlightColor.SetValues(Quantity_NOC_CYAN);
	}
	else
	{

		return;
	}

	// �����ѡ�У���ȡ��ѡ�и�����
	if (surfHashList->count(_surf))
	{
		// ��ɫ�ָ�Ϊ��ʼ״̬
		gWindow->GetOccConverter()->setSurfColorDefault(_surf);

		// ��ϣ����ɾ������Ŀ
		surfHashList->erase(_surf);

		// ɾ���б�����е���Ŀ
		removeListItem(pWidget, _surf);
	}
	else
	{
		// �任������ɫ
		double sR = highlightColor.Red();
		double sG = highlightColor.Green();
		double sB = highlightColor.Blue();
		_surf->SetColor(sR, sG, sB);

		// ��������뵽��ϣ��
		surfHashList->insert(_surf);

		// ���б�����м������Ŀ
		SURFACE_TYPE surfType;
		int indAll;
		int indType;
		QString surfName;
		gWindow->GetOccConverter()->FSurfInfo(_surf, surfType, indAll, indType, surfName);

		// �����б���Ŀ
		QListWidgetItem* listItem = new QListWidgetItem(surfName);

		// Ϊ�б���Ŀ����ͼ��
		switch (surfType)
		{
		case BSPLINE:
			listItem->setIcon(QIcon("../icons/BsplineSurface.png"));
			break;
		case CONE:
			listItem->setIcon(QIcon("../icons/ConicalSurface.png"));
			break;
		case CYLINDER:
			listItem->setIcon(QIcon("../icons/CylindricalSurface.png"));
			break;
		case PLANE:
			listItem->setIcon(QIcon("../icons/PlaneSurface.png"));
			break;
		case SPHERE:
			listItem->setIcon(QIcon("../icons/SphericalSurface.png"));
			break;
		case OTHER_SURF:
			listItem->setIcon(QIcon("../icons/OtherSurface.png"));
			break;
		default:
			break;
		}

		// Ϊ�б���Ŀ��������
		listItem->setData(Qt::UserRole, QVariant::fromValue((void*)(_surf)));

		// ���б���Ŀ���뵽�б����
		pWidget->addItem(listItem);

	}

	// ˢ�´�����ʾ
	gWindow->UpdateModelView();

	gTEnd = clock();
	double time_taken = double(gTEnd - gTStart) / CLOCKS_PER_SEC; // �����ʱ���룩
	std::cout << "��������ʱ��: " << time_taken << " ��" << std::endl;

}

FInsFeatureDlg::IFDListWidget::IFDListWidget(FInsFeatureDlg* _dlg, const int& _ind, const double& _r, const double& _g, const double& _b)
{
	m_pInsFeaDlg = _dlg;
	m_nInd = _ind;

	m_bcgR = _r;
	m_bcgG = _g;
	m_bcgB = _b;

	m_bcgDefR = 1;
	m_bcgDefG = 1;
	m_bcgDefB = 1;

	setStyleSheet(getStyleSheet(m_bcgDefR, m_bcgDefG, m_bcgDefB));
}

FInsFeatureDlg::IFDListWidget::~IFDListWidget()
{

}

void FInsFeatureDlg::IFDListWidget::setBcgDef()
{
	setStyleSheet(getStyleSheet(m_bcgDefR, m_bcgDefG, m_bcgDefB));
}

void FInsFeatureDlg::IFDListWidget::mousePressEvent(QMouseEvent* event)
{
	// ���û����mousePressEvent����ȷ���������¼�����
	QListWidget::mousePressEvent(event);

	m_pInsFeaDlg->setListWidgetBcgDef();

	// �������������Ҫִ�еĲ���
	if (event->button() == Qt::LeftButton)
	{
		setStyleSheet(getStyleSheet(m_bcgR, m_bcgG, m_bcgB));

		switch (m_nInd)
		{
		case 1:
		{

			m_pInsFeaDlg->faceSelG1();
			//m_pTabWidget->selCylin();

			break;
		}
		case 2:
		{
			m_pInsFeaDlg->faceSelG2();

			//m_pTabWidget->selBasePlane();
			break;
		}
		default:
			break;
		}

	}

}

QString FInsFeatureDlg::IFDListWidget::getStyleSheet(double _r, double _g, double _b)
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
