#include "CoorTrans.h"
#include "qlayout.h"
#include "occConverter.h"
#include "OrignManualTool.h"
#include <GeomConvert.hxx>
#include "MeasPt.h"
#include <BRep_Tool.hxx>

CoorTrans::CoorTrans()
{
	this->setWindowTitle(QString::fromLocal8Bit("更改全局坐标系"));
	this->setWindowFlags(Qt::WindowStaysOnTopHint);//永远在其他窗口之上
	this->setGeometry(1250, 150, 300, 400);

	IsToolUsed = false;
	m_pOrigTool = NULL;

	m_pGroupPathPlan = NULL;
	IsActivateFromGroupPathPlan = false;

	QGridLayout* MainLayout = new QGridLayout(this);

	MovX = new QLineEdit;
	MovY = new QLineEdit;
	MovZ = new QLineEdit;

	RotX = new QLineEdit;
	RotY = new QLineEdit;
	RotZ = new QLineEdit;

	MovX->setText(0);
	MovY->setText(0);
	MovZ->setText(0);
	RotX->setText(0);
	RotY->setText(0);
	RotZ->setText(0);

	theOrignX = new MeasPt(0, 0, 0, 3, 0, 0, 1, 0, 0, 10);
	theOrignY = new MeasPt(0, 0, 0, 0, 3, 0, 0, 1, 0, 10);
	theOrignZ = new MeasPt(0, 0, 0, 0, 0, 3, 0, 0, 1, 10);
	IsShowOrign = 1;
	onShowOrign();

	LabelMovX = new QLabel(QString::fromLocal8Bit("+X向移动距离"));
	LabelMovY = new QLabel(QString::fromLocal8Bit("+Y向移动距离"));
	LabelMovZ = new QLabel(QString::fromLocal8Bit("+Z向移动距离"));

	LabelRotX = new QLabel(QString::fromLocal8Bit("+X向转动角度"));
	LabelRotY = new QLabel(QString::fromLocal8Bit("+Y向转动角度"));
	LabelRotZ = new QLabel(QString::fromLocal8Bit("+Z向转动角度"));

	RefGeom = new QLabel;
	RefGeom->setStyleSheet("QLabel{""border-image:url(../icons/RefGeom.png) 4 4 4 4 stretch stretch;""}");
	RefGeom->setMinimumSize(140, 70);

	ShowOrign = new QPushButton(QString::fromLocal8Bit("显示/隐藏坐标系"));
	Confirm = new QPushButton(QString::fromLocal8Bit("确定"));
	OrignManual = new QPushButton(QString::fromLocal8Bit("使用参考几何体"));
	m_pResetLines = new QPushButton(QString::fromLocal8Bit("重置"));
	QPushButton* pMassToOrignBtn = new QPushButton(QString::fromLocal8Bit("居中"));

	MainLayout->addWidget(LabelMovX, 0, 0);
	MainLayout->addWidget(MovX, 0, 1);

	MainLayout->addWidget(LabelMovY, 1, 0);
	MainLayout->addWidget(MovY, 1, 1);

	MainLayout->addWidget(LabelMovZ, 2, 0);
	MainLayout->addWidget(MovZ, 2, 1);

	MainLayout->addWidget(LabelRotX, 3, 0);
	MainLayout->addWidget(RotX, 3, 1);

	MainLayout->addWidget(LabelRotY, 4, 0);
	MainLayout->addWidget(RotY, 4, 1);

	MainLayout->addWidget(LabelRotZ, 5, 0);
	MainLayout->addWidget(RotZ, 5, 1);

	MainLayout->addWidget(ShowOrign, 6, 0);
	MainLayout->addWidget(Confirm, 6, 1);
	MainLayout->addWidget(OrignManual, 7, 0);
	MainLayout->addWidget(RefGeom, 7, 1);
	MainLayout->addWidget(pMassToOrignBtn, 8, 0);
	MainLayout->addWidget(m_pResetLines, 8, 1);


	connect(Confirm, SIGNAL(clicked()), this, SLOT(onConfirm()));
	connect(ShowOrign, SIGNAL(clicked()), this, SLOT(onShowOrign()));
	connect(OrignManual, SIGNAL(clicked()), this, SLOT(onManualOrign()));
	connect(m_pResetLines, SIGNAL(clicked()), this, SLOT(onResetLines()));
	connect(pMassToOrignBtn, SIGNAL(clicked()), this, SLOT(onMassToOrign()));

}

CoorTrans::~CoorTrans()
{

}

void CoorTrans::ExeTeransRec()
{
	gWindow->GetMainDoc()->RemoveDispEnts(theOrignX);
	gWindow->GetMainDoc()->RemoveDispEnts(theOrignY);
	gWindow->GetMainDoc()->RemoveDispEnts(theOrignZ);

	for (int i = 0; i < m_TransRec.size(); i++)
	{
		ExeTransPara(m_TransRec[i]);
	}
}

void CoorTrans::ExeTransPara(TransPara theTrans)
{
	double MoveX = theTrans.Move_X;
	double MoveY = theTrans.Move_Y;
	double MoveZ = theTrans.Move_Z;

	double RotateX = theTrans.Rotate_X;
	double RotateY = theTrans.Rotate_Y;
	double RotateZ = theTrans.Rotate_Z;

	F3dVector tVec = F3dVector(MoveX, MoveY, MoveZ);

	//FForm MoveForm;
	//MoveForm.Translation(tVec);
	double OrigForm[4][4] = { {1,0,0,MoveX},{0,1,0,MoveY},{0,0,1,MoveZ},{0,0,0,1} };
	FForm MoveForm = FForm(OrigForm);

	FForm RotaFormX;
	RotaFormX.Rotation(RotateX, ON_3dVector(1, 0, 0), ON_3dPoint(0, 0, 0));

	FForm RotaFormY;
	RotaFormY.Rotation(RotateY, ON_3dVector(0, 1, 0), ON_3dPoint(0, 0, 0));

	FForm RotaFormZ;
	RotaFormZ.Rotation(RotateZ, ON_3dVector(0, 0, 1), ON_3dPoint(0, 0, 0));

	PFEntityList* m_pEntityList = gWindow->GetMainDoc()->GetDispEntList();

	gp_Trsf OcctMoveForm;
	OcctMoveForm.SetValues(1, 0, 0, MoveX, 0, 1, 0, MoveY, 0, 0, 1, MoveZ);

	gp_Trsf OcctRotaX;
	gp_Dir DirectionX = gp_Dir(1, 0, 0);
	gp_Pnt location = gp_Pnt(0, 0, 0);
	gp_Ax1 retation_axiX(location, DirectionX);
	OcctRotaX.SetRotation(retation_axiX, RotateX);

	gp_Trsf OcctRotaY;
	gp_Dir DirectionY = gp_Dir(0, 1, 0);
	gp_Ax1 retation_axiY(location, DirectionY);
	OcctRotaY.SetRotation(retation_axiY, RotateY);

	gp_Trsf OcctRotaZ;
	gp_Dir DirectionZ = gp_Dir(0, 0, 1);
	gp_Ax1 retation_axiZ(location, DirectionZ);
	OcctRotaZ.SetRotation(retation_axiZ, RotateZ);

	for (int i = 0; i < m_pEntityList->size(); i++)
	{
		if (/*m_pEntityList->at(i)->GetEntType() == FT_BODY || */m_pEntityList->at(i)->GetEntType() == FT_FACE)
		{
			if ((m_pEntityList->at(i) != theOrignX) && (m_pEntityList->at(i) != theOrignY) && (m_pEntityList->at(i) != theOrignZ))
			{
				m_pEntityList->at(i)->DoTransform(MoveForm);
				m_pEntityList->at(i)->DoTransform(RotaFormX);
				m_pEntityList->at(i)->DoTransform(RotaFormY);
				m_pEntityList->at(i)->DoTransform(RotaFormZ);
				m_pEntityList->at(i)->Update();
			}
		}
	}

	vector<TopoDS_Face> tmpFaces = gWindow->GetSTEPModelConverter()->GetTopoDS_FaceList();

	for (auto ita = tmpFaces.begin(); ita != tmpFaces.end(); ++ita)
	{
		BRep_Tool::Surface((*ita))->Transform(OcctMoveForm);
		BRep_Tool::Surface((*ita))->Transform(OcctRotaX);
		BRep_Tool::Surface((*ita))->Transform(OcctRotaY);
		BRep_Tool::Surface((*ita))->Transform(OcctRotaZ);
	}

	gWindow->UpdateModelView();
}

void CoorTrans::ResetModel()
{
	int nSize = m_TransRecSeparate.size();

	for (int i = 0; i < nSize; i++)
	{
		m_TransRecSeparate[nSize - 1 - i].Reverse();
		ExeTransPara(m_TransRecSeparate[nSize - 1 - i]);
	}

	m_TransRecSeparate.clear();
	m_TransRec.clear();
}

void CoorTrans::SetActivateFromGroupPathPlan(FGroupPathPlan* paraGroupPathPlan)
{
	IsActivateFromGroupPathPlan = true;

	m_pGroupPathPlan = paraGroupPathPlan;
}

void CoorTrans::onConfirm()
{
	if (IsToolUsed)
	{
		m_pOrigTool->RemoveOrign();
	}

	if (IsActivateFromGroupPathPlan)
	{
		if (RotX->text().toInt() != 0)
		{
			m_pGroupPathPlan->SetRotateLine(RotX->text());
			m_pGroupPathPlan->SetXYZ_Checked(0);
		}
		else if (RotY->text().toInt() != 0)
		{
			m_pGroupPathPlan->SetRotateLine(RotY->text());
			m_pGroupPathPlan->SetXYZ_Checked(1);
		}
		else if (RotZ->text().toInt() != 0)
		{
			m_pGroupPathPlan->SetRotateLine(RotZ->text());
			m_pGroupPathPlan->SetXYZ_Checked(2);
		}
		else
		{

		}
		
	}

	double MoveX = MovX->text().toDouble();
	double MoveY = MovY->text().toDouble();
	double MoveZ = MovZ->text().toDouble();

	double RotateX = ((RotX->text().toDouble()) * PI) / 180.0;
	double RotateY = ((RotY->text().toDouble()) * PI) / 180.0;
	double RotateZ = ((RotZ->text().toDouble()) * PI) / 180.0;

	F3dVector tVec = F3dVector(MoveX, MoveY, MoveZ);

	double OrigForm[4][4] = { {1,0,0,MoveX},{0,1,0,MoveY},{0,0,1,MoveZ},{0,0,0,1} };
	FForm MoveForm = FForm(OrigForm);

	FForm RotaFormX;
	RotaFormX.Rotation(RotateX, ON_3dVector(1, 0, 0), ON_3dPoint(0, 0, 0));

	FForm RotaFormY;
	RotaFormY.Rotation(RotateY, ON_3dVector(0, 1, 0), ON_3dPoint(0, 0, 0));

	FForm RotaFormZ;
	RotaFormZ.Rotation(RotateZ, ON_3dVector(0, 0, 1), ON_3dPoint(0, 0, 0));

	PFEntityList* m_pEntityList = gWindow->GetMainDoc()->GetDispEntList();

	gp_Trsf OcctMoveForm;
	OcctMoveForm.SetValues(1, 0, 0, MoveX, 0, 1, 0, MoveY, 0, 0, 1, MoveZ);

	gp_Trsf OcctRotaX;
	gp_Dir DirectionX = gp_Dir(1, 0, 0);
	gp_Pnt location = gp_Pnt(0, 0, 0);
	gp_Ax1 retation_axiX(location, DirectionX);
	OcctRotaX.SetRotation(retation_axiX, RotateX);
	
	gp_Trsf OcctRotaY;
	gp_Dir DirectionY = gp_Dir(0, 1, 0);
	gp_Ax1 retation_axiY(location, DirectionY);
	OcctRotaY.SetRotation(retation_axiY, RotateY);

	gp_Trsf OcctRotaZ;
	gp_Dir DirectionZ = gp_Dir(0, 0, 1);
	gp_Ax1 retation_axiZ(location, DirectionZ);
	OcctRotaZ.SetRotation(retation_axiZ, RotateZ);

	for (int i = 0; i < m_pEntityList->size(); i++)
	{
		if (/*m_pEntityList->at(i)->GetEntType() == FT_BODY || */m_pEntityList->at(i)->GetEntType() == FT_FACE)
		{
			if ((m_pEntityList->at(i) != theOrignX) && (m_pEntityList->at(i) != theOrignY) && (m_pEntityList->at(i) != theOrignZ))
			{
				m_pEntityList->at(i)->DoTransform(MoveForm);
				m_pEntityList->at(i)->DoTransform(RotaFormX);
				m_pEntityList->at(i)->DoTransform(RotaFormY);
				m_pEntityList->at(i)->DoTransform(RotaFormZ);
				m_pEntityList->at(i)->Update();
			}
		}
	}

	vector<TopoDS_Face> tmpFaces = gWindow->GetSTEPModelConverter()->GetTopoDS_FaceList();

	for (auto ita = tmpFaces.begin(); ita != tmpFaces.end(); ++ita)
	{
		BRep_Tool::Surface((*ita))->Transform(OcctMoveForm);
		BRep_Tool::Surface((*ita))->Transform(OcctRotaX);
		BRep_Tool::Surface((*ita))->Transform(OcctRotaY);
		BRep_Tool::Surface((*ita))->Transform(OcctRotaZ);
	}

	gWindow->UpdateModelView();

	m_TransRec.push_back(TransPara(MoveX, MoveY, MoveZ, RotateX, RotateY, RotateZ));

	m_TransRecSeparate.push_back(TransPara(MoveX, MoveY, MoveZ, 0, 0, 0));
	m_TransRecSeparate.push_back(TransPara(0, 0, 0, RotateX, 0, 0));
	m_TransRecSeparate.push_back(TransPara(0, 0, 0, 0, RotateY, 0));
	m_TransRecSeparate.push_back(TransPara(0, 0, 0, 0, 0, RotateZ));
}

void CoorTrans::onShowOrign()
{
	if (IsShowOrign % 2)
	{
		gWindow->GetMainDoc()->AddDispEnt(theOrignX);
		gWindow->GetMainDoc()->AddDispEnt(theOrignY);
		gWindow->GetMainDoc()->AddDispEnt(theOrignZ);
	}
	else
	{
		gWindow->GetMainDoc()->RemoveDispEnts(theOrignX);
		gWindow->GetMainDoc()->RemoveDispEnts(theOrignY);
		gWindow->GetMainDoc()->RemoveDispEnts(theOrignZ);
	}
	

	IsShowOrign += 1;

	gWindow->UpdateModelView();
}

void CoorTrans::onManualOrign()
{
	if (IsToolUsed)
	{
		m_pOrigTool->RemoveOrign();
		gWindow->UpdateModelView();
	}

	IsToolUsed = true;

	FFeatureTool* m_pCurTool = gWindow->GetCurTool();
	if (gWindow->GetCurTool())
	{
		gWindow->GetMainDoc()->PopTool();
		m_pCurTool = NULL;
	}
	m_pCurTool = new OrignManualTool(this);
	gWindow->GetMainDoc()->PushTool(m_pCurTool);
	((OrignManualTool*)m_pCurTool)->SetSelEntType(FT_FACE);

	m_pOrigTool = ((OrignManualTool*)m_pCurTool);
}

void CoorTrans::onResetLines()
{
	MovX->setText("0");
	MovY->setText("0");
	MovZ->setText("0");

	RotX->setText("0");
	RotY->setText("0");
	RotZ->setText("0");


	//if (IsToolUsed)
	//{
	//	m_pOrigTool->RemoveOrign();
	//}

	//double MoveX = MovX->text().toDouble();
	//double MoveY = MovY->text().toDouble();
	//double MoveZ = MovZ->text().toDouble();

	//double RotateX = ((RotX->text().toDouble()) * PI) / 180.0;
	//double RotateY = ((RotY->text().toDouble()) * PI) / 180.0;
	//double RotateZ = ((RotZ->text().toDouble()) * PI) / 180.0;

	//gp_Trsf OcctMoveForm;
	//OcctMoveForm.SetValues(1, 0, 0, MoveX, 0, 1, 0, MoveY, 0, 0, 1, MoveZ);

	//gp_Trsf OcctRotaX;
	//gp_Dir DirectionX = gp_Dir(1, 0, 0);
	//gp_Pnt location = gp_Pnt(0, 0, 0);
	//gp_Ax1 retation_axiX(location, DirectionX);
	//OcctRotaX.SetRotation(retation_axiX, RotateX);

	//gp_Trsf OcctRotaY;
	//gp_Dir DirectionY = gp_Dir(0, 1, 0);
	//gp_Ax1 retation_axiY(location, DirectionY);
	//OcctRotaY.SetRotation(retation_axiY, RotateY);

	//gp_Trsf OcctRotaZ;
	//gp_Dir DirectionZ = gp_Dir(0, 0, 1);
	//gp_Ax1 retation_axiZ(location, DirectionZ);
	//OcctRotaZ.SetRotation(retation_axiZ, RotateZ);

	//// 转移理论测点坐标
	//for (int j = 0; j < gWindow->GetMainDoc()->GetIpPntList().size(); j++)
	//{
	//	//先删除所有已显示的测点
	//	for (int k = 0; k < gWindow->GetMainDoc()->getPntsShowList().size(); k++)
	//	{
	//		gWindow->GetMainDoc()->RemoveDispEnts(gWindow->GetMainDoc()->getPntsShowList()[k]);
	//	}

	//	//删除m_pArrPntList和Doc中理论测点表中的对应测点
	//	vector<MeasPt*> tmpArrList = gWindow->GetMainDoc()->getPntsShowList();
	//	FIpPntList tmpIpLists = gWindow->GetMainDoc()->GetIpPntList();

	//	FIpPntList tmpIpList = tmpIpLists;

	//	for (int k = 0; k < tmpIpList.size(); k++)
	//	{
	//		// 转换显示点和ip点，先平移，再旋转XYZ
	//		TransFormIpPnt(tmpIpList[k], OcctMoveForm);
	//		TransFormIpPnt(tmpIpList[k], OcctRotaX);
	//		TransFormIpPnt(tmpIpList[k], OcctRotaY);
	//		TransFormIpPnt(tmpIpList[k], OcctRotaZ);

	//		TransFormMesPnt(tmpArrList[k], OcctMoveForm);
	//		TransFormMesPnt(tmpArrList[k], OcctRotaX);
	//		TransFormMesPnt(tmpArrList[k], OcctRotaY);
	//		TransFormMesPnt(tmpArrList[k], OcctRotaZ);

	//	}
	//	gWindow->GetMainDoc()->getPntsShowList() = tmpArrList;
	//	gWindow->GetMainDoc()->ClearIpPntList();
	//	gWindow->GetMainDoc()->SetIpPntList(tmpIpList);

	//	gWindow->UpdateModelView();
	//}
	//gWindow->UpdateModelView();



}

void CoorTrans::onMassToOrign()
{
	// 计算质心
	ON_3dPoint corner1, corner2;
	gWindow->GetSTEPModelConverter()->GetMaxMinCorner(corner1, corner2);

	double xMove = (corner1.x + corner2.x) / 2.0;
	double yMove = (corner1.y + corner2.y) / 2.0;
	double zMove = (corner1.z + corner2.z) / 2.0;

	MovX->setText(QString::number(-xMove));
	MovY->setText(QString::number(-yMove));
	MovZ->setText(QString::number(-zMove));

	RotX->setText("0");
	RotY->setText("0");
	RotZ->setText("0");

	onConfirm();

	MovX->setText("0");
	MovY->setText("0");
	MovZ->setText("0");

}

void CoorTrans::SetValInGroupPathPlan()
{
	if (IsActivateFromGroupPathPlan)
	{
		m_pGroupPathPlan->SetTransXLine(MovX->text());
		m_pGroupPathPlan->SetTransYLine(MovY->text());
		m_pGroupPathPlan->SetTransZLine(MovZ->text());
	}

}

void CoorTrans::closeEvent(QCloseEvent* event)
{
	gWindow->GetMainDoc()->RemoveDispEnts(theOrignX);
	gWindow->GetMainDoc()->RemoveDispEnts(theOrignY);
	gWindow->GetMainDoc()->RemoveDispEnts(theOrignZ);

	if (IsToolUsed)
	{
		m_pOrigTool->RemoveOrign();
		gWindow->ResetToDefaultCountenance();

		gWindow->GetMainDoc()->PopTool();
	}

	IsActivateFromGroupPathPlan = false;
	m_pGroupPathPlan = NULL;

	QWidget::closeEvent(event);

}

void CoorTrans::TransFormIpPnt(FIpPoint& thePnt, gp_Trsf transForm)
{
	gp_Pnt theLoc(0, 0, 0);
	gp_Pnt theDir(0, 0, 0);

	theLoc.SetX(thePnt.x);
	theLoc.SetY(thePnt.y);
	theLoc.SetZ(thePnt.z);

	theDir.SetX(thePnt.i);
	theDir.SetY(thePnt.j);
	theDir.SetZ(thePnt.k);

	theLoc.Transform(transForm);
	theDir.Transform(transForm);

	thePnt.x = theLoc.X();
	thePnt.y = theLoc.Y();
	thePnt.z = theLoc.Z();
	thePnt.i = theLoc.X();
	thePnt.j = theLoc.Y();
	thePnt.k = theLoc.Z();

}

void CoorTrans::TransFormMesPnt(MeasPt* thePnt, gp_Trsf transForm)
{
	gp_Pnt theLoc(0, 0, 0);
	gp_Pnt theDir(0, 0, 0);
	
	theLoc.SetX(thePnt->getm_x());
	theLoc.SetY(thePnt->getm_y());
	theLoc.SetZ(thePnt->getm_z());

	theDir.SetX(thePnt->getm_i());
	theDir.SetY(thePnt->getm_j());
	theDir.SetZ(thePnt->getm_k());

	theLoc.Transform(transForm);
	theDir.Transform(transForm);

	thePnt->getm_x() = theLoc.X();
	thePnt->getm_y() = theLoc.Y();
	thePnt->getm_z() = theLoc.Z();
	thePnt->getm_i() = theLoc.X();
	thePnt->getm_j() = theLoc.Y();
	thePnt->getm_k() = theLoc.Z();


}
