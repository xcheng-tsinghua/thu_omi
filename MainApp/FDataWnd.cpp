#include <QtSql>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include "connection.h"
#include "FDataWnd.h"
#include "FMainWindow.h"
#include <iostream>

extern FMainWindow* gWindow;
extern QString gDockTitleColor;

using namespace std;

FDataWnd::FDataWnd(QWidget* parent,FMainDoc* pDoc)
	:QDockWidget(parent), m_pDoc(pDoc)
{	
	this->setAttribute(Qt::WA_StyledBackground, true);

	//setWindowTitle(QObject::tr("Data"));
	setWindowTitle(QString::fromLocal8Bit("�������"));
	//container widget
	QWidget* dockWidgetContents = new QWidget();
	//dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
	//QWidget* widget = new QWidget(dockWidgetContents);
	//widget->setObjectName(QString::fromUtf8("widget"));
	//widget->setGeometry(0, 0, 500, 500);

	m_pTableView = new QTableView();
	m_pTableView->setSelectionBehavior(QAbstractItemView::SelectItems);
	m_pTableView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_pTableView->verticalHeader()->setVisible(false);
	//QPushButton* pSaveBtn = new QPushButton(QObject::tr("Save"));
	//QPushButton* pImportBtn = new QPushButton(QObject::tr("Import"));
	//QPushButton* pDeletePntBtn = new QPushButton(QObject::tr("Delete a Point"));
	//QPushButton* pAnalysisBtn = new QPushButton(QObject::tr("Analysis"));

	QGridLayout* mainlayout = new QGridLayout(dockWidgetContents);
	mainlayout->setObjectName(QString::fromUtf8("mainlayout"));
	mainlayout->setContentsMargins(0, 0, 0, 0);
	mainlayout->addWidget(m_pTableView, 0, 0, 5, 4);
	//mainlayout->addWidget(pDeletePntBtn, 1, 4, 1, 1);
	//mainlayout->addWidget(pSaveBtn, 2, 4, 1, 1);
	//mainlayout->addWidget(pImportBtn,3, 4, 1, 1);
	//mainlayout->addWidget(pAnalysisBtn, 4, 4, 1, 1);
//	m_pDataPage->setLayout(page1GridLayout);

	setWidget(dockWidgetContents);

	//setLayout(mainlayout);

	QStringList drivers = QSqlDatabase::drivers();
	foreach(QString driver, drivers)
		qDebug()<<driver;

	if (!createConnection())
	//if (false)
		QMessageBox::critical(this, QObject::tr("Warning"), QObject::tr("Data server connection failed!"));
	else
	{
		m_pDbModel = new QSqlTableModel;
		InitializeModel(m_pDbModel);
		m_pTableView->setModel(m_pDbModel);
	}
	
	connect(m_pTableView->selectionModel(),
		SIGNAL(currentChanged(const QModelIndex &,const QModelIndex &)),
		this, SLOT(onSetHighlightPnt(const QModelIndex &,const QModelIndex &)));

	//add signal-slots
	//QObject::connect(pDeletePntBtn,SIGNAL(clicked()), gWindow, SLOT(onMeasureStart()));
	//QObject::connect(pPauseBtn,SIGNAL(clicked()), this, SLOT(onPauseBtn()));
	//	QObject::connect(pUpdateButton,SIGNAL(clicked()), this, SLOT(UpdateFeature()));

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
void FDataWnd::InitializeModel(QSqlTableModel *model)
{
	model->setTable("Data");
	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model->select();

	//model->setHeaderData(0, Qt::Horizontal, QObject::tr("Vert"));
	//model->setHeaderData(1, Qt::Horizontal, QObject::tr("Coord"));
	//model->setHeaderData(2, Qt::Horizontal, QObject::tr("Theo"));
	//model->setHeaderData(3, Qt::Horizontal, QObject::tr("Meas"));
	//model->setHeaderData(4, Qt::Horizontal, QObject::tr("Err"));

	model->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("���"));
	model->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("�������"));
	model->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("����ֵ"));
	model->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("����ֵ"));
	model->setHeaderData(4, Qt::Horizontal, QString::fromLocal8Bit("���"));

}
FDataWnd::~FDataWnd(void)
{
}

void FDataWnd::onResetPointData( bool dspTheo,int mesNum,bool dspError )
{
	/*cout	<<	"reset point data...\n";*/
	int nRowCnt;
	while(nRowCnt = m_pDbModel->rowCount())
	{
		m_pDbModel->removeRows(0,nRowCnt);
		m_pDbModel->submitAll();
	}
	int s = m_pDoc->GetIpPntList().size();
	for (int i=0;i<m_pDoc->GetIpPntList().size();i++)
	{
		m_pDbModel->insertRows(i*3,3);
		//ÿ�����������,����XYZ
		m_pDbModel->setData(m_pDbModel->index(i*3,0),i+1);
		m_pDbModel->setData(m_pDbModel->index(i*3,1),"X");
		m_pDbModel->setData(m_pDbModel->index(i*3+1,1),"Y");
		m_pDbModel->setData(m_pDbModel->index(i*3+1,0),i+1);
		m_pDbModel->setData(m_pDbModel->index(i*3+2,1),"Z");
		m_pDbModel->setData(m_pDbModel->index(i*3+2,0),i+1);
		if (dspTheo)
		{

			m_pDbModel->setData(m_pDbModel->index(i*3,2),m_pDoc->GetIpPntList()[i].x);
			m_pDbModel->setData(m_pDbModel->index(i*3+1,2),m_pDoc->GetIpPntList()[i].y);
			m_pDbModel->setData(m_pDbModel->index(i*3+2,2),m_pDoc->GetIpPntList()[i].z);
		}
		if (mesNum)
		{
			/*int j = mesNum-1;
			if (i < m_pDoc->GetMesPntList().size())
			{
				m_pDbModel->setData(m_pDbModel->index(i*3,3),m_pDoc->GetMesPntList()[i].x);
				m_pDbModel->setData(m_pDbModel->index(i*3+1,3),m_pDoc->GetMesPntList()[i].y);
				m_pDbModel->setData(m_pDbModel->index(i*3+2,3),m_pDoc->GetMesPntList()[i].z);
			}*/
		}
		if (dspError)
		{
		}
	}
	m_pDbModel->submitAll();
}

void FDataWnd::onUpdateMesPointData(int num)
{
	m_pTableView->scrollToTop();
	int nRowCnt = m_pDbModel->rowCount();
	if (nRowCnt < num*3)
	{
		return;
	}
	int i = num - 1;
	bool bx = m_pDbModel->setData(m_pDbModel->index(i*3,3),m_pDoc->GetMesPntList()[i].x);
	bool by = m_pDbModel->setData(m_pDbModel->index(i*3+1,3),m_pDoc->GetMesPntList()[i].y);
	bool bz = m_pDbModel->setData(m_pDbModel->index(i*3+2,3),m_pDoc->GetMesPntList()[i].z);
	int curPos = (i)*3+3;
	cout << "curPos:" << curPos << endl;
	if(curPos>=nRowCnt)
		m_pTableView->scrollToBottom();
	else
		m_pTableView->scrollTo(m_pDbModel->index(curPos,0));

	/*QSqlRecord record = m_pDbModel->record(i*3);
	record.setValue(3,m_pDoc->GetMesPntList()[i].x);
	m_pDbModel->setRecord(i*3,record);
	record = m_pDbModel->record(i*3+1);
	record.setValue(3,m_pDoc->GetMesPntList()[i].y);
	m_pDbModel->setRecord(i*3+1,record);
	record = m_pDbModel->record(i*3+2);
	record.setValue(3,m_pDoc->GetMesPntList()[i].z);
	m_pDbModel->setRecord(i*3+2,record);


	m_pDbModel->submitAll();*/


	//m_pTableView->scrollToBottom();

	//m_pTableView->verticalScrollBar()->setMaximum(nRowCnt);
	//QScrollBar* scrollBar = m_pTableView->verticalScrollBar();
	//scrollBar->setMaximum(nRowCnt);
	//scrollBar->setMinimum(1);
	//cout << scrollBar->value() << endl;
	//cout << scrollBar->minimum() << endl;
	//cout << scrollBar->maximum() << endl;
	
	//scrollBar->setMaximum(500);
	//scrollBar->setMinimum(1);
	//scrollBar->setRange(1,100);
	//cout << scrollBar->minimum() << endl;
	//cout << scrollBar->maximum() << endl;
	//double curPos = (3.0*i-3)*100.0/(nRowCnt);
	//cout << curPos << endl;
	//scrollBar->setSliderPosition(500);
	//scrollBar->setValue(scrollBar->maximum()+50);
	//m_pTableView->scrollTo(m_pDbModel->index((i+80)*3,0));
	
}

void FDataWnd::onSetHighlightPnt( const QModelIndex & current, const QModelIndex & previous )
{
	if (current.column() == 0)
	{
		int idx = m_pDbModel->data(current).toInt();
		m_pDoc->setHighlightPnt(idx);
	}
	else
		m_pDoc->setHighlightPnt(-1);
	gWindow->UpdateAllViews();
	
}

void FDataWnd::clearData()
{
	int nRowCnt;
	while(nRowCnt = m_pDbModel->rowCount())
	{
		m_pDbModel->removeRows(0,nRowCnt);
		m_pDbModel->submitAll();
	}
}

void FDataWnd::onUpdateError(int num)
{
	m_pTableView->scrollToTop();
	int nRowCnt = m_pDbModel->rowCount();
	if (nRowCnt < num*3)
	{
		return;
	}
	int i = num -1;
	
	bool error = m_pDbModel->setData(m_pDbModel->index(i*3,4),m_pDoc->GetMeaErrList().at(i));
}
