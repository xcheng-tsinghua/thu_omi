#pragma once

#include"qwidget.h"
#include<qpainter.h>
#include<vector>
#include<iostream>
#include"AccuEvaluWnd.h"

using namespace std;

class InsFeaUnitPainter_Rep : public QWidget
{
	//Q_OBJECT
public:
	InsFeaUnitPainter_Rep(EvaluData Evalu, QWidget* parent = 0);
	~InsFeaUnitPainter_Rep();

	void paintEvent(QPaintEvent*);
	double GetHeight() { return Height; };

private:
	QPointF errorlist[3000];

	
	EvaluData p_Evalu;
	int Height;
	int Marginal;
	int InsFeaNameH;
	int RealNameH;
	int OvewHigh;
	int PntCompHigh;
	int NbRow;
	int Width;
	int UserInfoW;
	int TypeH;
	int NbPntperRow;
	int PntCompWordH;
};