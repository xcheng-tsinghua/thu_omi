#include "AccuEvaluWnd.h"
#include "FMainWindow.h"
#include "qgridlayout.h"
#include "qgroupbox.h"
#include "FInsReportDlg.h"
#include "qsplitter.h"
#include "ToleranceFunction.h"
#include "../External/fkernel/Include/DataStructure/FSurface.h"
#include "np.h"
#include "FInsFeaTreeWnd.h"
#include "occConverter.h"
#include "InspReportInfoDlg.h"
#include <functional>

extern FMainWindow* gWindow;
extern QString gDockTitleColor;

AccuEvaluWnd::AccuEvaluWnd(QWidget* parent, FMainDoc* pDoc)
	:QDockWidget(parent), m_pDoc(pDoc)
{
	this->setAttribute(Qt::WA_StyledBackground, true);
	setWindowTitle(QString::fromLocal8Bit("��������"));

	QSplitter* widget=new QSplitter(Qt::Horizontal, this);//���ô��ڷָ�

	m_pTree = new QTreeWidget(widget);
	m_pTree->setHeaderHidden(true);

	m_pEvaResView = new QTableView(widget);

	//��ʼ��ͼ��
	m_pEvaResModel = new QStandardItemModel(m_pEvaResView);
	InitializeModel(m_pEvaResModel);
	m_pEvaResView->setModel(m_pEvaResModel);
	m_pEvaResView->setAlternatingRowColors(true);
	m_pEvaResView->horizontalHeader()->setMinimumHeight(27);
	m_pEvaResView->verticalHeader()->setMinimumWidth(27);
	m_pEvaResView->setEditTriggers(QAbstractItemView::NoEditTriggers);	

	//��ʼ���Ҽ��˵�
	m_pAccuEvalu = new QAction(QString::fromLocal8Bit("��������"), this);
	m_pEvaluReport = new QAction(QString::fromLocal8Bit("���ɼ�ⱨ��"), this);
	m_pFeaDel = new QAction(QString::fromLocal8Bit("�Ƴ��������"), this);
	m_pAccuEvaDel = new QAction(QString::fromLocal8Bit("�Ƴ�������Ŀ"), this);
	m_pEvaluAll = new QAction(QString::fromLocal8Bit("����ȫ��"), this);
	m_pRemoveAllEvaluItem = new QAction(QString::fromLocal8Bit("�Ƴ�ȫ����Ŀ"), this);

	m_pPopMenu = QSharedPointer<QMenu>(new QMenu());

	widget->setStretchFactor(0, 20);
	widget->setStretchFactor(1, 80);

	setWidget(widget);

	connect(m_pAccuEvalu, SIGNAL(triggered()), this, SLOT(onToAccuEvalu()));
	connect(m_pEvaluReport, SIGNAL(triggered()), this, SLOT(onToEvaluReport()));
	connect(m_pFeaDel, SIGNAL(triggered()), this, SLOT(onFInsFeaDel()));
	connect(m_pAccuEvaDel, SIGNAL(triggered()), this, SLOT(onChartRowDel()));
	connect(m_pEvaluAll, SIGNAL(triggered()), this, SLOT(onEvaluAll()));
	connect(m_pRemoveAllEvaluItem, SIGNAL(triggered()), this, SLOT(onRemoveAllEvaluItem()));

	// style
	{
		//ͼ��
		m_pAccuEvalu->setIcon(QIcon("../icons/AccuEva.png"));
		m_pEvaluReport->setIcon(QIcon("../icons/AccuRep.png"));
		m_pFeaDel->setIcon(QIcon("../icons/FInsDel.png"));
		m_pAccuEvaDel->setIcon(QIcon("../icons/AccuRepDel.png"));
		m_pEvaluAll->setIcon(QIcon("../icons/EvaluAll.png"));
		m_pRemoveAllEvaluItem->setIcon(QIcon("../icons/RemoveAllEvaluItem.png"));

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
			"QTabBar::tab:selected"
			"{"
			"background:rgba(232,232,236,100%);"
			"}"
		);

		//�������۵�ͼ����ʽ
		m_pTree->setStyleSheet(
			"QTreeView::branch:has-children:!has-siblings:closed,\
QTreeView::branch:closed:has-children:has-siblings{border-image: none; image: url(../icons/TreeCollapse.png);}\
QTreeView::branch:open:has-children:!has-siblings,\
QTreeView::branch:open:has-children:has-siblings{border-image: none; image: url(../icons/TreeExpand.png);}");

		//�����ͷ��ʽ
		m_pEvaResView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(204,213,240);border: 1px solid rgb(64,86,141);font:10pt 'Microsoft YaHei';color: black;};");

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
		widget->setStyleSheet(
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

AccuEvaluWnd::~AccuEvaluWnd()
{

}

void AccuEvaluWnd::initTree(QList<QTreeWidgetItem*> treeList)
{
	if (treeList.size() > 0)
	{
		m_pTree->insertTopLevelItems(0, treeList);
		m_pTree->collapseAll();
	}
}

void AccuEvaluWnd::clearTree()
{
	m_pTree->clear();
}

void AccuEvaluWnd::initTree_assembly(FInsFeature* FinsFea)
{
	function<QString(FSurface*)> theFSurfName = [](FSurface* aFSurf)->QString
	{
		SURFACE_TYPE theSurfType;
		int ind_in_all = 0;
		int ind_in_type = 0;
		QString tmpFSurfName;

		gWindow->GetSTEPModelConverter()->FSurfInfo(aFSurf, theSurfType, ind_in_all, ind_in_type, tmpFSurfName);

		return tmpFSurfName;
	};

	//�������������Ҫ
	QTreeWidgetItem* m_pMainItem = new QTreeWidgetItem;
	m_pMainItem->setText(0, QString::fromLocal8Bit("�������") + QString::number(FinsFea->InsFeatureindex));
	m_pMainItem->setData(0, Qt::UserRole, QVariant::fromValue((void*)(FinsFea)));

	//���棬��һ
	QTreeWidgetItem* m_pFaceItem = new QTreeWidgetItem;
	m_pFaceItem->setText(0, QString::fromLocal8Bit("����"));

	for (int i = 0; i < FinsFea->m_FSurfList1.size(); i++)
	{
		QTreeWidgetItem* TempItem = new QTreeWidgetItem;

		TempItem->setText(0, theFSurfName(FinsFea->m_FSurfList1[i]));
		m_pFaceItem->addChild(TempItem);
	}

	for (int i = 0; i < FinsFea->m_FSurfList2.size(); i++)
	{
		QTreeWidgetItem* TempItem = new QTreeWidgetItem;
		TempItem->setText(0, theFSurfName(FinsFea->m_FSurfList2[i]));
		m_pFaceItem->addChild(TempItem);
	}

	//������ͣ��ڶ�
	QTreeWidgetItem* m_pTypeItemMain = new QTreeWidgetItem;
	m_pTypeItemMain->setText(0, QString::fromLocal8Bit("�������"));
	QTreeWidgetItem* m_pTypeItem = new QTreeWidgetItem;
	switch (FinsFea->type)
	{
		case PARALLELISM:
		{
			m_pTypeItem->setText(0, QString::fromLocal8Bit("ƽ�ж�"));
			break;
		}
		case ROUNDNESS:
		{
			m_pTypeItem->setText(0, QString::fromLocal8Bit("Բ��"));
			break;
		}
		case DISTANCE:
		{
			if (FinsFea->DirState == 1)
			{
				m_pTypeItem->setText(0, QString::fromLocal8Bit("X�����"));
			}
			else if (FinsFea->DirState == 2)
			{
				m_pTypeItem->setText(0, QString::fromLocal8Bit("Y�����"));
			}
			else if (FinsFea->DirState == 3)
			{
				m_pTypeItem->setText(0, QString::fromLocal8Bit("Z�����"));
			}
			else
			{
				m_pTypeItem->setText(0, QString::fromLocal8Bit("����"));
			}
			
			break;
		}
		case VERTICALITY:
		{
			m_pTypeItem->setText(0, QString::fromLocal8Bit("��ֱ��"));
			break;
		}
		case DIAMETER:
		{
			m_pTypeItem->setText(0, QString::fromLocal8Bit("ֱ��"));
			break;
		}
		case AUXIALITY:
		{
			m_pTypeItem->setText(0, QString::fromLocal8Bit("ͬ���"));
			break;
		}
		case FLATNESS:
		{
			m_pTypeItem->setText(0, QString::fromLocal8Bit("ƽ���"));
			break;
		}
		default:
		{
			m_pTypeItem->setText(0, QString::fromLocal8Bit("δѡ��"));
			break;
		}
	}
	m_pTypeItemMain->addChild(m_pTypeItem);
		
	//������������
	QTreeWidgetItem* m_pValueItem = new QTreeWidgetItem;
	m_pValueItem->setText(0, QString::fromLocal8Bit("����"));

	QTreeWidgetItem* pValueItem = new QTreeWidgetItem();
	QTreeWidgetItem* pUpDeviationItem = new QTreeWidgetItem();
	QTreeWidgetItem* pDownDeviationItem = new QTreeWidgetItem();

	if (FinsFea->type == PARALLELISM)
	{
		pValueItem->setText(0, QString::fromLocal8Bit("ƽ�ж����ֵ��") + QString::number(FinsFea->FInsToleranceValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pValueItem);
	}
	else if (FinsFea->type == ROUNDNESS)
	{
		pValueItem->setText(0, QString::fromLocal8Bit("Բ�����ֵ��") + QString::number(FinsFea->FInsToleranceValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pValueItem);
	}
	else if (FinsFea->type == DISTANCE)
	{

		pValueItem->setText(0, QString::fromLocal8Bit("�����׼ֵ��") + QString::number(FinsFea->FInsToleranceValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pValueItem);
		pUpDeviationItem->setText(0, QString::fromLocal8Bit("��ƫ�+") + QString::number(FinsFea->FInsUpDeviationValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pUpDeviationItem);
		pDownDeviationItem->setText(0, QString::fromLocal8Bit("��ƫ�-") + QString::number(FinsFea->FInsDownDeviationValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pDownDeviationItem);
	}
	else if (FinsFea->type == VERTICALITY)
	{
		pValueItem->setText(0, QString::fromLocal8Bit("��ֱ�����ֵ��") + QString::number(FinsFea->FInsToleranceValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pValueItem);
	}
	else if (FinsFea->type == DIAMETER)
	{
		pValueItem->setText(0, QString::fromLocal8Bit("ֱ����׼ֵ��") + QString::number(FinsFea->FInsToleranceValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pValueItem);
		pUpDeviationItem->setText(0, QString::fromLocal8Bit("��ƫ�+") + QString::number(FinsFea->FInsUpDeviationValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pUpDeviationItem);
		pDownDeviationItem->setText(0, QString::fromLocal8Bit("��ƫ�-") + QString::number(FinsFea->FInsDownDeviationValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pDownDeviationItem);
	}
	else if (FinsFea->type == AUXIALITY)
	{
		pValueItem->setText(0, QString::fromLocal8Bit("ͬ������ֵ��") + QString::number(FinsFea->FInsToleranceValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pValueItem);
	}
	else if (FinsFea->type == FLATNESS)
	{
		pValueItem->setText(0, QString::fromLocal8Bit("ƽ������ֵ��") + QString::number(FinsFea->FInsToleranceValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pValueItem);
	}

	m_pMainItem->addChild(m_pFaceItem);
	m_pMainItem->addChild(m_pTypeItemMain);
	m_pMainItem->addChild(m_pValueItem);

	m_pTree->addTopLevelItem(m_pMainItem);
	m_pTree->collapseAll();
}

void AccuEvaluWnd::contextMenuEvent(QContextMenuEvent* event)
{
	QPoint point = QCursor::pos();
	QPoint localPos = m_pTree->viewport()->mapFromGlobal(point);
	QTreeWidgetItem* pItem = m_pTree->itemAt(localPos);
	m_pTmpItem = pItem;

	//ͼ������Ŀ
	QPoint localPos2 = m_pEvaResView->viewport()->mapFromGlobal(point);
	QModelIndex index = m_pEvaResView->indexAt(localPos2);
	m_TableCurrRow = index.row();

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
			m_pPopMenu->addAction(m_pFeaDel);
			m_pPopMenu->exec(QCursor::pos());
		}
	}
	else if (index.isValid())//���ѡ����ͼ��
	{
		m_pPopMenu->clear();
		m_pPopMenu->addAction(m_pAccuEvaDel);
		m_pPopMenu->addAction(m_pRemoveAllEvaluItem);
		m_pPopMenu->exec(QCursor::pos());
	}
	else
	{
		m_pPopMenu->clear();
		m_pPopMenu->addAction(m_pEvaluReport);
		m_pPopMenu->exec(QCursor::pos());
	}
}

void AccuEvaluWnd::AcEvaluate(FInsFeature* pInsFeature)
{
	if (gWindow->GetMainDoc()->GetMesPntList().size() == 0)
	{
		QMessageBox::information(this, QString::fromLocal8Bit("WARINING"),
			QString::fromLocal8Bit("δ��⵽ʵ�ʲ�㣬��ȷ���ѻ�ȡʵ�ʲ������֮����ִ�д˲���"), "OK");

		return;
	}

	//FInsFeature* pInsFeature = gWindow->GetMainDoc()->m_InsFeatureList[InsFeaIndex];
	m_InsfratureList.push_back(pInsFeature);
	EvaluData InsRes = RealValueCal(pInsFeature);

	//�������Ϊ��ָ��˵���ü�������д���ĳЩ�����޲�㣬�޷����������趨613��ʵ��
	if (InsRes.p_InsFea == NULL)
	{
		return;
	}

	m_EvaluDataList.push_back(InsRes);
	//���ü��������
	m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 0, new QStandardItem(QString::fromLocal8Bit("�������") + QString::number(pInsFeature->InsFeatureindex)));

	if (pInsFeature->type == PARALLELISM || pInsFeature->type == ROUNDNESS || pInsFeature->type == VERTICALITY || pInsFeature->type == AUXIALITY || pInsFeature->type == FLATNESS || pInsFeature->type == PROFILE || pInsFeature->type == POSITION)//������λ����ʱ
	{
		//���ü�����ͺ��û��趨����
		if (pInsFeature->type == PARALLELISM)//���ƽ�ж�ʱ
		{
			//�������
			m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("ƽ�ж�")));
		}
		else if (pInsFeature->type == ROUNDNESS)//���Բ��ʱ
		{
			//�������
			m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("Բ��")));
		}
		else if (pInsFeature->type == VERTICALITY)//��ⴹֱ��ʱ
		{
			//�������
			m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("��ֱ��")));
		}
		else if (pInsFeature->type == AUXIALITY)//���ͬ���
		{
			//�������
			m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("ͬ���")));
		}
		else if (pInsFeature->type == FLATNESS)
		{
			//�������
			m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("ƽ���")));
		}
		else if (pInsFeature->type == PROFILE)
		{
			//�������
			m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("������")));
		}
		else if (pInsFeature->type == POSITION)
		{
			//�������
			m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("λ�ö�")));
		}

		//�û��趨����
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 2, new QStandardItem(QString::number(pInsFeature->FInsToleranceValue)));

		//ʵ�ʹ���
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 3, new QStandardItem(QString::number(InsRes.ToleranceValue)));
		
		//������������û���趨�ߴ硢ʵ�ʳߴ硢��ƫ���ƫ��
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 4, new QStandardItem("-"));
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 5, new QStandardItem("-"));
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 6, new QStandardItem("-"));
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 7, new QStandardItem("-"));
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 8, new QStandardItem("-"));
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 9, new QStandardItem("-"));
	}
	else if (pInsFeature->type == DISTANCE || pInsFeature->type == DIAMETER)//�����ߴ�ʱ
	{
		//���ü�����ͺ��û��趨�ߴ缰ʵ�ʳߴ�
		if (pInsFeature->type == DISTANCE)//������ʱ
		{
			//�������
			if (pInsFeature->DirState == 1)
			{
				m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("X�����")));
			}
			else if (pInsFeature->DirState == 2)
			{
				m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("Y�����")));
			}
			else if (pInsFeature->DirState == 3)
			{
				m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("Z�����")));
			}
			else
			{
				m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("����")));
			}
		}
		else if (pInsFeature->type == DIAMETER)//���ֱ��ʱ
		{
			//�������
			m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("ֱ��")));
		}
		
		//�ߴ������������趨�����ʵ�ʹ���
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 2, new QStandardItem("-"));
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 3, new QStandardItem("-"));

		//�趨����ϡ���ƫ��
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 4, new QStandardItem(QString::number(pInsFeature->FInsToleranceValue)));
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 5, new QStandardItem("+" + QString::number(pInsFeature->FInsUpDeviationValue)));
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 6, new QStandardItem("-" + QString::number(pInsFeature->FInsDownDeviationValue)));

		//ʵ�ʳߴ�
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 7, new QStandardItem(QString::number(InsRes.DimensionValue)));

		//ʵ����ƫ��
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 8, new QStandardItem("+" + QString::number(InsRes.upDeviation)));

		//ʵ����ƫ��
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 9, new QStandardItem(QString::number(InsRes.downDeviation)));
	}
	else
	{
		return;
	}

	//�ж��Ƿ�ϸ�
	if (InsRes.qualify)//�ϸ�
	{
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 10, new QStandardItem(QString::fromLocal8Bit("�ϸ�")));
		m_pEvaResModel->item(m_InsfratureList.size() - 1, 10)->setForeground(QBrush(QColor(0, 255, 0)));//������ɫ
	}
	else//���ϸ�
	{
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 10, new QStandardItem(QString::fromLocal8Bit("���ϸ�")));
		m_pEvaResModel->item(m_InsfratureList.size() - 1, 10)->setForeground(QBrush(QColor(255, 0, 0)));//������ɫ
	}

	m_pEvaResModel->item(m_InsfratureList.size() - 1, 10)->setTextAlignment(Qt::AlignCenter);//���ݾ�����ʾ
	m_pEvaResModel->item(m_InsfratureList.size() - 1, 10)->setFont(QFont("Times", 10, QFont::Black));//���ݼӴ�
}

void AccuEvaluWnd::InitializeModel(QStandardItemModel* model)
{
	model->setColumnCount(11);
	model->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("���������"));
	model->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("�������"));
	model->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("�趨����"));
	model->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("ʵ�ʹ���"));
	model->setHeaderData(4, Qt::Horizontal, QString::fromLocal8Bit("�趨�ߴ�"));
	model->setHeaderData(5, Qt::Horizontal, QString::fromLocal8Bit("�趨��ƫ��"));
	model->setHeaderData(6, Qt::Horizontal, QString::fromLocal8Bit("�趨��ƫ��"));
	model->setHeaderData(7, Qt::Horizontal, QString::fromLocal8Bit("ʵ�ʳߴ�"));
	model->setHeaderData(8, Qt::Horizontal, QString::fromLocal8Bit("ʵ����ƫ��"));
	model->setHeaderData(9, Qt::Horizontal, QString::fromLocal8Bit("ʵ����ƫ��"));
	model->setHeaderData(10, Qt::Horizontal, QString::fromLocal8Bit("�Ƿ�ϸ�"));
}

EvaluData AccuEvaluWnd::RealValueCal(FInsFeature* pInsFea)
{
	EvaluData Res;
	Res.qualify = false;
	Res.p_InsFea = pInsFea;

	//��ȡʵ�ʺ����۲��
	QVector<FIpPoint>MesPntLstTmp = gWindow->GetMainDoc()->GetMesPntList();
	QVector<FIpPoint>ThoPntLstTmp = gWindow->GetMainDoc()->GetIpPntList();

	QVector<FIpPoint>MesPntLst = MesPntLstTmp;
	QVector<FIpPoint>ThoPntLst = ThoPntLstTmp;

	// Ѱ��λ��Ŀ��������������ƽ���ڵĲ��
	// pair�У�FSurface* Ϊ���棬vector<gp_Pnt> Ϊ�����ϵĲ��
	vector<pair<FSurface*, vector<gp_Pnt>>> Surf_PntGroup1;
	vector<pair<FSurface*, vector<gp_Pnt>>> Surf_PntGroup2;

	int NbFace1 = pInsFea->m_FSurfList1.size();
	for (int i = 0; i < NbFace1; i++)
	{
		// ��¼�ҳ����ڱ���ѭ���е�Ŀ�������ϵĲ�����������ں���ɾ�������������ϵĵ㲻��Ҫ������������ҵ��ĵ�
		vector<int> TarPntInd;

		vector<gp_Pnt> SearchedPnts;
		FSurface* CurrSurf = pInsFea->m_FSurfList1[i];

		for (int j = 0; j < MesPntLst.size(); j++)
		{
			if (isPointInShape(GP_PNT(ThoPntLst[j]), gWindow->GetSTEPModelConverter()->FSurf2TDSFace(CurrSurf)))
			{
				SearchedPnts.push_back(GP_PNT(MesPntLst[j]));

				pair<gp_Pnt, gp_Pnt> Tho_Mes;
				Tho_Mes.first = GP_PNT(ThoPntLst[j]);
				Tho_Mes.second = GP_PNT(MesPntLst[j]);
				Res.ThoMesPntList.push_back(Tho_Mes);
			}

		}

		Surf_PntGroup1.push_back(pair<FSurface*, vector<gp_Pnt>>(CurrSurf, SearchedPnts));

		//ȥ���Ѿ���Ϊ�ϸ�ĵ㣬��һ��ѭ��ֱ��ʹ��ȥ����ǰ�����ϵĵ��ʣ�µĵ�
		QVector<FIpPoint> MesReNewPnts;
		QVector<FIpPoint> ThoReNewPnts;

		for (int j = 0; j < TarPntInd.size(); j++)
		{
			MesPntLst[TarPntInd[j]].x = 999999;
			ThoPntLst[TarPntInd[j]].x = 999999;
		}
		for (int j = 0; j < MesPntLst.size(); j++)
		{
			if (MesPntLst[j].x != 999999)
			{
				MesReNewPnts.push_back(MesPntLst[j]);
			}

			if (ThoPntLst[j].x != 999999)
			{
				ThoReNewPnts.push_back(ThoPntLst[j]);
			}
		}
		MesPntLst = MesReNewPnts;
		ThoPntLst = ThoReNewPnts;







		//// ��¼�ҳ����ڱ���ѭ���е�Ŀ�������ϵĲ�����������ں���ɾ�������������ϵĵ㲻��Ҫ���������
		//vector<int> TarPntInd;

		//vector<gp_Pnt> SearchedPnts;

		//FSurface* CurrSurf = pInsFea->m_FSurfList1[i];
		//ON_Mesh* CurrMesh = CurrSurf->GetMesh()->OnMesh();
		//ON_3fPoint onV[3];//������Ƭ�����ǵ�
		//int nTriFace = CurrMesh->FaceCount();

		//for (int j = 0; j < MesPntLst.size(); j++)
		//{
		//	//����ĳ����㵽��������ڵ����ÿ��������Ƭ֮��ľ���
		//	vector<double> Er;
		//	double ErRes;

		//	for (int k = 0; k < nTriFace; k++)
		//	{
		//		ON_MeshFace face = CurrMesh->m_F[k];
		//		onV[0] = CurrMesh->m_V[face.vi[0]];
		//		onV[1] = CurrMesh->m_V[face.vi[1]];
		//		onV[2] = CurrMesh->m_V[face.vi[2]];

		//		if (distance(ErRes, GP_PNT(ThoPntLst[j]), GP_PNT(onV[0]), GP_PNT(onV[1]), GP_PNT(onV[2])))//�ж����۲�㵽mesh�ľ��룬
		//		{
		//			Er.push_back(ErRes);
		//		}
		//	}
		//	if (Er.size() != 0 && MIN(Er) < 0.5)//��ֵ1���������ϵĲ�㵽������ľ��벻��С�ڸ���ֵ
		//	{
		//		SearchedPnts.push_back(GP_PNT(MesPntLst[j]));

		//		pair<gp_Pnt, gp_Pnt> Tho_Mes;
		//		Tho_Mes.first = GP_PNT(ThoPntLst[j]);
		//		Tho_Mes.second = GP_PNT(MesPntLst[j]);
		//		Res.ThoMesPntList.push_back(Tho_Mes);

		//		TarPntInd.push_back(j);
		//	}
		//}

		//Surf_PntGroup1.push_back(pair<FSurface*, vector<gp_Pnt>>(CurrSurf, SearchedPnts));

		////ȥ���Ѿ���Ϊ�ϸ�ĵ㣬��һ��ѭ��ֱ��ʹ��ȥ����ǰ�����ϵĵ��ʣ�µĵ�
		//QVector<FIpPoint> MesReNewPnts;
		//QVector<FIpPoint> ThoReNewPnts;

		//for (int j = 0; j < TarPntInd.size(); j++)
		//{
		//	MesPntLst[TarPntInd[j]].x = 999999;
		//	ThoPntLst[TarPntInd[j]].x = 999999;
		//}
		//for (int j = 0; j < MesPntLst.size(); j++)
		//{
		//	if (MesPntLst[j].x != 999999)
		//	{
		//		MesReNewPnts.push_back(MesPntLst[j]);
		//	}

		//	if (ThoPntLst[j].x != 999999)
		//	{
		//		ThoReNewPnts.push_back(ThoPntLst[j]);
		//	}
		//}
		//MesPntLst = MesReNewPnts;
		//ThoPntLst = ThoReNewPnts;
	}

	int NbFace2 = pInsFea->m_FSurfList2.size();
	for (int i = 0; i < NbFace2; i++)
	{
		// ��¼�ҳ����ڱ���ѭ���е�Ŀ�������ϵĲ�����������ں���ɾ�������������ϵĵ㲻��Ҫ���������
		vector<int> TarPntInd;

		vector<gp_Pnt> SearchedPnts;

		FSurface* CurrSurf = pInsFea->m_FSurfList2[i];

		for (int j = 0; j < MesPntLst.size(); j++)
		{
			if (isPointInShape(GP_PNT(ThoPntLst[j]), gWindow->GetSTEPModelConverter()->FSurf2TDSFace(CurrSurf)))
			{
				SearchedPnts.push_back(GP_PNT(MesPntLst[j]));

				pair<gp_Pnt, gp_Pnt> Tho_Mes;
				Tho_Mes.first = GP_PNT(ThoPntLst[j]);
				Tho_Mes.second = GP_PNT(MesPntLst[j]);
				Res.ThoMesPntList.push_back(Tho_Mes);

				TarPntInd.push_back(j);

			}

		}

		Surf_PntGroup2.push_back(pair<FSurface*, vector<gp_Pnt>>(CurrSurf, SearchedPnts));

		//ȥ���Ѿ���Ϊ�ϸ�ĵ㣬��һ��ѭ��ֱ��ʹ��ȥ����ǰ�����ϵĵ��ʣ�µĵ�
		QVector<FIpPoint> MesReNewPnts;
		QVector<FIpPoint> ThoReNewPnts;

		for (int j = 0; j < TarPntInd.size(); j++)
		{
			MesPntLst[TarPntInd[j]].x = 999999;
			ThoPntLst[TarPntInd[j]].x = 999999;
		}
		for (int j = 0; j < MesPntLst.size(); j++)
		{
			if (MesPntLst[j].x != 999999)
			{
				MesReNewPnts.push_back(MesPntLst[j]);
			}

			if (ThoPntLst[j].x != 999999)
			{
				ThoReNewPnts.push_back(ThoPntLst[j]);
			}
		}
		MesPntLst = MesReNewPnts;
		ThoPntLst = ThoReNewPnts;






		//// ��¼�ҳ����ڱ���ѭ���е�Ŀ�������ϵĲ�����������ں���ɾ�������������ϵĵ㲻��Ҫ���������
		//vector<int> TarPntInd;

		//vector<gp_Pnt> SearchedPnts;

		//FSurface* CurrSurf = pInsFea->m_FSurfList2[i];
		//ON_Mesh* CurrMesh = CurrSurf->GetMesh()->OnMesh();
		//ON_3fPoint onV[3];//������Ƭ�����ǵ�
		//int nTriFace = CurrMesh->FaceCount();

		//for (int j = 0; j < MesPntLst.size(); j++)
		//{
		//	//����ĳ����㵽��������ڵ����ÿ��������Ƭ֮��ľ���
		//	vector<double> Er;
		//	double ErRes;

		//	for (int k = 0; k < nTriFace; k++)
		//	{
		//		ON_MeshFace face = CurrMesh->m_F[k];
		//		onV[0] = CurrMesh->m_V[face.vi[0]];
		//		onV[1] = CurrMesh->m_V[face.vi[1]];
		//		onV[2] = CurrMesh->m_V[face.vi[2]];

		//		if (distance(ErRes, GP_PNT(ThoPntLst[j]), GP_PNT(onV[0]), GP_PNT(onV[1]), GP_PNT(onV[2])))//�ж����۲�㵽mesh�ľ��룬
		//		{
		//			Er.push_back(ErRes);
		//		}
		//	}
		//	if (Er.size() != 0 && MIN(Er) < 0.5)//��ֵ1���������ϵĲ�㵽������ľ��벻��С�ڸ���ֵ
		//	{
		//		SearchedPnts.push_back(GP_PNT(MesPntLst[j]));

		//		pair<gp_Pnt, gp_Pnt> Tho_Mes;
		//		Tho_Mes.first = GP_PNT(ThoPntLst[j]);
		//		Tho_Mes.second = GP_PNT(MesPntLst[j]);
		//		Res.ThoMesPntList.push_back(Tho_Mes);

		//		TarPntInd.push_back(j);
		//	}
		//}

		//Surf_PntGroup2.push_back(pair<FSurface*, vector<gp_Pnt>>(CurrSurf, SearchedPnts));

		////ȥ���Ѿ���Ϊ�ϸ�ĵ㣬��һ��ѭ��ֱ��ʹ��ȥ����ǰ�����ϵĵ��ʣ�µĵ�
		//QVector<FIpPoint> MesReNewPnts;
		//QVector<FIpPoint> ThoReNewPnts;

		//for (int j = 0; j < TarPntInd.size(); j++)
		//{
		//	MesPntLst[TarPntInd[j]].x = 999999;
		//	ThoPntLst[TarPntInd[j]].x = 999999;
		//}
		//for (int j = 0; j < MesPntLst.size(); j++)
		//{
		//	if (MesPntLst[j].x != 999999)
		//	{
		//		MesReNewPnts.push_back(MesPntLst[j]);
		//	}

		//	if (ThoPntLst[j].x != 999999)
		//	{
		//		ThoReNewPnts.push_back(ThoPntLst[j]);
		//	}
		//}
		//MesPntLst = MesReNewPnts;
		//ThoPntLst = ThoReNewPnts;
	}

	for (int i = 0; i < Surf_PntGroup1.size(); i++)
	{
		if (Surf_PntGroup1[i].second.size() == 0)
		{
			QMessageBox::information(this, QString::fromLocal8Bit("EROOR"),
				QString::fromLocal8Bit("��⵽�ü��������ĳЩ�������޲�㣬��ȷ�ϼ�����������������Ͼ��в�������"), "OK");

			Res.p_InsFea = NULL;
			return Res;
		}
	}

	for (int i = 0; i < Surf_PntGroup2.size(); i++)
	{
		if (Surf_PntGroup2[i].second.size() == 0)
		{
			QMessageBox::information(this, QString::fromLocal8Bit("EROOR"),
				QString::fromLocal8Bit("��⵽�ü��������ĳЩ�������޲�㣬��ȷ�ϼ�����������������Ͼ��в�������"), "OK");

			Res.p_InsFea = NULL;
			return Res;
		}
	}

	// ��ÿ��Ҫ���ϵĵ����һ��
	vector<gp_Pnt> AllSurfPntList1;
	vector<gp_Pnt> AllSurfPntList2;

	for (int i = 0; i < Surf_PntGroup1.size(); i++)
	{
		for (auto ita = Surf_PntGroup1[i].second.begin(); ita != Surf_PntGroup1[i].second.end(); ++ita)
		{
			AllSurfPntList1.push_back(*ita);
		}
	}

	for (int i = 0; i < Surf_PntGroup2.size(); i++)
	{
		for (auto ita = Surf_PntGroup2[i].second.begin(); ita != Surf_PntGroup2[i].second.end(); ++ita)
		{
			AllSurfPntList2.push_back(*ita);
		}
	}

	function<bool(FSurface*)> IsCylinder = [&](FSurface* aSurf)->bool
	{
		SURFACE_TYPE theType;
		int ind_in_all = 0;
		int ind_in_type = 0;
		QString tmpFSurfName;

		gWindow->GetSTEPModelConverter()->FSurfInfo(aSurf, theType, ind_in_all, ind_in_type, tmpFSurfName);

		if (theType == CYLINDER)
		{
			return true;
		}
		else
		{
			return false;
		}

	};

	if (pInsFea->type == PARALLELISM)//����ƽ�ж���λ����ʱ
	{
		//���ж��ǲ���Բ�����߶�Բ�����ߵ�ƽ�ж�(�в���)
		bool IsAxisToAxis = false;

		IsAxisToAxis = IsCylinder(Surf_PntGroup1[0].first);

		for (int i = 0; i < Surf_PntGroup1.size(); i++)
		{
			if (IsCylinder(Surf_PntGroup1[i].first))
			{
				IsAxisToAxis = true;
				break;
			}
		}

		if (!IsAxisToAxis)
		{
			for (int i = 0; i < Surf_PntGroup2.size(); i++)
			{
				if (IsCylinder(Surf_PntGroup2[i].first))
				{
					IsAxisToAxis = true;
					break;
				}
			}
		}

		if (IsAxisToAxis)
		{
			Res.ToleranceValue = ToleranceFunction::Parallelism_AxisToAxis(AllSurfPntList1, AllSurfPntList2, Surf_PntGroup1[0].first);

			if (pInsFea->FInsToleranceValue >= Res.ToleranceValue)
			{
				Res.qualify = true;
			}
		}
		else
		{
			Res.ToleranceValue = ToleranceFunction::Parallelism(AllSurfPntList1, AllSurfPntList2);

			if (pInsFea->FInsToleranceValue >= Res.ToleranceValue)
			{
				Res.qualify = true;
			}
		}


	}

	else if (pInsFea->type == ROUNDNESS)//����Բ����λ����ʱ
	{
		Res.ToleranceValue = ToleranceFunction::Roundness(AllSurfPntList1, Surf_PntGroup1[0].first);

		if (pInsFea->FInsToleranceValue >= Res.ToleranceValue)
		{
			Res.qualify = true;
		}
	}

	else if (pInsFea->type == VERTICALITY)//������ֱ����λ����ʱ
	{
		//���ж��ǲ���Բ�����߶�ƽ��Ĵ�ֱ��
		bool IsAxisToPlane = false;
		int theCylinderInd = -1;

		//���ҵ���һ����Բ��
		for (int i = 0; i < Surf_PntGroup1.size(); i++)
		{
			if (IsCylinder(Surf_PntGroup1[i].first))
			{
				IsAxisToPlane = true;
				theCylinderInd = 1;
				break;
			}
		}

		if (!IsAxisToPlane)
		{
			for (int i = 0; i < Surf_PntGroup2.size(); i++)
			{
				if (IsCylinder(Surf_PntGroup2[i].first))
				{
					IsAxisToPlane = true;
					theCylinderInd = 2;
					break;
				}
			}
		}

		if (IsAxisToPlane)
		{
			if (theCylinderInd == 1)
				Res.ToleranceValue = ToleranceFunction::Verticality_PlanToAxis(AllSurfPntList2, AllSurfPntList1, Surf_PntGroup1[0].first);
			else if (theCylinderInd == 2)
				Res.ToleranceValue = ToleranceFunction::Verticality_PlanToAxis(AllSurfPntList1, AllSurfPntList2, Surf_PntGroup2[0].first);
			else
				_DEBUG_ERROR("�оݴ���");

			if (pInsFea->FInsToleranceValue >= Res.ToleranceValue)
			{
				Res.qualify = true;
			}
		}
		else
		{
			Res.ToleranceValue = ToleranceFunction::Verticality(AllSurfPntList1, AllSurfPntList2);

			if (pInsFea->FInsToleranceValue >= Res.ToleranceValue)
			{
				Res.qualify = true;
			}
		}
	}

	else if (pInsFea->type == AUXIALITY)//����ͬ������
	{
		Res.ToleranceValue = ToleranceFunction::Axiality(AllSurfPntList1, AllSurfPntList2, Surf_PntGroup1[0].first);

		if (pInsFea->FInsToleranceValue >= Res.ToleranceValue)
		{
			Res.qualify = true;
		}
	}

	else if (pInsFea->type == DISTANCE)//��������ߴ�ʱ(�в���)
	{
		// ���ж��Ƿ���Բ�����ߵ�ƽ��ľ���
		SURFACE_TYPE atype = gWindow->GetOccConverter()->FSurfType(Surf_PntGroup1[0].first);
		SURFACE_TYPE btype = gWindow->GetOccConverter()->FSurfType(Surf_PntGroup2[0].first);

		if ((atype == CYLINDER && btype == PLANE) || (atype == PLANE && btype == CYLINDER))
		{
			if (atype == CYLINDER && btype == PLANE)
			{
				Res.DimensionValue = ToleranceFunction::Distance_AxisToPlane(AllSurfPntList1, AllSurfPntList2, Surf_PntGroup1[0].first, Surf_PntGroup2[0].first);

			}
			else
			{
				Res.DimensionValue = ToleranceFunction::Distance_AxisToPlane(AllSurfPntList2, AllSurfPntList1, Surf_PntGroup2[0].first, Surf_PntGroup1[0].first);

			}

			Res.Deviation = Res.DimensionValue - pInsFea->FInsToleranceValue;
		}
		else
		{
			//���жϾ������ͣ�Բ�����ߵ�Բ�����߻���ƽ�浽ƽ��
			bool IsAxisToAxis = false;
			for (int i = 0; i < Surf_PntGroup1.size(); i++)
			{
				if (IsCylinder(Surf_PntGroup1[i].first))
				{
					IsAxisToAxis = true;
					break;
				}
			}

			if (!IsAxisToAxis)
			{
				for (int i = 0; i < Surf_PntGroup2.size(); i++)
				{
					if (IsCylinder(Surf_PntGroup2[i].first))
					{
						IsAxisToAxis = true;
						break;
					}
				}
			}

			if (IsAxisToAxis)
			{
				Devdata tmpDev = ToleranceFunction::Distance_AxisToAxis(AllSurfPntList1, AllSurfPntList2, Surf_PntGroup1[0].first, Surf_PntGroup2[0].first, pInsFea->DirState);

				Res.DimensionValue = tmpDev.Value;
				Res.upDeviation = tmpDev.UpDeviation;
				Res.downDeviation = tmpDev.DownDeviation;

				Res.Deviation = Res.DimensionValue - pInsFea->FInsToleranceValue;

				if (Res.Deviation >= -pInsFea->FInsDownDeviationValue && Res.Deviation <= pInsFea->FInsUpDeviationValue)
				{
					Res.qualify = true;
				}

				//if (Res.DimensionValue >= pInsFea->FInsToleranceValue - pInsFea->FInsDownDeviationValue
				//	&& Res.DimensionValue <= pInsFea->FInsToleranceValue + pInsFea->FInsUpDeviationValue
				//	&& Res.DimensionValue + Res.downDeviation >= pInsFea->FInsToleranceValue - pInsFea->FInsDownDeviationValue
				//	&& Res.DimensionValue + Res.upDeviation <= pInsFea->FInsToleranceValue + pInsFea->FInsUpDeviationValue)
				//{
				//	Res.qualify = true;
				//}
			}
			else
			{
				Devdata tmpDev = ToleranceFunction::Distance(AllSurfPntList1, AllSurfPntList2, pInsFea->DirState);

				Res.DimensionValue = tmpDev.Value;
				Res.upDeviation = tmpDev.UpDeviation;
				Res.downDeviation = tmpDev.DownDeviation;

				Res.Deviation = Res.DimensionValue - pInsFea->FInsToleranceValue;

				if (Res.Deviation >= -pInsFea->FInsDownDeviationValue && Res.Deviation <= pInsFea->FInsUpDeviationValue)
				{
					Res.qualify = true;
				}

				//if (Res.DimensionValue >= pInsFea->FInsToleranceValue - pInsFea->FInsDownDeviationValue
				//	&& Res.DimensionValue <= pInsFea->FInsToleranceValue + pInsFea->FInsUpDeviationValue
				//	&& Res.DimensionValue + Res.downDeviation >= pInsFea->FInsToleranceValue - pInsFea->FInsDownDeviationValue
				//	&& Res.DimensionValue + Res.upDeviation <= pInsFea->FInsToleranceValue + pInsFea->FInsUpDeviationValue)
				//{
				//	Res.qualify = true;
				//}
			}
		}

	}

	else if (pInsFea->type == DIAMETER)//����ֱ���ߴ�ʱ
	{
		Devdata tmpDev = ToleranceFunction::Diameter(AllSurfPntList1, Surf_PntGroup1[0].first);

		Res.DimensionValue = tmpDev.Value;
		Res.upDeviation = tmpDev.UpDeviation;
		Res.downDeviation = tmpDev.DownDeviation;

		Res.Deviation = Res.DimensionValue - pInsFea->FInsToleranceValue;

		if (Res.Deviation >= -pInsFea->FInsDownDeviationValue && Res.Deviation <= pInsFea->FInsUpDeviationValue)
		{
			Res.qualify = true;
		}

		//if (Res.DimensionValue >= pInsFea->FInsToleranceValue - pInsFea->FInsDownDeviationValue
		//	&& Res.DimensionValue <= pInsFea->FInsToleranceValue + pInsFea->FInsUpDeviationValue
		//	&& Res.DimensionValue + Res.downDeviation >= pInsFea->FInsToleranceValue - pInsFea->FInsDownDeviationValue
		//	&& Res.DimensionValue + Res.upDeviation <= pInsFea->FInsToleranceValue + pInsFea->FInsUpDeviationValue)
		//{
		//	Res.qualify = true;
		//}
	}

	else if (pInsFea->type == FLATNESS)//����ƽ���ʱ
	{
		Res.ToleranceValue = ToleranceFunction::Flatness(AllSurfPntList1);

		if (pInsFea->FInsToleranceValue >= Res.ToleranceValue)
		{
			Res.qualify = true;
		}
	}

	else if (pInsFea->type == PROFILE)//����������ʱ
	{


		Res.qualify = true;

	}

	else if (pInsFea->type == POSITION)//����λ�ö�ʱ
	{
		Res.ToleranceValue = ToleranceFunction::PositionalTolerance(AllSurfPntList1, Surf_PntGroup1[0].first);

		if (pInsFea->FInsToleranceValue >= Res.ToleranceValue)
		{
			Res.qualify = true;
		}
	}

	else
	{
		QMessageBox::information(this, QString::fromLocal8Bit("WARINING"),
			QString::fromLocal8Bit("����������ʹ���"), "OK");
	}

	return Res;
}

void AccuEvaluWnd::onToEvaluReport()
{
	InspReportInfoDlg* theInfoDlg = new InspReportInfoDlg(m_EvaluDataList);

	theInfoDlg->show();


	//gWindow->SetInsReportDlg(new FInsReportDlg(m_EvaluDataList));
	//m_pInsReport = gWindow->GetInsReportDlg();

	/////////////////////////////////////////////////////////////��ͼ
	//QScrollArea* s = new QScrollArea();
	//s->setWindowIcon(QIcon("../icons/AccuRep.png"));
	//s->setWindowTitle(QString::fromLocal8Bit("��ⱨ��"));
	//s->setGeometry(100, 80, 1260, 900);
	//s->setFixedSize(1285, 900);
	//s->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	//s->setWidget(m_pInsReport);
	//s->show();
	/////////////////////////////////////////////////////////////////////////////////////////////

}

void AccuEvaluWnd::onFInsFeaDel()
{
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
	for (int i = 0; i < m_InsfratureList.size(); i++)
	{
		if (ExNumFrQsg(m_pEvaResModel->item(i, 0)->text()) == ExNumFrQsg(m_pTmpItem->text(0)))
		{
			m_pEvaResModel->removeRow(i);

			//���¾����������ڼ�������б�
			tmpLst.clear();
			for (int j = 0; j < m_InsfratureList.size(); j++)
			{
				if (j != i)
				{
					tmpLst.push_back(m_InsfratureList[j]);
				}
			}
			m_InsfratureList = tmpLst;

			//ɾ��������������ڵĶ�Ӧ��Ŀ
			vector<EvaluData> tmpEvaluDataList;
			for (int j = 0; j < m_EvaluDataList.size(); j++)
			{
				if (j != i)
				{
					tmpEvaluDataList.push_back(m_EvaluDataList[j]);
				}
			}
			m_EvaluDataList = tmpEvaluDataList;

			//break;
		}
	}

	//ɾ�����ȼ�����������ڵ�Ŀ��������
	int NbAccuWndItem = static_cast<FInsFeaTreeWnd*>(gWindow->GetSubWnd()[6])->GetTree()->topLevelItemCount();
	for (int i = 0; i < NbAccuWndItem; i++)
	{
		if (ExNumFrQsg(static_cast<FInsFeaTreeWnd*>(gWindow->GetSubWnd()[6])->GetTree()->topLevelItem(i)->text(0)) == ExNumFrQsg(m_pTmpItem->text(0)))
		{
			delete static_cast<FInsFeaTreeWnd*>(gWindow->GetSubWnd()[6])->GetTree()->topLevelItem(i);

			break;
		}
	}

	//ɾ����⾫���������ڵ�Ŀ��������
	delete m_pTmpItem;
}

void AccuEvaluWnd::onChartRowDel()
{
	//ɾ���Ҳ������������������ڶ�Ӧ�ļ������
	vector<FInsFeature*> tmpLst;
	for (int i = 0; i < m_InsfratureList.size(); i++)
	{
		if (i != m_TableCurrRow)
		{
			tmpLst.push_back(m_InsfratureList[i]);
		}
	}
	m_InsfratureList = tmpLst;

	//ɾ��������������ڵĶ�Ӧ��Ŀ
	vector<EvaluData> tmpEvaluDataList;
	for (int i = 0; i < m_EvaluDataList.size(); i++)
	{
		if (i != m_TableCurrRow)
		{
			tmpEvaluDataList.push_back(m_EvaluDataList[i]);
		}
	}
	m_EvaluDataList = tmpEvaluDataList;

	m_pEvaResModel->removeRow(m_TableCurrRow);
}

void AccuEvaluWnd::onEvaluAll()
{
	if (gWindow->GetMainDoc()->GetMesPntList().size() == 0)
	{
		QMessageBox::information(this, QString::fromLocal8Bit("WARINING"),
			QString::fromLocal8Bit("δ��⵽ʵ�ʲ�㣬��ȷ���ѻ�ȡʵ�ʲ������֮����ִ�д˲���"), "OK");

		return;
	}

	int NbAllItem = m_pTree->topLevelItemCount();
	for (int i = 0; i < NbAllItem; i++)
	{
		//���ĳ���Ѿ����������ظ���������Ŀ
		bool IsEvalued = false;

		for (int j = 0; j < m_EvaluDataList.size(); j++)
		{
			if ((FInsFeature*)(m_pTree->topLevelItem(i)->data(0, Qt::UserRole).value<void*>()) == m_EvaluDataList[j].p_InsFea)
			{
				IsEvalued = true;
				break;
			}
		}

		if (!IsEvalued)
		{
			AcEvaluate((FInsFeature*)(m_pTree->topLevelItem(i)->data(0, Qt::UserRole).value<void*>()));
		}
	}
}

void AccuEvaluWnd::onRemoveAllEvaluItem()
{

	for (int i = 0; i < m_InsfratureList.size(); i++)
	{
		m_pEvaResModel->removeRow(0);
	}

	m_InsfratureList.clear();
	m_EvaluDataList.clear();
}

void AccuEvaluWnd::printPreviewSlot(QPrinter* printerPixmap)
{
	printerPixmap->setOrientation(QPrinter::Landscape);
	//��ȡ�����ͼƬ
	QPixmap pixmap = QPixmap::grabWidget(m_pInsReport, m_pInsReport->rect());
	QPainter painterPixmap(this);
	painterPixmap.begin(printerPixmap);
	QRect rect = painterPixmap.viewport();
	int x = rect.width() / pixmap.width();
	int y = rect.height() / pixmap.height();
	painterPixmap.scale(x, y);
	painterPixmap.drawPixmap(0, 0, pixmap);
	painterPixmap.end();
}

void AccuEvaluWnd::onToAccuEvalu()
{
	AcEvaluate((FInsFeature*)(m_pTmpItem->data(0, Qt::UserRole).value<void*>()));
}

EvaluData::EvaluData()
{
	qualify = false;
	ToleranceValue = 0.0;
	DimensionValue = 0.0;
	upDeviation = 0.0;
	downDeviation = 0.0;
	p_InsFea = NULL;
	Deviation = 0.0;

}
