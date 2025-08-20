/*	
\brief	A window class to define the ajustprobe0 window.
\author: Xuhe
\date    2017-01
*/
#ifndef F_AdjustRingProbe_H
#define F_AdjustRingProbe_H
//#pragma once
#include "../External/fkernel/Include/FKernelDefines.h"
#include <Qtgui>
#include <QMdiSubWindow>


class FAdjustRingProbe :	public QWidget/*QMdiSubWindow*/
{
	Q_OBJECT
public:
	FAdjustRingProbe(QWidget* parent=NULL);
	~FAdjustRingProbe(void);
	QLineEdit *lineedit1;
	QLineEdit *lineedit2;
	QLineEdit *lineedit3;
	QPushButton* m_pCreateBtn;
public:
	//void OnAdjustProbeStart();
	float	m_Rad;
	int		m_Number;
	double	m_height;
protected:
	//void LoadFile(const QString &fileName);	//load NC code file

private slots:
	//void	onPauseBtn();		///triggered when the Pause button is clicked
	void OnAdjustProbeStart() ;
	void OnAdjustProbeCreate();
	void OnAdjustProbeQuit() ;
protected:
	//QTextEdit*		m_pTextEdit;		///show the NC code
	//FHighlighter*	m_pHighlighter;		///highlight rules in m_pRuleEdit
};

class AdjustProbe0{
public:
	AdjustProbe0(double rad,int number,double height);
	~AdjustProbe0();
};
class NCFile0D{
public:
	NCFile0D(double oldrad, int Ijp, double high);
	~NCFile0D();
};
#endif AdjustRingProbe_H