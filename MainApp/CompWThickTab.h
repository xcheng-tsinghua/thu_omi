/*
误差补偿窗口中，壁厚补偿标签页
*/
#pragma once

#include <qwidget.h>
#include <unordered_set>
#include "../fkernel/include/FKernelDefines.h"
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <vector>
#include <TopoDS_Face.hxx>

class QListWidget;
class QTextEdit;

class CompWThickTab : public QWidget
{
	Q_OBJECT
public:
	CompWThickTab(QWidget* parent = NULL);
	~CompWThickTab();

	// _group = 1: 添加到第一组
	// _group = 2: 添加到第二组
	void addPlane(FSurface* _surf, int _group);

	void setListWidgetBcgDef();

private slots:
	void onCal();
	void onSave();

private:
	void selPlane1();
	void selPlane2();

	class CWTListWidget :public QListWidget
	{
	public:
		CWTListWidget(CompWThickTab* _tab, const int& _ind, const double& _r, const double& _g, const double& _b);
		~CWTListWidget();

		void setBcgDef();

	protected:
		void mousePressEvent(QMouseEvent* event) override;

	private:
		CompWThickTab* m_pTabWidget;
		QString getStyleSheet(double _r, double _g, double _b);

		// = 1: 第一组
		// = 2: 第二组
		int m_nInd;

		// [0, 1]
		double m_bcgR;
		double m_bcgG;
		double m_bcgB;

		// [0, 1]
		double m_bcgDefR;
		double m_bcgDefG;
		double m_bcgDefB;

		QString m_styleSheet;

	};


	//gp_Pln fitPlaneWithGivenNorm(vector<gp_Pnt> _points, gp_Vec _norm);

	CWTListWidget* m_pSelPlanesWidget1;
	CWTListWidget* m_pSelPlanesWidget2;

	QTextEdit* m_pResTextShow;

	std::unordered_set<FSurface*> m_SelPlanesList1;
	std::unordered_set<FSurface*> m_SelPlanesList2;

	// 存放补偿后的平面
	std::vector<TopoDS_Face> m_CompstePlanes1;
	std::vector<TopoDS_Face> m_CompstePlanes2;

};



