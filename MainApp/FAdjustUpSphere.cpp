#include "qdir.h"
#include <String>
#include "FUtilities.h"
#include "GLU.h"
#include "MainDoc.h"
#include "FTHP.h"
#include "FMainWindow.h"
#include "FAdjustUpSphere.h"
#include <iomanip>
#include "FDataWnd.h"
#include <Eigen\Dense>
#include "qlayout.h"
#include "np.h"
#include <cmath>
#include "MeasPt.h"

using namespace Eigen;     // �ĳ�������� using Eigen::MatrixXd; 
using namespace std;

extern FMainWindow* gWindow;
extern QSettings* gDlgSetting;

static QString NC_CalibrationSaveDir;

FAdjustUpSphere::FAdjustUpSphere(QWidget* parent)
	/*:QMdiSubWindow(parent)*/
{
	QDir SaveDir = gAppPath;
	SaveDir.cdUp();
	SaveDir.cd("NC_Calibration");
	NC_CalibrationSaveDir = SaveDir.absolutePath();

	this->move(780, 133);
	m_D = gDlgSetting->value("CalibBallDiamater").toDouble();
	m_logNum = gDlgSetting->value("CalibLongNum").toInt();
	m_latNum = gDlgSetting->value("CalibLatNum").toInt();
	m_d = gDlgSetting->value("ProbeRedBallDiamater").toDouble();
	//IsMDC_DataRead = false;

	this->setWindowTitle(QString::fromLocal8Bit("�궨�ϰ���"));
	QPushButton* pGenPointsAndNCBtn = new QPushButton(QString::fromLocal8Bit("���ɱ궨NC"));

	m_pCreateBtn= new QPushButton(QString::fromLocal8Bit("���ɲ����ļ�"));
	m_pCreateBtn->setEnabled(0);
	QPushButton* pExitBtn = new QPushButton(QString::fromLocal8Bit("�˳�"));
	m_pDataBtn= new QPushButton(QString::fromLocal8Bit("����ʵ������"));
	m_pDataBtn->setEnabled(0);
	m_pDataTransfer = new QPushButton(QString::fromLocal8Bit("����NC������"));
	m_pDataTransfer->setEnabled(0);

	QLabel *label1=new QLabel(this);
	label1->setText(QString::fromLocal8Bit("������ֱ��"));
	m_diameterLineEdit=new QLineEdit(this); 	 
	m_diameterLineEdit->setText(QString::number(m_D));

	QLabel *label2=new QLabel(this);
	label2->setText(QString::fromLocal8Bit("���������������㣩"));//γ��Ȧ��
	m_logNumLineEdit=new	QLineEdit(this);
	m_logNumLineEdit->setText(QString::number(m_logNum));

	QLabel *label3=new QLabel(this);
	label3->setText(QString::fromLocal8Bit("ÿ�����"));//��������
	m_latNumLineEdit=new	QLineEdit(this);
	m_latNumLineEdit->setText(QString::number(m_latNum));

	QLabel *label4=new QLabel(this);
	//QFont ft;
	//ft.setBold(3);
	//label4->setFont(ft);
	//label4->setStyleSheet("color:red;");
	label4->setText(QString::fromLocal8Bit("�챦ʯֱ��"));
	//label4->setToolTip(QString::fromLocal8Bit("ע��SCENTER.SPF�ļ����40��ӦΪ\nR3=$AA_IW[Z]-R43-0.5d;0.283333\n��82��ӦΪ\nR3=$AA_IW[Z]-R43-0.5d;0.283333\ndΪ����ĺ챦ʯ��ֱ�����Ķ��챦ʯ��ֱ��ʱ����ע���޸Ĵ��ļ���"));
	m_dLineEdit=new QLineEdit(this); 	 
	m_dLineEdit->setText(QString::number(m_d));
	//m_dLineEdit->setFont(ft);
	//m_dLineEdit->setToolTip(QString::fromLocal8Bit("ע��SCENTER.SPF�ļ����40��ӦΪ\nR3=$AA_IW[Z]-R43-0.5d;0.283333\n��82��ӦΪ\nR3=$AA_IW[Z]-R43-0.5d;0.283333\ndΪ����ĺ챦ʯ��ֱ�����Ķ��챦ʯ��ֱ��ʱ����ע���޸Ĵ��ļ���"));

	QLabel *label5=new QLabel(this);
	QPixmap img("../icons/UpProbe.PNG");//б�߷���ͬ�Ͳ���ʾͼƬ
	label5->setAlignment(Qt::AlignCenter);
	label5->setPixmap(img);
	label5->setFixedSize(img.size());

	QTextCodec *code = QTextCodec::codecForName("gb2312");
	QHBoxLayout* mainHLayout = new QHBoxLayout(this);

	QGroupBox *ParamConfigBox=new QGroupBox(QString::fromLocal8Bit("��������"));
	QGridLayout* mainGridLayout1 = new QGridLayout;
	mainGridLayout1->addWidget(label1, 1, 0);  
	mainGridLayout1->addWidget(m_diameterLineEdit, 1, 1);  
	mainGridLayout1->addWidget(label2, 2, 0);  
	mainGridLayout1->addWidget(m_logNumLineEdit, 2, 1); 
	mainGridLayout1->addWidget(label3, 3, 0);  
	mainGridLayout1->addWidget(m_latNumLineEdit, 3, 1); 
	mainGridLayout1->addWidget(label4, 4, 0);  
	mainGridLayout1->addWidget(m_dLineEdit, 4, 1); 

	m_pProbeOnComdLabel = new QLabel(QString::fromLocal8Bit("��ͷ����ָ��"));
	m_pProbeOnComdLine = new QLineEdit(this);
	m_pProbeOnComdLine->setText(gDlgSetting->value("ProbeOnCmd").toString());
	m_pProbeOffComdLabel = new QLabel(QString::fromLocal8Bit("��ͷ�ر�ָ��"));
	m_pProbeOffComdLine = new QLineEdit(this);
	m_pProbeOffComdLine->setText(gDlgSetting->value("ProbeOffCmd").toString());

	mainGridLayout1->addWidget(m_pProbeOnComdLabel, 5, 0);
	mainGridLayout1->addWidget(m_pProbeOnComdLine, 5, 1);
	mainGridLayout1->addWidget(m_pProbeOffComdLabel, 6, 0);
	mainGridLayout1->addWidget(m_pProbeOffComdLine, 6, 1);

	ParamConfigBox->setLayout(mainGridLayout1);

	QGroupBox *PictureBox=new QGroupBox(QString::fromLocal8Bit("��ʼλ��"));
	QHBoxLayout *Hbox5 = new QHBoxLayout;
	Hbox5->addWidget(label5);
	Hbox5->addStretch(1);
	Hbox5->setAlignment(Qt::AlignCenter);
	PictureBox->setLayout(Hbox5);

	QGroupBox *ProcessAfterBox=new QGroupBox(QString::fromLocal8Bit("�궨����"));
	QGridLayout*Hbox7 = new QGridLayout;

	Hbox7->addWidget(pGenPointsAndNCBtn);
	Hbox7->addWidget(m_pDataTransfer);
	Hbox7->addWidget(m_pDataBtn);
	Hbox7->addWidget(m_pCreateBtn);
	Hbox7->addWidget(pExitBtn);
	//Hbox7->addStretch(1);
	ProcessAfterBox->setLayout(Hbox7);

	mainHLayout->addWidget(PictureBox);
	mainHLayout->addWidget(ParamConfigBox);
	mainHLayout->addWidget(ProcessAfterBox);
	//this->setLayout(mainHLayout);

	connect(pGenPointsAndNCBtn,SIGNAL(clicked()),this,SLOT(OnGenPointsAndNC()));
	connect(m_pCreateBtn,SIGNAL(clicked()),this,SLOT(OnAdjustProbeCreate()));
	connect(pExitBtn,SIGNAL(clicked()),this,SLOT(OnAdjustProbeQuit()));
	connect(m_pDataBtn,SIGNAL(clicked()),this,SLOT(OnImportData()));
	connect(m_pDataTransfer, SIGNAL(clicked()), this, SLOT(OnDataTransfer()));

}

void FAdjustUpSphere::ComputeInspectPnts(double D, int latNum, int longNum)
{
	gWindow->GetMainDoc()->clearPntsAll();

	//W.clear();
	//double w=1;
	//W.push_back(w);


	double orirad=D/2;	//radius of the calibration sphere
	double rad=D/2;		//radius of longitude circles
	double latStep=2*M_PI/latNum;	//angle step in latitude direction
	double longStep=(M_PI/2)/longNum;	//angle step in longitude direction
	//�Ӷ���
	FIpPoint tPnt;	
	//initial is the north point
	tPnt.x=0; 	tPnt.y=0;	tPnt.z=orirad;
	tPnt.deg=0; tPnt.degz=1.5708;  //1.5708=0.5pai   deg��x��нǣ�degz����XOYƽ��ļнǣ����㴦��90�ȡ�
	tPnt.i = 0; tPnt.j = 0; tPnt.k= 1;
	FIpPntList pointList;
	pointList.push_back(tPnt);

	MeasPt* showPnt = new MeasPt(tPnt);
	gWindow->GetMainDoc()->AddDispEnt(showPnt);
	//gWindow->GetMainDoc()->getPntsShowList().push_back(showPnt);

	for (int k=1;k<=longNum;k++)	//0 degree starts from z-axis
	{
		rad=orirad*cos(0.5*M_PI-longStep*k);	
		for(int i=1;i<=latNum;i++){	//0 degree starts from x-axis
			tPnt.x=rad*cos(latStep*(i-1));
			tPnt.y=rad*sin(latStep*(i-1));
			tPnt.z=orirad*sin(0.5*M_PI-longStep*k);
			tPnt.deg=latStep*(i-1);
			tPnt.degz=(0.5*M_PI-longStep*k);
			tPnt.i = tPnt.x / orirad;
			tPnt.j = tPnt.y / orirad;
			tPnt.k = tPnt.z / orirad;
			if (abs(tPnt.x) <= 0.0001) { tPnt.x = 0; }
			if (abs(tPnt.y) <= 0.0001) { tPnt.y = 0; }
			if (abs(tPnt.z) <= 0.0001) { tPnt.z = 0; }
			pointList.push_back(tPnt);
			//w=1/(1-cos(tPnt.degz)/(10*D));
			//W.push_back(w);
			//cout<<"w="<<w<<endl;

			showPnt = new MeasPt(tPnt);
			gWindow->GetMainDoc()->AddDispEnt(showPnt);
			//gWindow->GetMainDoc()->getPntsShowList().push_back(showPnt);

		}
	}
	
	gWindow->GetMainDoc()->ClearIpPntList();
	gWindow->GetMainDoc()->SetIpPntList(pointList, 0.5);

}

void FAdjustUpSphere::GenerateNCFile2D(double D, int latNum, int longNum)
{
	QString ipResult = gAppSetting->value("PARA/Distance").toString();  
	bool ok;
	double initDist= ipResult.toDouble(&ok);	//������ʼ����
	double rad=D/2+initDist;					//������ʼ�뾶
	double ddxx,ddyy,ddzz;						//������ʼ��
	QString dxx(""),dyy(""),dzz("");			//����ת��
	double ddeg1=2*M_PI/latNum;					//γ�򲽽��Ƕ�
	double ddeg2=M_PI/2/longNum;					//���򲽽��Ƕ�
	QString a;	//total string which are written to ProbeUpperSphere.mpf file
	QString b;
	QString DataIn[7];
	FTHP code1(b);

	gDlgSetting->setValue("ProbeOnCmd", m_pProbeOnComdLine->text());
	gDlgSetting->setValue("ProbeOffCmd", m_pProbeOffComdLine->text());

	a = code1.GetSaveToMachineToolLocalHead() + code1.TH_GetHead();
	//a = code1.TH_GetHead();

	QString MCH=code1.TH_GetProfileString("MCH","MCH_Type");	//��������
	QString NCS=code1.TH_GetProfileString("NCS","NC_Type");		//��������
	if((NCS!="FDIA3")&&(NCS!="FDIA5"))
	{
		DataIn[0]=code1.TH_GetProfileString(NCS,NCS+"_CONST0");	//F1(�ƶ��ٶ�)
		DataIn[1]=code1.TH_GetProfileString(NCS,NCS+"_CONST1");	//F2(�����ٶ�)
		DataIn[2]=code1.TH_GetProfileString(NCS,NCS+"_CONST2");	//F3(�ֲ��ٶ�/�����ٶ�)
		DataIn[3]=code1.TH_GetProfileString(NCS,NCS+"_CONST3");	//R(�궨��뾶)
		DataIn[4]=code1.TH_GetProfileString(NCS,NCS+"_CONST4");	//�궨��������X
		DataIn[5]=code1.TH_GetProfileString(NCS,NCS+"_CONST5");	//�궨��������Y
		DataIn[6]=code1.TH_GetProfileString(NCS,NCS+"_CONST6");	//�궨��������Z
	}

	QString dR;	//R
	QString csX,csY,csZ,csDX,csDY,csDZ,csL,csE,csR;
	//data[0].number(pPointtrue.x,'f',6);
	if((NCS!="FDIA3")&&(NCS!="FDIA5"))
	{
		dR = QString::number(D/2,'f',4);
		a+=DataIn[3]+"="+dR+"\n";
	}

	if((MCH=="5AxisAB")||(MCH=="5AxisBA"))
	{
		if((NCS!="FDIA3")&&(NCS!="FDIA5"))
		{
			a+="G01 G90 X0.0 Y0.0 Z5.0 A0 B0 F2000\n";
		}
		else
		{
			a+="G01 G90 X0.0 Y0.0 Z50.0 A0 B0 F2000\n";
		}
	}
	else if(MCH=="5AxisAC")
	{
		if((NCS!="FDIA3")&&(NCS!="FDIA5"))
		{
			a+="G01 G90 X0.0 Y0.0 Z5.0 C0 A0 F2000\n";
		}
		else
		{
			a+="G01 G90 X0.0 Y0.0 Z50.0 C0 A0 F2000\n";
		}
	}
	else if(MCH=="5AxisBC")
	{
		if((NCS!="FDIA3")&&(NCS!="FDIA5"))
		{
			//a+="G01 G90 X0.0 Y0.0 Z5.0 C0 B0 F2000\n";//ԭʼ

			a += "G01 G90 X0.0 Y0.0 Z5.0 F2000\n";//627��ʽ������C��
		}
		else
		{
			a+="G01 G90 X0.0 Y0.0 Z50.0 C0 B0 F2000\n";
		}
	}
	else
	{
		if((NCS!="FDIA3")&&(NCS!="FDIA5"))
		{
			a+="G01 G90 X0.0 Y0.0 Z5.0 F2000\n";
		}
		else
		{
			a+="G01 G90 X0.0 Y0.0 Z50.0 F2000\n";
		}
	}
	a+=code1.TH_GetCenter();
	csZ=csZ.number(rad,'f',4);
	if((NCS=="FDIA3")||(NCS=="FDIA5"))
	{
		a+="G01 G90 X0.0 Y0.0 Z"+csZ+" F2000\n";
	}
	else if((NCS=="FNUC3")||(NCS=="FNUC5"))
	{
		a+="G01 G90 X"+DataIn[4];
		a+=" Y"+DataIn[5];
		a+=" Z["+csZ;
		a+="+"+DataIn[6];
		a+="] F"+DataIn[0]+"\n";
	}
	else
	{
		a+="G01 G90 X="+DataIn[4];
		a+=" Y="+DataIn[5];
		a+=" Z="+csZ;
		a+="+"+DataIn[6];
		a+=" F="+DataIn[0]+"\n";
	}
	//�Ӷ���
	double ddii=0;	//i
	double ddjj=0;	//j
	double ddkk=1;	//k
	a+=code1.TH_GetProb(ddii,ddjj,ddkk,D);
	csZ=QString::number(rad,'f',4);
	if((NCS=="FDIA3")||(NCS=="FDIA5"))
	{
		a+="G01 G90 X0.0 Y0.0 Z"+csZ+" F2000\n";
	}
	else if((NCS=="FNUC3")||(NCS=="FNUC5"))
	{
		a+="G01 G90 X"+DataIn[4];
		a+=" Y"+DataIn[5];
		a+=" Z["+csZ;
		a+="+"+DataIn[6];
		a+="] F"+DataIn[0]+"\n";
	}
	else
	{
		a+="G01 G90 X="+DataIn[4];
		a+=" Y="+DataIn[5];
		a+=" Z="+csZ;
		a+="+"+DataIn[6];
		a+=" F="+DataIn[0]+"\n";
	}

	double deg1=0,deg2=M_PI/2;	            //��ʼ�Ƕ�
	double ddij;	                        //XYƽ������ͶӰ
	QString Adeg(""),Bdeg(""),Cdeg("");	    //A,B,C
	for (int k=1;k<=longNum;k++)
	{
		deg2-=ddeg2;	                     //fai, start from z-axis
		ddkk=sin(deg2);
		ddij=cos(deg2);	                    //ͶӰ����

		dxx=QString::number(rad*ddij,'f',4);
		dzz=QString::number(rad*ddkk,'f',4);
		if((NCS=="FDIA3")||(NCS=="FDIA5"))
		{
			a+="G01 G90 X"+dxx+" F2000\n";
			a+="G01 G90 Z"+dzz+" F2000\n";
		}
		else if((NCS=="FNUC3")||(NCS=="FNUC5"))
		{
			a+="G01 G90 X["+dxx;
			a+="+"+DataIn[4];
			a+="] F"+DataIn[0]+"\n";
			a+="G01 G90 Z["+dzz;
			a+="+"+DataIn[6];
			a+="] F"+DataIn[0]+"\n";
		}
		else
		{
			a+="G01 G90 X="+dxx;
			a+="+"+DataIn[4];
			a+=" F="+DataIn[0]+"\n";
			a+="G01 G90 Z="+dzz;
			a+="+"+DataIn[6];
			a+=" F="+DataIn[0]+"\n";
		}

		deg1=0;
		ddii=ddij*cos(deg1);
		ddjj=ddij*sin(deg1);
		for(int i=1;i<=latNum;i++)
		{
			a+=code1.TH_GetProb(ddii,ddjj,ddkk,D);
			dxx=QString::number(rad*ddii,'f',4);
			dyy=QString::number(rad*ddjj,'f',4);
			dzz=QString::number(rad*ddkk,'f',4);
			if((NCS=="FDIA3")||(NCS=="FDIA5"))
			{
				a+="G01 X"+dxx;
				a+=" Y"+dyy;
				a+=" Z"+dzz+" F2000\n";
			}
			else if((NCS=="FNUC3")||(NCS=="FNUC5"))
			{
				a+="G01 G90 X["+dxx;
				a+="+"+DataIn[4];
				a+="] Y["+dyy;
				a+="+"+DataIn[5];
				a+="] Z["+dzz;
				a+="+"+DataIn[6];
				a+="] F"+DataIn[0]+"\n";
			}
			else
			{
				a+="G01 G90 X="+dxx;
				a+="+"+DataIn[4];
				a+=" Y="+dyy;
				a+="+"+DataIn[5];
				a+=" Z="+dzz;
				a+="+"+DataIn[6];
				a+=" F="+DataIn[0]+"\n";
			}

			deg1+=ddeg1;
			ddii=ddij*cos(deg1);
			ddjj=ddij*sin(deg1);
			dxx=QString::number(rad*ddii,'f',4);
			dyy=QString::number(rad*ddjj,'f',4);

			if((NCS=="FDIA3")||(NCS=="FDIA5"))
			{
				a+="G03 X"+dxx;
				a+=" Y"+dyy;
				//dR.Format("%.4f",-ddij*rad);
				dR=QString::number(-ddij*rad,'f',4);
				a+=" R"+dR+" F2000\n";
			}
			else if((NCS=="FNUC3")||(NCS=="FNUC5"))
			{
				a+="G03 G90 G17 X["+dxx;
				a+="+"+DataIn[4];
				a+="] Y["+dyy;
				a+="+"+DataIn[5];
				//dR.Format("%.4f",ddij*rad);
				dR=QString::number(ddij*rad,'f',4);
				a+="] R"+dR;
				a+=" F"+DataIn[0]+"\n";
			}
			else
			{
				a+="G03 G90 G17 X="+dxx;
				a+="+"+DataIn[4];
				a+=" Y="+dyy;
				a+="+"+DataIn[5];
				dR=QString::number(ddij*rad,'f',4);
				a+=" CR="+dR;
				a+=" F="+DataIn[0]+"\n";
			}

		}

	}
	//csZ.Format("%.4f",rad);
	csZ=QString::number(rad,'f',4);

	if((NCS=="FDIA3")||(NCS=="FDIA5"))
	{
		a+="G01 G90 X"+csZ+" F2000\n";
		a+="G01 G90 Z"+csZ+" F2000\n";
	}
	else if((NCS=="FNUC3")||(NCS=="FNUC5"))
	{
		a+="G01 G90 X["+csZ;
		a+="+"+DataIn[4];
		a+="] F"+DataIn[0]+"\n";
		a+="G01 G90 Z["+csZ;
		a+="+"+DataIn[6];
		a+="] F"+DataIn[0]+"\n";
	}
	else
	{
		a+="G01 G90 X="+csZ;
		a+="+"+DataIn[4];
		a+=" F="+DataIn[0]+"\n";
		a+="G01 G90 Z="+csZ;
		a+="+"+DataIn[6];
		a+=" F="+DataIn[0]+"\n";
	}

	a.replace("SPROBE\n", "SPROBE\n" + code1.GetSaveToMachineToolLocalPnt());
	a += code1.TH_GetTail() + code1.GetSaveToMachineToolLocalLast();

	QString filename = NC_CalibrationSaveDir + "/ProbeUpperNC.mpf";

	// ���ɼ�����������Meas֮ǰ��ͣ
	code1.setIsPauseBeforeMeas(false);
	code1.ProcessAndSaveNC(a, filename);

}

void FAdjustUpSphere::OnGenPointsAndNC()
{
	//step1. read parameters
	m_D=m_diameterLineEdit->text().toDouble();
	m_latNum=m_latNumLineEdit->text().toInt();
	m_logNum=m_logNumLineEdit->text().toInt();
	m_d=m_dLineEdit->text().toInt();

	// �����������ݵ������ļ�
	gDlgSetting->setValue("CalibBallDiamater", m_D);
	gDlgSetting->setValue("CalibLatNum", m_latNum);
	gDlgSetting->setValue("CalibLongNum", m_logNum);
	gDlgSetting->setValue("ProbeRedBallDiamater", m_d);
	gDlgSetting->setValue("ProbeOnCmd", m_pProbeOnComdLine->text());
	gDlgSetting->setValue("ProbeOffCmd", m_pProbeOffComdLine->text());

	// �������ݵ� Doc
	gWindow->GetMainDoc()->setProbeDia(m_dLineEdit->text().toDouble());
	gWindow->GetMainDoc()->setRefDia(m_diameterLineEdit->text().toDouble());
	gWindow->GetMainDoc()->setNbLongitudePnt(m_logNumLineEdit->text().toInt());
	gWindow->GetMainDoc()->setNbLatitudePnt(m_latNumLineEdit->text().toInt());

	//MesgBox(QString::fromLocal8Bit("�ɹ����ú챦ʯ��ֱ��Ϊ%1����ȡ����ֱ��Ϊ%2").arg(m_dLineEdit->text().toDouble()).arg(gWindow->GetMainDoc()->getProbeDia()));

	//step2. Generate calibration points
	ComputeInspectPnts(m_D, m_latNum, m_logNum);

	//step3. Generate NC file
	GenerateNCFile2D(m_D, m_latNum, m_logNum);

	gWindow->GetMainDoc()->CreateCalSphere(m_D/2);	//import the probe model
	m_pDataTransfer->setEnabled(1);
	gWindow->GetMainDoc()->AdjustProbeFlag = 1;
}

FAdjustUpSphere::~FAdjustUpSphere(void)
{

}

void FAdjustUpSphere::OnAdjustProbeCreate() 
{
	//gWindow->GetMainDoc()->SetIpPntList(gWindow->GetMainDoc()->GetIpPntList());
	FIpPntList mesPointsList, theoPntList;
	mesPointsList=gWindow->GetMainDoc()->GetMesPntList();
	theoPntList=gWindow->GetMainDoc()->GetIpPntList();
	int mesNum=mesPointsList.size();
	int theoNum=theoPntList.size();
	
	float x=0;
	float y=0;
	float z=0;
	float err=0;
	float deg=0;
	float degz=0;
	char   c = '\n';
	if (mesNum!=theoNum)
	{
		QMessageBox::information(this,QString::fromLocal8Bit("���������ݺ����۵����ݲ�ƥ��,ERROR!"),
			QString::fromLocal8Bit("���������ݺ����۵����ݲ�ƥ��,ERROR!"),"OK");
	}
	else
	{//�����궨�ļ���ѭ��Ƕ�ף���Ƕ��Ϊij����ת���ĽǶȣ���Ƕ��Ϊk����ת���ĽǶ�		
		QString nameWithPath = gAppPath+QString("/settings/nc/ErrUpperSphere.txt");
		FILE * file=fopen(nameWithPath.toLocal8Bit().data(),"w");
		fprintf(file, "%i %i%c",m_latNum,m_logNum,c);

		/*vector<double> T;
         T= WeightLSM(mesPointsList,W); */       

	/*	 cout<<T[0]<<"   "<<T[1]<<"   "<<T[2]<<"   "<<T[3]<<endl;*/

	 for (int i=0;i<=m_logNum*m_latNum;i++)//
	 {
		x=mesPointsList[i].x- theoPntList[i].x;
		y=mesPointsList[i].y- theoPntList[i].y;
		z=mesPointsList[i].z- theoPntList[i].z;
		double dd=pow(x,2)+pow(y,2)+pow(z,2);
		err=pow(dd,0.5);
		if (err < 0.000001)
		{
			err = 0;
		}
		 cout<<mesPointsList[i].x<<"   "<<mesPointsList[i].y<<"   "<<mesPointsList[i].z<<"   "<<x<<"   "<<y<<"   "<<z<<endl;
			deg=theoPntList[i].deg;
			degz=theoPntList[i].degz;
			if (i==0)    
			{
					fprintf(file, "%f %f %f%c",err,deg,degz,c);
					/*cout<<i<<"    "<<err<<"   "<<deg<<"   "<<degz<<endl;*/
			} 
			else
			{
				fprintf(file, "%f %f %f%c",err,deg,degz,c);
				/*cout<<i<<"    "<<err<<"   "<<deg<<"   "<<degz<<endl;*/
			}
			mesPointsList[i].err = err;

			/*m_p2pDoc->addToMesErrList(i + 1, err);*/
			//gWindow->GetMainDoc()->addToMesErrList(i, err);
	 }	

		fclose(file);

		QMessageBox::information(this,QString::fromLocal8Bit("�ϰ���궨�ļ����ɣ�"),
			QString::fromLocal8Bit("�ϰ���궨�ļ����ɣ�"),"OK");
	}
}

vector<double> FAdjustUpSphere::WeightLSM( FIpPntList FP,vector<double> S)
{
	int Num=FP.size();
	Vector4f X=Vector4f ::Zero();
	Vector4f Y=Vector4f ::Zero();
	MatrixXf A(4, 4);
	double m1=0,m2=0,m3=0,m4=0,m5=0,m6=0,m7=0,m8=0,m9=0;

	for (int i = 0; i <Num; i++)
    {
        Y[0] += pow(S[i],3)*(pow(FP[i].x,3) + pow(FP[i].y,2)*FP[i].x+ pow(FP[i].z,2)*FP[i].x);
        Y[1] += pow(S[i],3)*(pow(FP[i].y,3) + pow(FP[i].x,2)*FP[i].y+ pow(FP[i].z,2)*FP[i].y);
        Y[2] += pow(S[i],3)*(pow(FP[i].z,3) + pow(FP[i].x,2)*FP[i].z+ pow(FP[i].y,2)*FP[i].z);    
        Y[3] -= pow(S[i],2)*(pow(FP[i].x,2) + pow(FP[i].y,2) + pow(FP[i].z,2));
    }
		
	for (int j = 0;j<Num;j++)
    {    
        m1+= S[j]*pow(FP[j].x,2); 
        m2 += S[j]*FP[j].x*FP[j].y;
        m3 += S[j]*FP[j].x*FP[j].z;
        m4-= S[j]*FP[j].x;
        m5 += S[j]*pow(FP[j].y,2); 
        m6 += S[j]*FP[j].y*FP[j].z;
        m7 -= S[j]*FP[j].y;
        m8 += S[j]*pow(FP[j].z,2);
        m9 -= S[j]*FP[j].z;
    }

	A(0,0) = m1;
    A(0,1) = m2;
    A(0,2) = m3;
    A(0,3) = m4;
    A(1,1) = m5;
    A(1,2) = m6;
    A(1,3) = m7;
    A(2,2) = m8;
    A(2,3) = m9;

    A(1,0) = A(0,1);
    A(2,0) = A(0,2);
    A(2,1) = A(1,2);
    A(3,0) = A(0,3);
    A(3,1) = A(1,3);
    A(3,2) = A(2,3);    
    A(3,3) = Num;

	cout<< A(0,0)<<"   "<<A(0,1)<<"   "<<A(0,2)<<"   "<<A(0,3)<<endl;
	cout<< A(1,0)<<"   "<<A(1,1)<<"   "<<A(1,2)<<"   "<<A(1,3)<<endl;
	cout<< A(2,0)<<"   "<<A(2,1)<<"   "<<A(2,2)<<"   "<<A(2,3)<<endl;
	cout<< A(3,0)<<"   "<<A(3,1)<<"   "<<A(3,2)<<"   "<<A(3,3)<<endl;
	
    double m=5;
	m=A.determinant();
	cout<<m<<endl;
	
	vector<double> LSM;
	if(m==0)printf("���󲻿���");

	else{
		 MatrixXf IA(4, 4);
		 IA=A.inverse();
    X=IA*Y; 
	double x0= X (0,0)/2;
	double y0=X (1,0)/2;
	double z0=X(2,0)/2;
	double R=pow(pow(x0,2)+pow(y0,2)+pow(z0,2)-X(3,0),0.5);

	/*cout<< IA(0,0)<<"   "<<IA(1,0)<<"   "<<IA(2,0)<<"   "<<IA(3,0)<<endl;*/
	
	cout<<"x0="<<x0<<"   "<<"y0="<<y0<<"   "<<"z0="<<z0<<"   "<<"   "<<"R="<<R<<endl;



	LSM.clear();
	LSM.push_back(x0);
	LSM.push_back(y0);
	LSM.push_back(z0);
	LSM.push_back(R);
	}
	return LSM;
	
 
}

void FAdjustUpSphere::OnAdjustProbeQuit() 
{
	close();

	if (gWindow->GetMainDoc())
	{
		gWindow->GetMainDoc()->clear();
	}
}

void FAdjustUpSphere::OnImportData() 
{
	CSDataTrans dt = CSDataTrans();

	if (dt.txtFileReader(gWindow->GetMainDoc(), 0, m_d))
	{
		m_pCreateBtn->setEnabled(1);
	}
	
}

void FAdjustUpSphere::OnDataTransfer()
{
	// ���ļ���
	QDesktopServices::openUrl(QUrl::fromLocalFile(NC_CalibrationSaveDir));

	// ����������Ϣ��
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Warning);
	msgBox.setText(QString::fromLocal8Bit("��ʹ���ļ�����ϵͳ���궨��NC���봫����������ִ�У�NCִ��ǰ��Ҫ��ͼ���趨��׼�����ͷ�����λ�ã�������λ�ô��Ļ����������������Ϊ�㡣"));
	msgBox.setStandardButtons(QMessageBox::Ok);
	// ������Ϣ��ʼ��λ����������֮��
	msgBox.setWindowFlags(msgBox.windowFlags() | Qt::WindowStaysOnTopHint);
	msgBox.exec();

	m_pDataBtn->setEnabled(1);

}
