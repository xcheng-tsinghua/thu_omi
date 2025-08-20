#include <QMenu>
#include <QAction>
#include <iostream>
#include "../External/fkernel/Include/FKernelHeader.h"
#include "FTreeWnd.h"
#include "FMainWindow.h"
#include "occConverter.h"
#include "EntityHighlight.h"
#include "AutoInsFea.h"
#include "FInsFeaTreeWnd.h"

extern FMainWindow* gWindow;
extern QString gDockTitleColor;

using namespace std;

FTreeWnd::FTreeWnd(QWidget* parent,FMainDoc* pDoc)
	:QDockWidget(parent), m_pDoc(pDoc)
{	
	this->setAttribute(Qt::WA_StyledBackground, true);

	setWindowTitle(QString::fromLocal8Bit("װ��ṹ��"));	
	setMinimumWidth(200);
	QWidget* dockWidgetContents = new QWidget();
	dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
	QWidget* widget = new QWidget(dockWidgetContents);
	widget->setObjectName(QString::fromUtf8("widget"));
	//QVBoxLayout* verticalLayout_2 = new QVBoxLayout(widget);
	QVBoxLayout* verticalLayout_2 = new QVBoxLayout(dockWidgetContents);
	verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
	verticalLayout_2->setContentsMargins(0, 0, 0, 0);
	m_pTree = new FTreeWidget(widget);
	m_pTree->setDoc(m_pDoc);
	//m_pTree->setHeaderLabel(QString::fromLocal8Bit("Assembly Structure"));
	m_pTree->setHeaderHidden(true);

	verticalLayout_2->addWidget(m_pTree);

	setWidget(dockWidgetContents);

	m_pPopMenu = QSharedPointer<QMenu>(new QMenu());

	m_pActionRename = new QAction(QString::fromLocal8Bit("������"), this);
	m_pActionConfig = new QAction(QString::fromLocal8Bit("����"), this);
	m_pActionTest = new QAction(QString::fromLocal8Bit("����"), this);
	m_pActionNewFeat = new QAction(QString::fromLocal8Bit("�¼������"), this);
	m_pActionAddEle = new QAction(QString::fromLocal8Bit("��ӵ�����������б�"), this);
	m_pFoldAll = new QAction(QString::fromLocal8Bit("�۵�ȫ��"), this);
	m_pExpandAll = new QAction(QString::fromLocal8Bit("չ��ȫ��"), this);
	m_pShowNearbySurf = new QAction(QString::fromLocal8Bit("��ʾ����"), this);
	m_pActionAutoInsfea = new QAction(QString::fromLocal8Bit("�Զ��������"), this);

	m_pActionRename->setIcon(QIcon("../icons/Rename.png"));
	m_pActionConfig->setIcon(QIcon("../icons/Config.png"));
	m_pActionTest->setIcon(QIcon("../icons/Test.png"));
	m_pActionNewFeat->setIcon(QIcon("../icons/NewFeat.png"));
	m_pActionAddEle->setIcon(QIcon("../icons/AddEle.png"));
	m_pFoldAll->setIcon(QIcon("../icons/Fold.png"));
	m_pExpandAll->setIcon(QIcon("../icons/Expand.png"));
	m_pShowNearbySurf->setIcon(QIcon("../icons/Nearby.png"));
	m_pActionAutoInsfea->setIcon(QIcon("../icons/AutoInsFea.png"));

	connect(m_pActionRename, SIGNAL(triggered()),this, SLOT(onRenameItem()));
	connect(m_pActionTest, SIGNAL(triggered()),this, SLOT(onTest()));
	connect(m_pActionConfig, SIGNAL(triggered()),this, SLOT(onConfig()));
	connect(m_pActionNewFeat, SIGNAL(triggered()),this, SLOT(onNewFeat()));
	connect(m_pActionAddEle, SIGNAL(triggered()),this, SLOT(onAddEle()));
	connect(m_pFoldAll, SIGNAL(triggered()), this, SLOT(onFoldAll()));
	connect(m_pExpandAll, SIGNAL(triggered()), this, SLOT(onExpandAll()));
	connect(m_pShowNearbySurf, SIGNAL(triggered()), this, SLOT(onShowNearSurf()));
	connect(m_pActionAutoInsfea, SIGNAL(triggered()), this, SLOT(onAutoInsFea()));


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
	}
}

FTreeWnd::~FTreeWnd(void)
{
}

void FTreeWnd::initTree()
{
	//QMap<int,QVector<FIpFeature*> >	featMap = m_pDoc->getFeatureMap();

	//if (featMap.contains(FIpFeature::AR))
	//{
	//	QTreeWidgetItem*	pPlaneItem	=	new QTreeWidgetItem(m_pTree, FIpFeature::AR);
	//	pPlaneItem->setText(0, QString::fromLocal8Bit("�뾶"));
	//	QVector<FIpFeature*> planeVec = featMap[FIpFeature::AR];
	//	for (int i = 0; i < planeVec.size(); i++)
	//	{
	//		//10002��ʾ2���ڵ�
	//		QTreeWidgetItem*	pTmpItem	=	new QTreeWidgetItem(pPlaneItem,10002);
	//		pTmpItem->setText(0, QString::fromLocal8Bit("�뾶%1").arg(i+1));
	//	}
	//}
	//if (featMap.contains(FIpFeature::FN))
	//{
	//	QTreeWidgetItem*	pPlaneItem	=	new QTreeWidgetItem(m_pTree,  FIpFeature::FN);
	//	pPlaneItem->setText(0, QString::fromLocal8Bit("ƽ���"));
	//	QVector<FIpFeature*> planeVec = featMap[FIpFeature::FN];
	//	for (int i = 0; i < planeVec.size(); i++)
	//	{
	//		QTreeWidgetItem*	pTmpItem	=	new QTreeWidgetItem(pPlaneItem,10002);
	//		pTmpItem->setText(0, QString::fromLocal8Bit("ƽ���%1").arg(i+1));
	//		//pTmpItem->setText(1, QString::fromLocal8Bit("��"));
	//	}
	//}
	//if (featMap.contains(FIpFeature::PR))
	//{
	//	QTreeWidgetItem*	pPlaneItem	=	new QTreeWidgetItem(m_pTree,  FIpFeature::PR);
	//	pPlaneItem->setText(0, QString::fromLocal8Bit("������"));
	//	QVector<FIpFeature*> planeVec = featMap[FIpFeature::PR];
	//	for (int i = 0; i < planeVec.size(); i++)
	//	{
	//		QTreeWidgetItem*	pTmpItem	=	new QTreeWidgetItem(pPlaneItem,10002);
	//		pTmpItem->setText(0, QString::fromLocal8Bit("������%1").arg(i+1));
	//	}
	//}
	//if (featMap.contains(FIpFeature::CY))
	//{
	//	QTreeWidgetItem*	pPlaneItem	=	new QTreeWidgetItem(m_pTree, FIpFeature::CY);
	//	pPlaneItem->setText(0, QString::fromLocal8Bit("Բ����"));
	//	QVector<FIpFeature*> planeVec = featMap[FIpFeature::CY];
	//	for (int i = 0; i < planeVec.size(); i++)
	//	{
	//		QTreeWidgetItem*	pTmpItem	=	new QTreeWidgetItem(pPlaneItem,10002);
	//		pTmpItem->setText(0, QString::fromLocal8Bit("Բ����%1").arg(i+1));
	//		//pTmpItem->setFlags(Qt::ItemFlag(63));
	//	}
	//}


	/*QTreeWidgetItem*	pPlaneItem	=	new QTreeWidgetItem(m_pTree);
	pPlaneItem->setText(0, QString::fromLocal8Bit("ƽ��"));
	for (int i = 0; i < 14; i++)
	{
		QTreeWidgetItem*	pTmpItem	=	new QTreeWidgetItem(pPlaneItem);
		pTmpItem->setText(0, QString::fromLocal8Bit("ƽ��%1").arg(i));
	}*/
}
void    FTreeWnd::initTree_YU()
{
	if (m_pDoc->getrib().size()!=0)
	{
		QTreeWidgetItem*	pRibItem	=	new QTreeWidgetItem(m_pTree, 1001);
		pRibItem->setText(0, QString::fromLocal8Bit("��"));
		//QVector<FIpFeature*> planeVec = featMap[FIpFeature::AR];
		for (int i = 0; i < m_pDoc->getrib().size(); i++)
		{
			//10002��ʾ2���ڵ�
			QTreeWidgetItem*	pTmpItem	=	new QTreeWidgetItem(pRibItem,10001);
			pTmpItem->setText(0, QString::fromLocal8Bit("��%1").arg(i+1));
		}
	}
	if (m_pDoc->getribtop().size()!=0)
	{
		QTreeWidgetItem*	pRibItem	=	new QTreeWidgetItem(m_pTree, 1002);
		pRibItem->setText(0, QString::fromLocal8Bit("�"));
		for (int i = 0; i < m_pDoc->getribtop().size(); i++)
		{
			//10002��ʾ2���ڵ�
			QTreeWidgetItem*	pTmpItem	=	new QTreeWidgetItem(pRibItem,10002);
			pTmpItem->setText(0, QString::fromLocal8Bit("�%1").arg(i+1));
		}
	}
			
	if (m_pDoc->getslot().size()!=0)
	{
		QTreeWidgetItem*	pSlotItem	=	new QTreeWidgetItem(m_pTree, 1003);
		pSlotItem->setText(0, QString::fromLocal8Bit("��"));
		for (int i = 0; i < m_pDoc->getslot().size(); i++)
		{
			//10002��ʾ2���ڵ�
			QTreeWidgetItem*	pTmpItem	=	new QTreeWidgetItem(pSlotItem,10002);
			pTmpItem->setText(0, QString::fromLocal8Bit("��%1").arg(i+1));
		}
	}
	if (m_pDoc->getprofile().size()!=0)
	{
		QTreeWidgetItem*	pProfileItem	=	new QTreeWidgetItem(m_pTree, 1004);
		pProfileItem->setText(0, QString::fromLocal8Bit("����"));
	
		QTreeWidgetItem*	pTmpItem	=	new QTreeWidgetItem(pProfileItem,10002);
		pTmpItem->setText(0, QString::fromLocal8Bit("����%1").arg(1));
		
	}
	if (m_pDoc->gethole().size()!=0)
	{
		QTreeWidgetItem*	pHoleItem	=	new QTreeWidgetItem(m_pTree, 1005);
		pHoleItem->setText(0, QString::fromLocal8Bit("��"));
	for (int i = 0; i < m_pDoc->gethole().size(); i++)
		{
			//10002��ʾ2���ڵ�
			QTreeWidgetItem*	pTmpItem	=	new QTreeWidgetItem(pHoleItem,10002);
		pTmpItem->setText(0, QString::fromLocal8Bit("��%1").arg(1));
		}
		
		
	}
}

void FTreeWnd::initTree_assembly(QList<QTreeWidgetItem*> treeList)
{
	if (treeList.size() > 0)
	{
		m_pTree->insertTopLevelItems(0,treeList);
		m_pTree->expandAll();

		//connect(m_pTree, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
		//	this, SLOT(onLeftPressShow(QTreeWidgetItem*, int)));
		/*for (int i = 0; i < treeList.size(); i++)
		{
			pAssemItem->addChild(treeList.at(i));
		}*/
		
	}
}

void FTreeWnd::clearTree()
{
	m_pTree->clear();
}

void FTreeWnd::contextMenuEvent( QContextMenuEvent * event )
{
	QPoint point = QCursor::pos();
	QPoint localPos = m_pTree->viewport()->mapFromGlobal(point);
	QTreeWidgetItem* pItem = m_pTree->itemAt(localPos);
	m_pTmpItem	=	pItem;

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
		if (pItem->parent() == nullptr)
		{
			return;
		}
		//��Ϊ�����ڵ�
		m_pPopMenu->clear();
		m_pPopMenu->addAction(m_pActionRename);
		m_pPopMenu->addAction(m_pActionConfig);
		m_pPopMenu->addAction(m_pActionTest);
		m_pPopMenu->addSeparator();

		m_pPopMenu->addAction(m_pShowNearbySurf);
		m_pPopMenu->addSeparator();

		m_pPopMenu->addAction(m_pActionNewFeat);
		m_pPopMenu->addAction(m_pActionAddEle);
		m_pPopMenu->addAction(m_pActionAutoInsfea);
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

void FTreeWnd::onRenameItem()
{
	Qt::ItemFlags	tmpFlags = m_pTmpItem->flags();
	m_pTmpItem->setFlags(Qt::ItemIsEditable |Qt::ItemIsEnabled);
	m_pTree->editItem(m_pTmpItem);
	m_pTmpItem->setFlags(tmpFlags);
}

void FTreeWnd::onTest()
{
	//m_pDoc->getFeatureMap()
	//	[m_pTmpItem->parent()->type()][m_pTmpItem->parent()->indexOfChild(m_pTmpItem)]->Print();
}

void FTreeWnd::onConfig()
{
	/*FIpFeature* pInspectFt = m_pDoc->getFeatureMap()
		[m_pTmpItem->parent()->type()][m_pTmpItem->parent()->indexOfChild(m_pTmpItem)];*/

	/*FFeatConfigWnd*	configWnd = new FFeatConfigWnd(this, m_pDoc, m_pTmpItem);
	configWnd->show();*/

	//if (m_pTmpItem->parent() == nullptr)
	//{
	//	return;
	//}
	//else if(m_pTmpItem->parent()->type() == FIpFeature::FN)
	//{
	//	FPlaneConfigWnd*	configWnd = new FPlaneConfigWnd(this, m_pDoc, m_pTmpItem);
	//	configWnd->SetupWnd();
	//	configWnd->show();
	//}

}

void FTreeWnd::getNumFromName(const char* str, int* num)
{
	int len = strlen(str);
	int index = 0;
	int t;
	for (int i = 0; i < len; i++)
	{
		while (!(str[i] > '0' && str[i] < '9'))
		{
			i++;
		}
		while (str[i] >= '0' && str[i] < '9')
		{
			t = str[i] - '0';
			num[index] = num[index] * 10 + t;
			i++;
		}
		index++;
	}
}


//void FTreeWnd::onLeftPressShow(QTreeWidgetItem* pressedItem, int column)
//{
//	
//	if (pressedItem->childCount() != 0)
//	{
//		QMessageBox::information(this, QString::fromLocal8Bit("WARINING"),
//			QString::fromLocal8Bit("������Ĳ�������������������������"), "OK");
//	}
//
//	QTreeWidgetItem* parentItem = pressedItem->parent();
//	QTreeWidgetItem* gpItem = parentItem->parent();
//
//	if (parentItem->childCount() != 0 && gpItem->childCount() != 0)
//	{
//		int pIdx = gpItem->indexOfChild(parentItem);
//		string name = pressedItem->text(0).toStdString();
//		char* p = (char*)name.data();
//		int* surfIdx;
//		getNumFromName(p, surfIdx);
//		//int idx = (int)surfIdx;
//		switch (pIdx)
//		{
//		case 0:
//			m_pDoc->getSurfaceList(0).at(*surfIdx)->SetColor(1, 0, 0);
//			m_pDoc->UpdateAllViews();
//			break;
//		case 1:
//			m_pDoc->getSurfaceList(1).at(*surfIdx)->SetColor(1, 0, 0);
//			m_pDoc->UpdateAllViews();
//			break;
//		case 2:
//			m_pDoc->getSurfaceList(2).at(*surfIdx)->SetColor(1, 0, 0);
//			m_pDoc->UpdateAllViews();
//			break;
//		case 3:
//			m_pDoc->getSurfaceList(3).at(*surfIdx)->SetColor(1, 0, 0);
//			m_pDoc->UpdateAllViews();
//			break;
//		case 4:
//			m_pDoc->getSurfaceList(4).at(*surfIdx)->SetColor(1, 0, 0);
//			m_pDoc->UpdateAllViews();
//			break;
//		case 5:
//			m_pDoc->getSurfaceList(5).at(*surfIdx)->SetColor(1, 0, 0);
//			m_pDoc->UpdateAllViews();
//			break;
//		default:
//			std::cout << "Error! Cannot find the corrsponding surface!" << endl; break;
//		}
//	}
//}

void FTreeWnd::onExpandAll()
{
	m_pTree->expandAll();
}

void FTreeWnd::onFoldAll()
{
	m_pTree->collapseAll();
}

void FTreeWnd::onShowNearSurf()
{
	gWindow->ResetToDefaultCountenance();

	// ��ȡ�����Ŀ��Ӧ��FSurface
	FSurface* CurrSurf = (FSurface*)m_pTmpItem->data(0, Qt::UserRole).value<void*>();

	vector<SurfNearbyGraph_Node*> NodeList = m_pDoc->GetSurfNearbyGraph();

	vector<FSurface*> tmpFSurfList = gWindow->GetSTEPModelConverter()->GetFSurfList();

	for (auto ita = NodeList.begin(); ita != NodeList.end(); ++ita)
	{
		if (tmpFSurfList[(*ita)->GetGeomIndex()] == CurrSurf)
		{
			tmpFSurfList[(*ita)->GetGeomIndex()]->SetColor(1, 0, 0);

			for (auto ita2 = (*ita)->GetNearbyNodeList().begin(); ita2 != (*ita)->GetNearbyNodeList().end(); ++ita2)
			{
				tmpFSurfList[(*ita2)->GetGeomIndex()]->SetColor(1, 0, 0);
			}

			break;
		}

	}




}

void FTreeWnd::onAutoInsFea()
{
	AutoInsFea* pNewFea = new AutoInsFea;

	gWindow->addDockWidget(Qt::LeftDockWidgetArea, pNewFea);

	// �۵���ม������
	gWindow->tabifyDockWidget(pNewFea, (FTreeWnd*)gWindow->GetSubWnd()[0]);
	gWindow->tabifyDockWidget((FTreeWnd*)gWindow->GetSubWnd()[0], (FInsFeaTreeWnd*)gWindow->GetSubWnd()[6]);
	
	pNewFea->show();

	pNewFea->raise();

}

void FTreeWnd::onNewFeat()
{
	//edited by chengxi
	if (gWindow->GetCreateInsFeatureDlg() != NULL)
	{
		gWindow->GetCreateInsFeatureDlg()->show();
	}
	else
	{
		gWindow->SetCreateInsFeatureDlg(new FInsFeatureDlg());
		gWindow->GetCreateInsFeatureDlg()->show();
	}

	//QMessageBox::information(0, QString::fromLocal8Bit("��ܰ��ʾ"), QString::fromLocal8Bit("����������漰���Բ�����뽻��ѡȡ�ֱ������ڲ�ͬԲ���İ�Բ���档"), "OK");
}


void FTreeWnd::onAddEle()
{
	//if (gWindow->GetCreateInsFeatureDlg() == NULL)
	//{
	//	QMessageBox::information(this, QString::fromLocal8Bit("EROOR"),
	//		QString::fromLocal8Bit("���ȵ���¼�������������¼������"), "OK");

	//	gWindow->SetCreateInsFeatureDlg(new FInsFeatureDlg());
	//}

	////Qt::ItemFlags	tmpFlags = m_pTmpItem->flags();
	//gWindow->GetCreateInsFeatureDlg()->show();

	//FSurface* CurrSurf = (FSurface*)m_pTmpItem->data(0, Qt::UserRole).value<void*>();

	//QListWidgetItem* ReListItem = new QListWidgetItem(m_pTmpItem->text(0));

	//if (m_pTmpItem->parent()->text(0).contains("BSpline"))
	//{
	//	ReListItem->setIcon(QIcon("../icons/BsplineSurface.png"));
	//}
	//else if (m_pTmpItem->parent()->text(0).contains("Conical"))
	//{
	//	ReListItem->setIcon(QIcon("../icons/ConicalSurface.png"));
	//}
	//else if (m_pTmpItem->parent()->text(0).contains("Cylindrical"))
	//{
	//	ReListItem->setIcon(QIcon("../icons/CylindricalSurface.png"));
	//}
	//else if (m_pTmpItem->parent()->text(0).contains("Spherical"))
	//{
	//	ReListItem->setIcon(QIcon("../icons/SphericalSurface.png"));
	//}
	//else if (m_pTmpItem->parent()->text(0).contains("Plane"))
	//{
	//	ReListItem->setIcon(QIcon("../icons/PlaneSurface.png"));
	//}
	//else if (m_pTmpItem->parent()->text(0).contains("Other"))
	//{
	//	ReListItem->setIcon(QIcon("../icons/OtherSurface.png"));
	//}
	//else
	//{
	//	QMessageBox::information(this, QString::fromLocal8Bit("EROOR"),
	//		QString::fromLocal8Bit("������������"), "OK");
	//	return;
	//}

	//ReListItem->setData(3, QVariant::fromValue((void*)(CurrSurf)));
	//gWindow->GetCreateInsFeatureDlg()->GetFaceListWidget()->addItem(ReListItem);

	//QTreeWidgetItem* pFaceItem = new QTreeWidgetItem;
	//pFaceItem->setText(0,m_pTmpItem->text(0));
	//gWindow->GetCreateInsFeatureDlg()->TreeFaceItemList.append(pFaceItem);

	//gWindow->GetCreateInsFeatureDlg()->FSurfaceEntityList.push_back(CurrSurf);
}

FTreeWidget::FTreeWidget(QWidget* parent):QTreeWidget(parent)
{
	LastSelItem = NULL;
	IsCtrlPressed = 0;

	//�����۵�ͼ����ʽ
	this->setStyleSheet("QTreeView::branch:has-children:!has-siblings:closed,\
QTreeView::branch:closed:has-children:has-siblings{border-image: none; image: url(../icons/TreeCollapse.png);}\
QTreeView::branch:open:has-children:!has-siblings,\
QTreeView::branch:open:has-children:has-siblings{border-image: none; image: url(../icons/TreeExpand.png);}"
	);

	//this->setAttribute(Qt::WA_StyledBackground, true);
	//this->setStyleSheet("background-color: rgb(204,213,240)");
	//this->setAutoFillBackground(true);
	//QPalette palette = this->palette();
	//palette.setBrush(QPalette::Window, QBrush(QPixmap("../backgrounds/ModelTreeWnd.png").scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));// ʹ��ƽ�������ŷ�ʽ
	//this->setPalette(palette);// ��widget���ϱ���ͼ
}

void FTreeWidget::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		// ʹ��Ĭ�Ϲ���

		gWindow->GetMainDoc()->PopTool();
		FFeatureTool* pDefaultTool = gWindow->GetCurTool();
		pDefaultTool = new EntityHighlight();
		gWindow->GetMainDoc()->PushTool(pDefaultTool);

		/////////////////////////

		QPoint point = QCursor::pos();
		QPoint localPos = viewport()->mapFromGlobal(point);
		QTreeWidgetItem* pItem = itemAt(localPos);
		//m_pTmpItem = pItem;

		if (!pItem) return;

		if (pItem->childCount() != 0)
		{
			//QMessageBox::information(this, QString::fromLocal8Bit("WARINING"),
			//	QString::fromLocal8Bit("������Ĳ�������������������������"), "OK");

			if (pItem->isExpanded())
			{
				collapseItem(pItem);
			}
			else
			{
				expandItem(pItem);
			}

			return;
		}

		else if (pItem->childCount() == 0 && (pItem->parent()->child(0)->data(0, Qt::UserRole).value<int>() == 1))
		{
			return;
		}

		else 
		{
			FSurface* CurrSurf = (FSurface*)pItem->data(0, Qt::UserRole).value<void*>();
			if (!CurrSurf) return;

			if (IsCtrlPressed == 0)//δ����ctrl����ѡ
			{
				float r, g, b;
				CurrSurf->GetColor(r, g, b);
				if (r == 1.0)
				{
					gWindow->ResetToDefaultCountenance();

					QTreeWidgetItemIterator m_pCurrItem(this);
					while (*m_pCurrItem)
					{
						(*m_pCurrItem)->setSelected(0);
						++m_pCurrItem;
					}
				}
				else
				{
					gWindow->ResetToDefaultCountenance();

					CurrSurf->SetColor(1, 0, 0);

					QTreeWidgetItemIterator m_pCurrItem(this);
					while (*m_pCurrItem)
					{
						(*m_pCurrItem)->setSelected(0);
						++m_pCurrItem;
					}

					pItem->setSelected(1);
				}
				gWindow->UpdateModelView();
			}


			if (IsCtrlPressed == 1)//�Ѱ���ctrl����ѡ
			{
				float r, g, b;
				CurrSurf->GetColor(r, g, b);
				if (r == 1.0)
				{
					gWindow->GetOccConverter()->setSurfColorDefault(CurrSurf);
					pItem->setSelected(0);
				}
				else
				{
					CurrSurf->SetColor(1, 0, 0);
					pItem->setSelected(1);
				}
				gWindow->UpdateModelView();
			}
		}
	}
}

void FTreeWidget::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Control)
	{
		IsCtrlPressed = 1;
	}
}

void FTreeWidget::keyReleaseEvent(QKeyEvent* event)
{
	IsCtrlPressed = 0;
}
