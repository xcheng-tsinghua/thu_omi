// Ŀǰʹ�ô˹��ܣ����ܼ���Բ�����϶���㣬Բ����������Z��ƽ�У�ֻ��XY���Ȳ�������
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
	// ��ʼ�����ڿؼ�
	void InitWIdgets();

	QComboBox* m_pModeSel;
	QLineEdit* m_pOrignX;
	QLineEdit* m_pOrignY;
	QLineEdit* m_pOrignZ;

	int m_CalMode;

	vector<gp_Pnt> m_PlanePntList;
	vector<gp_Pnt> m_CylinderPntList;











};



