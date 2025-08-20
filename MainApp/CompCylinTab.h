/*
误差补偿窗口中，圆柱孔位补偿的标签页
*/

#pragma once
#include <qwidget.h>
#include "../fkernel/include/FKernelDefines.h"
#include <unordered_set>
#include <QTextEdit>
#include <gp_Pnt.hxx>

class QListWidget;
class QLabel;

class CompCylinTab : public QWidget
{
	Q_OBJECT
public:
	CompCylinTab(QWidget* parent = NULL);
	~CompCylinTab();

	QListWidget* getListWidget();

	void addSurf(FSurface* _surf);

	void setListWidgetBcgDef();

private slots:
	void onCal();

private:
	void selCylin();
	void selBasePlane();

	class CCYListWidget :public QListWidget
	{
	public:
		CCYListWidget(CompCylinTab* _tab, const int& _ind, const double& _r, const double& _g, const double& _b);
		~CCYListWidget();

		void setBcgDef();

	protected:
		void mousePressEvent(QMouseEvent* event) override;

	private:
		CompCylinTab* m_pTabWidget;
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

	};

	// 给定轴线方向的情况下拟合圆柱轴线
	gp_Pnt fitCylinAxis(std::vector<gp_Pnt> _points, gp_Vec _axisDir);

	void toolProcess();

	CCYListWidget* m_pSurfListWidget;

	CCYListWidget* m_pBasePlaneListWidget;

	QTextEdit* m_pResTextShow;

	std::unordered_set<FSurface*> m_SelSurf;

	FSurface* m_pBasePlane;

	std::vector<gp_Pnt> m_MeasPnts;

	// 选择模式
	// = 0: 未选择
	// = 1: 选择圆柱面
	// = 2: 选择基面
	int m_SelMode;

};



