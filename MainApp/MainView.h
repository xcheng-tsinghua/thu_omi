#ifndef JCP3VIEW_H
#define JCP3VIEW_H

#include "../External/fkernel/Include/FKernelDefines.h"
#include "../External/fkernel/Include/FKernelHeader.h"

//namespace FT{
//	class FView;
//	class FEntity;
//}
class FMainDoc;
class FSubWindow;
class QJSelectBox;
using namespace FT;

class FMainView : public QGLWidget
{
//	Q_OBJECT
public: 
	FMainView(FMainDoc* pDoc=NULL);
	FMainView(QWidget *parent, FMainDoc* pDoc=NULL);
	FMainView(FMainView& rhs);
	~FMainView();	

	///////////////////////////// accessing methods /////////////////////////////////////////////
	FT::FCameraType GetCameraType(){return m_iCameraType;}
	FT::FView*		GetKernelView(){return m_pFView;}
	void			SetSubWindow(FSubWindow* pWindow);
	FSubWindow*		GetSubWindow();

	////////////////////////////// public methods ////////////////////////////////////////////
	void	Update();
	/*!
	\brief	Select entities derived from GLentity or given entity type.
	\param [i]pe			pick event.
	\param [o]ent			the selected entity if any.
	\param [i]entityType	given entity type, i.e. only entities with this type are selected.
	\param [i]showBox		if true, all selected entity will be returned in a popup menu.
							otherwise false, only the nearest entity will be selected. 
	\return	true if success, otherwise false.	*/		
	bool	pick_entity(const FT::FEvent& pe, FT::FEntity*& ent, FT::FEntType entityType=FEntType::FT_ENTITY, bool showBox = true );
	bool	pick_entity(FT::PFEntityList& entities,const FT::FEvent &pe,FT::FEntType entityType=FEntType::FT_ENTITY, bool showBox = true );


protected:
	/*!
	\brief	Select entities derived from GLentity or given entity type.
	\param [i]pe			pick event.
	\param [o]ent			the selected entity if any.
	\param [i]entityType	given entity type, i.e. only entities with this type are selected.
	\param [i]showBox		if true, all selected entity will be returned in a popup menu.
							otherwise false, only the nearest entity will be selected. 
	\return	true if success, otherwise false.	*/	
	void initializeGL();
	void paintGL();
	void resizeGL(int w,int h);
	void dropEvent(QDropEvent* event);
	/*!
	\brief	Event handler for mouse/keyboard events, reimplemented in subclass of QWidget.
	\param [i]e		mouse event, contains parameters describing a mouse event,e.g. e->pos().
	\return	void.	*/
	virtual void mouseMoveEvent( QMouseEvent* e);	///occur when mouse moves since setMouseTracking(true)
	virtual void mousePressEvent(QMouseEvent* e);	///if a mouse button left/middle/right button is pressed
	virtual void mouseReleaseEvent(QMouseEvent* e);
	virtual void onLButtonDown(QMouseEvent* e);
	virtual void onRButtonDown(QMouseEvent* e);
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void keyReleaseEvent(QKeyEvent *e);
	virtual void wheelEvent(QWheelEvent *e);
	//virtual void contextMenuEvent(QContextMenuEvent *e);

private:
	FT::FCameraType		m_iCameraType;
	bool				m_bCtrlKeyDown;
	FT::FView*			m_pFView;
	FMainDoc*			m_pMainDoc;				///accessing data
	bool				RotateDbl;
	QJSelectBox *		m_pQJselectBox;			//select box
	FSubWindow*			m_pSubWindow;			//parent sub-window, Jacky 2012-08-10
};

#endif // JCP3VIEW_H
