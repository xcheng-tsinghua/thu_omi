#include "AutoInsFea.h"
#include <qmenu.h>
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qscrollarea.h>
#include <qlistwidget.h>
#include <qlabel.h>
#include "FMainWindow.h"
#include "FTreeWnd.h"
#include "FInsFeaTreeWnd.h"
#include "np.h"
#include "occConverter.h"
#include <functional>
#include "AccuEvaluWnd.h"
#include <WindowsX.h>
#include <qcheckbox.h>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

extern FMainWindow* gWindow;
extern int gInsFeaIndex;

AutoInsFea::AutoInsFea()
{
	setAttribute(Qt::WA_StyledBackground, true);

	setWindowTitle(QString::fromLocal8Bit("�Զ��������"));
	
	QWidget* dockWidgetContents = new QWidget;

	MainVLayput = new QVBoxLayout(dockWidgetContents);

	QGroupBox* pFirstGroup = new QGroupBox(QString::fromLocal8Bit("����"));

	QHBoxLayout* SubLayout1 = new QHBoxLayout(pFirstGroup);

	QPushButton* UpdateBnt = new QPushButton(QString::fromLocal8Bit("����"));

	QPushButton* ExitBnt = new QPushButton(QString::fromLocal8Bit("�ر�"));

	SubLayout1->addWidget(UpdateBnt);

	SubLayout1->addWidget(ExitBnt);

	QGroupBox* pSecondGroup = new QGroupBox(QString::fromLocal8Bit("����"));

	QGridLayout* SubLayout2 = new QGridLayout(pSecondGroup);

	m_pTolValueLine = new QLineEdit;
	m_pUpDevLine = new QLineEdit;
	m_pDownDevLine = new QLineEdit;

	m_pTolValueLine->setText("0.05");
	m_pUpDevLine->setText("0.05");
	m_pDownDevLine->setText("0.05");

	SubLayout2->addWidget(new QLabel(QString::fromLocal8Bit("������ֵ")), 0, 0);
	SubLayout2->addWidget(m_pTolValueLine, 0, 1);

	SubLayout2->addWidget(new QLabel(QString::fromLocal8Bit("��ƫ��:+")), 1, 0);
	SubLayout2->addWidget(m_pUpDevLine, 1, 1);

	SubLayout2->addWidget(new QLabel(QString::fromLocal8Bit("��ƫ��:-")), 2, 0);
	SubLayout2->addWidget(m_pDownDevLine, 2, 1);

	m_pTree = new AutoInsFeaTreeWidget;
	m_pTree->setHeaderHidden(true);

	MainVLayput->addWidget(pFirstGroup);
	MainVLayput->addWidget(pSecondGroup);
	MainVLayput->addWidget(m_pTree);

	setContentsMargins(0, 0, 0, 0);
	setWidget(dockWidgetContents);

	m_pPARALLELISM_Item = new QTreeWidgetItem;
	m_pROUNDNESS_Item = new QTreeWidgetItem;
	m_pDISTANCE_Item = new QTreeWidgetItem;
	m_pVERTICALITY_Item = new QTreeWidgetItem;
	m_pDIAMETER_Item = new QTreeWidgetItem;
	m_pAUXIALITY_Item = new QTreeWidgetItem;
	m_pFLATNESS_Item = new QTreeWidgetItem;

	m_pPARALLELISM_Item->setIcon(0, QIcon("../icons/tolerance/PARALLELISM.png"));
	m_pPARALLELISM_Item->setText(0, QString::fromLocal8Bit("ƽ�ж�"));

	m_pROUNDNESS_Item->setIcon(0, QIcon("../icons/tolerance/ROUNDNESS.png"));
	m_pROUNDNESS_Item->setText(0, QString::fromLocal8Bit("Բ��"));

	m_pDISTANCE_Item->setIcon(0, QIcon("../icons/tolerance/DISTANCE.png"));
	m_pDISTANCE_Item->setText(0, QString::fromLocal8Bit("����"));

	m_pVERTICALITY_Item->setIcon(0, QIcon("../icons/tolerance/VERTICALITY.png"));
	m_pVERTICALITY_Item->setText(0, QString::fromLocal8Bit("��ֱ��"));

	m_pDIAMETER_Item->setIcon(0, QIcon("../icons/tolerance/DIAMETER.png"));
	m_pDIAMETER_Item->setText(0, QString::fromLocal8Bit("ֱ��"));

	m_pAUXIALITY_Item->setIcon(0, QIcon("../icons/tolerance/AUXIALITY.png"));
	m_pAUXIALITY_Item->setText(0, QString::fromLocal8Bit("ͬ���"));

	m_pFLATNESS_Item->setIcon(0, QIcon("../icons/tolerance/FLATNESS.png"));
	m_pFLATNESS_Item->setText(0, QString::fromLocal8Bit("ƽ���"));

	m_pTree->addTopLevelItem(m_pPARALLELISM_Item);
	m_pTree->addTopLevelItem(m_pROUNDNESS_Item);
	m_pTree->addTopLevelItem(m_pDISTANCE_Item);
	m_pTree->addTopLevelItem(m_pVERTICALITY_Item);
	m_pTree->addTopLevelItem(m_pDIAMETER_Item);
	m_pTree->addTopLevelItem(m_pAUXIALITY_Item);
	m_pTree->addTopLevelItem(m_pFLATNESS_Item);

	m_pTmpItem = NULL;
	m_LocalInsFeaInd = 0;
	m_pFilterParasDlg = new FilterParasDlg(this);
	m_PlaneAreaRate = 0;
	m_PlaneAreaRank = 0;

	m_pFoldAll = new QAction(QString::fromLocal8Bit("�۵�ȫ��"), this);
	m_pExpandAll = new QAction(QString::fromLocal8Bit("չ��ȫ��"), this);
	m_pAddInsFea = new QAction(QString::fromLocal8Bit("��Ӽ������"), this);
	m_pFiltParaSetting = new QAction(QString::fromLocal8Bit("ɸѡ����"), this);

	m_pFoldAll->setIcon(QIcon("../icons/Fold.png"));
	m_pExpandAll->setIcon(QIcon("../icons/Expand.png"));
	m_pAddInsFea->setIcon(QIcon("../icons/AddInsFea.png"));
	m_pFiltParaSetting->setIcon(QIcon("../icons/Filter.png"));

	m_pPopMenu = QSharedPointer<QMenu>(new QMenu());

	connect(ExitBnt, SIGNAL(clicked()), this, SLOT(onClose()));
	connect(UpdateBnt, SIGNAL(clicked()), this, SLOT(onSearchingInsFea()));
	connect(m_pFoldAll, SIGNAL(triggered()), this, SLOT(onCollapseAll()));
	connect(m_pExpandAll, SIGNAL(triggered()), this, SLOT(onExpandAll()));
	connect(m_pAddInsFea, SIGNAL(triggered()), this, SLOT(onAddInsFea()));
	connect(m_pFiltParaSetting, SIGNAL(triggered()), this, SLOT(onFilterParaSetting()));

	//style
	{
		//this->setFont(QFont("Microsoft YaHei"));
		//������
		this->setStyleSheet("QDockWidget::title"
			"{"
			"padding-left: 10px;"
			"padding-top: 4px;"
			"background:" + gDockTitleColor +
			"}"
			"QDockWidget"
			"{"
			"color: rgb(247,249,254);"//��������������ɫ
			//"titlebar-close-icon: url(../icons/StrollDown.png);"//�رհ�ťͼ��
			"}"
			"QDockWidget::close-button {"
			"icon:url(../icons/StrollDown.png);"
			"}"
		);

		//������
		dockWidgetContents->setStyleSheet(
			"QWidget"
			"{"
			"border: 1px solid rgb(247,249,254);"
			"background:rgb(247,249,254);"
			"}"
			"QScrollBar:vertical"
			"{"
			"width:22px;"
			"background:rgba(0,0,0,0%);"
			"margin:0px,0px,0px,0px;"
			"padding-top:18px;"
			"padding-bottom:18px;"
			"padding-right:5px;"
			"padding-left:5px;"
			"background:rgba(247,249,254,100%);"
			"}"
			// ���������˱����Բ
			"QScrollBar::handle:vertical"
			"{"
			"width:8px;"
			"background:rgba(0,0,0,25%);"
			"min-height:20;"
			"max-width:10;"
			"}"
			// ���ŵ��������ϵ�ʱ����ɫ����
			"QScrollBar::handle:vertical:hover"
			"{"
			"width:8px;"
			"background:rgba(0,0,0,50%);"
			"min-height:20;"
			"}"
			// ���Ӧ���������¼�ͷ�ģ�3.png���Ǽ�ͷ
			"QScrollBar::add-line:vertical"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollDown.png);"
			"subcontrol-position:bottom;"
			"}"
			// �����ϼ�ͷ
			"QScrollBar::sub-line:vertical"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollUp.png);"
			"subcontrol-position:top;"
			"}"
			// �����ŵ��¼�ͷ�ϵ�ʱ��
			"QScrollBar::add-line:vertical:hover"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollDown2.png);"
			"subcontrol-position:bottom;"
			"}"
			// �����ŵ��¼�ͷ�ϵ�ʱ��
			"QScrollBar::sub-line:vertical:hover"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollUp2.png);"
			"subcontrol-position:top;"
			"}"
			// ��������������ʱ������Ĳ��ֺ�����Ĳ���
			"QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical"
			"{"
			"background:rgba(247,249,254,100%);"
			"}"

			//����
			"QScrollBar:horizontal"
			"{"
			"height:22px;"
			"background:rgba(0,0,0,0%);"
			"margin:0px,0px,0px,0px;"
			"padding-top:5px;"
			"padding-bottom:5px;"
			"padding-right:18px;"
			"padding-left:18px;"
			"background:rgba(247,249,254,100%);"
			"}"
			// ���������˱����Բ
			"QScrollBar::handle:horizontal"
			"{"
			"height:8px;"
			"background:rgba(0,0,0,25%);"
			"min-width:10;"
			"max-height:20;"
			"}"
			// ���ŵ��������ϵ�ʱ����ɫ����
			"QScrollBar::handle:horizontal:hover"
			"{"
			"height:8px;"
			"background:rgba(0,0,0,50%);"
			"min-width:20;"
			"}"
			// �������ͷ�ģ�3.png���Ǽ�ͷ
			"QScrollBar::sub-line:horizontal"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollLeft.png);"
			"subcontrol-position:left;"
			"}"
			// �����Ҽ�ͷ
			"QScrollBar::add-line:horizontal"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollRight.png);"
			"subcontrol-position:right;"
			"}"
			// �����ŵ����ͷ�ϵ�ʱ��
			"QScrollBar::sub-line:horizontal:hover"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollLeft2.png);"
			"subcontrol-position:left;"
			"}"
			// �����ŵ��Ҽ�ͷ�ϵ�ʱ��
			"QScrollBar::add-line:horizontal:hover"
			"{"
			"height:18px;width:18px;"
			"border-image:url(../icons/StrollRight2.png);"
			"subcontrol-position:right;"
			"}"
			// ��������������ʱ������Ĳ��ֺ�����Ĳ���
			"QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal"
			"{"
			"background:rgba(247,249,254,100%);"
			"}"
		);

		//�Ҽ��˵�
		m_pPopMenu->setStyleSheet(
			"QMenu"
			"{"
			"background-color:rgb(233,238,255);"
			//"border:0px solid rgb(148,166,202);"
			"border:1px solid rgb(93,107,153);"
			"}"
			"QMenu::item:selected"
			"{"
			"color:black;"
			"background-color:rgb(177,197,255);"
			"}"
			"QMenu::item:pressed"
			"{"
			"color:black;"
			"background-color:rgb(177,197,255);"
			"}"
		);

		ExitBnt->setStyleSheet(
			"QPushButton{background-color:rgb(204,213,240);}"
			"QPushButton:hover{background-color:rgb(245,204,132);}"
			"QPushButton:pressed{background-color:rgb(108,226,108);}");

		UpdateBnt->setStyleSheet(
			"QPushButton{background-color:rgb(204,213,240);}"
			"QPushButton:hover{background-color:rgb(245,204,132);}"
			"QPushButton:pressed{background-color:rgb(108,226,108);}");

		m_pTolValueLine->setStyleSheet("border:1px solid rgb(50,124,46);");
		m_pUpDevLine->setStyleSheet("border:1px solid rgb(50,124,46);");
		m_pDownDevLine->setStyleSheet("border:1px solid rgb(50,124,46);");

		pFirstGroup->setStyleSheet(
			"QGroupBox{"
			"border: 1px solid rgb(64,80,141);"
			"margin-top:2ex;"
			"}"
			"QGroupBox:title{subcontrol-origin:margin;padding:3px;}"
		);

		pSecondGroup->setStyleSheet(
			"QGroupBox{"
			"border: 1px solid rgb(64,80,141);"
			"margin-top:2ex;"
			"}"
			"QGroupBox:title{subcontrol-origin:margin;padding:3px;}"
		);

	}

}

void AutoInsFea::onClose()
{
	gWindow->removeDockWidget(this);

	gWindow->splitDockWidget((FTreeWnd*)gWindow->GetSubWnd()[0], (FInsFeaTreeWnd*)gWindow->GetSubWnd()[6], Qt::Vertical);

	gWindow->addDockWidget(Qt::LeftDockWidgetArea, (FTreeWnd*)gWindow->GetSubWnd()[0]);
	gWindow->addDockWidget(Qt::LeftDockWidgetArea, (FInsFeaTreeWnd*)gWindow->GetSubWnd()[6]);

	close();
}

void AutoInsFea::SetFilterVals(double paraPlaneAreaRate, double paraPlaneAreaRank)
{
	m_PlaneAreaRate = paraPlaneAreaRate;

	m_PlaneAreaRank = paraPlaneAreaRank;
}

void AutoInsFea::onSearchingInsFea()
{
	ClearTreeItems();

	////ƽ�ж�
	// ����ƽ��֮��ƽ�ж�
	vector<int> plane_indexs = gWindow->GetSTEPModelConverter()->GetPlaneInds();
	vector<FSurface*> allSurfList = gWindow->GetSTEPModelConverter()->GetFSurfList();

	double tmpTolValue = m_pTolValueLine->text().toDouble();
	double tmpUpDevValue = m_pUpDevLine->text().toDouble();
	double tmpDownDevValue = m_pDownDevLine->text().toDouble();

	// ռλ�ã���Ϊ���ּ�������޵ڶ�Ҫ��
	vector<FSurface*> empty_list;

	int nSize = plane_indexs.size();

	// �������ƽ������
	double MaxPlaneArea = 0;

	multiset<double> tmpAreaList;
	//set<double> tmpAreaList;
	//map<double, int> tmpAreaList;

	double tmpArea = 0;

	for (int i = 0; i < nSize - 1; i++)
	{
		tmpArea = FSurfaceArea(allSurfList[plane_indexs[i]]);

		tmpAreaList.insert(tmpArea);
		MaxPlaneArea = max(MaxPlaneArea, tmpArea);
	}

	function<bool(FSurface*, double, double)> IsPlaneAreaValid = [&](FSurface* paraFSurf, double rate, double rank)->bool
	{
		int MidInd = nSize * rank;

		int tmpBegin = 0;

		bool IsSatisfyRank = false;

		double SurfArea = FSurfaceArea(paraFSurf);

		for (auto ita = tmpAreaList.begin(); ita != tmpAreaList.end(); ++ita)
		{
			if (tmpBegin > MidInd)
			{
				if (SurfArea >= (*ita)) IsSatisfyRank = true;

				break;
			}

			tmpBegin++;
		}

		bool IsSatisfyRate = false;

		if (SurfArea >= rate * MaxPlaneArea)
		{
			IsSatisfyRate = true;
		}

		if (IsSatisfyRate || IsSatisfyRank)
		{
			return true;
		}
		else
		{
			return false;
		}

	};

	// ��������ڵ�����25%�������ƽ�������0.1

	// ���ֻѡ����Բ�����ѡ��
	//if (m_pFilterParasDlg->GetIsCylinderOnly()) goto SkipCylinderSraeck;

	for (int i = 0; i < nSize - 1; i++)
	{
		// �����������С��ƽ��
		if (!IsPlaneAreaValid(allSurfList[plane_indexs[i]], m_PlaneAreaRate, m_PlaneAreaRank)) continue;

		//////////////////////////////// ����ƽ�湹��ƽ��ȼ������ ////////////////////////////////
		// ��һҪ��TreeItem
		vector<FSurface*> FirstFSurfList;
		FirstFSurfList.push_back(allSurfList[plane_indexs[i]]);

		FInsFeature* flat_fea = new FInsFeature(FirstFSurfList, empty_list, FLATNESS, tmpTolValue, tmpUpDevValue, tmpDownDevValue, 0, m_LocalInsFeaInd);

		QTreeWidgetItem* tmpFlatItem = new QTreeWidgetItem;
		tmpFlatItem->setIcon(0, QIcon("../icons/tolerance/FLATNESS.png"));

		tmpFlatItem->setText(0, QString::fromLocal8Bit("ƽ���") + QString::number(m_LocalInsFeaInd));

		tmpFlatItem->setData(0, Qt::UserRole, QVariant::fromValue((void*)(flat_fea)));

		//tmpFlatItem->addChildren(firstItemList);

		tmpFlatItem->setToolTip(0, QString::fromLocal8Bit("ƽ���:") + QString::number(tmpTolValue));

		m_pFLATNESS_Item->addChild(tmpFlatItem);

		m_LocalInsFeaInd++;

		// ��Ҫ����������һ�����ƽ���
		if (i == (nSize - 2))
		{
			int ExInd = i + 1;
			// ��һҪ��TreeItem
			vector<FSurface*> ExfirstFSurfList;
			ExfirstFSurfList.push_back(allSurfList[plane_indexs[ExInd]]);

			FInsFeature* Exflat_fea = new FInsFeature(ExfirstFSurfList, empty_list, FLATNESS, tmpTolValue, tmpUpDevValue, tmpDownDevValue, 0, m_LocalInsFeaInd);

			QTreeWidgetItem* ExtmpFlatItem = new QTreeWidgetItem;
			ExtmpFlatItem->setIcon(0, QIcon("../icons/tolerance/FLATNESS.png"));

			ExtmpFlatItem->setText(0, QString::fromLocal8Bit("ƽ���") + QString::number(m_LocalInsFeaInd));

			ExtmpFlatItem->setData(0, Qt::UserRole, QVariant::fromValue((void*)(Exflat_fea)));

			//ExtmpFlatItem->addChildren(ExfirstItemList);

			m_pFLATNESS_Item->addChild(ExtmpFlatItem);

			m_LocalInsFeaInd++;

		}

		for (int j = i + 1; j < nSize; j++)
		{
			// �����������С��ƽ��
			if (!IsPlaneAreaValid(allSurfList[plane_indexs[j]], m_PlaneAreaRate, m_PlaneAreaRank)) continue;

			// �ж��Ƿ�ƽ��
			double A1, B1, C1, D1, A2, B2, C2, D2;

			Handle(Geom_Plane)::DownCast(BRep_Tool::Surface(gWindow->GetSTEPModelConverter()->GetTopoDS_FaceList()[plane_indexs[i]]))->Coefficients(A1, B1, C1, D1);
			Handle(Geom_Plane)::DownCast(BRep_Tool::Surface(gWindow->GetSTEPModelConverter()->GetTopoDS_FaceList()[plane_indexs[j]]))->Coefficients(A2, B2, C2, D2);

			gp_Dir dir1(A1, B1, C1);
			gp_Dir dir2(A2, B2, C2);

			// �Ƿ�ƽ��
			if (dir1.IsParallel(dir2, 0.001))
			{
				// ����̫��������
				double distance_1_to_2 = abs(D2 - D1) / sqrt(A1 * A1 + B1 * B1 + C1 * C1);

				if (distance_1_to_2 > 0.01)
				{
					//////////////////////////////// ����ƽ�жȣ�ƽ��-ƽ�棩������� ////////////////////////////////

					// �ڶ�Ҫ��TreeItem
					vector<FSurface*> SecondFSurfList;
					SecondFSurfList.push_back(allSurfList[plane_indexs[j]]);

					FInsFeature* parallel_fea = new FInsFeature(FirstFSurfList, SecondFSurfList, PARALLELISM, tmpTolValue, tmpUpDevValue, tmpDownDevValue, 0, m_LocalInsFeaInd);

					QTreeWidgetItem* tmpParallelItem = new QTreeWidgetItem;
					tmpParallelItem->setIcon(0, QIcon("../icons/PlaneSurface.png"));

					tmpParallelItem->setText(0, QString::fromLocal8Bit("ƽ�ж�(ƽ��-ƽ��)") + QString::number(m_LocalInsFeaInd));

					tmpParallelItem->setData(0, Qt::UserRole, QVariant::fromValue((void*)(parallel_fea)));

					//tmpParallelItem->addChildren(firstItemList);
					//tmpParallelItem->addChildren(secondItemList);

					tmpParallelItem->setToolTip(0, QString::fromLocal8Bit("ƽ�ж�:") + QString::number(tmpTolValue));

					m_pPARALLELISM_Item->addChild(tmpParallelItem);

					m_LocalInsFeaInd++;

					//////////////////////////////// ƽ���ƽ�ж�ͬʱҲ�ɹ���ƽ������������ ////////////////////////////////
					FInsFeature* distance_plane_fea = new FInsFeature(FirstFSurfList, SecondFSurfList, DISTANCE, distance_1_to_2, tmpUpDevValue, tmpDownDevValue, 0, m_LocalInsFeaInd);

					QTreeWidgetItem* tmpDisPlaneItem = new QTreeWidgetItem;
					tmpDisPlaneItem->setIcon(0, QIcon("../icons/PlaneSurface.png"));

					tmpDisPlaneItem->setText(0, QString::fromLocal8Bit("����(ƽ��-ƽ��)") + QString::number(m_LocalInsFeaInd));

					tmpDisPlaneItem->setData(0, Qt::UserRole, QVariant::fromValue((void*)(distance_plane_fea)));

					//tmpDisPlaneItem->addChildren(firstItemList);
					//tmpDisPlaneItem->addChildren(secondItemList);

					tmpDisPlaneItem->setToolTip(0, QString::fromLocal8Bit("����:") + QString::number(distance_1_to_2) + "\n" +
						QString::fromLocal8Bit("��ƫ��:+") + QString::number(tmpUpDevValue) + "\n" +
						QString::fromLocal8Bit("��ƫ��:-") + QString::number(tmpDownDevValue));

					m_pDISTANCE_Item->addChild(tmpDisPlaneItem);

					m_LocalInsFeaInd++;

				}
			}

			//////////////////////////////// ����ƽ��䴹ֱ�ȼ������ ////////////////////////////////
			// �ж��Ƿ�ֱ
			if (abs(A1 * A2 + B1 * B2 + C1 * C2) < 0.001)
			{
				// �ڶ�Ҫ��TreeItem
				vector<FSurface*> SecondFSurfList;
				SecondFSurfList.push_back(allSurfList[plane_indexs[j]]);

				FInsFeature* vertical_fea = new FInsFeature(FirstFSurfList, SecondFSurfList, VERTICALITY, tmpTolValue, tmpUpDevValue, tmpDownDevValue, 0, m_LocalInsFeaInd);

				QTreeWidgetItem* tmpVerticalItem = new QTreeWidgetItem;
				tmpVerticalItem->setIcon(0, QIcon("../icons/tolerance/VERTICALITY.png"));

				tmpVerticalItem->setText(0, QString::fromLocal8Bit("��ֱ��(ƽ��-ƽ��)") + QString::number(m_LocalInsFeaInd));

				tmpVerticalItem->setData(0, Qt::UserRole, QVariant::fromValue((void*)(vertical_fea)));

				//tmpVerticalItem->addChildren(firstItemList);
				//tmpVerticalItem->addChildren(secondItemList);

				tmpVerticalItem->setToolTip(0, QString::fromLocal8Bit("��ֱ��:") + QString::number(tmpTolValue));

				m_pVERTICALITY_Item->addChild(tmpVerticalItem);

				m_LocalInsFeaInd++;

			}
		}
	}

	// ����Բ������֮��ƽ�ж�
	vector<TopoDS_Face> tmpGeomSurfList = gWindow->GetSTEPModelConverter()->GetTopoDS_FaceList();

	//vector<Handle(Geom_Surface)> tmpGeomSurfList = gWindow->GetSTEPModelConverter()->GetGeomSurfList();
	vector<FSurface*> tmpFSurfList = gWindow->GetSTEPModelConverter()->GetFSurfList();

	nSize = tmpGeomSurfList.size();
	unordered_map<int, int> cylinder_nearby_map = gWindow->GetSTEPModelConverter()->GetSameCylinderMap();
	unordered_map<int, int> cylinder_nearby_map_rev = gWindow->GetSTEPModelConverter()->GetSameCylinderMapRev();

	vector<TopoDS_Face> AllTopoDS_Surf = gWindow->GetSTEPModelConverter()->GetTopoDS_FaceList();
	vector<FSurface*> AllFSurfList = gWindow->GetSTEPModelConverter()->GetFSurfList();

	// �ж�һ��Բ���Ƿ�����������Ϊ��һ����Բ��������
	vector<SurfNearbyGraph_Node*> theNearGraph = gWindow->GetMainDoc()->GetSurfNearbyGraph();
	function<bool(int)> IsCylinIntact = [&](int first_subcylin_ind)->bool
	{

		// �ж�Edge�Ƿ���ͬ
		function<bool(TopoDS_Edge, TopoDS_Edge)> IsEdgeTheSame = [=](TopoDS_Edge Edge1, TopoDS_Edge Edge2)->bool
		{
			double CurveStart1 = 0, CurveEnd1 = 0;

			BRep_Tool::Range(Edge1, CurveStart1, CurveEnd1);

			Handle(Geom_Curve) Curve1 = BRep_Tool::Curve(Edge1, CurveStart1, CurveEnd1);

			double CurveStart2 = 0, CurveEnd2 = 0;

			BRep_Tool::Range(Edge2, CurveStart2, CurveEnd2);

			Handle(Geom_Curve) Curve2 = BRep_Tool::Curve(Edge2, CurveStart2, CurveEnd2);

			if (Curve1.IsNull() || Curve2.IsNull())
			{
				return false;
			}

			gp_Pnt PtStartCurve1 = Curve1->Value(CurveStart1);
			gp_Pnt PtEndCurve1 = Curve1->Value(CurveEnd1);
			gp_Pnt PtMidCurve1 = Curve1->Value((CurveStart1 + CurveEnd1) / 2.0);

			gp_Pnt PtStartCurve2 = Curve2->Value(CurveStart2);
			gp_Pnt PtEndCurve2 = Curve2->Value(CurveEnd2);
			gp_Pnt PtMidCurve2 = Curve2->Value((CurveStart2 + CurveEnd2) / 2.0);

			bool IsStartOverlap = false;
			bool IsEndOverlap = false;
			bool IsMidOverlap = false;

			if (abs(PtStartCurve1.Distance(PtStartCurve2)) < 1e-6 || abs(PtStartCurve1.Distance(PtEndCurve2)) < 1e-6)
			{
				IsStartOverlap = true;
			}

			if (abs(PtEndCurve1.Distance(PtStartCurve2)) < 1e-6 || abs(PtEndCurve1.Distance(PtEndCurve2)) < 1e-6)
			{
				IsEndOverlap = true;
			}

			if (abs(PtMidCurve1.Distance(PtMidCurve2)) < 1e-6)
			{
				IsMidOverlap = true;
			}

			return IsStartOverlap && IsEndOverlap && IsMidOverlap;

		};

		int tmp_face_index = first_subcylin_ind;
		int last_ind = 0;

		// ֻ��������Բ��������
		if (!cylinder_nearby_map.count(cylinder_nearby_map[tmp_face_index]))
		{
			vector<TopoDS_Edge> EdgeList1;
			vector<TopoDS_Edge> EdgeList2;

			TopoDS_Face Face1 = AllTopoDS_Surf[theNearGraph[tmp_face_index]->GetGeomIndex()];

			for (TopExp_Explorer aExpEdge(Face1, TopAbs_EDGE); aExpEdge.More(); aExpEdge.Next())
			{
				EdgeList1.push_back(TopoDS::Edge(aExpEdge.Current()));
			}

			TopoDS_Face Face2 = AllTopoDS_Surf[theNearGraph[cylinder_nearby_map[tmp_face_index]]->GetGeomIndex()];

			for (TopExp_Explorer aExpEdge(Face2, TopAbs_EDGE); aExpEdge.More(); aExpEdge.Next())
			{
				EdgeList2.push_back(TopoDS::Edge(aExpEdge.Current()));
			}

			int NumOfNearEdges = 0;

			for (auto ita1 = EdgeList1.begin(); ita1 != EdgeList1.end(); ++ita1)
			{
				for (auto ita2 = EdgeList2.begin(); ita2 != EdgeList2.end(); ++ita2)
				{
					if (IsEdgeTheSame(*ita1, *ita2))
					{
						NumOfNearEdges++;
					}
				}
			}

			if (NumOfNearEdges == 2)
			{
				return true;
			}
			else
			{
				return false;
			}

		}


		while (true)
		{
			tmp_face_index = cylinder_nearby_map[tmp_face_index];

			if (!cylinder_nearby_map.count(tmp_face_index))
			{
				break;
			}
		}

		return theNearGraph[first_subcylin_ind]->IsNearby(theNearGraph[tmp_face_index]);
	};

	// ����һ��Բ����������
	function<vector<FSurface*>(int, unordered_map<int, int>, vector<FSurface*>)> SearchAllCylin = [](int first_subcylin_ind, unordered_map<int, int> nearby_ump, vector<FSurface*> surf_list)
		->vector<FSurface*>
	{
		vector<FSurface*> CylinderFSurfList;

		int tmp_face_index = first_subcylin_ind;

		while (true)
		{
			CylinderFSurfList.push_back(surf_list[tmp_face_index]);

			tmp_face_index = nearby_ump[tmp_face_index];

			if (!nearby_ump.count(tmp_face_index))
			{
				CylinderFSurfList.push_back(surf_list[tmp_face_index]);

				break;
			}
		}

		return CylinderFSurfList;
	};

	for (int i = 0; i < nSize - 1; i++)
	{
		// Բ��������ڽ��棬��û�У�˵����֮ǰ�ѱ����������Բ����ֻ���������ǰ�ĵ�һ��
		if (!(cylinder_nearby_map.count(i) && (!cylinder_nearby_map_rev.count(i))))
		{
			continue;
		}

		// ��������Բ���治����
		if (!IsCylinIntact(i)) continue;

		//////////////////////////////// ����Բ�ȼ������ ////////////////////////////////
		vector<FSurface*> firstItemList = SearchAllCylin(i, cylinder_nearby_map, tmpFSurfList);

		// ���ѡ����ֻ����ƽ�����ѡ��
		//if (m_pFilterParasDlg->GetIsPlaneOnly()) goto SkipCylinderOnly;

		FInsFeature* round_fea = new FInsFeature(firstItemList, empty_list, PARALLELISM, tmpTolValue, tmpUpDevValue, tmpDownDevValue, 0, m_LocalInsFeaInd);

		QTreeWidgetItem* tmpRoundItem = new QTreeWidgetItem;
		tmpRoundItem->setIcon(0, QIcon("../icons/tolerance/ROUNDNESS.png"));

		tmpRoundItem->setText(0, QString::fromLocal8Bit("Բ��") + QString::number(m_LocalInsFeaInd));

		tmpRoundItem->setData(0, Qt::UserRole, QVariant::fromValue((void*)(round_fea)));

		//tmpRoundItem->addChildren(firstItemList);

		tmpRoundItem->setToolTip(0, QString::fromLocal8Bit("Բ��:") + QString::number(tmpTolValue));

		m_pROUNDNESS_Item->addChild(tmpRoundItem);

		m_LocalInsFeaInd++;

		//////////////////////////////// Բ��ͬʱҲ��ֱ��������� ////////////////////////////////

		Handle(Geom_CylindricalSurface) first_cylinder = Handle(Geom_CylindricalSurface)::DownCast(BRep_Tool::Surface(tmpGeomSurfList[i]));

		FInsFeature* diameter_fea = new FInsFeature(firstItemList, empty_list, DIAMETER, first_cylinder->Radius(), tmpUpDevValue, tmpDownDevValue, 0, m_LocalInsFeaInd);

		QTreeWidgetItem* tmpDiameterItem = new QTreeWidgetItem;
		tmpDiameterItem->setIcon(0, QIcon("../icons/tolerance/DIAMETER.png"));

		tmpDiameterItem->setText(0, QString::fromLocal8Bit("ֱ��") + QString::number(m_LocalInsFeaInd));

		tmpDiameterItem->setData(0, Qt::UserRole, QVariant::fromValue((void*)(diameter_fea)));

		//tmpDiameterItem->addChildren(firstItemList);

		tmpDiameterItem->setToolTip(0, QString::fromLocal8Bit("ֱ��:") + QString::number(first_cylinder->Radius()) + "\n" +
			QString::fromLocal8Bit("��ƫ��:+") + QString::number(tmpUpDevValue) + "\n" +
			QString::fromLocal8Bit("��ƫ��:-") + QString::number(tmpDownDevValue));

		m_pDIAMETER_Item->addChild(tmpDiameterItem);

		m_LocalInsFeaInd++;

		for (int j = i + 1; j < nSize; j++)
		{
			// Բ��������ڽ��棬��û�У�˵����֮ǰ�ѱ����������Բ����ֻ���������ǰ�ĵ�һ��
			if (!(cylinder_nearby_map.count(j) && (!cylinder_nearby_map_rev.count(j))))
			{
				continue;
			}

			// ��������Բ���治����
			if (!IsCylinIntact(j)) continue;

			// ��Բ������ƽ�������߲��غϲſɼ���ƽ�ж�
			Handle(Geom_CylindricalSurface) second_cylinder = Handle(Geom_CylindricalSurface)::DownCast(BRep_Tool::Surface(tmpGeomSurfList[j]));

			gp_Pnt aPoint = first_cylinder->Axis().Location();
			gp_Pnt bPoint = second_cylinder->Axis().Location();

			// gp_Dir һ���ǵ�λ����
			gp_Dir aDir = first_cylinder->Axis().Direction();
			gp_Dir bDir = second_cylinder->Axis().Direction();

			bool IsAxisParallel = aDir.IsParallel(bDir, 0.001);

			bool IsOverlap = false;

			double DisOfParallel = 0;

			if (aPoint.Distance(bPoint) < 0.001)// �غ�
			{
				IsOverlap = true;
			}
			else
			{
				gp_Dir tmpDir = gp_Dir(aPoint.X() - bPoint.X(), aPoint.Y() - bPoint.Y(), aPoint.Z() - bPoint.Z());

				// ������߹��ɵ��������һ��Բ���������ڵ�����ƽ��
				if (tmpDir.IsParallel(aDir, 0.001))
				{
					IsOverlap = true;
				}
				else
				{
					gp_Pnt vertCross(0, 0, 0);

					gp_Pnt vertA_B(bPoint.X() - aPoint.X(), bPoint.Y() - aPoint.Y(), bPoint.Z() - aPoint.Z());

					CROSS(vertCross, vertA_B, aDir);
	
					DisOfParallel = LENGTH(vertCross) / LENGTH(aDir);

					if (DisOfParallel < 0.001)
					{
						IsOverlap = true;
					}
				}
			}

			//////////////////////////////// ƽ���Ҳ��غϣ�����Բ�����߼�ƽ�жȼ������ ////////////////////////////////
			if (IsAxisParallel && (!IsOverlap))
			{
				// ����Բ�����߼�ƽ�жȼ������
				// ��һҪ��TreeItemList
				// firstItemList = SearchAllCylin(i, cylinder_nearby_map, tmpFSurfList);

				// �ڶ�Ҫ��TreeItemList
				vector<FSurface*> secondItemList = SearchAllCylin(j, cylinder_nearby_map, tmpFSurfList);

				FInsFeature* parallel_fea = new FInsFeature(firstItemList, secondItemList, PARALLELISM, tmpTolValue, tmpUpDevValue, tmpDownDevValue, 0, m_LocalInsFeaInd);

				QTreeWidgetItem* tmpParallelItem = new QTreeWidgetItem;
				tmpParallelItem->setIcon(0, QIcon("../icons/tolerance/parallel-cylind.png"));

				tmpParallelItem->setText(0, QString::fromLocal8Bit("ƽ�ж�(Բ��-Բ��)") + QString::number(m_LocalInsFeaInd));

				tmpParallelItem->setData(0, Qt::UserRole, QVariant::fromValue((void*)(parallel_fea)));

				//tmpParallelItem->addChildren(firstItemList);
				//tmpParallelItem->addChildren(secondItemList);

				tmpParallelItem->setToolTip(0, QString::fromLocal8Bit("ƽ�ж�:") + QString::number(tmpTolValue));

				m_pPARALLELISM_Item->addChild(tmpParallelItem);

				m_LocalInsFeaInd++;


				//////////////////////////////// Բ����ƽ�ж�ͬʱҲ�Ǿ��������� ////////////////////////////////
				// ����Բ�����߼����������

				FInsFeature* distance_cylinder_fea = new FInsFeature(firstItemList, secondItemList, DISTANCE, DisOfParallel, tmpUpDevValue, tmpDownDevValue, 0, m_LocalInsFeaInd);

				QTreeWidgetItem* tmpDisCylinItem = new QTreeWidgetItem;
				tmpDisCylinItem->setIcon(0, QIcon("../icons/tolerance/parallel-cylind.png"));

				tmpDisCylinItem->setText(0, QString::fromLocal8Bit("����(Բ��-Բ��)") + QString::number(m_LocalInsFeaInd));

				tmpDisCylinItem->setData(0, Qt::UserRole, QVariant::fromValue((void*)(distance_cylinder_fea)));

				//tmpDisCylinItem->addChildren(firstItemList);
				//tmpDisCylinItem->addChildren(secondItemList);

				tmpDiameterItem->setToolTip(0, QString::fromLocal8Bit("����:") + QString::number(DisOfParallel) + "\n" +
					QString::fromLocal8Bit("��ƫ��:+") + QString::number(tmpUpDevValue) + "\n" +
					QString::fromLocal8Bit("��ƫ��:-") + QString::number(tmpDownDevValue));

				m_pDISTANCE_Item->addChild(tmpDisCylinItem);

				m_LocalInsFeaInd++;


			}

			//////////////////////////////// ƽ���������غϣ�����ͬ��ȼ������ ////////////////////////////////
			if (IsAxisParallel && IsOverlap)
			{
				// ��һҪ��TreeItemList
				// firstItemList = SearchAllCylin(i, cylinder_nearby_map, tmpFSurfList);

				// �ڶ�Ҫ��TreeItemList
				vector<FSurface*> secondItemList = SearchAllCylin(j, cylinder_nearby_map, tmpFSurfList);

				FInsFeature* auxiality_fea = new FInsFeature(firstItemList, secondItemList, AUXIALITY, tmpTolValue, tmpUpDevValue, tmpDownDevValue, 0, m_LocalInsFeaInd);

				QTreeWidgetItem* tmpAuxialityItem = new QTreeWidgetItem;
				tmpAuxialityItem->setIcon(0, QIcon("../icons/tolerance/AUXIALITY.png"));

				tmpAuxialityItem->setText(0, QString::fromLocal8Bit("ͬ���") + QString::number(m_LocalInsFeaInd));

				tmpAuxialityItem->setData(0, Qt::UserRole, QVariant::fromValue((void*)(auxiality_fea)));

				tmpAuxialityItem->setToolTip(0, QString::fromLocal8Bit("ͬ���:") + QString::number(tmpTolValue));

				m_pAUXIALITY_Item->addChild(tmpAuxialityItem);

				//tmpAuxialityItem->addChildren(firstItemList);
				//tmpAuxialityItem->addChildren(secondItemList);

				m_LocalInsFeaInd++;
			}
		}

		//////////////////////////////// ����Բ��-ƽ�洹ֱ�ȼ������ ////////////////////////////////
		for (auto ita = theNearGraph[i]->GetNearbyNodeList().begin(); ita != theNearGraph[i]->GetNearbyNodeList().end(); ++ita)
		{
			// ���ж��Ƿ���ƽ��
			Handle(Geom_Surface) currentSur = BRep_Tool::Surface(AllTopoDS_Surf[(*ita)->GetGeomIndex()]);

			if (currentSur->DynamicType() == STANDARD_TYPE(Geom_Plane))
			{
				Handle(Geom_Plane) plasurf = Handle(Geom_Plane)::DownCast(currentSur);

				double tmpA, tmpB, tmpC, tmpD;
				plasurf->Coefficients(tmpA, tmpB, tmpC, tmpD);

				gp_Dir cylinder_Ax_dir = first_cylinder->Axis().Direction();
				gp_Dir plane_norm_dir(tmpA, tmpB, tmpC);

				// �ж�ƽ�淨������Բ�������Ƿ�ƽ��
				if (cylinder_Ax_dir.IsParallel(plane_norm_dir, 0.01))
				{
					// �ڶ�Ҫ��TreeItem
					vector<FSurface*> secondItemList;
					secondItemList.push_back(AllFSurfList[(*ita)->GetGeomIndex()]);

					FInsFeature* vertical_fea = new FInsFeature(firstItemList, secondItemList, VERTICALITY, tmpTolValue, tmpUpDevValue, tmpDownDevValue, 0, m_LocalInsFeaInd);

					QTreeWidgetItem* tmpVerticalItem = new QTreeWidgetItem;
					tmpVerticalItem->setIcon(0, QIcon("../icons/tolerance/vertical-cylinder-plane.png"));

					tmpVerticalItem->setText(0, QString::fromLocal8Bit("��ֱ��(Բ��-ƽ��)") + QString::number(m_LocalInsFeaInd));

					tmpVerticalItem->setData(0, Qt::UserRole, QVariant::fromValue((void*)(vertical_fea)));

					//tmpVerticalItem->addChildren(firstItemList);
					//tmpVerticalItem->addChildren(secondItemList);

					tmpVerticalItem->setToolTip(0, QString::fromLocal8Bit("��ֱ��:") + QString::number(tmpTolValue));

					m_pVERTICALITY_Item->addChild(tmpVerticalItem);

					m_LocalInsFeaInd++;

				}
			}
		}

	}

}

void AutoInsFea::onCollapseAll()
{
	m_pTree->collapseAll();
}

void AutoInsFea::onExpandAll()
{
	m_pTree->expandAll();
}

void AutoInsFea::onAddInsFea()
{
	FInsFeature* CurrInsFea = (FInsFeature*)m_pTmpItem->data(0, Qt::UserRole).value<void*>();
	CurrInsFea->InsFeatureindex = gInsFeaIndex;

	initTree_by_insfea(CurrInsFea, static_cast<FInsFeaTreeWnd*>(gWindow->GetSubWnd()[6])->GetTree());
	//static_cast<FInsFeaTreeWnd*>(gWindow->GetSubWnd()[6])->onUpdateMeasPnts();
	initTree_by_insfea(CurrInsFea, static_cast<AccuEvaluWnd*>(gWindow->GetSubWnd()[7])->GetTree());

	gWindow->GetMainDoc()->GetInsFeaList().push_back(CurrInsFea);

	gInsFeaIndex++;
}

void AutoInsFea::onFilterParaSetting()
{
	m_pFilterParasDlg->move(QCursor::pos());
	m_pFilterParasDlg->show();
}

void AutoInsFea::contextMenuEvent(QContextMenuEvent* event)
{
	QPoint point = QCursor::pos();
	QPoint localPos = m_pTree->viewport()->mapFromGlobal(point);
	QTreeWidgetItem* pItem = m_pTree->itemAt(localPos);
	m_pTmpItem = pItem;

	//������ѡ���item�Ƿ�׼ȷ
	if (pItem)
	{
		std::cout << pItem->text(0).toLocal8Bit().data() << std::endl;
		std::cout << pItem->type() << std::endl;
	}

	//���ѡ����item
	if (pItem)
	{
		//������ڵ�Ϊ�գ���Ϊ���ڵ�(һ��)���������£�
		if ((pItem->parent() != NULL)&& pItem->parent()->parent()==NULL)
		{
			m_pPopMenu->clear();

			m_pPopMenu->addAction(m_pAddInsFea);
			m_pPopMenu->addAction(m_pFiltParaSetting);
			m_pPopMenu->addSeparator();

			m_pPopMenu->addAction(m_pFoldAll);
			m_pPopMenu->addAction(m_pExpandAll);

			m_pPopMenu->exec(QCursor::pos());
		}

		else
		{
			m_pPopMenu->clear();

			m_pPopMenu->addAction(m_pFiltParaSetting);
			m_pPopMenu->addSeparator();

			m_pPopMenu->addAction(m_pFoldAll);
			m_pPopMenu->addAction(m_pExpandAll);

			m_pPopMenu->exec(QCursor::pos());
		}
	}
	else
	{
		m_pPopMenu->clear();
		m_pPopMenu->addAction(m_pFiltParaSetting);
		m_pPopMenu->exec(QCursor::pos());
	}

}

void AutoInsFea::closeEvent(QCloseEvent* event)
{
	onClose();

	QDockWidget::closeEvent(event);

	m_pFilterParasDlg->close();
}

void AutoInsFea::ClearTreeItems()
{
	function<void(QTreeWidgetItem*)> ClearChildren = [&](QTreeWidgetItem* paraTreeItem)->void
	{
		int itemChildrenCounts = paraTreeItem->childCount();
		while (itemChildrenCounts--)
		{
			QTreeWidgetItem* child = paraTreeItem->child(itemChildrenCounts); //index�Ӵ�С����ɾ������
			paraTreeItem->removeChild(child);
			delete child;
			child = nullptr;
		}
	};

	ClearChildren(m_pPARALLELISM_Item);
	ClearChildren(m_pROUNDNESS_Item);
	ClearChildren(m_pDISTANCE_Item);
	ClearChildren(m_pVERTICALITY_Item);
	ClearChildren(m_pDIAMETER_Item);
	ClearChildren(m_pAUXIALITY_Item);
	ClearChildren(m_pFLATNESS_Item);
}

AutoInsFeaTreeWidget::AutoInsFeaTreeWidget(QWidget* parent) :QTreeWidget(parent)
{
	//�����۵�ͼ����ʽ
	this->setStyleSheet("QTreeView::branch:has-children:!has-siblings:closed,\
QTreeView::branch:closed:has-children:has-siblings{border-image: none; image: url(../icons/TreeCollapse.png);}\
QTreeView::branch:open:has-children:!has-siblings,\
QTreeView::branch:open:has-children:has-siblings{border-image: none; image: url(../icons/TreeExpand.png);}");
}

void AutoInsFeaTreeWidget::mousePressEvent(QMouseEvent* event)
{
	this->QTreeWidget::mousePressEvent(event);

	if (event->button() == Qt::LeftButton)
	{
		gWindow->ResetToDefaultCountenance();

		QPoint point = QCursor::pos();
		QPoint localPos = viewport()->mapFromGlobal(point);
		QTreeWidgetItem* pItem = itemAt(localPos);

		if (!pItem) return;

		// �㵽�ڶ��㣬������������
		if (pItem->parent() != NULL && pItem->parent()->parent() == NULL)
		{
			FInsFeature* CurrInsFea = (FInsFeature*)pItem->data(0, Qt::UserRole).value<void*>();

			for (int i = 0; i < CurrInsFea->m_FSurfList1.size(); i++)
			{
				CurrInsFea->m_FSurfList1[i]->SetColor(1, 0, 0);
			}

			for (int i = 0; i < CurrInsFea->m_FSurfList2.size(); i++)
			{
				CurrInsFea->m_FSurfList2[i]->SetColor(0, 1, 1);
			}

			gWindow->UpdateModelView();
		}

	}

}

FilterParasDlg::FilterParasDlg(AutoInsFea* paraMainDlg)
{
	m_pAutoInsFea = paraMainDlg;
	m_IsCylinderOnly = false;
	m_IsPlaneOnly = false;

	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

	setFixedSize(140, 190);

	move(QCursor::pos());

	QVBoxLayout* MainLayout = new QVBoxLayout(this);

	QGridLayout* upGridLayout = new QGridLayout;

	m_pPlaneAreaRate = new QLineEdit;
	m_pPlaneAreaRate->setText("0.25");
	m_pPlaneAreaRank = new QLineEdit;
	m_pPlaneAreaRank->setText("0.25");

	upGridLayout->addWidget(new QLabel(QString::fromLocal8Bit("�������")), 0, 0);
	upGridLayout->addWidget(m_pPlaneAreaRate, 0, 1);
	upGridLayout->addWidget(new QLabel(QString::fromLocal8Bit("�������")), 1, 0);
	upGridLayout->addWidget(m_pPlaneAreaRank, 1, 1);

	QVBoxLayout* MidCheckBoxLayout = new QVBoxLayout;

	m_pIsCylinderOnlyFilt = new QCheckBox(QString::fromLocal8Bit("��Բ�����"));
	MidCheckBoxLayout->addWidget(m_pIsCylinderOnlyFilt);

	m_pIsPlaneOnlyFilt = new QCheckBox(QString::fromLocal8Bit("��ƽ�����"));
	MidCheckBoxLayout->addWidget(m_pIsPlaneOnlyFilt);

	QHBoxLayout* DownLayout = new QHBoxLayout;

	// ���Ҷ���
	DownLayout->addStretch();

	QPushButton* pConfirm = new QPushButton(QString::fromLocal8Bit("ȷ��"));

	DownLayout->addWidget(pConfirm, 0);

	MainLayout->addLayout(upGridLayout);
	MainLayout->addLayout(MidCheckBoxLayout);
	MainLayout->addLayout(DownLayout);

	connect(pConfirm, SIGNAL(clicked()), this, SLOT(onConfirm()));
	connect(m_pIsCylinderOnlyFilt, SIGNAL(stateChanged(int)), this, SLOT(onOnlyCylinderCheckBox()));
	connect(m_pIsPlaneOnlyFilt, SIGNAL(stateChanged(int)), this, SLOT(onOnlyPlaneCheckBox()));



	setStyleSheet("background-color:rgb(247,249,254);");

}

void FilterParasDlg::onConfirm()
{
	m_pAutoInsFea->SetFilterVals(m_pPlaneAreaRate->text().toDouble(), m_pPlaneAreaRank->text().toDouble());

	close();
}

void FilterParasDlg::paintEvent(QPaintEvent* event)
{
	QPainter p(this);
	p.setPen(QColor("rgb(245,204,132)")); //���û��ʼ���ɫ
	p.drawRect(0, 0, width() - 1, height() - 1); //���Ʊ߿�
}

void FilterParasDlg::onOnlyCylinderCheckBox()
{
	Qt::CheckState	s = m_pIsCylinderOnlyFilt->checkState();

	if (s == Qt::Checked)
	{
		m_IsCylinderOnly = true;
	}
	else
	{
		m_IsCylinderOnly = false;
	}


}

void FilterParasDlg::onOnlyPlaneCheckBox()
{
	Qt::CheckState	s = m_pIsPlaneOnlyFilt->checkState();

	if (s == Qt::Checked)
	{
		m_IsPlaneOnly = true;
	}
	else
	{
		m_IsPlaneOnly = false;
	}

}

//bool FilterParasDlg::winEvent(MSG* message, long* result)
//{
//	int m_iMarginWidth = 5;
//	
//	switch (message->message)
//	{
//	case WM_NCHITTEST:
//	    int iPosX = GET_X_LPARAM(message->lParam) - this->frameGeometry().x();//���λ��x
//	    int iPosY = GET_Y_LPARAM(message->lParam) - this->frameGeometry().y();//���λ��y
//	
//	    *result = HTCAPTION;
//	
//	    if (iPosX >= 0 && iPosX <= m_iMarginWidth)
//	    {
//	        *result = HTLEFT;//��
//	        int iHeight = this->height();
//	        if (iPosY >= 0 && iPosY <= m_iMarginWidth)
//	        {
//	            *result = HTTOPLEFT;//����
//	        }
//	        if (iPosY >= iHeight - m_iMarginWidth && iPosY <= iHeight)
//	        {
//	            *result = HTBOTTOMLEFT;//����
//	        }
//	        return true;
//	    }
//	
//	    int iWidth = this->width();
//	    int iHeight = this->height();
//	    if (iPosX >= iWidth - m_iMarginWidth && iPosX <= iWidth)
//	    {
//	        *result = HTRIGHT;//��
//	        if (iPosY >= 0 && iPosY <= m_iMarginWidth)
//	        {
//	            *result = HTTOPRIGHT;//����
//	            return true;
//	        }
//	        if (iPosY >= iHeight - m_iMarginWidth && iPosY <= iHeight)
//	        {
//	            *result = HTBOTTOMRIGHT;//����
//	            return true;
//	        }
//	        return true;
//	    }
//	    if (iPosY >= 0 && iPosY <= m_iMarginWidth)
//	    {
//	        *result = HTTOP;//��
//	        return true;
//	    }
//	    if (iPosY >= iHeight - m_iMarginWidth && iPosY <= iHeight)
//	    {
//	        *result = HTBOTTOM;//��
//	        return true;
//	    }
//	    return false;
//	}
//	return false;
//	
//
//}
