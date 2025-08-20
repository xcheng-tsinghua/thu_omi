/*
Îó²î²¹³¥´°¿Ú
*/

#pragma once
#include <qtabwidget.h>

class CompSurfTab;
class CompCylinTab;
class CompWThickTab;

class ErrCompsteDlg :public QTabWidget
{
	Q_OBJECT
public:
	ErrCompsteDlg(QWidget* parent = NULL);
	~ErrCompsteDlg(void);

private:
	CompSurfTab* m_pCompSurfTab;
	CompCylinTab* m_pCompCylinTab;
	CompWThickTab* m_pCompWThickTab;

};


