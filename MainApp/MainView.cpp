//#include "../FKernel/FKernelDefines.h"
//#include "../FKernel/FView.h"
#include "../External/fkernel/Include/FKernelHeader.h"
#include "FUtilities.h"
#include "MainDoc.h"
#include "MainView.h"
#include "FMainWindow.h"

FMainView::FMainView(FMainDoc* pDoc)
	: QGLWidget(QGLFormat(QGL::SampleBuffers), NULL)
{
	m_pMainDoc = pDoc;
	setAttribute( Qt::WA_DeleteOnClose );
	setFocusPolicy(Qt::StrongFocus);//enable to handle keyboard events
	setMouseTracking (true ) ;//enable mouse move event
	setFocusPolicy( Qt::StrongFocus );
	m_pFView = new FT::FView(m_pMainDoc);
	RotateDbl = false;
	m_bCtrlKeyDown = false;
	m_iCameraType = FCameraType::NORMAL_CAM;
	m_pSubWindow = NULL;	
}

FMainView::FMainView(QWidget* parent, FMainDoc* pDoc)
	: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
	m_pMainDoc = pDoc;
	setAttribute( Qt::WA_DeleteOnClose );
	setFocusPolicy(Qt::StrongFocus);//enable to handle keyboard events
	setMouseTracking (true ) ;//enable mouse move event
	setFocusPolicy( Qt::StrongFocus );
	m_pFView = new FT::FView(m_pMainDoc);
	updateGL();
	RotateDbl = false;
	m_bCtrlKeyDown = false;
	m_iCameraType = FCameraType::NORMAL_CAM;
	m_pSubWindow = (FSubWindow*)parent;		
}

FMainView::FMainView(FMainView& rhs)
{
	setAttribute( Qt::WA_DeleteOnClose );
	setFocusPolicy(Qt::StrongFocus);
	setMouseTracking (true );
	setFocusPolicy( Qt::StrongFocus );

	m_pFView = new FT::FView(*(rhs.GetKernelView()));
	RotateDbl = false;

	m_bCtrlKeyDown = false;
	m_iCameraType = FCameraType::NORMAL_CAM;
	m_pSubWindow = NULL;
}

FMainView::~FMainView()
{
	if(m_pFView != NULL) 
	{
		delete m_pFView;
	}
}


void FMainView::initializeGL()
{
	if ( m_pFView && m_pFView->GetDocument()==NULL )
	{
		m_pFView->GLInit();
	}
}

void FMainView::resizeGL(int w,int h)
{
	m_pFView->OnSize(w,h);
}

void FMainView::paintGL()
{
	m_pFView->GLRefresh();
}

void FMainView::Update()
{
	this->update();
}
bool FMainView::pick_entity(const FT::FEvent& pe, FT::FEntity*& ent, FT::FEntType entityType/*=GL_ENTITY*/, bool showBox/* = true */)
{
	if ( NULL == pe.view )
		return false;
	if (!showBox )
	{
		//	return pe.view->pick_entity(ent,pe.x,pe.y,entityType);
	}
	return false;
}

bool FMainView::pick_entity(FT::PFEntityList& entities,const FT::FEvent &pe,FT::FEntType entityType/*=GL_ENTITY*/, bool showBox/* = true */)
{
	if ( NULL == pe.view )
		return false;
	entities.clear();
	if ( !showBox )
	{
		//	return pe.view->pick_entity(entities,pe.x,pe.y,entityType);
	}
	return false;
}

void FMainView::wheelEvent(QWheelEvent *e)
{
	if(!m_pFView || !m_pMainDoc)	return ;
	this->setFocus();
	m_pFView->OnWheelEvent(e);
	this->update();
}

void FMainView::mouseMoveEvent(QMouseEvent *e)
{	
	if(!m_pFView || !m_pMainDoc)	return ;
	m_pFView->OnMouseMove(e);
	//gSetHintMessage(QObject::tr("Middle(drag/roll): Rotate/Zoom; Right(drag): Translate"));
	gSetHintMessage(QString::fromLocal8Bit("ÖĞ¼ü(ÍÏ×§/¹ö¶¯): Ğı×ª/Ëõ·Å; ÓÒ¼ü(ÍÏ×§): Æ½ÒÆ"));
	this->update();
}

void FMainView::dropEvent(QDropEvent* event)
{
	int ii=0;
}

void FMainView::mousePressEvent(QMouseEvent *e)
{
	if(!m_pFView || !m_pMainDoc)	return;
	m_pFView->SetbMousePressed(true);
	m_pFView->SetOldMousePos(e->pos().x(), e->pos().y());

	if(e->button()==Qt::LeftButton)
		onLButtonDown(e);
	else if(e->button()==Qt::RightButton)
		onRButtonDown(e);
	//else if (e->button()==Qt::MidButton)
	//{
	//	gSetHintMessage(QObject::tr("Mouse middle key to rotate model"));
	//}
	e->accept();
	this->update();
}

void FMainView::keyPressEvent(QKeyEvent *e)
{
	if(!m_pFView || !m_pMainDoc)	return ;
	m_pFView->OnKeyDown(e);
	this->update();
}

void FMainView::keyReleaseEvent(QKeyEvent *e)
{
	if(!m_pFView || !m_pMainDoc)	return ;
	m_pFView->OnKeyUp(e);
	this->update();
}
void FMainView::onRButtonDown(QMouseEvent *e)
{
	if(!m_pFView || !m_pMainDoc)	return ;
	m_pFView->OnRButtonDown(e);
	this->update();
}

void FMainView::onLButtonDown(QMouseEvent *e)
{
	if(!m_pFView || !m_pMainDoc)	return ;
	m_pFView->OnLButtonDown(e);
	this->update();
}

void FMainView::mouseReleaseEvent(QMouseEvent *e)
{
	if(e->button()==Qt::LeftButton)
	{
		m_pFView->OnLButtonUp(e);
	}
	else if(e->button()==Qt::RightButton)
	{			
		m_pFView->OnRButtonUp(e);
	}
	GetKernelView()->SetCameraType(FCameraType::NORMAL_CAM);
	GetKernelView()->SetbMousePressed(false);
	e->accept();
}

//void FMainView::onViewIsometricview()
//{
//	this->setWindowTitle( "3D" );
//	m_pFView->SetViewDirection(VD_ISOMETRICVIEW);
//	this->update();
//}

//void FMainView::onViewFrontview()
//{
//	this->setWindowTitle( "Front(X/Z)" );
//	m_pFView->SetViewDirection(VD_FRONTVIEW);
//	this->update();
//}

//void FMainView::onViewBackview()
//{
//	this->setWindowTitle( "Back(X/Z)" );
//	m_pFView->SetViewDirection(VD_BACKVIEW);
//	this->update();
//}

//void FMainView::onViewTopview()
//{
//	this->setWindowTitle( "Top(X/Y)" );
//	m_pFView->SetViewDirection(VD_TOPVIEW);
//	this->update();
//}

//void FMainView::onViewBottomview()
//{
//	this->setWindowTitle( "Bottom(X/Y)" );
//	m_pFView->SetViewDirection(VD_BOTTOMVIEW);
//	this->update();
//}

//void FMainView::onViewLeftview()
//{
//	this->setWindowTitle( "Left(Y/Z)" );
//	m_pFView->SetViewDirection(VD_LEFTVIEW);
//	this->update();
//}

//void FMainView::onViewRightview()
//{
//	this->setWindowTitle( "Right(Y/Z)" );
//	m_pFView->SetViewDirection(VD_RIGHTVIEW);	
//	this->update();
//}

//void FMainView::onViewZoomin()
//{
//	m_pFView->zoom(1.5);
//	this->update();
//}

//void FMainView::onViewZoomout()
//{
//	m_pFView->zoom(0.75);	
//	this->update();
//}

//void FMainView::onViewZoomall()
//{
//	m_pFView->zoom_all_in_view();
//	this->update();
//}

//void FMainView::onViewResetView()
//{
//	this->GetKernelView()->eyeAimAt( 0.0f, 0.0f, 0.0f );
//	this->update();
//}

//void FMainView::onViewWcs()
//{
//	m_pFView->SetAxisDisplay(!(m_pFView->GetAxisDisplay()));
//	this->update();
//}

//void FMainView::onViewOrigin()
//{
//	m_pFView->SetOriginDisplay(!(m_pFView->GetOriginDisplay()));
//	this->update();
//}

//void FMainView::onViewMesh()
//{
//	UiMainWindow *ui = (UiMainWindow*)GetUi();
//	ui->actionMesh->setChecked(true);
//	m_pFView->SetGroundMeshDisplay(true);
//	m_pFView->SetMesh(true);
//	m_pFView->SetProfile(false);
//	m_pFView->SetFlatShading(false);
//	m_pFView->SetSmoothShading(false);
//	m_pFView->SetTexture(false);
//	m_pFView->SetTransparent(false);
//	m_pFView->SetDeformation(false);
//	this->update();
//}


//void FMainView::onViewProfile()
//{
//	UiMainWindow *ui = (UiMainWindow*)GetUi();
//	ui->actionProfile->setChecked(true);
//	m_pFView->SetGroundMeshDisplay(true);
//	m_pFView->SetMesh(false);
//	m_pFView->SetProfile(true);
//	m_pFView->SetFlatShading(false);
//	m_pFView->SetSmoothShading(false);
//	m_pFView->SetTexture(false);
//	m_pFView->SetTransparent(false);
//	m_pFView->SetDeformation(false);
//	this->update();
//}

//void FMainView::onViewFlatShading()
//{
//	UiMainWindow *ui = (UiMainWindow*)GetUi();
//	ui->actionFlatShade->setChecked(true);
//	m_pFView->SetGroundMeshDisplay(false);
//	m_pFView->SetProfile(false);
//	m_pFView->SetMesh(false);
//	m_pFView->SetFlatShading(true);
//	m_pFView->SetSmoothShading(false);
//	m_pFView->SetTexture(false);
//	m_pFView->SetTransparent(false);
//	m_pFView->SetDeformation(false);
//	this->update();
//}

//void FMainView::onViewSmoothShading()
//{
//	UiMainWindow *ui = (UiMainWindow*)GetUi();
//	ui->actionSmoothShade->setChecked(true);
//	m_pFView->SetGroundMeshDisplay(false);
//	m_pFView->SetFlatShading(false);
//	m_pFView->SetSmoothShading(true);
//	m_pFView->SetProfile(false);
//	m_pFView->SetMesh(false);
//	m_pFView->SetTexture(false);
//	m_pFView->SetTransparent(false);
//	m_pFView->SetDeformation(false);
//	this->update();
//}

//void FMainView::onViewTexture()
//{
//	UiMainWindow *ui = (UiMainWindow*)GetUi();
//	ui->actionTexture->setChecked(true);
//	m_pFView->SetGroundMeshDisplay(false);
//	m_pFView->SetTexture(true);
//	m_pFView->SetFlatShading(false);
//	m_pFView->SetSmoothShading(false);
//	m_pFView->SetProfile(false);
//	m_pFView->SetMesh(false);
//	m_pFView->SetTransparent(false);
//	m_pFView->SetDeformation(false);
//	this->update();
//}


//void FMainView::onViewTransparent()
//{
//	UiMainWindow *ui = (UiMainWindow*)GetUi();
//	ui->actionTransparent->setChecked(true);
//	m_pFView->SetGroundMeshDisplay(false);
//	m_pFView->SetTransparent(true);
//	m_pFView->SetFlatShading(false);
//	m_pFView->SetSmoothShading(false);
//	m_pFView->SetProfile(false);
//	m_pFView->SetMesh(false);
//	m_pFView->SetTexture(false);
//	m_pFView->SetDeformation(false);
//	this->update();
//}