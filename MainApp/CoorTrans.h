#pragma once
#include "MainDoc.h"
#include "FMainWindow.h"
#include "qwidget.h"
#include "qlineedit.h"
#include "qpushbutton.h"
#include "FGroupPathPlan.h"

class OrignManualTool;

struct TransPara
{
	TransPara() {};
	TransPara(double MovX, double MovY, double MovZ, double RotX, double RotY, double RotZ) :
		Move_X(MovX), Move_Y(MovY), Move_Z(MovZ), Rotate_X(RotX), Rotate_Y(RotY), Rotate_Z(RotZ) {};

	void Reverse() { Move_X *= -1; Move_Y *= -1; Move_Z *= -1; Rotate_X *= -1; Rotate_Y *= -1; Rotate_Z *= -1; };

	double Move_X;
	double Move_Y;
	double Move_Z;
	double Rotate_X;
	double Rotate_Y;
	double Rotate_Z;
};

class CoorTrans :public QWidget
{
	Q_OBJECT
public:
	CoorTrans();
	~CoorTrans();
	vector<TransPara> GetTransRec() { return m_TransRec; };
	void SetTransRec(vector<TransPara> FileTransRec) { m_TransRec = FileTransRec; };
	void ExeTeransRec();
	void ExeTransPara(TransPara theTrans);
	void ResetModel();

	void SetActivateFromGroupPathPlan(FGroupPathPlan* paraGroupPathPlan);

	void SetMoveX_Val(double paraMoveX) { MovX->setText(QString::number(paraMoveX)); };
	void SetMoveY_Val(double paraMoveY) { MovY->setText(QString::number(paraMoveY)); };
	void SetMoveZ_Val(double paraMoveZ) { MovZ->setText(QString::number(paraMoveZ)); };
	void SetRotateX_Val(double paraRotateX) { RotX->setText(QString::number(paraRotateX)); };
	void SetRotateY_Val(double paraRotateY) { RotY->setText(QString::number(paraRotateY)); };
	void SetRotateZ_Val(double paraRotateZ) { RotZ->setText(QString::number(paraRotateZ)); };

	void SetValInGroupPathPlan();

protected:
	void closeEvent(QCloseEvent* event);

private slots:
	void onConfirm();
	void onShowOrign();
	void onManualOrign();
	void onResetLines();

	// 将质心移动至原点
	void onMassToOrign();

private:
	QLineEdit* MovX;
	QLineEdit* MovY;
	QLineEdit* MovZ;

	QLineEdit* RotX;
	QLineEdit* RotY;
	QLineEdit* RotZ;

	QLabel* LabelMovX;
	QLabel* LabelMovY;
	QLabel* LabelMovZ;

	QLabel* LabelRotX;
	QLabel* LabelRotY;
	QLabel* LabelRotZ;

	QLabel* RefGeom;

	QPushButton* Confirm;
	QPushButton* ShowOrign;
	QPushButton* OrignManual;
	QPushButton* m_pResetLines;

	MeasPt* theOrignX;
	MeasPt* theOrignY;
	MeasPt* theOrignZ;

	int IsShowOrign;
	OrignManualTool* m_pOrigTool;
	bool IsToolUsed;

	vector<TransPara> m_TransRec;

	// 将平移、三个方向的旋转分开储存
	vector<TransPara> m_TransRecSeparate;

	void TransFormIpPnt(FIpPoint& thePnt, gp_Trsf transForm);
	void TransFormMesPnt(MeasPt*, gp_Trsf transForm);

	bool IsActivateFromGroupPathPlan;

	FGroupPathPlan* m_pGroupPathPlan;

};