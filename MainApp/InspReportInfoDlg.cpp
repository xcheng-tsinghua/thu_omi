#include "InspReportInfoDlg.h"
#include "FMainWindow.h"
#include "FInsReportDlg.h"

extern FMainWindow* gWindow;
extern QSettings* gDlgSetting;

InspReportInfoDlg::InspReportInfoDlg(vector<EvaluData> InspData)
{
	m_pInspData = InspData;
	m_pInsReport = NULL;

	setWindowTitle(QString::fromLocal8Bit("��������Ϣ"));

	setGeometry(500, 250, 500, 250);

	QGridLayout* MainLayout = new QGridLayout(this);

	//���ļ���ȡĬ��ֵ
	DefaultStrPath = "../Default records/The Default inspection report information.txt";
	ifstream infile;
	infile.open(DefaultStrPath.toStdString());
	string s;

	vector<QString> DefaultStrFromFile;
	while (getline(infile, s))
	{
		DefaultStrFromFile.push_back(QString::fromLocal8Bit(s.data()));
		//cout<<s<<endl;
	}

	m_pPartNumberLabel = new QLabel(QString::fromLocal8Bit("���ͼ��"));
	m_pPartNumberLine = new QLineEdit;
	m_pPartNumberLine->setText(gDlgSetting->value("PartNumber").toString());
	//m_pPartNumberLine->setText(DefaultStrFromFile[0]);

	m_pInspStaffLabel = new QLabel(QString::fromLocal8Bit("�����"));
	m_pInspStaffLine = new QLineEdit;
	m_pInspStaffLine->setText(gDlgSetting->value("InspStaff").toString());
	//m_pInspStaffLine->setText(DefaultStrFromFile[1]);

	m_pInspEquipmentLabel = new QLabel(QString::fromLocal8Bit("����豸�������"));
	m_pInspEquipmentLine = new QLineEdit;
	m_pInspEquipmentLine->setText(gDlgSetting->value("InspEquipment").toString());
	//m_pInspEquipmentLine->setText(DefaultStrFromFile[2]);

	m_pOtherDataLabel = new QLabel(QString::fromLocal8Bit("��ע��Ϣ"));
	m_pOtherDataLine = new QTextEdit;
	m_pOtherDataLine->setText(gDlgSetting->value("OtherData").toString());
	//m_pOtherDataLine->setText(DefaultStrFromFile[3]);

	m_pOtherDataLine->setStyleSheet(
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
		"background:rgba(232,232,236,100%);"
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
		"background:rgba(232,232,236,100%);"
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
		"background:rgba(232,232,236,100%);"
		"}"
		);

	m_pConfBnt = new QPushButton(QString::fromLocal8Bit("���ɼ�ⱨ��"));

	MainLayout->addWidget(m_pPartNumberLabel, 0, 0);
	MainLayout->addWidget(m_pPartNumberLine, 0, 1);
	MainLayout->addWidget(m_pInspStaffLabel, 1, 0);
	MainLayout->addWidget(m_pInspStaffLine, 1, 1);
	MainLayout->addWidget(m_pInspEquipmentLabel, 2, 0);
	MainLayout->addWidget(m_pInspEquipmentLine, 2, 1);
	MainLayout->addWidget(m_pOtherDataLabel, 3, 0);
	MainLayout->addWidget(m_pOtherDataLine, 3, 1);

	MainLayout->addWidget(m_pConfBnt, 4, 0);

	connect(m_pConfBnt, SIGNAL(clicked()), this, SLOT(onConfirm()));



}

InspReportInfoDlg::~InspReportInfoDlg()
{

}

void InspReportInfoDlg::JL_DataExe(vector<QString> fpInsRepInfo, int InspMonth, bool IsWS119)
{
	gWindow->SetInsReportDlg(new FInsReportDlg(m_pInspData, fpInsRepInfo[0], fpInsRepInfo[1], fpInsRepInfo[2], fpInsRepInfo[3]));
	m_pInsReport = gWindow->GetInsReportDlg();

	QScrollArea* s = new QScrollArea();
	s->setWindowIcon(QIcon("../icons/AccuRep.png"));
	s->setWindowTitle(QString::fromLocal8Bit("��ⱨ��"));
	s->setGeometry(100, 80, 1260, 900);
	s->setFixedSize(1285, 900);
	s->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	s->setWidget(m_pInsReport);

	s->setStyleSheet(
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
		"background:rgba(232,232,236,100%);"
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
		"background:rgba(232,232,236,100%);"
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
		"background:rgba(232,232,236,100%);"
		"}"
	);

	// �����ⱨ��
	m_pInsReport->JL_SaveInsReport(InspMonth, IsWS119);

}

void InspReportInfoDlg::onConfirm()
{
	QString InputPartNumber = m_pPartNumberLine->text();
	QString InputInspStaff = m_pInspStaffLine->text();
	QString InputEquipment = m_pInspEquipmentLine->text();
	QString InputOtherData = m_pOtherDataLine->document()->toPlainText();

	gDlgSetting->setValue("PartNumber", InputPartNumber);
	gDlgSetting->setValue("InspStaff", InputInspStaff);
	gDlgSetting->setValue("InspEquipment", InputEquipment);
	gDlgSetting->setValue("OtherData", InputOtherData);

	if (InputPartNumber.isEmpty() || InputInspStaff.isEmpty() || InputEquipment.isEmpty() || InputOtherData.isEmpty())
	{
		QMessageBox::warning(
			QApplication::activeWindow(), QObject::tr("Caution"), QString::fromLocal8Bit("����δ����ı�����Ŀ��"));
		return;
	}

	gWindow->SetInsReportDlg(new FInsReportDlg(m_pInspData, InputPartNumber, InputInspStaff, InputEquipment, InputOtherData));
	m_pInsReport = gWindow->GetInsReportDlg();

	//������д���ĵ�
	//QFile file(DefaultStrPath);
	//if (!file.open(QIODevice::WriteOnly)) {
	//	QMessageBox::information(gWindow, QString::fromLocal8Bit("д�����,ERROR!"),
	//		QString::fromLocal8Bit("�ļ�����д�룡"), "OK");
	//	return;
	//}
	//else {
	//	QTextStream stream(&file);

	//	if (InputPartNumber.isEmpty())
	//	{
	//		stream << "NULL" << endl;
	//	}
	//	else
	//	{
	//		stream << InputPartNumber << endl;
	//	}

	//	if (InputInspStaff.isEmpty())
	//	{
	//		stream << "NULL" << endl;
	//	}
	//	else
	//	{
	//		stream << InputInspStaff << endl;
	//	}

	//	if (InputEquipment.isEmpty())
	//	{
	//		stream << "NULL" << endl;
	//	}
	//	else
	//	{
	//		stream << InputEquipment << endl;
	//	}

	//	if (InputOtherData.isEmpty())
	//	{
	//		stream << "NULL" << endl;
	//	}
	//	else
	//	{
	//		stream << InputOtherData << endl;
	//	}

	//	stream.flush();
	//	file.close();
	//}


	///////////////////////////////////////////////////////////��ͼ
	QScrollArea* s = new QScrollArea();
	s->setWindowIcon(QIcon("../icons/AccuRep.png"));
	s->setWindowTitle(QString::fromLocal8Bit("��ⱨ��"));
	s->setGeometry(100, 80, 1260, 900);
	s->setFixedSize(1285, 900);
	s->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	s->setWidget(m_pInsReport);

	s->setStyleSheet(
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
		"background:rgba(232,232,236,100%);"
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
		"background:rgba(232,232,236,100%);"
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
		"background:rgba(232,232,236,100%);"
		"}"
	);

	s->show();
	///////////////////////////////////////////////////////////////////////////////////////////
	close();
}

