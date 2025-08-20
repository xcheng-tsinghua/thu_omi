#include<String>
#include "FUtilities.h"
#include "GLU.h"
#include "MainDoc.h"
#include "FTHP.h"
#include "qdir.h"
#include "FAdjustAngleProbe.h"
#include "QPixmap"
#include <QImageReader>
#include <QDebug>
#include <iostream>
#include <Eigen\Dense>

//导入的球模型的半径是按第一次输入的，之后再更改球半径，模型尺寸不会变，需要重新编译后才能。要是先输入了大球直径再输入小的会看不到测点//
//如果把这个if给注释掉，每次点退出后再输入不同直径就会生成不同的模型
//void FMainDoc::CreateCalSphere(double radius)
//{
//	if(!m_pCalSphere)
//	{
//		m_pCalSphere = new FSphere(radius);
//		m_pCalSphere->Update();
//	}
//	AddDispEnt(m_pCalSphere);
//}

using namespace Eigen;     // 改成这样亦可 using Eigen::MatrixXd; 
using namespace std;
//////////////////////////////////////////////////////////////////////////

extern FMainWindow* gWindow;

FAdjustAngleProbe::FAdjustAngleProbe(QWidget* parent)
{
	m_D = 30;
	m_logNum = 3;
	m_latNum = 8;
	m_probeAngle = 90;
	m_startAngle = 0.0;
	m_d = 3;
	this->setWindowTitle(QString::fromLocal8Bit("标定固定摆角"));

	QPushButton* pStartBtn = new QPushButton(QString::fromLocal8Bit("生成理论测点列表标定及NC程序"));
	m_pCreateBtn = new QPushButton(QString::fromLocal8Bit("生成测头补偿文件"));
	m_pCreateBtn->setEnabled(0);
	QPushButton* pExitBtn = new QPushButton(QString::fromLocal8Bit("退出"));
	m_pDataBtn = new QPushButton(QString::fromLocal8Bit("导入实测数据"));
	m_pDataBtn->setEnabled(0);
	m_pTCPConnect = new QPushButton(QString::fromLocal8Bit("使用TCP/IP方式连接机床"));
	m_pOPCConnect = new QPushButton(QString::fromLocal8Bit("使用OPC UA方式连接机床"));
	m_pTCPConnect->setEnabled(0);
	m_pOPCConnect->setEnabled(0);

	QLabel* label1 = new QLabel(this);
	label1->setText(QString::fromLocal8Bit("输入标定球直径标定直径D"));//标定直径
	m_diameterLineEdit = new	QLineEdit(this);
	m_diameterLineEdit->setText(QString::number(m_D));

	QLabel* label2 = new QLabel(this);
	label2->setText(QString::fromLocal8Bit("径向测量点数（不含极点）"));//纬向圈数
	m_logNumLineEdit = new	QLineEdit(this);
	m_logNumLineEdit->setText(QString::number(m_logNum));

	QLabel* label3 = new QLabel(this);
	label3->setText(QString::fromLocal8Bit("纬向测量点数"));//经向条数
	m_latNumLineEdit = new	QLineEdit(this);
	m_latNumLineEdit->setText(QString::number(m_latNum));


	QLabel* label4 = new QLabel(this);
	label4->setText(QString::fromLocal8Bit("probe摆角"));
	m_probeAngleLineEdit = new	QLineEdit(this);
	m_probeAngleLineEdit->setText(QString::number(m_probeAngle));


	QLabel* label5 = new QLabel(this);
	label5->setText(QString::fromLocal8Bit("start转角"));
	m_startAngleLineEdit = new	QLineEdit(this);
	m_startAngleLineEdit->setText(QString::number(m_startAngle));

	QLabel* label6 = new QLabel(this);
	label6->setText(QString::fromLocal8Bit("红宝石直径d"));
	m_dLineEdit = new	QLineEdit(this);
	m_dLineEdit->setText(QString::number(m_d));


	QLabel* label7 = new QLabel(this);
	QPixmap img("images/Tools/AngleProbe.PNG");//斜线方向不同就不显示图片
	label7->setPixmap(img);


	QTextCodec* code = QTextCodec::codecForName("gb2312");
	QGridLayout* mainGridLayout = new QGridLayout(this);
	QGroupBox* createFirstExclusiveGroup = new QGroupBox(QString::fromLocal8Bit("标定前操作"));
	QHBoxLayout* Hbox1 = new QHBoxLayout;
	Hbox1->addWidget(pStartBtn);
	//Hbox1->addWidget(pDataBtn);
	//Hbox1->addWidget(m_pCreateBtn);
	//Hbox1->addWidget(pExitBtn);
	Hbox1->addStretch(1);
	createFirstExclusiveGroup->setLayout(Hbox1);

	QGroupBox* createSecondExclusiveGroup = new QGroupBox(QString::fromLocal8Bit("测点分布数据设定"));
	QGridLayout* mainGridLayout1 = new QGridLayout;
	mainGridLayout1->addWidget(label1, 1, 0);
	mainGridLayout1->addWidget(m_diameterLineEdit, 1, 1);
	mainGridLayout1->addWidget(label2, 2, 0);
	mainGridLayout1->addWidget(m_logNumLineEdit, 2, 1);
	mainGridLayout1->addWidget(label3, 3, 0);
	mainGridLayout1->addWidget(m_latNumLineEdit, 3, 1);
	mainGridLayout1->addWidget(label4, 4, 0);
	mainGridLayout1->addWidget(m_probeAngleLineEdit, 4, 1);
	mainGridLayout1->addWidget(label5, 5, 0);
	mainGridLayout1->addWidget(m_startAngleLineEdit, 5, 1);
	mainGridLayout1->addWidget(label6, 6, 0);
	mainGridLayout1->addWidget(m_dLineEdit, 6, 1);
	createSecondExclusiveGroup->setLayout(mainGridLayout1);

	QGroupBox* createThirdExclusiveGroup = new QGroupBox(QString::fromLocal8Bit("图例"));
	QHBoxLayout* Hbox5 = new QHBoxLayout;
	Hbox5->addWidget(label7);
	Hbox5->addStretch(1);
	createThirdExclusiveGroup->setLayout(Hbox5);

	QGroupBox* createFourthExclusiveGroup = new QGroupBox(QString::fromLocal8Bit("连接操作"));
	QHBoxLayout* Hbox6 = new QHBoxLayout;
	Hbox6->addWidget(m_pTCPConnect);
	Hbox6->addWidget(m_pOPCConnect);
	Hbox6->addStretch(1);
	createFourthExclusiveGroup->setLayout(Hbox6);

	QGroupBox* createFifthExclusiveGroup = new QGroupBox(QString::fromLocal8Bit("标定后处理"));
	QHBoxLayout* Hbox7 = new QHBoxLayout;
	Hbox7->addWidget(m_pDataBtn);
	Hbox7->addWidget(m_pCreateBtn);
	Hbox7->addWidget(pExitBtn);
	Hbox7->addStretch(1);
	createFifthExclusiveGroup->setLayout(Hbox7);

	mainGridLayout->addWidget(createFirstExclusiveGroup, 3, 0);
	mainGridLayout->addWidget(createSecondExclusiveGroup, 1, 0);
	mainGridLayout->addWidget(createThirdExclusiveGroup, 2, 0);
	mainGridLayout->addWidget(createFourthExclusiveGroup, 4, 0);
	mainGridLayout->addWidget(createFifthExclusiveGroup, 5, 0);
	this->setLayout(mainGridLayout);


	connect(pStartBtn, SIGNAL(clicked()), this, SLOT(OnAdjustProbeStart()));
	connect(m_pCreateBtn, SIGNAL(clicked()), this, SLOT(OnAdjustProbeCreate()));
	connect(pExitBtn, SIGNAL(clicked()), this, SLOT(OnAdjustProbeQuit()));
	connect(m_pDataBtn, SIGNAL(clicked()), this, SLOT(OnImportData()));
	connect(m_pTCPConnect, SIGNAL(clicked()), this, SLOT(OnTCPConfig()));
	connect(m_pOPCConnect, SIGNAL(clicked()), this, SLOT(OnOPCUA()));


}


FAdjustAngleProbe::~FAdjustAngleProbe(void)
{
}


void FAdjustAngleProbe::OnAdjustProbeStart() 
{
	//step1. read parameters
	m_D=m_diameterLineEdit->text().toDouble();
	m_logNum=m_logNumLineEdit->text().toInt();
	m_latNum=m_latNumLineEdit->text().toInt();
	m_probeAngle=m_probeAngleLineEdit->text().toInt();
	m_startAngle=m_startAngleLineEdit->text().toInt();	
	m_d=m_dLineEdit->text().toInt();	

	//step2. Generate calibration points
	ComputeInspectPnts(m_D/2, m_logNum, m_latNum,m_probeAngle,m_startAngle);

	//step3. Generate NC file
	GenerateNCFile2D(m_D, m_logNum, m_latNum,m_probeAngle,m_startAngle);
	gWindow->GetMainDoc()->CreateCalSphere(m_D/2);	//import the probe model
	//m_pCreateBtn->setEnabled(1);
	m_pTCPConnect->setEnabled(1);
	m_pOPCConnect->setEnabled(1);
	gWindow->GetMainDoc()->AdjustProbeFlag=1;

	
}
void FAdjustAngleProbe::ComputeInspectPnts(double R,int logNum, int latNum,float probeAngle,float startAngle)
{
	double orirad=R;                 //radius of calibration sphere
	double dddii,dddjj,dddkk,dddij;    //计算摆角时用于计算刀具法矢
	double deg1=0,deg2=0.5*M_PI;	       //起始角度
	double latStep=2*M_PI/latNum;       //纬向步进角度
	double logStep=0.5*M_PI/logNum;//经向步进角度

	double ddxx,ddyy,ddzz;			//测量起始点
	double ddij,ddk;	           //XY平面向量投影,Z轴投影
    double ddii,ddjj,ddkk;        	//i,j,k
    double txx,txy,txz,tyx,tyy,tyz,tzx,tzy,tzz;  //齐次坐标转换项

	
	double AA,BB,CC;//存放角度


	QString a,b;
	QString DataIn[7];

	FTHP code1(b);     //什么意思？？


	a=code1.TH_GetHead();
	QString MCH=code1.TH_GetProfileString("MCH","MCH_Type");	//机床类型
	
	FIpPoint tPnt;
    FIpPntList pointList;
	

	 	if(MCH=="5AxisBC")
	{
	 
	  AA=0.0;
	  BB=probeAngle/180*M_PI;
	  CC=startAngle/180*M_PI;
		
	}
		else if(MCH=="5AxisAB")
	{
	  
	  AA=probeAngle/180*M_PI;
	  BB=startAngle/180*M_PI;
	  CC=0.0;
	}
	
		else  if(MCH=="5AxisAC")
	{
	 
	  AA=probeAngle/180*M_PI;
	  BB=0.0;
	  CC=startAngle/180*M_PI;
	}


	  	 //CBA,矩阵相乘顺序
	txx=cos(BB)*cos(CC);
	txy=sin(AA)*sin(BB)*cos(CC)-cos(AA)*sin(CC);
	txz=cos(AA)*sin(BB)*cos(CC)+sin(AA)*sin(CC);

	 tyx=cos(BB)*sin(CC);
	 tyy=sin(AA)*sin(BB)*sin(CC)+cos(AA)*cos(CC);
	 tyz=cos(AA)*sin(BB)*sin(CC)-sin(AA)*cos(CC);

	  tzx=-sin(BB);
	  tzy=sin(AA)*cos(BB);
	  tzz=cos(AA)*cos(BB);

	 ////BCA,矩阵相乘顺序
	 // txx=cos(BB)*cos(CC);
	 // txy=-cos(AA)*cos(BB)*sin(CC)+sin(AA)*sin(BB);
	 // txz=sin(AA)*cos(BB)*sin(CC)+cos(AA)*sin(BB);

	 //    tyx=sin(CC);
		// tyy=cos(AA)*cos(CC);
		// tyz=-sin(AA)*cos(CC);

	 //tzx=-sin(BB)*cos(CC);
	 //tzy=cos(AA)*sin(BB)*sin(CC)+sin(AA)*cos(BB);
	 //tzz=-sin(AA)*sin(BB)*sin(CC)+cos(AA)*cos(BB);


	 /////BCB,矩阵相乘顺序
	 //txx=cos(BB)*cos(BB)*cos(CC)+sin(BB)*sin(BB);
	 //txy=-cos(BB)*sin(CC);
	 //txz=sin(BB)*cos(BB)*cos(CC)-sin(BB)*cos(BB);

	 //    tyx=cos(BB)*sin(CC);
		// tyy=cos(CC);
		// tyz=sin(BB)*sin(CC);

  //    tzx=sin(BB)*cos(BB)*cos(CC)-sin(BB)*cos(BB);

	 // tzy=-sin(BB)*sin(CC);
	 // tzz=sin(BB)*sin(BB)*cos(CC)+cos(BB)*cos(BB);
	 


	//加顶点
	//initial is the north point
	tPnt.x=txz*R;
	tPnt.y=tyz*R;
	tPnt.z=tzz*R;
	tPnt.deg=(startAngle); tPnt.degz=(0.5*M_PI-probeAngle);  //   轴线投影deg与x轴夹角，degz是与XOY平面的夹角。

	pointList.push_back(tPnt);

	for (int k=1;k<=logNum;k++)
	{
		deg2-=logStep;
		deg1=0;		
		for(int i=1;i<=latNum;i++){


		       ddij=cos(deg2);	//投影长度
		       ddk=sin(deg2);
		       ddii=ddij*cos(deg1);
		       ddjj=ddij*sin(deg1);
		       ddkk=ddk;

			FIpPoint tPnt ;
			tPnt.x=txx*R*ddii+txy*R*ddjj+txz*R*ddkk;
			tPnt.y=tyx*R*ddii+tyy*R*ddjj+tyz*R*ddkk;
			tPnt.z=tzx*R*ddii+tzy*R*ddjj+tzz*R*ddkk;
			tPnt.deg=startAngle-deg1;
			tPnt.degz=(0.5*M_PI-probeAngle-logStep*k);
  deg1+=latStep;
			pointList.push_back(tPnt);

			
		}
	
	}
	

	gWindow->GetMainDoc()->ClearIpPntList();
	gWindow->GetMainDoc()->SetIpPntList(pointList);

}

void FAdjustAngleProbe::GenerateNCFile2D(double D,int logNum, int latNum,float probeAngle,float startAngle)
{

	double R=D/2;    //标准球半径
 
     QString ipResult = gAppSetting->value("PARA/Distance").toString();  
   //  QString ipResult = gAppSetting->value("PARA/Distance").toString();  
	 
	 double initDist= ipResult.toDouble();    //测量起始距离
	
	double rad=R+initDist;				//测量起始半径
	double ddxx,ddyy,ddzz;			//测量起始点
	double ddii,ddjj,ddkk;	//i,j,k
	double txx,txy,txz,tyx,tyy,tyz,tzx,tzy,tzz;  //齐次坐标转换项
	double aangle,bangle,cangle;
	double dddii,dddjj,dddkk,dddij;	//计算摆角时用于计算刀具法矢
	double deg1=0,deg2=0.5*M_PI;	//起始角度
	double ddij,ddk;	//XY平面向量投影,Z轴投影

	double AA,BB,CC;//存放角度

	QString dxx(""),dyy(""),dzz("");//类型转换
	QString radd("");//类型转换




	double logStep=M_PI/logNum/2; ;			//经向步进角度
	double latStep=2*M_PI/latNum;	//纬向步进角度
	/////////////////////baosheng-start///////////////////////////

	QString a,b;
	QString DataIn[7];

	FTHP code1(b);     //什么意思？？


	a=code1.TH_GetHead();
	QString MCH=code1.TH_GetProfileString("MCH","MCH_Type");	//机床类型
	QString NCS=code1.TH_GetProfileString("NCS","NC_Type");		//数控类型

	    QString ipResult2 = gAppSetting->value("MCH/MCH_BLimit0").toString();  
   //  QString ipResult = gAppSetting->value("PARA/Distance").toString();  
	 
	 int Blimit= ipResult2.toInt();    //B轴旋转下下限
	

	if((NCS!="FDIA3")&&(NCS!="FDIA5"))
	{
		DataIn[0]=code1.TH_GetProfileString(NCS,NCS+"_CONST0");	//F1(移动速度)
		DataIn[1]=code1.TH_GetProfileString(NCS,NCS+"_CONST1");	//F2(测量速度)
		DataIn[2]=code1.TH_GetProfileString(NCS,NCS+"_CONST2");	//F3(粗测速度/后退速度)
		DataIn[3]=code1.TH_GetProfileString(NCS,NCS+"_CONST3");	//R(标定球半径)
		DataIn[4]=code1.TH_GetProfileString(NCS,NCS+"_CONST4");	//标定球心坐标X
		DataIn[5]=code1.TH_GetProfileString(NCS,NCS+"_CONST5");	//标定球心坐标Y
		DataIn[6]=code1.TH_GetProfileString(NCS,NCS+"_CONST6");	//标定球心坐标Z
	}

	QString dR;	//R
	QString csX,csY,csZ,csDX,csDY,csDZ,csL,csE,csR;
 	if(MCH=="5AxisBC")
	{
	  a+="G01 G90 X0.0 Y0.0 Z5.0 C0 B0 F2000\n";
	  AA=0;
	  BB=probeAngle/180*M_PI;
	  CC=startAngle/180*M_PI;
		
	}
    if(MCH=="5AxisAB")
	{
	  a+="G01 G90 X0.0 Y0.0 Z5.0 A0 B0 F2000\n";
	  AA=probeAngle/180*M_PI;
	  BB=startAngle/180*M_PI;
	  CC=0;
	}
	
	  if(MCH=="5AxisAC")
	{
	  a+="G01 G90 X0.0 Y0.0 Z5.0 A0 C0 F2000\n";
	  AA=probeAngle/180*M_PI;
	  BB=0;
	  CC=startAngle/180*M_PI;
	}
	a+=code1.TH_GetCenter();

	csX=QString::number(rad*cos(deg2),'f',4);
	csZ=QString::number(rad*sin(deg2),'f',4);


	 //CBA,矩阵相乘顺序
	txx=cos(BB)*cos(CC);
	txy=sin(AA)*sin(BB)*cos(CC)-cos(AA)*sin(CC);
	txz=cos(AA)*sin(BB)*cos(CC)+sin(AA)*sin(CC);

	 tyx=cos(BB)*sin(CC);
	 tyy=sin(AA)*sin(BB)*sin(CC)+cos(AA)*cos(CC);
	 tyz=cos(AA)*sin(BB)*sin(CC)-sin(AA)*cos(CC);

	  tzx=-sin(BB);
	  tzy=sin(AA)*cos(BB);
	  tzz=cos(AA)*cos(BB);

	 // 	  //BCA,矩阵相乘顺序
	 // txx=cos(BB)*cos(CC);
	 // txy=-cos(AA)*cos(BB)*sin(CC)+sin(AA)*sin(BB);
	 // txz=sin(AA)*cos(BB)*sin(CC)+cos(AA)*sin(BB);

	 //    tyx=sin(CC);
		// tyy=cos(AA)*cos(CC);
		// tyz=-sin(AA)*cos(CC);

	 //tzx=-sin(BB)*cos(CC);
	 //tzy=cos(AA)*sin(BB)*sin(CC)+sin(AA)*cos(BB);
	 //tzz=-sin(AA)*sin(BB)*sin(CC)+cos(AA)*cos(BB);


//测头移动到下一行
	QString Adeg(""),Bdeg(""),Cdeg("");	//A,B,C
	double bbdeg;


//测头转角
	if(MCH=="5AxisBC")
		{
			Bdeg=QString::number(probeAngle,'f',4);
            Cdeg=QString::number(startAngle,'f',4);

			
			if((NCS=="SMS3")||(NCS=="SMS5"))
			{
				a+="G01 G91 X40.0 F="+DataIn[0]+"\n";
				a+="G01 G91 Z40.0 F="+DataIn[0]+"\n";
				a+="G01 G90 B"+Bdeg+" C"+Cdeg+" F1000\n";
			}
			
	}
	
	

	else if(MCH=="5AxisAB")
		{
			Adeg=QString::number(probeAngle,'f',4);
            Bdeg=QString::number(startAngle,'f',4);

			
			if((NCS=="SMS3")||(NCS=="SMS5"))
			{
				a+="G01 G91 X40.0 F="+DataIn[0]+"\n";
				a+="G01 G91 Z40.0 F="+DataIn[0]+"\n";
				a+="G01 G90 A"+Adeg+" B"+Bdeg+" F1000\n";
			}
			
		}	


   else if(MCH=="5AxisAC")
		{
			//Adeg.Format("%.4f",probeangle);
         Adeg= QString::number(probeAngle,'f',4);
		 Cdeg= QString::number(startAngle,'f',4);


			if((NCS=="SMS3")||(NCS=="SMS5"))
			{
				a+="G01 G91 X40.0 F="+DataIn[0]+"\n";
				a+="G01 G91 Z40.0 F="+DataIn[0]+"\n";
				a+="G01 G90 A"+Adeg+" C"+Cdeg+" F1000\n";
			}
			
		}

		else
		{
			//MessageBox("定角标定仅用于五轴机床！","ERROR");
			QMessageBox customMsgBox;
			customMsgBox.setWindowTitle(QString::fromLocal8Bit("ERROR"));
			customMsgBox.setText(QString::fromLocal8Bit("定角标定仅用于五轴机床！"));
			customMsgBox.exec();
		}




	for (int k=0;k<=logNum;k++)
	{
		deg1=0;
		ddij=cos(deg2);	//投影长度
		ddk=sin(deg2);
		ddii=ddij*cos(deg1);
		ddjj=ddij*sin(deg1);
		ddkk=ddk;
		//dxx.Format("%.4f",rad*ddij);
		//dzz.Format("%.4f",rad*ddkk);
		dxx= QString::number(txx*rad*ddii+txy*rad*ddjj+txz*rad*ddkk,'f',4);
		dyy= QString::number(tyx*rad*ddii+tyy*rad*ddjj+tyz*rad*ddkk,'f',4);
		dzz= QString::number(tzx*rad*ddii+tzy*rad*ddjj+tzz*rad*ddkk,'f',4);
		radd= QString::number(rad,'f',4);



		//测头移动到新一行第一个测点的预备位置
		  if(NCS=="SMS5")
		{
			
				a+="G03 G90 X="+dxx;
				a+="+"+DataIn[4];
				//a+=" F="+DataIn[0]+"\n";
				a+=" Y="+dyy;
				a+="+"+DataIn[5];
				//a+=" F="+DataIn[0]+"\n";
				a+=" Z="+dzz;
				a+="+"+DataIn[6];
                a+=" CR="+radd;  
				a+=" F="+DataIn[0]+"\n";
			
		}

		if(deg2==0.5*M_PI)
		  {
            a+=code1.TH_GetProb(txx*ddii+txy*ddjj+txz*ddkk,tyx*ddii+tyy*ddjj+tyz*ddkk,tzx*ddii+tzy*ddjj+tzz*ddkk,D);
			dxx=QString::number(rad*(txx*ddii+txy*ddjj+txz*ddkk),'f',4);
			dyy=QString::number(rad*(tyx*ddii+tyy*ddjj+tyz*ddkk),'f',4);
			dzz=QString::number(rad*(tzx*ddii+tzy*ddjj+tzz*ddkk),'f',4);
			
			//测头回退
			if(NCS=="SMS5")
			   {
				a+="G01 G90 X="+dxx;
				a+="+"+DataIn[4];
				a+=" Y="+dyy;
				a+="+"+DataIn[5];
				a+=" Z="+dzz;
				a+="+"+DataIn[6];
				a+=" F="+DataIn[0]+"\n";
			    }

     	    deg2-=logStep;
		  }
	
        else
		 {
	    
		for(int i=1;i<=latNum;i++)
		    {
			a+=code1.TH_GetProb(txx*ddii+txy*ddjj+txz*ddkk,tyx*ddii+tyy*ddjj+tyz*ddkk,tzx*ddii+tzy*ddjj+tzz*ddkk,D);
			dxx=QString::number(rad*(txx*ddii+txy*ddjj+txz*ddkk),'f',4);
			dyy=QString::number(rad*(tyx*ddii+tyy*ddjj+tyz*ddkk),'f',4);
			dzz=QString::number(rad*(tzx*ddii+tzy*ddjj+tzz*ddkk),'f',4);
			
			//测头回退
			if(NCS=="SMS5")
			     {
				a+="G01 G90 X="+dxx;
				a+="+"+DataIn[4];
				a+=" Y="+dyy;
				a+="+"+DataIn[5];
				a+=" Z="+dzz;
				a+="+"+DataIn[6];
				a+=" F="+DataIn[0]+"\n";
			     }

			deg1+=latStep;//
			ddii=ddij*cos(deg1);
			ddjj=ddij*sin(deg1);
			ddkk=ddk;
			dxx=QString::number(rad*(txx*ddii+txy*ddjj+txz*ddkk),'f',4);
			dyy=QString::number(rad*(tyx*ddii+tyy*ddjj+tyz*ddkk),'f',4);
			dzz=QString::number(rad*(tzx*ddii+tzy*ddjj+tzz*ddkk),'f',4);

			//测头移动到下一个位置
			if(NCS=="SMS5")
			    {
				a+="G03 G90 G17 X="+dxx;
				a+="+"+DataIn[4];
				a+=" Y="+dyy;
				a+="+"+DataIn[5];
                a+=" Z="+dzz;
			    a+="+"+DataIn[6];
				dR=QString::number(ddij*rad,'f',4);
				a+=" CR="+dR;
				a+=" F="+DataIn[0]+"\n";
			    }

	          }
		deg2-=logStep;
		 }   //else  end,deg2不是90度。
		
	} // for k=0,k<=logNum,,,,end



	csZ=QString::number(rad,'f',4);
	
	if(NCS=="SMS5")
	{
		a+="G01 G90 X="+csZ;
		a+="+"+DataIn[4];
		a+=" F="+DataIn[0]+"\n";
		a+="G01 G90 Z="+csZ;
		a+="+"+DataIn[6];
		a+=" F="+DataIn[0]+"\n";
	}

	if(MCH=="5AxisBC")
	{
		a+="G01 G90 C0 B0 F1000\n";
	}
	else
	{
	}
	
	a+=code1.TH_GetTail();
		
	code1.TH_ProbeFileWrite(a, QString("ProbeAngleNC.mpf"));	
	if(MCH=="5AxisAB")
	{
		a+="G01 G90 A0 B0 F1000\n";
	}
	else
	{
	}
	
	a+=code1.TH_GetTail();
	code1.TH_ProbeFileWrite(a, QString("ProbeAngleNC.mpf"));		

	if(MCH=="5AxisAC")
	{
		a+="G01 G90 A0 C0 F1000\n";
	}
	else
	{
	}
	
	a+=code1.TH_GetTail();
	code1.TH_ProbeFileWrite(a, QString("ProbeAngleNC.mpf"));	
}


	

void FAdjustAngleProbe::OnAdjustProbeCreate()
{	/*gWindow->GetMainDoc()->SetIpPntList(gWindow->GetMainDoc()->GetIpPntList());*/
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
	if (mesNum!=theoNum)//
	{
		QMessageBox::information(this,QString::fromLocal8Bit("测量点数据和理论点数据不匹配,ERROR!"),
			QString::fromLocal8Bit("测量点数据和理论点数据不匹配,ERROR!"),"OK");
	}
	else{//生产标定文件，循环嵌套，内嵌套为ij方向转换的角度，外嵌套为k方向转换的角度		
		QString nameWithPath = gAppPath+QString("/settings/nc/ErrAngleSphere.txt");
		FILE * file=fopen(nameWithPath.toLocal8Bit().data(),"w");
		fprintf(file, "%i %i %i %c",m_logNum, m_latNum, m_probeAngle,m_startAngle,c);

//	FIpPntList romesPointsList;     //旋转后的数据
//	romesPointsList=ACrotation(mesPointsList, m_probeAngle, m_startAngle);
//		
//	//计算拟合球心
//	   FIpPntList SlectedFP;
//	    for (int i = 0; i <=mesNum; i++)
//    {
//	   if(abs(mesPointsList[i].y)<3)
//	   {
//		  
//		   SlectedFP.push_back(mesPointsList[i]);
//	   }
//
//		}
//
//		vector <double> sphereCen;  //存放拟合球心数据
//		sphereCen=SphereFit(SlectedFP);
//
//		FIpPoint tPnt3;
//		tPnt3=romesPointsList[0];
//		double zc_ad=tPnt3.z-(m_d/2+m_D/2-0.24);
//
//		FIpPntList MovMesPointsList;     //平移后的数据
//		FIpPoint tPnt4;
//	    for (int i = 0; i <=mesNum; i++)
//    {
//	   tPnt4.x=romesPointsList[i].x-sphereCen[0];
//	    tPnt4.y=romesPointsList[i].y-sphereCen[1];
//		 tPnt4.z=romesPointsList[i].z- zc_ad;
//		 MovMesPointsList.push_back(tPnt3);
//		}
//
//
////计算误差
//	for (int i=0;i<=m_logNum*m_latNum;i++)
//	{
//		double x=MovMesPointsList[i].x;
//		double y=MovMesPointsList[i].y;
//		double z=MovMesPointsList[i].z;
//		err=(m_d+m_D)/2-pow(x*x+y*y+z*z,0.5);
//
//			deg=theoPntList[i].deg;
//			degz=theoPntList[i].degz;
		//
		//		fprintf(file, "%f %f %f%c",err,deg,degz,c);
		//	

		//}	


		/*vector<double> T;
         T= SphereFit(mesPointsList);  */      

	

	for (int i=0;i<=m_logNum*m_latNum;i++)//
	{
		x = mesPointsList[i].x - theoPntList[i].x;
		y = mesPointsList[i].y - theoPntList[i].y;
		z = mesPointsList[i].z - theoPntList[i].z;
		double dd=pow(x,2)+pow(y,2)+pow(z,2);//各测量点到球心的距离
		err= pow(dd, 0.5);
		if (err < 0.000001)
		{
			err = 0;
		}
			deg=theoPntList[i].deg;
			degz=theoPntList[i].degz;
			if (i==0)    
			{
					fprintf(file, "%f %f %f%c",err,deg,degz,c);
					
			} 
			else
			{
				fprintf(file, "%f %f %f%c",err,deg,degz,c);
			
			}
			mesPointsList[i].err = err;
			gWindow->GetMainDoc()->addToMesErrList(i, err);

		}	

		fclose(file);
		QMessageBox::information(this,QString::fromLocal8Bit("固定摆角标定文件生成！"),
			QString::fromLocal8Bit("固定摆角标定文件生成！"),"OK");
	}

}

void FAdjustAngleProbe::OnAdjustProbeQuit()
{
	if (gWindow->GetMainDoc())
	{
		gWindow->GetMainDoc()->clear();
	}
	close();

}

//FIpPntList FAdjustAngleProbe:: ACrotation(FIpPntList FP,float probeAngle,float startAngle)
//{
//double xx=cos(startAngle/180*M_PI);
//double xy=-cos(probeAngle/180*M_PI)*sin(probeAngle/180*M_PI);
//double xz=sin(probeAngle/180*M_PI)*sin(startAngle/180*M_PI);
//double yx=sin(startAngle/180*M_PI);
//double yy=cos(probeAngle/180*M_PI)*cos(startAngle/180*M_PI);
//double yz=-sin(probeAngle/180*M_PI)*cos(startAngle/180*M_PI);
//double zx=0;
//double zy=sin(probeAngle/180*M_PI);
//double zz=cos(probeAngle/180*M_PI);
//
//FIpPntList RFP;
//FIpPoint tPnt;
//int Num=FP.size();
//     for (int i = 0; i <=Num; i++)
//   {
//	   tPnt.x=xx*FP[i].x+xy*FP[i].y+xz*FP[i].z;
//	   tPnt.y=yx*FP[i].x+yy*FP[i].y+yz*FP[i].z;
//	   tPnt.y=zx*FP[i].x+zy*FP[i].y+zz*FP[i].z;
//
//	   RFP.push_back(tPnt);
//		   
//   }
//	 return RFP;
//}

vector<double> FAdjustAngleProbe:: SphereFit(FIpPntList FP)
{

	    int Num=FP.size();
		Vector4f X=Vector4f ::Zero();
        Vector4f Y=Vector4f ::Zero();
		 MatrixXf A(4, 4);
		 double m1=0,m2=0,m3=0,m4=0,m5=0,m6=0,m7=0,m8=0,m9=0;

		 for (int i = 0; i <Num; i++)
    {
        Y[0] += (pow(FP[i].x,3) + pow(FP[i].y,2)*FP[i].x+ pow(FP[i].z,2)*FP[i].x);
        Y[1] += (pow(FP[i].y,3) + pow(FP[i].x,2)*FP[i].y+ pow(FP[i].z,2)*FP[i].y);
        Y[2] += (pow(FP[i].z,3) + pow(FP[i].x,2)*FP[i].z+ pow(FP[i].y,2)*FP[i].z);    
        Y[3] -= (pow(FP[i].x,2) + pow(FP[i].y,2) + pow(FP[i].z,2));
    }
		
		 for (int j = 0;j<Num;j++)
    {    
        m1+= pow(FP[j].x,2); 
        m2 += FP[j].x*FP[j].y;
        m3 += FP[j].x*FP[j].z;
        m4-= FP[j].x;
        m5 += pow(FP[j].y,2); 
        m6 += FP[j].y*FP[j].z;
        m7 -= FP[j].y;
        m8 += pow(FP[j].z,2);
        m9 -= FP[j].z;
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

	/*cout<< A(0,0)<<"   "<<A(0,1)<<"   "<<A(0,2)<<"   "<<A(0,3)<<endl;
	cout<< A(1,0)<<"   "<<A(1,1)<<"   "<<A(1,2)<<"   "<<A(1,3)<<endl;
	cout<< A(2,0)<<"   "<<A(2,1)<<"   "<<A(2,2)<<"   "<<A(2,3)<<endl;
	cout<< A(3,0)<<"   "<<A(3,1)<<"   "<<A(3,2)<<"   "<<A(3,3)<<endl;
	*/
    double m=5;
	m=A.determinant();
	cout<<m<<endl;
	
	vector<double> LSM;
	if(m==0)printf("矩阵不可逆");

	else{
		 MatrixXf IA(4, 4);
		 IA=A.inverse();
    X=IA*Y; 
	double x0= X (0,0)/2;
	double y0=X (1,0)/2;
	double z0=X(2,0)/2;
	double R=pow(pow(x0,2)+pow(y0,2)+pow(z0,2)-X(3,0),0.5);

	cout<<"x0="<<x0<<"   "<<"y0="<<y0<<"   "<<"z0="<<z0<<"   "<<"   "<<"R="<<R<<endl;

	LSM.clear();
	LSM.push_back(x0);
	LSM.push_back(y0);
	LSM.push_back(z0);
	LSM.push_back(R);
	}
	return LSM;
	

}
void FAdjustAngleProbe::OnImportData()
{
	CSDataTrans dt = CSDataTrans();
	dt.txtFileReader(gWindow->GetMainDoc(), 0, m_d);
	m_pCreateBtn->setEnabled(1);
}

void FAdjustAngleProbe::OnTCPConfig()
{
	m_pOPCConnect->setEnabled(0);
	//FConnectConfig* pConnectConfig = new FConnectConfig(this);
	//pConnectConfig->show();
	m_pDataBtn->setEnabled(1);
}

void FAdjustAngleProbe::OnOPCUA()
{
	m_pTCPConnect->setEnabled(0);
	QProcess* myProcess = new QProcess();
	myProcess->start("OpcUaTHU/OpcUaTHU.exe");
	m_pDataBtn->setEnabled(1);
}
