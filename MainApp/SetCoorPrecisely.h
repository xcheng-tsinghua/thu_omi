// 目前使用此功能，仅能计算圆柱面上多层测点，圆柱面轴线与Z轴平行，只定XY，先补偿数据
#pragma once
#include <qwidget.h>
#include <vector>

using namespace std;

class QComboBox;
class QLineEdit;
class gp_Pnt;

class SetCoorPrecisely :public QWidget
{
	Q_OBJECT
public:
	SetCoorPrecisely();
	~SetCoorPrecisely();




protected slots:
	void onSelDataFile1();
	void onSelDataFile2();
	void onCalOrign();
	void onGetModel(int);


private:
	// 初始化窗口控件
	void InitWIdgets();

	QComboBox* m_pModeSel;
	QLineEdit* m_pOrignX;
	QLineEdit* m_pOrignY;
	QLineEdit* m_pOrignZ;

	int m_CalMode;

	vector<gp_Pnt> m_PlanePntList;
	vector<gp_Pnt> m_CylinderPntList;











};



