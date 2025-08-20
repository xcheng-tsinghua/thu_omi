#include "FInsFeaTreeWnd.h"
#include "PointPlanDlg.h"
#include "FInsFeature.h"
#include "FMainWindow.h"
#include "AccuEvaluWnd.h"
#include "np.h"
#include "occConverter.h"

using namespace std;
extern QString gDockTitleColor;

FInsFeaTreeWnd::FInsFeaTreeWnd(QWidget* parent, FMainDoc* pDoc)
	:QDockWidget(parent), m_pDoc(pDoc)
{
	this->setAttribute(Qt::WA_StyledBackground, true);

	//�Ҽ��˵�
	m_pToPointPlan = new QAction(QString::fromLocal8Bit("���滮"), this);
	m_pAccuEvalu = new QAction(QString::fromLocal8Bit("��������"), this);
	m_pFInsFeaDel = new QAction(QString::fromLocal8Bit("�Ƴ��������"), this);
	m_pFoldAll = new QAction(QString::fromLocal8Bit("�۵�ȫ��"), this);
	m_pExpandAll = new QAction(QString::fromLocal8Bit("չ��ȫ��"), this);
	m_pEvaluAll = new QAction(QString::fromLocal8Bit("����ȫ��"), this);
	m_pFacePntNumCal = new QAction(QString::fromLocal8Bit("�������ͼ"), this);

	m_pToPointPlan->setIcon(QIcon("../icons/PntPlan.png"));
	m_pAccuEvalu->setIcon(QIcon("../icons/AccuEva.png"));
	m_pFInsFeaDel->setIcon(QIcon("../icons/FInsDel.png"));
	m_pFoldAll->setIcon(QIcon("../icons/Fold.png"));
	m_pExpandAll->setIcon(QIcon("../icons/Expand.png"));
	m_pEvaluAll->setIcon(QIcon("../icons/EvaluAll.png"));
	m_pFacePntNumCal->setIcon(QIcon("../icons/InsFeaGraph.png"));

	m_pPopMenu = QSharedPointer<QMenu>(new QMenu());

	setWindowTitle(QString::fromLocal8Bit("���������"));
	setMinimumWidth(200);
	QWidget* dockWidgetContents = new QWidget();
	dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
	QWidget* widget = new QWidget(dockWidgetContents);
	widget->setObjectName(QString::fromUtf8("widget"));
	//QVBoxLayout* verticalLayout_2 = new QVBoxLayout(widget);
	QVBoxLayout* verticalLayout_2 = new QVBoxLayout(dockWidgetContents);
	verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
	verticalLayout_2->setContentsMargins(0, 0, 0, 0);

	m_pTree = new FInsTreeWidget(widget);
	//m_pTree->setHeaderLabel(QString::fromLocal8Bit("Inspection Features"));
	m_pTree->setHeaderHidden(true);
	verticalLayout_2->addWidget(m_pTree);

	setWidget(dockWidgetContents);

	connect(m_pToPointPlan, SIGNAL(triggered()),this, SLOT(onToPointPlan()));
	connect(m_pAccuEvalu, SIGNAL(triggered()), this, SLOT(onToAccuEvalu()));
	connect(m_pFInsFeaDel, SIGNAL(triggered()), this, SLOT(onFInsFeaDel()));
	connect(m_pEvaluAll, SIGNAL(triggered()), this, SLOT(onEvaluAll()));
	connect(m_pFoldAll, SIGNAL(triggered()), this, SLOT(onFoldAll()));
	connect(m_pExpandAll, SIGNAL(triggered()), this, SLOT(onExpandAll()));
	connect(m_pFacePntNumCal, SIGNAL(triggered()), this, SLOT(onComputeSurfPntNum()));

	//style
	{
		//������
		this->setStyleSheet("QDockWidget::title"
			"{"
			"padding-left: 10px;"
			"padding-top: 4px;"
			"background:" + gDockTitleColor +
			"}"
			"QDockWidget"
			"{"
			"color: rgb(247,249,254);"//��������������ɫ
			"}"
		);

		//�Ҽ��˵�
		m_pPopMenu->setStyleSheet(
			"QMenu"
			"{"
			"background-color:rgb(233,238,255);"
			//"border:0px solid rgb(148,166,202);"
			"border:1px solid rgb(93,107,153);"
			"}"
			"QMenu::item:selected"
			"{"
			"color:black;"
			"background-color:rgb(177,197,255);"
			"}"
			"QMenu::item:pressed"
			"{"
			"color:black;"
			"background-color:rgb(177,197,255);"
			"}"
		);

		//������
		dockWidgetContents->setStyleSheet(
			"QWidget"
			"{"
			"border: 1px solid rgb(232,232,236);"
			"background:rgb(247,249,254);"
			"}"
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
			"}"
		);
	}
}

FInsFeaTreeWnd::~FInsFeaTreeWnd()
{

}

void FInsFeaTreeWnd::initTree(QList<QTreeWidgetItem*> treeList)
{
	if (treeList.size() > 0)
	{
		m_pTree->insertTopLevelItems(0, treeList);
		m_pTree->expandAll();
	}
}

void FInsFeaTreeWnd::clearTree()
{
	m_pTree->clear();
}

void FInsFeaTreeWnd::initTree_assembly(QList<QTreeWidgetItem*> treeList)
{
	if (treeList.size() > 0)
	{
		m_pTree->insertTopLevelItems(0, treeList);
		m_pTree->expandAll();

		/*for (int i = 0; i < treeList.size(); i++)
		{
			pAssemItem->addChild(treeList.at(i));
		}*/

	}
}

void FInsFeaTreeWnd::contextMenuEvent(QContextMenuEvent* event)
{
	QPoint point = QCursor::pos();
	QPoint localPos = m_pTree->viewport()->mapFromGlobal(point);
	QTreeWidgetItem* pItem = m_pTree->itemAt(localPos);
	m_pTmpItem = pItem;

	//������ѡ���item�Ƿ�׼ȷ
	if (pItem)
	{
		std::cout << pItem->text(0).toLocal8Bit().data() << std::endl;
		std::cout << pItem->type() << std::endl;
	}

	//���ѡ����item
	if (pItem)
	{
		//������ڵ�Ϊ�գ���Ϊ���ڵ�(һ��)���������£�
		if (pItem->parent() == NULL)
		{
			m_pPopMenu->clear();
			m_pPopMenu->addAction(m_pAccuEvalu);
			m_pPopMenu->addAction(m_pEvaluAll);
			m_pPopMenu->addSeparator();

			m_pPopMenu->addAction(m_pFInsFeaDel);
			m_pPopMenu->addSeparator();

			m_pPopMenu->addAction(m_pFacePntNumCal);
			m_pPopMenu->addSeparator();

			m_pPopMenu->addAction(m_pFoldAll);
			m_pPopMenu->addAction(m_pExpandAll);

			m_pPopMenu->exec(QCursor::pos());
		}

		else if (pItem->parent()->text(0).contains(QString::fromLocal8Bit("����")))
		{
			m_pPopMenu->clear();
			m_pPopMenu->addAction(m_pToPointPlan);
			m_pPopMenu->addSeparator();

			m_pPopMenu->addAction(m_pFacePntNumCal);
			m_pPopMenu->addSeparator();

			m_pPopMenu->addAction(m_pFoldAll);
			m_pPopMenu->addAction(m_pExpandAll);

			m_pPopMenu->exec(QCursor::pos());
		}
		else
		{
			m_pPopMenu->clear();

			m_pPopMenu->addAction(m_pFacePntNumCal);
			m_pPopMenu->addSeparator();

			m_pPopMenu->addAction(m_pFoldAll);
			m_pPopMenu->addAction(m_pExpandAll);

			m_pPopMenu->exec(QCursor::pos());
		}
	}
}

void FInsFeaTreeWnd::onToAccuEvalu()
{
	//QString FaceName = m_pTmpItem->text(0);
	//QString Number;
	//for (int j = 0; j < FaceName.length(); j++)
	//{
	//	if (FaceName[j] >= '0' && FaceName[j] < '9')
	//		Number.append(FaceName[j]);
	//}
	//int trs = Number.toInt();
	//static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->initTree_assembly(gWindow->GetMainDoc()->m_InsFeatureList[Number.toInt()]);

	QString FaceName = m_pTmpItem->text(0);
	QString Number;
	for (int j = 0; j < FaceName.length(); j++)
	{
		if (FaceName[j] >= '0' && FaceName[j] <= '9')
			Number.append(FaceName[j]);
	}

	static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->AcEvaluate((FInsFeature*)(m_pTmpItem->data(0, Qt::UserRole).value<void*>()));
	static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->raise();
}

void FInsFeaTreeWnd::onFInsFeaDel()
{
	//ɾ��m_pPntItemList�еĲ��
	vector<QTreeWidgetItem*> tmpItemList;
	for (int i = 0; i < m_pTree->topLevelItemCount(); i++)
	{
		if (m_pTmpItem->child(3) != m_pTree->topLevelItem(i)->child(3))
		{
			tmpItemList.push_back(m_pTree->topLevelItem(i)->child(3));
		}
	}
	m_pPntItemList = tmpItemList;

	//�ҵ�indexΪѡ��item�ļ������,ɾ��Doc���ӦԪ��
	vector<FInsFeature*> tmpLst;
	for (int i = 0; i < gWindow->GetMainDoc()->GetInsFeaList().size(); i++)
	{
		if (gWindow->GetMainDoc()->GetInsFeaList()[i]->InsFeatureindex == ExNumFrQsg(m_pTmpItem->text(0)))
		{
			for (int j = 0; j < gWindow->GetMainDoc()->GetInsFeaList().size(); j++)
			{
				if (j != i)
				{
					tmpLst.push_back(gWindow->GetMainDoc()->GetInsFeaList()[j]);
				}
			}
			
			gWindow->GetMainDoc()->GetInsFeaList() = tmpLst;

			break;//һ��ֻɾ��һ��
		}
	}

	//����ü�����������˾������������Ҳ��б�ͬʱɾ���Ҳ��б�����
	for (int i = 0; i < static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->GetEvaInsList().size(); i++)
	{
		if (ExNumFrQsg(static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->GetModel()->item(i, 0)->text()) == ExNumFrQsg(m_pTmpItem->text(0)))
		{
			static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->GetModel()->removeRow(i);

			//���¾����������ڼ�������б�
			tmpLst.clear();
			for (int j = 0; j < static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->GetEvaInsList().size(); j++)
			{
				if (j != i)
				{
					tmpLst.push_back(static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->GetEvaInsList()[j]);
				}
			}
			static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->GetEvaInsList() = tmpLst;

			//ɾ��������������ڵĶ�Ӧ��Ŀ
			vector<EvaluData> tmpEvaluDataList;
			for (int j = 0; j < static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->GetEvaDataList().size(); j++)
			{
				if (j != i)
				{
					tmpEvaluDataList.push_back(static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->GetEvaDataList()[j]);
				}
			}
			static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->GetEvaDataList() = tmpEvaluDataList;

			//break;
		}
	}

	//ɾ�������������ڵ�Ŀ��������
	int NbAccuWndItem = static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->GetTree()->topLevelItemCount();

	for (int i = 0; i < NbAccuWndItem; i++)
	{
		if ((FSurface*)(static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->GetTree()->topLevelItem(i)->data(0, Qt::UserRole).value<void*>()) == (FSurface*)(m_pTmpItem)->data(0, Qt::UserRole).value<void*>())
		{
			delete static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->GetTree()->topLevelItem(i);

			break;
		}
	}
	
	//ɾ��������������ڵ�Ŀ��������
	delete m_pTmpItem;
}

void FInsFeaTreeWnd::onExpandAll()
{
	m_pTree->expandAll();
}

void FInsFeaTreeWnd::onFoldAll()
{
	m_pTree->collapseAll();
}

void FInsFeaTreeWnd::onEvaluAll()
{
	if (gWindow->GetMainDoc()->GetMesPntList().size() == 0)
	{
		QMessageBox::information(this, QString::fromLocal8Bit("WARINING"),
			QString::fromLocal8Bit("δ��⵽ʵ�ʲ�㣬��ȷ���ѻ�ȡʵ�ʲ������֮����ִ�д˲���"), "OK");

		return;
	}

	int NbAllItem = static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->GetTree()->topLevelItemCount();
	for (int i = 0; i < NbAllItem; i++)
	{
		//���ĳ���Ѿ����������ظ���������Ŀ
		bool IsEvalued = false;

		for (int j = 0; j < static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->GetEvaDataList().size(); j++)
		{
			if ((FInsFeature*)(static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->GetTree()->topLevelItem(i)->data(0, Qt::UserRole).value<void*>()) == (static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->GetEvaDataList()[j].p_InsFea))
			{
				IsEvalued = true;
				break;
			}
		}

		if (!IsEvalued)
		{
			static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->AcEvaluate((FInsFeature*)(static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->GetTree()->topLevelItem(i)->data(0, Qt::UserRole).value<void*>()));
		}
	}

	static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->raise();
}

//void FInsFeaTreeWnd::onUpdateMeasPnts()
//{
//	//��ɾ���������еĲ���
//	int NumOfInsedPntItems = m_pPntItemList.size();
//	for (int i = 0; i < NumOfInsedPntItems; i++)
//	{
//		delete m_pPntItemList[NumOfInsedPntItems - 1 - i];
//	}
//	m_pPntItemList.clear();
//
//	//Ϊÿ�����������Ӳ���
//	int NumOfInsFeature = m_pTree->topLevelItemCount();
//
//	for (int i = 0; i < NumOfInsFeature; i++)
//	{
//		vector<int> pntIndexList;
//
//		FIpPntList theoPntsAll = gWindow->GetMainDoc()->GetIpPntList();
//		FInsFeature* cFInsFea = (FInsFeature*)(m_pTree->topLevelItem(i)->data(0, Qt::UserRole).value<void*>());
//		gWindow->AssignTheoPnts(cFInsFea, pntIndexList, theoPntsAll);
//
//		int AllPntsNumber = pntIndexList.size();
//
//		QTreeWidgetItem* PntsItem = new QTreeWidgetItem();
//		PntsItem->setText(0, QString::fromLocal8Bit("����(��") + QString::number(AllPntsNumber) + QString::fromLocal8Bit("��)"));
//
//		for (int j = 0; j < AllPntsNumber; j++)
//		{
//			QTreeWidgetItem* SinglePntItem = new QTreeWidgetItem();
//
//			QString PntNameStr = QString::fromLocal8Bit("���");
//			PntNameStr += QString::number(pntIndexList[j]);
//
//			SinglePntItem->setText(0, PntNameStr);
//
//			PntsItem->addChild(SinglePntItem);
//		}
//
//		m_pTree->topLevelItem(i)->addChild(PntsItem);
//		m_pPntItemList.push_back(PntsItem);
//	}
//
//	//QMessageBox::warning(
//	//	QApplication::activeWindow(), QObject::tr("Caution"), QObject::tr(" File extension is not supported in current version!"));
//}

void FInsFeaTreeWnd::onComputeSurfPntNum()
{
	FInsFeaGraph* pNewFea = new FInsFeaGraph;

	gWindow->addDockWidget(Qt::LeftDockWidgetArea, pNewFea);

	// �۵���ม������
	gWindow->tabifyDockWidget(pNewFea, (FTreeWnd*)gWindow->GetSubWnd()[0]);
	gWindow->tabifyDockWidget((FTreeWnd*)gWindow->GetSubWnd()[0], (FInsFeaTreeWnd*)gWindow->GetSubWnd()[6]);

	pNewFea->show();

	pNewFea->raise();


}

void FInsFeaTreeWnd::onToPointPlan()
{
	PointPlanDlg* pPointPlan = NULL;
	if (!gWindow->GetPointPlanDlg())
	{
		pPointPlan = new PointPlanDlg;
		gWindow->SetPointPlanDlg(pPointPlan);
		pPointPlan = gWindow->GetPointPlanDlg();
	}
	else
	{
		pPointPlan = gWindow->GetPointPlanDlg();
	}

	QString InsName = m_pTmpItem->parent()->parent()->text(0);
	QString Number;
	for (int j = 0; j < InsName.length(); j++)
	{
		if (InsName[j] >= '0' && InsName[j] <= '9')
			Number.append(InsName[j]);
	}
	FInsFeature* m_pFInsFeature = gWindow->GetMainDoc()->GetInsFeaList()[Number.toInt()];
	pPointPlan->setSelSurf(GetSurfFromItem(m_pTmpItem));

	if (m_pTmpItem->text(0).contains(QString::fromLocal8Bit("B��������")))
	{
		pPointPlan->setCBoxStatus(true);
	}
	else
	{
		pPointPlan->setCBoxStatus(false);
	}

	pPointPlan->GetSelPlaneLabel()->setText(m_pTmpItem->text(0));

	pPointPlan->show();

}

FInsTreeWidget::FInsTreeWidget(QWidget* parent) :QTreeWidget(parent)
{
	//�����۵�ͼ����ʽ
	this->setStyleSheet("QTreeView::branch:has-children:!has-siblings:closed,\
QTreeView::branch:closed:has-children:has-siblings{border-image: none; image: url(../icons/TreeCollapse.png);}\
QTreeView::branch:open:has-children:!has-siblings,\
QTreeView::branch:open:has-children:has-siblings{border-image: none; image: url(../icons/TreeExpand.png);}");
}

void FInsTreeWidget::mousePressEvent(QMouseEvent* event)
{
	this->QTreeWidget::mousePressEvent(event);

	if (event->button() == Qt::LeftButton)
	{
		gWindow->ResetToDefaultCountenance();

		QPoint point = QCursor::pos();
		QPoint localPos = viewport()->mapFromGlobal(point);
		QTreeWidgetItem* pItem = itemAt(localPos);
		//m_pTmpItem = pItem;

		if (!pItem) return;

		if (pItem->childCount() != 0 && pItem->child(0)->text(0).contains(QString::fromLocal8Bit("����")))//�㵽���㣬�����ü����������������
		{
			FInsFeature* CurrInsFea = (FInsFeature*)pItem->data(0, Qt::UserRole).value<void*>();

			for (int i = 0; i < CurrInsFea->m_FSurfList1.size(); i++)
			{
				CurrInsFea->m_FSurfList1[i]->SetColor(1, 0, 0);
			}

			for (int i = 0; i < CurrInsFea->m_FSurfList2.size(); i++)
			{
				CurrInsFea->m_FSurfList2[i]->SetColor(1, 0, 0);
			}

			gWindow->UpdateModelView();
		}
		if (pItem->parent() && pItem->parent()->text(0).contains(QString::fromLocal8Bit("����")))
		{
			FSurface* tmpFSurf = (FSurface*)pItem->data(0, Qt::UserRole).value<void*>();

			tmpFSurf->SetColor(1, 0, 0);

			gWindow->UpdateModelView();
		}


	}

}