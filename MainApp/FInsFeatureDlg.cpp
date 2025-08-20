#include "FInsFeatureDlg.h"
#include "FMainWindow.h"
#include "FInsFeaTreeWnd.h"
#include "AccuEvaluWnd.h"
#include "Tools/InsFeaTool.h"
#include "../External/fkernel/Include/DataStructure/FBody.h"
#include "occConverter.h"
#include "EntityHighlight.h"
#include "np.h"
#include <functional>
#include <Quantity_Color.hxx>

extern int gInsFeaIndex;
extern clock_t gTStart, gTEnd;

FInsFeatureDlg::FInsFeatureDlg(QWidget* parent)
{
	m_pCurTool = gWindow->GetCurTool();

	this->setWindowIcon(QIcon("../icons/NewFeat.png"));
	this->setFixedSize(690, 485);
	this->setWindowTitle(QString::fromLocal8Bit("生成检测特征"));
	this->setWindowFlags(Qt::WindowStaysOnTopHint);
	this->move(1200, 100);

	// 设置右键菜单
	m_pPopMenu = QSharedPointer<QMenu>(new QMenu());
	m_pActionDelete = new QAction(QString::fromLocal8Bit("移除型面"), this);
	m_pDelAllSurf = new QAction(QString::fromLocal8Bit("移除所有型面"), this);

	m_pActionDelete->setIcon(QIcon("../icons/Delete.png"));
	m_pDelAllSurf->setIcon(QIcon("../icons/DelAllSurf.png"));

	ClickedFaceListWidget = 0;

	SelType = UNKNOW;
	DirSelState = 0;

	ToleranceValue = 0;
	UpDeviationValue = 0;
	DownDeviationValue = 0;

	// 初始化输入参数控件
	pToleranceValueEditLine = new QLineEdit(this);
	pUpDeviationEditLine = new QLineEdit(this);
	pDownDeviationEditLine = new QLineEdit(this);

	pToleranceValueEditLine->move(10000, 10000);
	pUpDeviationEditLine->move(10000, 10000);
	pDownDeviationEditLine->move(10000, 10000);

	pToleranceValueEditLine->setText(QString::number(0.05));
	pUpDeviationEditLine->setText(QString::number(0.05));
	pDownDeviationEditLine->setText(QString::number(0.05));

	pToleranceValueLabel = new QLabel(QString::fromLocal8Bit("公差要求/mm"));
	pUpDeviationLabel = new QLabel(QString::fromLocal8Bit("上偏差/mm：+"));
	pDownDeviationLabel = new QLabel(QString::fromLocal8Bit("下偏差/mm：-"));

	/////////////////////////////////////////主布局
	QVBoxLayout* MainVBoxLayout = new QVBoxLayout(this);

	/////////////////////////////////////////左上角选择检测类型子窗口
	QGroupBox* ToleranceTypeGroup = new QGroupBox(QString::fromLocal8Bit("检测类型"));
	ToleranceTypeGroup->setFixedSize(334, 270);

	//检测要求布局
	InsRequestGridLayout = new QGridLayout;

	//检测要求类型按钮
	QRadioButton* pParallelismBox = new QRadioButton(QString::fromLocal8Bit("平行度"));
	QRadioButton* pRoundnessBox = new QRadioButton(QString::fromLocal8Bit("圆度"));
	QRadioButton* pDistanceBox = new QRadioButton(QString::fromLocal8Bit("距离"));
	QRadioButton* pVerticalityBox = new QRadioButton(QString::fromLocal8Bit("垂直度"));
	QRadioButton* pDiameterBox = new QRadioButton(QString::fromLocal8Bit("直径"));
	QRadioButton* pAxialityBox = new QRadioButton(QString::fromLocal8Bit("同轴度"));
	QRadioButton* pFlatnessBox = new QRadioButton(QString::fromLocal8Bit("平面度"));
	QRadioButton* pProfileBox = new QRadioButton(QString::fromLocal8Bit("轮廓度"));
	QRadioButton* pPositionBox = new QRadioButton(QString::fromLocal8Bit("位置度"));

	//检测要求距离方向类型按钮
	pDis_Stra = new QRadioButton(QString::fromLocal8Bit("直线距离"));
	pDis_X = new QRadioButton(QString::fromLocal8Bit("X向距离"));
	pDis_Y = new QRadioButton(QString::fromLocal8Bit("Y向距离"));
	pDis_Z = new QRadioButton(QString::fromLocal8Bit("Z向距离"));

	InsRequestGridLayout->addWidget(pParallelismBox, 0, 0);
	InsRequestGridLayout->addWidget(pRoundnessBox, 1, 0);
	InsRequestGridLayout->addWidget(pDistanceBox, 2, 0);
	InsRequestGridLayout->addWidget(pVerticalityBox, 3, 0);
	InsRequestGridLayout->addWidget(pDiameterBox, 4, 0);
	InsRequestGridLayout->addWidget(pAxialityBox, 5, 0);
	InsRequestGridLayout->addWidget(pFlatnessBox, 6, 0);
	InsRequestGridLayout->addWidget(pProfileBox, 7, 0);
	InsRequestGridLayout->addWidget(pPositionBox, 8, 0);

	QButtonGroup* m_pButtonGroup = new QButtonGroup;
	m_pButtonGroup->addButton(pParallelismBox);
	m_pButtonGroup->addButton(pRoundnessBox);
	m_pButtonGroup->addButton(pDistanceBox);
	m_pButtonGroup->addButton(pVerticalityBox);
	m_pButtonGroup->addButton(pDiameterBox);
	m_pButtonGroup->addButton(pAxialityBox);
	m_pButtonGroup->addButton(pFlatnessBox);
	m_pButtonGroup->addButton(pProfileBox);
	m_pButtonGroup->addButton(pPositionBox);

	QButtonGroup* m_pDirSelGroup = new QButtonGroup;
	m_pDirSelGroup->addButton(pDis_Stra);
	m_pDirSelGroup->addButton(pDis_X);
	m_pDirSelGroup->addButton(pDis_Y);
	m_pDirSelGroup->addButton(pDis_Z);

	ToleranceTypeGroup->setLayout(InsRequestGridLayout);

	/////////////////////////////////////////左下角输入检测参数子窗口
	InsParameterGroup = new QGroupBox(QString::fromLocal8Bit("检测参数"));
	InsParameterGroup->setFixedHeight(150);
	InsParameterGridLayout = new QGridLayout;/////////////检测参数列表布局

	InsParameterGroup->setLayout(InsParameterGridLayout);

	/////////////////////////////////////////右侧选择型面列表子窗口
	QGroupBox* FaceListExclusiveGroup = new QGroupBox(QString::fromLocal8Bit("型面列表"));
	FaceListExclusiveGroup->setFixedWidth(200 / 0.618);

	//型面列表布局
	FaceListGridLayout = new QGridLayout;

	//型面列表
	FaceList1 = new IFDListWidget(this, 1, 1, 0, 0);
	FaceList1->setFrameStyle(QFrame::Panel | QFrame::Raised);

	FaceList2 = new IFDListWidget(this, 2, 0, 0, 1);//////////////////型面列表
	FaceList2->setFrameStyle(QFrame::Panel | QFrame::Raised);

	FaceListExclusiveGroup->setLayout(FaceListGridLayout);

	//左侧窗口布局
	QVBoxLayout* LeftVBoxLayout = new QVBoxLayout;
	LeftVBoxLayout->addWidget(ToleranceTypeGroup);
	LeftVBoxLayout->addWidget(InsParameterGroup);

	//上侧窗口布局
	QHBoxLayout* UpLayout = new QHBoxLayout;
	UpLayout->addLayout(LeftVBoxLayout);
	UpLayout->addWidget(FaceListExclusiveGroup);

	/////////////////////////////////////////最下方生成检测特征和取消按钮
	QPushButton* pGenerateInsFeature = new QPushButton(QString::fromLocal8Bit("生成检测特征"), this);
	QPushButton* pExit = new QPushButton(QString::fromLocal8Bit("退出"), this);

	QHBoxLayout* DownButtonLayout = new QHBoxLayout;

	// 向右对齐
	DownButtonLayout->addStretch();

	DownButtonLayout->addWidget(pGenerateInsFeature,0);
	DownButtonLayout->addWidget(pExit, 0);

	/////////////////////////////////////////将控件添加至主布局
	MainVBoxLayout->addLayout(UpLayout);
	MainVBoxLayout->addLayout(DownButtonLayout);

	// 功能函数
	connect(FaceList1, SIGNAL(currentRowChanged(int)), this, SLOT(ChooseTargetFace(int)));
	connect(m_pButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));
	connect(m_pDirSelGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onDirSelButtonClicked(QAbstractButton*)));
	connect(pGenerateInsFeature, SIGNAL(clicked()), this, SLOT(GenerateInsFeature()));
	connect(pExit, SIGNAL(clicked()), this, SLOT(ExitGenerateInsFeature()));

	// 三个右键菜单函数
	connect(m_pDelAllSurf, SIGNAL(triggered()), this, SLOT(onRemoveAllSurf()));
	connect(m_pActionDelete, SIGNAL(triggered()),this, SLOT(onDeleteFace()));

}

FInsFeatureDlg::~FInsFeatureDlg()
{

}

void FInsFeatureDlg::ChooseTargetFace(int i)
{

}

void FInsFeatureDlg::showInsParameterList()
{
	hideValueInputLabelLine();

	////////////////////////初始化布局
	switch (SelType)
	{
		case PARALLELISM:
		{
			pToleranceValueLabel->setText(QString::fromLocal8Bit("平行度公差值/mm"));

			showToleranceValueInputLabelLine();

			break;
		}
		case ROUNDNESS:
		{
			pToleranceValueLabel->setText(QString::fromLocal8Bit("圆度公差值/mm"));

			showToleranceValueInputLabelLine();

			break;
		}
		case DISTANCE:
		{
			pToleranceValueLabel->setText(QString::fromLocal8Bit("距离标准值/mm"));

			showDimensionValueInputLabelLine();

			break;
		}
		case VERTICALITY:
		{
			pToleranceValueLabel->setText(QString::fromLocal8Bit("垂直度公差值/mm"));

			showToleranceValueInputLabelLine();

			break;
		}
		case DIAMETER:
		{
			pToleranceValueLabel->setText(QString::fromLocal8Bit("直径标准值/mm"));

			showDimensionValueInputLabelLine();

			break;
		}
		case AUXIALITY:
		{
			pToleranceValueLabel->setText(QString::fromLocal8Bit("同轴度公差值/mm"));

			showToleranceValueInputLabelLine();

			break;
		}
		case FLATNESS:
		{
			pToleranceValueLabel->setText(QString::fromLocal8Bit("平面度公差值/mm"));

			showToleranceValueInputLabelLine();

			break;
		}

		case PROFILE:
		{
			pToleranceValueLabel->setText(QString::fromLocal8Bit("轮廓度公差值/mm"));

			showToleranceValueInputLabelLine();

			break;


		}

		case POSITION:
		{
			pToleranceValueLabel->setText(QString::fromLocal8Bit("位置度公差值/mm"));

			showToleranceValueInputLabelLine();

			break;

		}

		default:
			break;
	}

	InsParameterGridLayout->update();

}

void FInsFeatureDlg::GenerateInsFeature()
{
	ToleranceValue = pToleranceValueEditLine->text().toDouble();
	UpDeviationValue = pUpDeviationEditLine->text().toDouble();
	DownDeviationValue = pDownDeviationEditLine->text().toDouble();

	vector<FSurface*> surfListVec1 = vector<FSurface*>(m_HashSurfList1.begin(), m_HashSurfList1.end());
	vector<FSurface*> surfListVec2 = vector<FSurface*>(m_HashSurfList2.begin(), m_HashSurfList2.end());
	FInsFeature* pInsFeature = new FInsFeature(surfListVec1, surfListVec2, SelType, ToleranceValue, UpDeviationValue, DownDeviationValue, DirSelState, gInsFeaIndex);

	initTree_by_insfea(pInsFeature, static_cast<FInsFeaTreeWnd*>(gWindow->GetSubWnd()[6])->GetTree());

	initTree_by_insfea(pInsFeature, static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->GetTree());

	gWindow->GetMainDoc()->GetInsFeaList().push_back(pInsFeature);
	
	gInsFeaIndex += 1;
}

void FInsFeatureDlg::ExitGenerateInsFeature()
{
	if (m_pCurTool)
	{
		gWindow->GetMainDoc()->PopTool();
		m_pCurTool = NULL;

		m_pCurTool = new EntityHighlight();
		gWindow->GetMainDoc()->PushTool(m_pCurTool);
		((EntityHighlight*)m_pCurTool)->SetSelEntType(FT_FACE);
	}

	close();

	//退出窗口后将所有面颜色置为默认
	gWindow->ResetToDefaultCountenance();

	gWindow->UpdateModelView();
	//退出后移除型面列表所有内容
	onRemoveAllSurf();

}

void FInsFeatureDlg::onDeleteFace()
{
	QListWidget* pListWidget = NULL;
	unordered_set<FSurface*>* pHashList;

	if (ClickedFaceListWidget == 1)
	{
		pListWidget = FaceList1;
		pHashList = &m_HashSurfList1;

	}
	else if (ClickedFaceListWidget == 2)
	{
		pListWidget = FaceList2;
		pHashList = &m_HashSurfList2;

	}
	else
	{
		return;
	}

	// 获取曲面
	int row = pListWidget->currentRow();
	QListWidgetItem* cItem = pListWidget->item(row);
	FSurface* cSurf = (FSurface*)cItem->data(Qt::UserRole).value<void*>();

	// 从树组件中删除该项目
	pListWidget->takeItem(row);
	delete cItem;

	// 恢复该曲面颜色
	gWindow->GetOccConverter()->setSurfColorDefault(cSurf);

	// 从哈希表中删除该项目
	pHashList->erase(cSurf);

}

void FInsFeatureDlg::onAIS_FaceSel()
{
	//先恢复曲面颜色
	//gWindow->ResetToDefaultCountenance();

	gWindow->UpdateModelView();
	if (m_pCurTool)
	{
		gWindow->GetMainDoc()->PopTool();
		m_pCurTool = NULL;
	}
	m_pCurTool = new InsFeaTool(this, gWindow->GetMainDoc());
	gWindow->GetMainDoc()->PushTool(m_pCurTool);
	((InsFeaTool*)m_pCurTool)->SetSelEntType(FT_FACE);

}

void FInsFeatureDlg::onRemoveAllSurf()
{
	QListWidget* pListWidget = NULL;
	unordered_set<FSurface*>* pHashList;

	if (ClickedFaceListWidget == 1)
	{
		pListWidget = FaceList1;
		pHashList = &m_HashSurfList1;

	}
	else if (ClickedFaceListWidget == 2)
	{
		pListWidget = FaceList2;
		pHashList = &m_HashSurfList2;

	}
	else
	{
		return;
	}

	// 恢复曲面颜色
	for (auto ita = pHashList->begin(); ita != pHashList->end(); ++ita)
	{
		gWindow->GetOccConverter()->setSurfColorDefault(*ita);
	}

	// 清空对应哈希表及树项目
	pHashList->clear();
	pListWidget->clear();

	// 刷新模型窗口显示
	gWindow->UpdateModelView();

}

void FInsFeatureDlg::onButtonClicked(QAbstractButton* button)
{
	gWindow->GetMainDoc()->PopTool();
	this->setListWidgetBcgDef();
	// 清空两个列表的显示内容
	m_HashSurfList1.clear();
	m_HashSurfList2.clear();
	FaceList1->clear();
	FaceList2->clear();

	FaceListGridLayout->removeWidget(FaceList1);
	FaceListGridLayout->removeWidget(FaceList2);
	FaceList2->move(9999, 9999);
	
	FaceListGridLayout->addWidget(FaceList1);
	gWindow->ResetToDefaultCountenance();
	gWindow->UpdateModelView();

	if (button->text() == QString::fromLocal8Bit("平行度"))
	{
		//QMessageBox::information(0, QString::fromLocal8Bit("温馨提示"), QString::fromLocal8Bit("如果需要计算两圆柱轴线之间的平行度，请交错选取分别隶属于它们的半圆柱面。"), "OK");
		SelType = PARALLELISM;

		DirSelState = 0;

		hideXYZSel();

		FaceListGridLayout->addWidget(FaceList2);
	}
	else if (button->text() == QString::fromLocal8Bit("圆度"))
	{
		SelType = ROUNDNESS;

		DirSelState = 0;

		hideXYZSel();
	}
	else if (button->text() == QString::fromLocal8Bit("距离"))
	{
		SelType = DISTANCE;

		pDis_Stra->setChecked(true);
		DirSelState = 0;

		showXYZSel();

		FaceListGridLayout->addWidget(FaceList2);

	}
	else if (button->text() == QString::fromLocal8Bit("垂直度"))
	{
		SelType = VERTICALITY;

		DirSelState = 0;

		hideXYZSel();

		FaceListGridLayout->addWidget(FaceList2);
	}
	else if (button->text() == QString::fromLocal8Bit("直径"))
	{
		SelType = DIAMETER;

		DirSelState = 0;

		hideXYZSel();
	}
	else if (button->text() == QString::fromLocal8Bit("同轴度"))
	{
		SelType = AUXIALITY;

		DirSelState = 0;

		hideXYZSel();

		FaceListGridLayout->addWidget(FaceList2);

	}
	else if (button->text() == QString::fromLocal8Bit("平面度"))
	{
		SelType = FLATNESS;

		DirSelState = 0;

		hideXYZSel();
	}

	else if (button->text() == QString::fromLocal8Bit("轮廓度"))
	{
		SelType = PROFILE;

		DirSelState = 0;

		hideXYZSel();
	}

	else if (button->text() == QString::fromLocal8Bit("位置度"))
	{
		SelType = POSITION;

		DirSelState = 0;

		hideXYZSel();
	}

	else
	{
		QMessageBox::warning(0, QString::fromLocal8Bit("发生错误"),
			QString::fromLocal8Bit("系统错误"));
	}

	showInsParameterList();

}

void FInsFeatureDlg::onDirSelButtonClicked(QAbstractButton* button)
{
	if (button->text() == QString::fromLocal8Bit("直线距离"))
	{
		DirSelState = 0;
	}
	else if (button->text() == QString::fromLocal8Bit("X向距离"))
	{
		DirSelState = 1;
	}
	else if (button->text() == QString::fromLocal8Bit("Y向距离"))
	{
		DirSelState = 2;
	}
	else if (button->text() == QString::fromLocal8Bit("Z向距离"))
	{
		DirSelState = 3;
	}
	else
	{
		QMessageBox::warning(0, QString::fromLocal8Bit("发生错误"),
			QString::fromLocal8Bit("系统错误"));
	}
}

void FInsFeatureDlg::setListWidgetBcgDef()
{
	FaceList1->setBcgDef();
	FaceList2->setBcgDef();
}

void FInsFeatureDlg::faceSelG1()
{
	ClickedFaceListWidget = 1;
	onAIS_FaceSel();

}

void FInsFeatureDlg::faceSelG2()
{
	ClickedFaceListWidget = 2;
	onAIS_FaceSel();

}

void FInsFeatureDlg::closeEvent(QCloseEvent* event)
{
	//退出工具并使用默认工具
	gWindow->GetMainDoc()->PopTool();
	if (m_pCurTool)
	{
		m_pCurTool = NULL;
	}
	m_pCurTool = new EntityHighlight();
	gWindow->GetMainDoc()->PushTool(m_pCurTool);
	((EntityHighlight*)m_pCurTool)->SetSelEntType(FT_FACE);

	//退出窗口后将所有面颜色置为默认
	gWindow->ResetToDefaultCountenance();

	gWindow->UpdateModelView();
	//退出后移除型面列表所有内容
	ClickedFaceListWidget = 1;
	onRemoveAllSurf();

	ClickedFaceListWidget = 2;
	onRemoveAllSurf();

}

void FInsFeatureDlg::hideXYZSel()
{
	InsRequestGridLayout->removeWidget(pDis_Stra);
	InsRequestGridLayout->removeWidget(pDis_X);
	InsRequestGridLayout->removeWidget(pDis_Y);
	InsRequestGridLayout->removeWidget(pDis_Z);
	pDis_Stra->move(9999, 9999);
	pDis_X->move(9999, 9999);
	pDis_Y->move(9999, 9999);
	pDis_Z->move(9999, 9999);

	FaceListGridLayout->removeWidget(FaceList2);
}

void FInsFeatureDlg::showXYZSel()
{
	InsRequestGridLayout->addWidget(pDis_Stra, 0, 1);
	InsRequestGridLayout->addWidget(pDis_X, 1, 1);
	InsRequestGridLayout->addWidget(pDis_Y, 2, 1);
	InsRequestGridLayout->addWidget(pDis_Z, 3, 1);

	FaceListGridLayout->removeWidget(FaceList2);

}

void FInsFeatureDlg::hideValueInputLabelLine()
{
	InsParameterGridLayout->removeWidget(pToleranceValueLabel);
	InsParameterGridLayout->removeWidget(pUpDeviationLabel);
	InsParameterGridLayout->removeWidget(pDownDeviationLabel);

	InsParameterGridLayout->removeWidget(pUpDeviationEditLine);
	InsParameterGridLayout->removeWidget(pDownDeviationEditLine);
	InsParameterGridLayout->removeWidget(pToleranceValueEditLine);
}

void FInsFeatureDlg::showDimensionValueInputLabelLine()
{
	InsParameterGridLayout->addWidget(pToleranceValueLabel, 0, 0);
	InsParameterGridLayout->addWidget(pToleranceValueEditLine, 0, 1);
	InsParameterGridLayout->addWidget(pUpDeviationLabel, 1, 0);
	InsParameterGridLayout->addWidget(pUpDeviationEditLine, 1, 1);
	InsParameterGridLayout->addWidget(pDownDeviationLabel, 2, 0);
	InsParameterGridLayout->addWidget(pDownDeviationEditLine, 2, 1);
}

void FInsFeatureDlg::showToleranceValueInputLabelLine()
{
	InsParameterGridLayout->addWidget(pToleranceValueLabel, 0, 0);
	InsParameterGridLayout->addWidget(pToleranceValueEditLine, 0, 1);

	pUpDeviationLabel->move(10000, 10000);
	pUpDeviationEditLine->move(10000, 10000);
	pDownDeviationLabel->move(10000, 10000);
	pDownDeviationEditLine->move(10000, 10000);
}

void FInsFeatureDlg::contextMenuEvent(QContextMenuEvent* event)
{
	QPoint point = QCursor::pos();
	QPoint localPos = FaceList1->viewport()->mapFromGlobal(point);

	function<void(QListWidgetItem*, int)> ft = [&](QListWidgetItem* tmpItem, int axis)->void
	{
		//测试所选择的item是否准确
		if (tmpItem)
		{
			std::cout << tmpItem->text().toLocal8Bit().data() << std::endl;
			std::cout << tmpItem->type() << std::endl;
		}

		if (tmpItem)
		{
			m_pPopMenu->clear();
			m_pPopMenu->addAction(m_pActionDelete);
			m_pPopMenu->addAction(m_pDelAllSurf);
			m_pPopMenu->exec(QCursor::pos());
		}
		else
		{
		}
	};

	// 右键点在第一个FaceListWidget
	if (FaceList1->geometry().contains(localPos))
	{
		QListWidgetItem* pItem = FaceList1->itemAt(localPos);
		m_pTmpItem = pItem;

		ClickedFaceListWidget = 1;

		ft(pItem, 1);
	}
	// 右键点在第二个FaceListWidget
	else if(FaceList2->geometry().contains(localPos))
	{
		QListWidgetItem* pItem = FaceList2->itemAt(FaceList2->viewport()->mapFromGlobal(point));
		m_pTmpItem = pItem;

		ClickedFaceListWidget = 2;

		ft(pItem, 2);

	}

}

void FInsFeatureDlg::addSurf(FSurface* _surf)
{
	QListWidget* pWidget = NULL;
	unordered_set<FSurface*>* surfHashList;
	Quantity_Color highlightColor;

	if (ClickedFaceListWidget == 1)
	{
		pWidget = FaceList1;
		surfHashList = &m_HashSurfList1;
		highlightColor.SetValues(Quantity_NOC_RED);
	}
	else if (ClickedFaceListWidget == 2)
	{
		pWidget = FaceList2;
		surfHashList = &m_HashSurfList2;
		highlightColor.SetValues(Quantity_NOC_CYAN);
	}
	else
	{

		return;
	}

	// 如果已选中，则取消选中该型面
	if (surfHashList->count(_surf))
	{
		// 颜色恢复为初始状态
		gWindow->GetOccConverter()->setSurfColorDefault(_surf);

		// 哈希表中删除该项目
		surfHashList->erase(_surf);

		// 删除列表组件中的项目
		removeListItem(pWidget, _surf);
	}
	else
	{
		// 变换曲面颜色
		double sR = highlightColor.Red();
		double sG = highlightColor.Green();
		double sB = highlightColor.Blue();
		_surf->SetColor(sR, sG, sB);

		// 将曲面加入到哈希表
		surfHashList->insert(_surf);

		// 在列表组件中加入该项目
		SURFACE_TYPE surfType;
		int indAll;
		int indType;
		QString surfName;
		gWindow->GetOccConverter()->FSurfInfo(_surf, surfType, indAll, indType, surfName);

		// 创建列表项目
		QListWidgetItem* listItem = new QListWidgetItem(surfName);

		// 为列表项目设置图标
		switch (surfType)
		{
		case BSPLINE:
			listItem->setIcon(QIcon("../icons/BsplineSurface.png"));
			break;
		case CONE:
			listItem->setIcon(QIcon("../icons/ConicalSurface.png"));
			break;
		case CYLINDER:
			listItem->setIcon(QIcon("../icons/CylindricalSurface.png"));
			break;
		case PLANE:
			listItem->setIcon(QIcon("../icons/PlaneSurface.png"));
			break;
		case SPHERE:
			listItem->setIcon(QIcon("../icons/SphericalSurface.png"));
			break;
		case OTHER_SURF:
			listItem->setIcon(QIcon("../icons/OtherSurface.png"));
			break;
		default:
			break;
		}

		// 为列表项目设置数据
		listItem->setData(Qt::UserRole, QVariant::fromValue((void*)(_surf)));

		// 将列表项目加入到列表组件
		pWidget->addItem(listItem);

	}

	// 刷新窗口显示
	gWindow->UpdateModelView();

	gTEnd = clock();
	double time_taken = double(gTEnd - gTStart) / CLOCKS_PER_SEC; // 计算耗时（秒）
	std::cout << "函数运行时间: " << time_taken << " 秒" << std::endl;

}

FInsFeatureDlg::IFDListWidget::IFDListWidget(FInsFeatureDlg* _dlg, const int& _ind, const double& _r, const double& _g, const double& _b)
{
	m_pInsFeaDlg = _dlg;
	m_nInd = _ind;

	m_bcgR = _r;
	m_bcgG = _g;
	m_bcgB = _b;

	m_bcgDefR = 1;
	m_bcgDefG = 1;
	m_bcgDefB = 1;

	setStyleSheet(getStyleSheet(m_bcgDefR, m_bcgDefG, m_bcgDefB));
}

FInsFeatureDlg::IFDListWidget::~IFDListWidget()
{

}

void FInsFeatureDlg::IFDListWidget::setBcgDef()
{
	setStyleSheet(getStyleSheet(m_bcgDefR, m_bcgDefG, m_bcgDefB));
}

void FInsFeatureDlg::IFDListWidget::mousePressEvent(QMouseEvent* event)
{
	// 调用基类的mousePressEvent，以确保正常的事件处理
	QListWidget::mousePressEvent(event);

	m_pInsFeaDlg->setListWidgetBcgDef();

	// 在这里添加你想要执行的操作
	if (event->button() == Qt::LeftButton)
	{
		setStyleSheet(getStyleSheet(m_bcgR, m_bcgG, m_bcgB));

		switch (m_nInd)
		{
		case 1:
		{

			m_pInsFeaDlg->faceSelG1();
			//m_pTabWidget->selCylin();

			break;
		}
		case 2:
		{
			m_pInsFeaDlg->faceSelG2();

			//m_pTabWidget->selBasePlane();
			break;
		}
		default:
			break;
		}

	}

}

QString FInsFeatureDlg::IFDListWidget::getStyleSheet(double _r, double _g, double _b)
{
	int cR = 255 * _r;
	int cG = 255 * _g;
	int cB = 255 * _b;

	QString style =
		QString("QListWidget{border:1px solid black; background-color: rgba(%1, %2, %3, 50); }").arg(cR).arg(cG).arg(cB) +
		"QListWidget::Item{padding-top:12px; padding-bottom:12px; }"
		"QListWidget::Item:hover{background:skyblue; }"
		"QListWidget::item:selected{background:rgb(204,213,240); color:rgb(59,79,129); }"
		"QListWidget::item:selected:!active{border-width:0px; background:rgb(204,213,240); }"

		// 滚动条
		"QScrollBar:vertical"
		"{"
		"width:22px;"
		"background:rgba(0,0,0,0%);"
		"margin:0px,0px,0px,0px;"
		"padding-top:18px;"
		"padding-bottom:18px;"
		"padding-right:5px;"
		"padding-left:5px;"
		"background:rgba(232,232,236,100%);"
		"}"
		// 滚动条两端变成椭圆
		"QScrollBar::handle:vertical"
		"{"
		"width:8px;"
		"background:rgba(0,0,0,25%);"
		"min-height:20;"
		"max-width:10;"
		"}"
		// 鼠标放到滚动条上的时候，颜色变深
		"QScrollBar::handle:vertical:hover"
		"{"
		"width:8px;"
		"background:rgba(0,0,0,50%);"
		"min-height:20;"
		"}"
		// 这个应该是设置下箭头的，3.png就是箭头
		"QScrollBar::add-line:vertical"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollDown.png);"
		"subcontrol-position:bottom;"
		"}"
		// 设置上箭头
		"QScrollBar::sub-line:vertical"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollUp.png);"
		"subcontrol-position:top;"
		"}"
		// 当鼠标放到下箭头上的时候
		"QScrollBar::add-line:vertical:hover"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollDown2.png);"
		"subcontrol-position:bottom;"
		"}"
		// 当鼠标放到下箭头上的时候
		"QScrollBar::sub-line:vertical:hover"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollUp2.png);"
		"subcontrol-position:top;"
		"}"
		// 当滚动条滚动的时候，上面的部分和下面的部分
		"QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical"
		"{"
		"background:rgba(232,232,236,100%);"
		"}"

		//横向
		"QScrollBar:horizontal"
		"{"
		"height:22px;"
		"background:rgba(0,0,0,0%);"
		"margin:0px,0px,0px,0px;"
		"padding-top:5px;"
		"padding-bottom:5px;"
		"padding-right:18px;"
		"padding-left:18px;"
		"background:rgba(232,232,236,100%);"
		"}"
		// 滚动条两端变成椭圆
		"QScrollBar::handle:horizontal"
		"{"
		"height:8px;"
		"background:rgba(0,0,0,25%);"
		"min-width:10;"
		"max-height:20;"
		"}"
		// 鼠标放到滚动条上的时候，颜色变深
		"QScrollBar::handle:horizontal:hover"
		"{"
		"height:8px;"
		"background:rgba(0,0,0,50%);"
		"min-width:20;"
		"}"
		// 设置左箭头的，3.png就是箭头
		"QScrollBar::sub-line:horizontal"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollLeft.png);"
		"subcontrol-position:left;"
		"}"
		// 设置右箭头
		"QScrollBar::add-line:horizontal"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollRight.png);"
		"subcontrol-position:right;"
		"}"
		// 当鼠标放到左箭头上的时候
		"QScrollBar::sub-line:horizontal:hover"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollLeft2.png);"
		"subcontrol-position:left;"
		"}"
		// 当鼠标放到右箭头上的时候
		"QScrollBar::add-line:horizontal:hover"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollRight2.png);"
		"subcontrol-position:right;"
		"}"
		// 当滚动条滚动的时候，上面的部分和下面的部分
		"QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal"
		"{"
		"background:rgba(232,232,236,100%);"
		"}";

	return style;

}
