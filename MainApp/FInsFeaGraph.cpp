#include "FInsFeaGraph.h"
#include "qevent.h"
#include "FMainWindow.h"
#include <unordered_map>
#include <functional>
#include "occConverter.h"
#include <qboxlayout.h>
#include "FInsFeaTreeWnd.h"
#include "PointPlanDlg.h"
#include "np.h"

extern FMainWindow* gWindow;
extern QString gDockTitleColor;

FInsFeaGraph::FInsFeaGraph()
{
	setAttribute(Qt::WA_StyledBackground, true);

	setWindowTitle(QString::fromLocal8Bit("�������ͼ"));

	QWidget* dockWidgetContents = new QWidget;

	MainVLayput = new QVBoxLayout(dockWidgetContents);

	QGroupBox* pFirstGroup = new QGroupBox(QString::fromLocal8Bit("����"));

	QHBoxLayout* SubLayout1 = new QHBoxLayout(pFirstGroup);

	QPushButton* UpdateBnt = new QPushButton(QString::fromLocal8Bit("����"));

	QPushButton* ExitBnt = new QPushButton(QString::fromLocal8Bit("�ر�"));

	SubLayout1->addWidget(UpdateBnt);

	SubLayout1->addWidget(ExitBnt);

	m_pPopMenu = QSharedPointer<QMenu>(new QMenu());
	m_pSampling = new QAction(QString::fromLocal8Bit("���滮"), this);
	m_pFoldAll = new QAction(QString::fromLocal8Bit("�۵�ȫ��"), this);
	m_pExpandAll = new QAction(QString::fromLocal8Bit("չ��ȫ��"), this);
	m_pTmpItem = NULL;

	m_pSampling->setIcon(QIcon("../icons/PntPlan.png"));
	m_pFoldAll->setIcon(QIcon("../icons/Fold.png"));
	m_pExpandAll->setIcon(QIcon("../icons/Expand.png"));

	m_pTree = new FInsFeaGraphTreeWidget;
	m_pTree->setHeaderHidden(true);

	MainVLayput->addWidget(pFirstGroup);
	MainVLayput->addWidget(m_pTree);

	setContentsMargins(0, 0, 0, 0);
	setWidget(dockWidgetContents);


	connect(UpdateBnt, SIGNAL(clicked()), this, SLOT(onComputeInsFeaGraph()));
	connect(ExitBnt, SIGNAL(clicked()), this, SLOT(onClose()));
	connect(m_pSampling, SIGNAL(triggered()), this, SLOT(onSampling()));
	connect(m_pFoldAll, SIGNAL(triggered()), this, SLOT(onCollapseAll()));
	connect(m_pExpandAll, SIGNAL(triggered()), this, SLOT(onExpandAll()));

	//style
	{
		//this->setFont(QFont("Microsoft YaHei"));
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
			//"titlebar-close-icon: url(../icons/StrollDown.png);"//�رհ�ťͼ��
			"}"
			"QDockWidget::close-button {"
			"icon:url(../icons/StrollDown.png);"
			"}"
		);

		//������
		dockWidgetContents->setStyleSheet(
			"QWidget"
			"{"
			"border: 1px solid rgb(247,249,254);"
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
			"background:rgba(247,249,254,100%);"
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
			"background:rgba(247,249,254,100%);"
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
			"background:rgba(247,249,254,100%);"
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
			"background:rgba(247,249,254,100%);"
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

		ExitBnt->setStyleSheet(
			"QPushButton{background-color:rgb(204,213,240);}"
			"QPushButton:hover{background-color:rgb(245,204,132);}"
			"QPushButton:pressed{background-color:rgb(108,226,108);}");

		UpdateBnt->setStyleSheet(
			"QPushButton{background-color:rgb(204,213,240);}"
			"QPushButton:hover{background-color:rgb(245,204,132);}"
			"QPushButton:pressed{background-color:rgb(108,226,108);}");

		pFirstGroup->setStyleSheet(
			"QGroupBox{"
			"border: 1px solid rgb(64,80,141);"
			"margin-top:2ex;"
			"}"
			"QGroupBox:title{subcontrol-origin:margin;padding:3px;}"
		);


	}

}

void FInsFeaGraph::onClose()
{
	gWindow->removeDockWidget(this);

	gWindow->splitDockWidget((FTreeWnd*)gWindow->GetSubWnd()[0], (FInsFeaTreeWnd*)gWindow->GetSubWnd()[6], Qt::Vertical);

	gWindow->addDockWidget(Qt::LeftDockWidgetArea, (FTreeWnd*)gWindow->GetSubWnd()[0]);
	gWindow->addDockWidget(Qt::LeftDockWidgetArea, (FInsFeaTreeWnd*)gWindow->GetSubWnd()[6]);

	close();
}

void FInsFeaGraph::onComputeInsFeaGraph()
{
	ClearTreeItems();

	vector<FInsFeature*> allInsFeas = gWindow->GetMainDoc()->GetInsFeaList();

	unordered_map<FSurface*, vector<FInsFeature*>> ump;

	function<vector<FSurface*>(FInsFeature*)> GetAllFSurf = [](FInsFeature* theFeature)->vector<FSurface*>
	{
		vector<FSurface*> allSurfList;

		vector<FSurface*> tmplist1 = theFeature->m_FSurfList1;

		vector<FSurface*> tmplist2 = theFeature->m_FSurfList2;

		for (auto ita = tmplist1.begin(); ita != tmplist1.end(); ++ita)
		{
			allSurfList.push_back(*ita);
		}

		for (auto ita = tmplist2.begin(); ita != tmplist2.end(); ++ita)
		{
			allSurfList.push_back(*ita);
		}

		return allSurfList;
	};

	for (auto ita = allInsFeas.begin(); ita != allInsFeas.end(); ++ita)
	{
		vector<FSurface*> currFSurfs = GetAllFSurf(*ita);

		for (auto ita2 = currFSurfs.begin(); ita2 != currFSurfs.end(); ++ita2)
		{
			ump[*ita2].push_back(*ita);
		}
	}

	function<QString(FSurface*)> theFSurfName = [](FSurface* aFSurf)->QString
	{
		SURFACE_TYPE theSurfType;
		int ind_in_all = 0;
		int ind_in_type = 0;
		QString tmpFSurfName;

		gWindow->GetSTEPModelConverter()->FSurfInfo(aFSurf, theSurfType, ind_in_all, ind_in_type, tmpFSurfName);

		return tmpFSurfName;
	};

	function<QString(FInsFeature*)> insTypeStr = [](FInsFeature* theFea)->QString
	{
		InsType theCurrType = theFea->type;

		switch (theCurrType)
		{
		case PARALLELISM:return QString::fromLocal8Bit("ƽ�ж�");
			break;
		case ROUNDNESS:return QString::fromLocal8Bit("Բ��");
			break;
		case DISTANCE:return QString::fromLocal8Bit("����");
			break;
		case VERTICALITY:return QString::fromLocal8Bit("��ֱ��");
			break;
		case DIAMETER:return QString::fromLocal8Bit("ֱ��");
			break;
		case AUXIALITY:return QString::fromLocal8Bit("ͬ���");
			break;
		case FLATNESS:return QString::fromLocal8Bit("ƽ���");
			break;
		case UNKNOW:return QString::fromLocal8Bit("δ֪����");
			break;
		default:return QString::fromLocal8Bit("δ֪����");
			break;
		}
	};
	
	// ������ϣ����������Ŀ
	for (auto ita = ump.begin(); ita != ump.end(); ++ita)
	{
		QTreeWidgetItem* tmpSurfItem = new QTreeWidgetItem;

		tmpSurfItem->setData(0, Qt::UserRole, QVariant::fromValue((void*)((*ita).first)));

		tmpSurfItem->setText(0, theFSurfName((*ita).first));

		vector<FInsFeature*> InsFeas = (*ita).second;

		for (auto ita2 = InsFeas.begin(); ita2 != InsFeas.end(); ++ita2)
		{
			QTreeWidgetItem* tmpInsFeaItem = new QTreeWidgetItem;

			tmpInsFeaItem->setText(0, QString::fromLocal8Bit("�������") + QString::number((*ita2)->InsFeatureindex));

			tmpInsFeaItem->setData(0, Qt::UserRole, QVariant::fromValue((void*)((*ita2))));

			tmpInsFeaItem->setToolTip(0, insTypeStr(*ita2));

			tmpSurfItem->addChild(tmpInsFeaItem);

		}

		m_pTree->addTopLevelItem(tmpSurfItem);
	}
}

void FInsFeaGraph::onSampling()
{
	PointPlanDlg* pPointPlan = NULL;
	if (!gWindow->GetPointPlanDlg())
	{
		gWindow->SetPointPlanDlg(new PointPlanDlg(this));
		pPointPlan = gWindow->GetPointPlanDlg();
	}
	else
	{
		pPointPlan = gWindow->GetPointPlanDlg();
	}

	FSurface* CurrSurf = (FSurface*)m_pTmpItem->data(0, Qt::UserRole).value<void*>();

	pPointPlan->show();

}

void FInsFeaGraph::onCollapseAll()
{
	m_pTree->collapseAll();
}

void FInsFeaGraph::onExpandAll()
{
	m_pTree->expandAll();
}

void FInsFeaGraph::contextMenuEvent(QContextMenuEvent* event)
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

			m_pPopMenu->addAction(m_pSampling);
			m_pPopMenu->addSeparator();

			m_pPopMenu->addAction(m_pFoldAll);
			m_pPopMenu->addAction(m_pExpandAll);

			m_pPopMenu->exec(QCursor::pos());
		}

		else
		{
			m_pPopMenu->clear();

			m_pPopMenu->addAction(m_pFoldAll);
			m_pPopMenu->addAction(m_pExpandAll);

			m_pPopMenu->exec(QCursor::pos());
		}
	}


}

void FInsFeaGraph::closeEvent(QCloseEvent* event)
{
	onClose();

	QDockWidget::closeEvent(event);

}

void FInsFeaGraph::ClearTreeItems()
{
	function<void(QTreeWidgetItem*)> ClearChildren = [&](QTreeWidgetItem* paraTreeItem)->void
	{
		int itemChildrenCounts = paraTreeItem->childCount();
		while (itemChildrenCounts--)
		{
			QTreeWidgetItem* child = paraTreeItem->child(itemChildrenCounts); //index�Ӵ�С����ɾ������
			paraTreeItem->removeChild(child);
			delete child;
			child = nullptr;
		}
	};

	for (int i = 0; i < m_pTree->topLevelItemCount(); i++)
	{
		ClearChildren(m_pTree->topLevelItem(0));

		delete m_pTree->topLevelItem(0);
	}


}

FInsFeaGraphTreeWidget::FInsFeaGraphTreeWidget(QWidget* parent) :QTreeWidget(parent)
{
	//�����۵�ͼ����ʽ
	this->setStyleSheet("QTreeView::branch:has-children:!has-siblings:closed,\
QTreeView::branch:closed:has-children:has-siblings{border-image: none; image: url(../icons/TreeCollapse.png);}\
QTreeView::branch:open:has-children:!has-siblings,\
QTreeView::branch:open:has-children:has-siblings{border-image: none; image: url(../icons/TreeExpand.png);}");
}

void FInsFeaGraphTreeWidget::mousePressEvent(QMouseEvent* event)
{
	this->QTreeWidget::mousePressEvent(event);

	if (event->button() == Qt::LeftButton)
	{
		gWindow->ResetToDefaultCountenance();

		QPoint point = QCursor::pos();
		QPoint localPos = viewport()->mapFromGlobal(point);
		QTreeWidgetItem* pItem = itemAt(localPos);

		if (!pItem) return;

		// �������ͼ
		if ((!pItem->parent()) && pItem->text(0).contains(QString::fromLocal8Bit("��")))
		{
			FSurface* tmpFSurf = (FSurface*)pItem->data(0, Qt::UserRole).value<void*>();

			tmpFSurf->SetColor(1, 0, 0);

			gWindow->UpdateModelView();

		}
		// ѡ���˼������
		if (pItem->parent() != NULL && pItem->parent()->parent() == NULL)
		{
			function<vector<FSurface*>(FInsFeature*)> GetAllFSurf = [](FInsFeature* theFeature)->vector<FSurface*>
			{
				vector<FSurface*> allSurfList;

				vector<FSurface*> tmplist1 = theFeature->m_FSurfList1;

				vector<FSurface*> tmplist2 = theFeature->m_FSurfList2;

				for (auto ita = tmplist1.begin(); ita != tmplist1.end(); ++ita)
				{
					allSurfList.push_back(*ita);
				}

				for (auto ita = tmplist2.begin(); ita != tmplist2.end(); ++ita)
				{
					allSurfList.push_back(*ita);
				}

				return allSurfList;
			};

			vector<FSurface*> allSurfs = GetAllFSurf((FInsFeature*)pItem->data(0, Qt::UserRole).value<void*>());

			for (auto ita = allSurfs.begin(); ita != allSurfs.end(); ++ita)
			{
				(*ita)->SetColor(1, 0, 0);
			}
		}
	}

}
