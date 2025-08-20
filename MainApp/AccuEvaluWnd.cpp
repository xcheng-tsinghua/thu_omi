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
	setWindowTitle(QString::fromLocal8Bit("精度评定"));

	QSplitter* widget=new QSplitter(Qt::Horizontal, this);//设置窗口分割

	m_pTree = new QTreeWidget(widget);
	m_pTree->setHeaderHidden(true);

	m_pEvaResView = new QTableView(widget);

	//初始化图表
	m_pEvaResModel = new QStandardItemModel(m_pEvaResView);
	InitializeModel(m_pEvaResModel);
	m_pEvaResView->setModel(m_pEvaResModel);
	m_pEvaResView->setAlternatingRowColors(true);
	m_pEvaResView->horizontalHeader()->setMinimumHeight(27);
	m_pEvaResView->verticalHeader()->setMinimumWidth(27);
	m_pEvaResView->setEditTriggers(QAbstractItemView::NoEditTriggers);	

	//初始化右键菜单
	m_pAccuEvalu = new QAction(QString::fromLocal8Bit("精度评定"), this);
	m_pEvaluReport = new QAction(QString::fromLocal8Bit("生成检测报告"), this);
	m_pFeaDel = new QAction(QString::fromLocal8Bit("移除检测特征"), this);
	m_pAccuEvaDel = new QAction(QString::fromLocal8Bit("移除评定项目"), this);
	m_pEvaluAll = new QAction(QString::fromLocal8Bit("评定全部"), this);
	m_pRemoveAllEvaluItem = new QAction(QString::fromLocal8Bit("移除全部项目"), this);

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
		//图标
		m_pAccuEvalu->setIcon(QIcon("../icons/AccuEva.png"));
		m_pEvaluReport->setIcon(QIcon("../icons/AccuRep.png"));
		m_pFeaDel->setIcon(QIcon("../icons/FInsDel.png"));
		m_pAccuEvaDel->setIcon(QIcon("../icons/AccuRepDel.png"));
		m_pEvaluAll->setIcon(QIcon("../icons/EvaluAll.png"));
		m_pRemoveAllEvaluItem->setIcon(QIcon("../icons/RemoveAllEvaluItem.png"));

		//标题栏
		this->setStyleSheet("QDockWidget::title"
			"{"
			"padding-left: 10px;"
			"padding-top: 4px;"
			"background:" + gDockTitleColor +
			"}"
			"QDockWidget"
			"{"
			"color: rgb(247,249,254);"//标题栏中文字颜色
			"}"
			"QTabBar::tab:selected"
			"{"
			"background:rgba(232,232,236,100%);"
			"}"
		);

		//树窗口折叠图标样式
		m_pTree->setStyleSheet(
			"QTreeView::branch:has-children:!has-siblings:closed,\
QTreeView::branch:closed:has-children:has-siblings{border-image: none; image: url(../icons/TreeCollapse.png);}\
QTreeView::branch:open:has-children:!has-siblings,\
QTreeView::branch:open:has-children:has-siblings{border-image: none; image: url(../icons/TreeExpand.png);}");

		//横向表头样式
		m_pEvaResView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(204,213,240);border: 1px solid rgb(64,86,141);font:10pt 'Microsoft YaHei';color: black;};");

		//右键菜单
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

		//滚动条
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
			// 滚动条两端变成椭圆
			"QScrollBar::handle:vertical"
			"{"
			"width:8px;"
			"background:rgba(0,0,0,25%);"
			"min-height:20;"
			"max-width:10;"
			"}"
			// 鼠标放到滚动条上的时候，颜色变深
			"QScrollBar::handle:vertical:hover"
			"{"
			"width:8px;"
			"background:rgba(0,0,0,50%);"
			"min-height:20;"
			"}"
			// 这个应该是设置下箭头的，3.png就是箭头
			"QScrollBar::add-line:vertical"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollDown.png);"
			"subcontrol-position:bottom;"
			"}"
			// 设置上箭头
			"QScrollBar::sub-line:vertical"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollUp.png);"
			"subcontrol-position:top;"
			"}"
			// 当鼠标放到下箭头上的时候
			"QScrollBar::add-line:vertical:hover"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollDown2.png);"
			"subcontrol-position:bottom;"
			"}"
			// 当鼠标放到下箭头上的时候
			"QScrollBar::sub-line:vertical:hover"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollUp2.png);"
			"subcontrol-position:top;"
			"}"
			// 当滚动条滚动的时候，上面的部分和下面的部分
			"QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical"
			"{"
			"background:rgba(232,232,236,100%);"
			"}"

			//横向
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
			// 滚动条两端变成椭圆
			"QScrollBar::handle:horizontal"
			"{"
			"height:8px;"
			"background:rgba(0,0,0,25%);"
			"min-width:10;"
			"max-height:20;"
			"}"
			// 鼠标放到滚动条上的时候，颜色变深
			"QScrollBar::handle:horizontal:hover"
			"{"
			"height:8px;"
			"background:rgba(0,0,0,50%);"
			"min-width:20;"
			"}"
			// 设置左箭头的，3.png就是箭头
			"QScrollBar::sub-line:horizontal"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollLeft.png);"
			"subcontrol-position:left;"
			"}"
			// 设置右箭头
			"QScrollBar::add-line:horizontal"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollRight.png);"
			"subcontrol-position:right;"
			"}"
			// 当鼠标放到左箭头上的时候
			"QScrollBar::sub-line:horizontal:hover"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollLeft2.png);"
			"subcontrol-position:left;"
			"}"
			// 当鼠标放到右箭头上的时候
			"QScrollBar::add-line:horizontal:hover"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollRight2.png);"
			"subcontrol-position:right;"
			"}"
			// 当滚动条滚动的时候，上面的部分和下面的部分
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

	//检测特征名，主要
	QTreeWidgetItem* m_pMainItem = new QTreeWidgetItem;
	m_pMainItem->setText(0, QString::fromLocal8Bit("检测特征") + QString::number(FinsFea->InsFeatureindex));
	m_pMainItem->setData(0, Qt::UserRole, QVariant::fromValue((void*)(FinsFea)));

	//曲面，第一
	QTreeWidgetItem* m_pFaceItem = new QTreeWidgetItem;
	m_pFaceItem->setText(0, QString::fromLocal8Bit("曲面"));

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

	//检测类型，第二
	QTreeWidgetItem* m_pTypeItemMain = new QTreeWidgetItem;
	m_pTypeItemMain->setText(0, QString::fromLocal8Bit("检测类型"));
	QTreeWidgetItem* m_pTypeItem = new QTreeWidgetItem;
	switch (FinsFea->type)
	{
		case PARALLELISM:
		{
			m_pTypeItem->setText(0, QString::fromLocal8Bit("平行度"));
			break;
		}
		case ROUNDNESS:
		{
			m_pTypeItem->setText(0, QString::fromLocal8Bit("圆度"));
			break;
		}
		case DISTANCE:
		{
			if (FinsFea->DirState == 1)
			{
				m_pTypeItem->setText(0, QString::fromLocal8Bit("X向距离"));
			}
			else if (FinsFea->DirState == 2)
			{
				m_pTypeItem->setText(0, QString::fromLocal8Bit("Y向距离"));
			}
			else if (FinsFea->DirState == 3)
			{
				m_pTypeItem->setText(0, QString::fromLocal8Bit("Z向距离"));
			}
			else
			{
				m_pTypeItem->setText(0, QString::fromLocal8Bit("距离"));
			}
			
			break;
		}
		case VERTICALITY:
		{
			m_pTypeItem->setText(0, QString::fromLocal8Bit("垂直度"));
			break;
		}
		case DIAMETER:
		{
			m_pTypeItem->setText(0, QString::fromLocal8Bit("直径"));
			break;
		}
		case AUXIALITY:
		{
			m_pTypeItem->setText(0, QString::fromLocal8Bit("同轴度"));
			break;
		}
		case FLATNESS:
		{
			m_pTypeItem->setText(0, QString::fromLocal8Bit("平面度"));
			break;
		}
		default:
		{
			m_pTypeItem->setText(0, QString::fromLocal8Bit("未选择"));
			break;
		}
	}
	m_pTypeItemMain->addChild(m_pTypeItem);
		
	//检测参数，第三
	QTreeWidgetItem* m_pValueItem = new QTreeWidgetItem;
	m_pValueItem->setText(0, QString::fromLocal8Bit("参数"));

	QTreeWidgetItem* pValueItem = new QTreeWidgetItem();
	QTreeWidgetItem* pUpDeviationItem = new QTreeWidgetItem();
	QTreeWidgetItem* pDownDeviationItem = new QTreeWidgetItem();

	if (FinsFea->type == PARALLELISM)
	{
		pValueItem->setText(0, QString::fromLocal8Bit("平行度误差值：") + QString::number(FinsFea->FInsToleranceValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pValueItem);
	}
	else if (FinsFea->type == ROUNDNESS)
	{
		pValueItem->setText(0, QString::fromLocal8Bit("圆度误差值：") + QString::number(FinsFea->FInsToleranceValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pValueItem);
	}
	else if (FinsFea->type == DISTANCE)
	{

		pValueItem->setText(0, QString::fromLocal8Bit("距离标准值：") + QString::number(FinsFea->FInsToleranceValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pValueItem);
		pUpDeviationItem->setText(0, QString::fromLocal8Bit("上偏差：+") + QString::number(FinsFea->FInsUpDeviationValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pUpDeviationItem);
		pDownDeviationItem->setText(0, QString::fromLocal8Bit("下偏差：-") + QString::number(FinsFea->FInsDownDeviationValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pDownDeviationItem);
	}
	else if (FinsFea->type == VERTICALITY)
	{
		pValueItem->setText(0, QString::fromLocal8Bit("垂直度误差值：") + QString::number(FinsFea->FInsToleranceValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pValueItem);
	}
	else if (FinsFea->type == DIAMETER)
	{
		pValueItem->setText(0, QString::fromLocal8Bit("直径标准值：") + QString::number(FinsFea->FInsToleranceValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pValueItem);
		pUpDeviationItem->setText(0, QString::fromLocal8Bit("上偏差：+") + QString::number(FinsFea->FInsUpDeviationValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pUpDeviationItem);
		pDownDeviationItem->setText(0, QString::fromLocal8Bit("下偏差：-") + QString::number(FinsFea->FInsDownDeviationValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pDownDeviationItem);
	}
	else if (FinsFea->type == AUXIALITY)
	{
		pValueItem->setText(0, QString::fromLocal8Bit("同轴度误差值：") + QString::number(FinsFea->FInsToleranceValue) + QString::fromLocal8Bit("mm"));
		m_pValueItem->addChild(pValueItem);
	}
	else if (FinsFea->type == FLATNESS)
	{
		pValueItem->setText(0, QString::fromLocal8Bit("平面度误差值：") + QString::number(FinsFea->FInsToleranceValue) + QString::fromLocal8Bit("mm"));
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

	//图表中项目
	QPoint localPos2 = m_pEvaResView->viewport()->mapFromGlobal(point);
	QModelIndex index = m_pEvaResView->indexAt(localPos2);
	m_TableCurrRow = index.row();

	//测试所选择的item是否准确
	if (pItem)
	{
		std::cout << pItem->text(0).toLocal8Bit().data() << std::endl;
		std::cout << pItem->type() << std::endl;
	}

	//如果选中了item
	if (pItem)
	{
		//如果父节点为空，即为根节点(一级)，操作如下：
		if (pItem->parent() == NULL)
		{
			m_pPopMenu->clear();
			m_pPopMenu->addAction(m_pAccuEvalu);
			m_pPopMenu->addAction(m_pEvaluAll);
			m_pPopMenu->addAction(m_pFeaDel);
			m_pPopMenu->exec(QCursor::pos());
		}
	}
	else if (index.isValid())//如果选中了图表
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
			QString::fromLocal8Bit("未检测到实际测点，请确认已获取实际测点数据之后再执行此操作"), "OK");

		return;
	}

	//FInsFeature* pInsFeature = gWindow->GetMainDoc()->m_InsFeatureList[InsFeaIndex];
	m_InsfratureList.push_back(pInsFeature);
	EvaluData InsRes = RealValueCal(pInsFeature);

	//检测特征为空指针说明该检测特征中存在某些面上无测点，无法评定，该设定613行实现
	if (InsRes.p_InsFea == NULL)
	{
		return;
	}

	m_EvaluDataList.push_back(InsRes);
	//设置检测特征名
	m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 0, new QStandardItem(QString::fromLocal8Bit("检测特征") + QString::number(pInsFeature->InsFeatureindex)));

	if (pInsFeature->type == PARALLELISM || pInsFeature->type == ROUNDNESS || pInsFeature->type == VERTICALITY || pInsFeature->type == AUXIALITY || pInsFeature->type == FLATNESS || pInsFeature->type == PROFILE || pInsFeature->type == POSITION)//测量形位公差时
	{
		//设置检测类型和用户设定公差
		if (pInsFeature->type == PARALLELISM)//检测平行度时
		{
			//检测类型
			m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("平行度")));
		}
		else if (pInsFeature->type == ROUNDNESS)//检测圆度时
		{
			//检测类型
			m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("圆度")));
		}
		else if (pInsFeature->type == VERTICALITY)//检测垂直度时
		{
			//检测类型
			m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("垂直度")));
		}
		else if (pInsFeature->type == AUXIALITY)//检测同轴度
		{
			//检测类型
			m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("同轴度")));
		}
		else if (pInsFeature->type == FLATNESS)
		{
			//检测类型
			m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("平面度")));
		}
		else if (pInsFeature->type == PROFILE)
		{
			//检测类型
			m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("轮廓度")));
		}
		else if (pInsFeature->type == POSITION)
		{
			//检测类型
			m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("位置度")));
		}

		//用户设定公差
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 2, new QStandardItem(QString::number(pInsFeature->FInsToleranceValue)));

		//实际公差
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 3, new QStandardItem(QString::number(InsRes.ToleranceValue)));
		
		//公差类检测特征没有设定尺寸、实际尺寸、上偏差、下偏差
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 4, new QStandardItem("-"));
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 5, new QStandardItem("-"));
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 6, new QStandardItem("-"));
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 7, new QStandardItem("-"));
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 8, new QStandardItem("-"));
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 9, new QStandardItem("-"));
	}
	else if (pInsFeature->type == DISTANCE || pInsFeature->type == DIAMETER)//测量尺寸时
	{
		//设置检测类型和用户设定尺寸及实际尺寸
		if (pInsFeature->type == DISTANCE)//检测距离时
		{
			//检测类型
			if (pInsFeature->DirState == 1)
			{
				m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("X向距离")));
			}
			else if (pInsFeature->DirState == 2)
			{
				m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("Y向距离")));
			}
			else if (pInsFeature->DirState == 3)
			{
				m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("Z向距离")));
			}
			else
			{
				m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("距离")));
			}
		}
		else if (pInsFeature->type == DIAMETER)//检测直径时
		{
			//检测类型
			m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 1, new QStandardItem(QString::fromLocal8Bit("直径")));
		}
		
		//尺寸类检测特征无设定公差和实际公差
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 2, new QStandardItem("-"));
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 3, new QStandardItem("-"));

		//设定公差及上、下偏差
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 4, new QStandardItem(QString::number(pInsFeature->FInsToleranceValue)));
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 5, new QStandardItem("+" + QString::number(pInsFeature->FInsUpDeviationValue)));
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 6, new QStandardItem("-" + QString::number(pInsFeature->FInsDownDeviationValue)));

		//实际尺寸
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 7, new QStandardItem(QString::number(InsRes.DimensionValue)));

		//实际上偏差
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 8, new QStandardItem("+" + QString::number(InsRes.upDeviation)));

		//实际下偏差
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 9, new QStandardItem(QString::number(InsRes.downDeviation)));
	}
	else
	{
		return;
	}

	//判断是否合格
	if (InsRes.qualify)//合格
	{
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 10, new QStandardItem(QString::fromLocal8Bit("合格")));
		m_pEvaResModel->item(m_InsfratureList.size() - 1, 10)->setForeground(QBrush(QColor(0, 255, 0)));//数据颜色
	}
	else//不合格
	{
		m_pEvaResModel->setItem(m_InsfratureList.size() - 1, 10, new QStandardItem(QString::fromLocal8Bit("不合格")));
		m_pEvaResModel->item(m_InsfratureList.size() - 1, 10)->setForeground(QBrush(QColor(255, 0, 0)));//数据颜色
	}

	m_pEvaResModel->item(m_InsfratureList.size() - 1, 10)->setTextAlignment(Qt::AlignCenter);//数据居中显示
	m_pEvaResModel->item(m_InsfratureList.size() - 1, 10)->setFont(QFont("Times", 10, QFont::Black));//数据加粗
}

void AccuEvaluWnd::InitializeModel(QStandardItemModel* model)
{
	model->setColumnCount(11);
	model->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("检测特征名"));
	model->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("检测类型"));
	model->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("设定公差"));
	model->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("实际公差"));
	model->setHeaderData(4, Qt::Horizontal, QString::fromLocal8Bit("设定尺寸"));
	model->setHeaderData(5, Qt::Horizontal, QString::fromLocal8Bit("设定上偏差"));
	model->setHeaderData(6, Qt::Horizontal, QString::fromLocal8Bit("设定下偏差"));
	model->setHeaderData(7, Qt::Horizontal, QString::fromLocal8Bit("实际尺寸"));
	model->setHeaderData(8, Qt::Horizontal, QString::fromLocal8Bit("实际上偏差"));
	model->setHeaderData(9, Qt::Horizontal, QString::fromLocal8Bit("实际下偏差"));
	model->setHeaderData(10, Qt::Horizontal, QString::fromLocal8Bit("是否合格"));
}

EvaluData AccuEvaluWnd::RealValueCal(FInsFeature* pInsFea)
{
	EvaluData Res;
	Res.qualify = false;
	Res.p_InsFea = pInsFea;

	//获取实际和理论测点
	QVector<FIpPoint>MesPntLstTmp = gWindow->GetMainDoc()->GetMesPntList();
	QVector<FIpPoint>ThoPntLstTmp = gWindow->GetMainDoc()->GetIpPntList();

	QVector<FIpPoint>MesPntLst = MesPntLstTmp;
	QVector<FIpPoint>ThoPntLst = ThoPntLstTmp;

	// 寻找位于目标检测特征包含的平面内的测点
	// pair中，FSurface* 为曲面，vector<gp_Pnt> 为曲面上的测点
	vector<pair<FSurface*, vector<gp_Pnt>>> Surf_PntGroup1;
	vector<pair<FSurface*, vector<gp_Pnt>>> Surf_PntGroup2;

	int NbFace1 = pInsFea->m_FSurfList1.size();
	for (int i = 0; i < NbFace1; i++)
	{
		// 记录找出的在本次循环中的目标曲面上的测点索引，用于后续删除，其它曲面上的点不需要找这个曲面已找到的点
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

		//去掉已经视为合格的点，下一次循环直接使用去掉当前曲面上的点后剩下的点
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







		//// 记录找出的在本次循环中的目标曲面上的测点索引，用于后续删除，其它曲面上的点不需要找这个曲面
		//vector<int> TarPntInd;

		//vector<gp_Pnt> SearchedPnts;

		//FSurface* CurrSurf = pInsFea->m_FSurfList1[i];
		//ON_Mesh* CurrMesh = CurrSurf->GetMesh()->OnMesh();
		//ON_3fPoint onV[3];//三角面片三个角点
		//int nTriFace = CurrMesh->FaceCount();

		//for (int j = 0; j < MesPntLst.size(); j++)
		//{
		//	//计算某个测点到检测特征内的面的每个三角面片之间的距离
		//	vector<double> Er;
		//	double ErRes;

		//	for (int k = 0; k < nTriFace; k++)
		//	{
		//		ON_MeshFace face = CurrMesh->m_F[k];
		//		onV[0] = CurrMesh->m_V[face.vi[0]];
		//		onV[1] = CurrMesh->m_V[face.vi[1]];
		//		onV[2] = CurrMesh->m_V[face.vi[2]];

		//		if (distance(ErRes, GP_PNT(ThoPntLst[j]), GP_PNT(onV[0]), GP_PNT(onV[1]), GP_PNT(onV[2])))//判断理论测点到mesh的距离，
		//		{
		//			Er.push_back(ErRes);
		//		}
		//	}
		//	if (Er.size() != 0 && MIN(Er) < 0.5)//阈值1，任意面上的测点到其他面的距离不能小于该阈值
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

		////去掉已经视为合格的点，下一次循环直接使用去掉当前曲面上的点后剩下的点
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
		// 记录找出的在本次循环中的目标曲面上的测点索引，用于后续删除，其它曲面上的点不需要找这个曲面
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

		//去掉已经视为合格的点，下一次循环直接使用去掉当前曲面上的点后剩下的点
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






		//// 记录找出的在本次循环中的目标曲面上的测点索引，用于后续删除，其它曲面上的点不需要找这个曲面
		//vector<int> TarPntInd;

		//vector<gp_Pnt> SearchedPnts;

		//FSurface* CurrSurf = pInsFea->m_FSurfList2[i];
		//ON_Mesh* CurrMesh = CurrSurf->GetMesh()->OnMesh();
		//ON_3fPoint onV[3];//三角面片三个角点
		//int nTriFace = CurrMesh->FaceCount();

		//for (int j = 0; j < MesPntLst.size(); j++)
		//{
		//	//计算某个测点到检测特征内的面的每个三角面片之间的距离
		//	vector<double> Er;
		//	double ErRes;

		//	for (int k = 0; k < nTriFace; k++)
		//	{
		//		ON_MeshFace face = CurrMesh->m_F[k];
		//		onV[0] = CurrMesh->m_V[face.vi[0]];
		//		onV[1] = CurrMesh->m_V[face.vi[1]];
		//		onV[2] = CurrMesh->m_V[face.vi[2]];

		//		if (distance(ErRes, GP_PNT(ThoPntLst[j]), GP_PNT(onV[0]), GP_PNT(onV[1]), GP_PNT(onV[2])))//判断理论测点到mesh的距离，
		//		{
		//			Er.push_back(ErRes);
		//		}
		//	}
		//	if (Er.size() != 0 && MIN(Er) < 0.5)//阈值1，任意面上的测点到其他面的距离不能小于该阈值
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

		////去掉已经视为合格的点，下一次循环直接使用去掉当前曲面上的点后剩下的点
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
				QString::fromLocal8Bit("检测到该检测特征中某些型面上无测点，请确认检测特征中所有型面上均有测点后再试"), "OK");

			Res.p_InsFea = NULL;
			return Res;
		}
	}

	for (int i = 0; i < Surf_PntGroup2.size(); i++)
	{
		if (Surf_PntGroup2[i].second.size() == 0)
		{
			QMessageBox::information(this, QString::fromLocal8Bit("EROOR"),
				QString::fromLocal8Bit("检测到该检测特征中某些型面上无测点，请确认检测特征中所有型面上均有测点后再试"), "OK");

			Res.p_InsFea = NULL;
			return Res;
		}
	}

	// 将每个要素上的点放在一组
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

	if (pInsFea->type == PARALLELISM)//测量平行度形位公差时
	{
		//先判断是不是圆柱轴线对圆柱轴线的平行度(有补充)
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

	else if (pInsFea->type == ROUNDNESS)//测量圆度形位公差时
	{
		Res.ToleranceValue = ToleranceFunction::Roundness(AllSurfPntList1, Surf_PntGroup1[0].first);

		if (pInsFea->FInsToleranceValue >= Res.ToleranceValue)
		{
			Res.qualify = true;
		}
	}

	else if (pInsFea->type == VERTICALITY)//测量垂直度形位公差时
	{
		//先判断是不是圆柱轴线对平面的垂直度
		bool IsAxisToPlane = false;
		int theCylinderInd = -1;

		//先找到哪一组是圆柱
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
				_DEBUG_ERROR("判据错误");

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

	else if (pInsFea->type == AUXIALITY)//测量同轴度误差
	{
		Res.ToleranceValue = ToleranceFunction::Axiality(AllSurfPntList1, AllSurfPntList2, Surf_PntGroup1[0].first);

		if (pInsFea->FInsToleranceValue >= Res.ToleranceValue)
		{
			Res.qualify = true;
		}
	}

	else if (pInsFea->type == DISTANCE)//测量距离尺寸时(有补充)
	{
		// 先判断是否是圆柱轴线到平面的距离
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
			//先判断距离类型，圆柱轴线到圆柱轴线或者平面到平面
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

	else if (pInsFea->type == DIAMETER)//测量直径尺寸时
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

	else if (pInsFea->type == FLATNESS)//测量平面度时
	{
		Res.ToleranceValue = ToleranceFunction::Flatness(AllSurfPntList1);

		if (pInsFea->FInsToleranceValue >= Res.ToleranceValue)
		{
			Res.qualify = true;
		}
	}

	else if (pInsFea->type == PROFILE)//测量轮廓度时
	{


		Res.qualify = true;

	}

	else if (pInsFea->type == POSITION)//测量位置度时
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
			QString::fromLocal8Bit("检测特征类型错误"), "OK");
	}

	return Res;
}

void AccuEvaluWnd::onToEvaluReport()
{
	InspReportInfoDlg* theInfoDlg = new InspReportInfoDlg(m_EvaluDataList);

	theInfoDlg->show();


	//gWindow->SetInsReportDlg(new FInsReportDlg(m_EvaluDataList));
	//m_pInsReport = gWindow->GetInsReportDlg();

	/////////////////////////////////////////////////////////////长图
	//QScrollArea* s = new QScrollArea();
	//s->setWindowIcon(QIcon("../icons/AccuRep.png"));
	//s->setWindowTitle(QString::fromLocal8Bit("检测报告"));
	//s->setGeometry(100, 80, 1260, 900);
	//s->setFixedSize(1285, 900);
	//s->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	//s->setWidget(m_pInsReport);
	//s->show();
	/////////////////////////////////////////////////////////////////////////////////////////////

}

void AccuEvaluWnd::onFInsFeaDel()
{
	//找到index为选中item的检测特征,删除Doc里对应元素
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

			break;//一次只删除一个
		}
	}

	//如果该检测特征加入了精度评定窗口右侧列表，同时删除右侧列表内容
	for (int i = 0; i < m_InsfratureList.size(); i++)
	{
		if (ExNumFrQsg(m_pEvaResModel->item(i, 0)->text()) == ExNumFrQsg(m_pTmpItem->text(0)))
		{
			m_pEvaResModel->removeRow(i);

			//更新精度评定窗口检测特征列表
			tmpLst.clear();
			for (int j = 0; j < m_InsfratureList.size(); j++)
			{
				if (j != i)
				{
					tmpLst.push_back(m_InsfratureList[j]);
				}
			}
			m_InsfratureList = tmpLst;

			//删除评定结果数组内的对应项目
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

	//删除精度检测特征树窗口的目标检测特征
	int NbAccuWndItem = static_cast<FInsFeaTreeWnd*>(gWindow->GetSubWnd()[6])->GetTree()->topLevelItemCount();
	for (int i = 0; i < NbAccuWndItem; i++)
	{
		if (ExNumFrQsg(static_cast<FInsFeaTreeWnd*>(gWindow->GetSubWnd()[6])->GetTree()->topLevelItem(i)->text(0)) == ExNumFrQsg(m_pTmpItem->text(0)))
		{
			delete static_cast<FInsFeaTreeWnd*>(gWindow->GetSubWnd()[6])->GetTree()->topLevelItem(i);

			break;
		}
	}

	//删除检测精度评定窗口的目标检测特征
	delete m_pTmpItem;
}

void AccuEvaluWnd::onChartRowDel()
{
	//删除右侧评定框检测特征向量内对应的检测特征
	vector<FInsFeature*> tmpLst;
	for (int i = 0; i < m_InsfratureList.size(); i++)
	{
		if (i != m_TableCurrRow)
		{
			tmpLst.push_back(m_InsfratureList[i]);
		}
	}
	m_InsfratureList = tmpLst;

	//删除评定结果数组内的对应项目
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
			QString::fromLocal8Bit("未检测到实际测点，请确认已获取实际测点数据之后再执行此操作"), "OK");

		return;
	}

	int NbAllItem = m_pTree->topLevelItemCount();
	for (int i = 0; i < NbAllItem; i++)
	{
		//如果某项已经评定，则不重复评定该项目
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
	//获取界面的图片
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
