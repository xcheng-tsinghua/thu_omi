#include "FStatVisualWnd.h"
#include "qcustomplot.h"

extern QString gDockTitleColor;

FStatVisualWnd::FStatVisualWnd( QWidget* parent,FMainDoc* pDoc )
	:QDockWidget(parent), m_pDoc(pDoc)
{
	this->setAttribute(Qt::WA_StyledBackground, true);

	//setWindowTitle(QObject::tr("Statistic Visualization"));
	setWindowTitle(QString::fromLocal8Bit("数据可视化"));

	m_pCentralWgt = new QWidget;
	setWidget(m_pCentralWgt);

	m_pMainVLayout = new QVBoxLayout(m_pCentralWgt);
	m_pPlotTabWgt = new QTabWidget;
	m_pMainVLayout->addWidget(m_pPlotTabWgt);

	m_pCustomPlot1 = new QCustomPlot();
	m_pCustomPlot1->setInteractions(QCustomPlot::iRangeZoom|QCustomPlot::iRangeDrag);
	m_pPlotTabWgt->addTab(m_pCustomPlot1, tr("test1"));

	m_pCustomPlot2 = new QCustomPlot();
	m_pPlotTabWgt->addTab(m_pCustomPlot2, tr("test2"));

	//PlotPic1();

	//style
	{
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
		);

		//滚动条
		m_pCentralWgt->setStyleSheet(
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

FStatVisualWnd::~FStatVisualWnd( void )
{

}

void FStatVisualWnd::PlotPic1()
{
	if (!m_pCustomPlot1)
	{
		return;
	}
	m_pCustomPlot1->clearGraphs();

	double midVal = 0.5;
	QVector<double> midValVec;
	midValVec.push_back(midVal);
	midValVec.push_back(midVal);
	QVector<double> midNumVec;
	midNumVec.push_back(0.5);
	midNumVec.push_back(10.5);

	double upperVal = 0.9;
	QVector<double> upperValVec;
	upperValVec.push_back(upperVal);
	upperValVec.push_back(upperVal);
	QVector<double> upperNumVec;
	upperNumVec.push_back(0.5);
	upperNumVec.push_back(10.5);

	double lowerVal = 0.1;
	QVector<double> lowerValVec;
	lowerValVec.push_back(lowerVal);
	lowerValVec.push_back(lowerVal);
	QVector<double> lowerNumVec;
	lowerNumVec.push_back(0.5);
	lowerNumVec.push_back(10.5);

	QVector<double> data_x;
	for (int i = 1; i <= 10; i++)
	{
		data_x.push_back(i);
	}
	QVector<double> data_y;
	data_y.push_back(0.91);
	data_y.push_back(0.17);
	data_y.push_back(0.17);
	data_y.push_back(0.77);
	data_y.push_back(0.14);
	data_y.push_back(0.45);
	data_y.push_back(0.60);
	data_y.push_back(0.37);
	data_y.push_back(0.76);
	data_y.push_back(0.75);

	QCPGraph* pUpperGraph = m_pCustomPlot1->addGraph();
	QPen pen;
	pen.setColor(QColor(255, 0, 0));
	pUpperGraph->setLineStyle(QCPGraph::lsLine);//设置数据点由一条直线连接
	pUpperGraph->setPen(pen);
	pUpperGraph->setData(upperNumVec, upperValVec);

	QCPGraph* pMidGraph = m_pCustomPlot1->addGraph();
	pMidGraph->setLineStyle(QCPGraph::lsLine);//设置数据点由一条直线连接
	pMidGraph->setPen(pen);
	pMidGraph->setData(midNumVec, midValVec);

	QCPGraph* pLowerGraph = m_pCustomPlot1->addGraph();
	pLowerGraph->setLineStyle(QCPGraph::lsLine);//设置数据点由一条直线连接
	pLowerGraph->setPen(pen);
	pLowerGraph->setData(lowerNumVec, lowerValVec);

	QCPGraph* pDataLineGraph = m_pCustomPlot1->addGraph();
	pDataLineGraph->setLineStyle(QCPGraph::lsLine);//设置数据点由一条直线连接
	QPen dataPen;
	dataPen.setColor(QColor(0, 0, 255));
	pDataLineGraph->setPen(dataPen);
	pDataLineGraph->setData(data_x, data_y);

	QCPGraph* pDataDotGraph = m_pCustomPlot1->addGraph();
	pDataDotGraph->setScatterStyle(QCP::ssDisc);//设置数据点由一条直线连接
	pDataDotGraph->setPen(dataPen);
	pDataDotGraph->setData(data_x, data_y);

		/*pUpperGraph->setPen(QPen(Qt::red));
	pUpperGraph->setLineStyle(QCPGraph::lsNone);*/
	//pUpperGraph->setScatterStyle(QCPScatterStyle::ssDisc);

	// 设置轴范围和显示全部数据
	m_pCustomPlot1->xAxis->setRange(0,11);
	m_pCustomPlot1->yAxis->setRange(0, 1);

	m_pCustomPlot1->rescaleAxes();
	m_pCustomPlot1->xAxis->setAutoTickStep(false);
	m_pCustomPlot1->xAxis->setTickStep(1); 
	m_pCustomPlot1->xAxis->setTicks(false);
	//m_pCustomPlot1->xAxis->setSubTickCount(2);

	m_pCustomPlot1->replot();
	
}
