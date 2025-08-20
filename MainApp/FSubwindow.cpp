#include "FMainWindow.h"
#include "MainView.h"
#include <QEvent>
#include "FSubwindow.h"
#include "CustomTitle.h"

FSubWindow::FSubWindow(QWidget* parent, FMainDoc* pDoc)
	:QMdiSubWindow(parent)
{
	setWindowFlags(Qt::FramelessWindowHint);//Òþ²Ø±ß¿ò
	QWidget* dockWidgetContents = new QWidget();
	m_pTitleBar = new TitleBar(this);
	QVBoxLayout* pLayout = new QVBoxLayout(dockWidgetContents);
	pLayout->setSpacing(0);
	pLayout->setMargin(0);
	pLayout->addWidget(m_pTitleBar);

	m_pDoc = pDoc;
	m_pView = NULL;
	setAttribute(Qt::WA_DeleteOnClose);
	//setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowMinMaxButtonsHint);
	setFocusPolicy(Qt::ClickFocus);
	m_pView = new FMainView(this, m_pDoc);
	m_pView->Update();
	pLayout->addWidget(m_pView);
	dockWidgetContents->setLayout(pLayout);
	setWidget(dockWidgetContents);
}

FSubWindow::~FSubWindow()
{

}

void FSubWindow::SetView(FMainView* pview)
{
	m_pView = pview;
	if (m_pView)
		setWidget(m_pView);
}
void FSubWindow::Update()
{
	if (m_pView)
	{
		m_pView->Update();
	}
}

void FSubWindow::resizeEvent(QResizeEvent* resizeEvent)
{
	if (m_pView)
	{
		QSize iSize = resizeEvent->size();
		//	m_pView->resizeGL(iSize.width(), iSize.height());
	}
	QMdiSubWindow::resizeEvent(resizeEvent);
}
