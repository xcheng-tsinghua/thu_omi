#include <String>
#include "FUtilities.h"
#include "FAdjustRingProbe.h"
#include "GLU.h"
#include "MainDoc.h"
#include "FTHP.h"
#include "FMainWindow.h"
#include "qdir.h"

extern FMainWindow* gWindow;

FAdjustRingProbe::FAdjustRingProbe(QWidget* parent)
	/*:QMdiSubWindow(parent)*/
{
	m_Rad = 0.0;
	m_Number = 0;
	m_height = 0.0;
	this->setWindowTitle(QString::fromLocal8Bit("标定环规"));
	
	QPushButton* pstartBtn = new QPushButton(QString::fromLocal8Bit("开始标定测头"));
	m_pCreateBtn= new QPushButton(QString::fromLocal8Bit("开始标定测头"));
	m_pCreateBtn->setEnabled(0);
	QPushButton* pexitBtn = new QPushButton(QString::fromLocal8Bit("退出"));
	QLabel *label1=new QLabel(this);
	label1->setText(QString::fromLocal8Bit("输入标定直径D"));//标定直径

	 lineedit1=new QLineEdit(this); 

	QString m_Rad1=lineedit1->text();
	bool ok;
	 m_Rad=(float)m_Rad1.toDouble(&ok);
	 lineedit1->setPlaceholderText("25");

	QLabel *label2=new QLabel(this);
	label2->setText(QString::fromLocal8Bit("输入标定点数n"));//标定点数
	lineedit2=new QLineEdit(this); 

	QString m_Number1=lineedit2->text();
	bool ok1;
	 m_Number=m_Number1.toInt(&ok1, 10);
	 lineedit2->setPlaceholderText("3");

	QLabel *label3=new QLabel(this);
	label3->setText(QString::fromLocal8Bit("输入标定高度h"));//标定高度
	lineedit3=new QLineEdit(this); 
	bool ok2;
	QString  m_height1=lineedit3->text();
	 m_height= m_height1.toDouble(&ok2);
     lineedit3->setPlaceholderText("8");
	 ;
	QLabel *label5=new QLabel(this);
	QMovie *movie =new QMovie(":/Others/Others/ringprobe.bmp");
    label5->setMovie(movie);
    movie->start();

	QTextCodec *code = QTextCodec::codecForName("gb2312");
//std::string name = code->fromUnicode(tr).data();
	QGridLayout* mainGridLayout = new QGridLayout(this);
    QGroupBox *createFirstExclusiveGroup=new QGroupBox(QString::fromLocal8Bit("操作栏"));
	QHBoxLayout *Hbox1 = new QHBoxLayout;
      Hbox1->addWidget(pstartBtn);
      Hbox1->addWidget(m_pCreateBtn);
      Hbox1->addWidget(pexitBtn);
      Hbox1->addStretch(1);
      createFirstExclusiveGroup->setLayout(Hbox1);

	QGroupBox *createSecondExclusiveGroup=new QGroupBox(QString::fromLocal8Bit("输入栏"));
	QGridLayout* mainGridLayout1 = new QGridLayout;
	 mainGridLayout1->addWidget(label1, 1, 0);  
     mainGridLayout1->addWidget(lineedit1, 1, 1);  
     mainGridLayout1->addWidget(label2, 2, 0);  
     mainGridLayout1->addWidget(lineedit2, 2, 1); 
	 mainGridLayout1->addWidget(label3, 3, 0);  
     mainGridLayout1->addWidget(lineedit3, 3, 1); 
	 createSecondExclusiveGroup->setLayout(mainGridLayout1);
	/*QVBoxLayout *Vbox2 = new QVBoxLayout;

	QHBoxLayout *Hbox2 = new QHBoxLayout;
	  Hbox2->addWidget(label1);
      Hbox2->addWidget(lineedit1);
	QHBoxLayout *Hbox3 = new QHBoxLayout;
	  Hbox3->addWidget(label2);
      Hbox3->addWidget(lineedit2);
	QHBoxLayout *Hbox4 = new QHBoxLayout;
	  Hbox4->addWidget(label3);
	  Hbox4->addWidget(lineedit3);

      Vbox2->addLayout(Hbox2);
      Vbox2->addLayout(Hbox3);
      Vbox2->addLayout(Hbox4);
      Vbox2->addStretch(1);
      createSecondExclusiveGroup->setLayout(Vbox2);*/
	QGroupBox *createThirdExclusiveGroup=new QGroupBox(QString::fromLocal8Bit("图例"));
	QHBoxLayout *Hbox5 = new QHBoxLayout;
	 Hbox5->addWidget(label5);
	 Hbox5->addStretch(1);
     createThirdExclusiveGroup->setLayout(Hbox5);
	 mainGridLayout->addWidget(createFirstExclusiveGroup, 1, 0);  
     mainGridLayout->addWidget(createSecondExclusiveGroup, 2, 0);  
     mainGridLayout->addWidget(createThirdExclusiveGroup, 3, 0);  
  // this->setFixedSize( this->width (),this->height ());
		this->setLayout(mainGridLayout);
       connect(pstartBtn,SIGNAL(clicked()),this,SLOT(OnAdjustProbeStart()));
	   connect(m_pCreateBtn,SIGNAL(clicked()),this,SLOT(OnAdjustProbeCreate()));
	   connect(pexitBtn,SIGNAL(clicked()),this,SLOT(OnAdjustProbeQuit()));

	//add signal-slots
	//QObject::connect(pExitBtn,SIGNAL(clicked()), this, SLOT(onPauseBtn()));
		//QObject::connect(pUpdateButton,SIGNAL(clicked()), this, SLOT(UpdateFeature()));
}
FAdjustRingProbe::~FAdjustRingProbe(void)
{
}
void FAdjustRingProbe::OnAdjustProbeStart() 
{
	QString m_Rad1=lineedit1->text();
	bool ok;
	 m_Rad=m_Rad1.toDouble(&ok);

	 QString m_Number1=lineedit2->text();
	bool ok1;
	 m_Number=m_Number1.toInt(&ok1, 10);

	 bool ok2;
	QString m_height1=lineedit3->text();
	 m_height=m_height1.toDouble(&ok2);
	
	double a = m_Rad/2;
	int b = m_Number-1;
	double c=m_height;
	int d = 1;

	//将参数传给form
	AdjustProbe0(a,b,c);
	NCFile0D(a,b,c);
	gWindow->GetMainDoc()->AdjustProbeFlag=1;
	m_pCreateBtn->setEnabled(1);
}
AdjustProbe0::AdjustProbe0(double rad,int number,double height){
	double deg=M_PI*2/number;

	//FIpPoint pPointtrue;
	
	QString s1;
	QString s3;
	QString num[5000];
    int iPos=0;
	
	QString string;
	QString data[12];
	//FIpPntList m_pointList;
	//m_pointList=gWindow->GetMainDoc()->m_pointList;
	FIpPntList m_pointList;
	 m_pointList=gWindow->GetMainDoc()->GetIpPntList();
//	FILE * file=fopen("D:\\src\\0dceliang.txt","w");

	//CListCtrl * m_ListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST1);

	for(int i=0;i<number;i++){
        FIpPoint pPointtrue;
		pPointtrue.x=rad*cos(deg*i);
		pPointtrue.y=rad*sin(deg*i);
		pPointtrue.z=0;
		pPointtrue.deg=deg*i;
		m_pointList.push_back(pPointtrue);

		data[0].number(pPointtrue.x,'g',6);
		data[1].number(pPointtrue.y,'g',6);
		data[2].number(pPointtrue.z,'g',6);
		data[3].number(pPointtrue.x/20,'g',6);
		data[4].number(pPointtrue.y/20,'g',6);
		data[5].number(pPointtrue.z/20,'g',6);

		//data[0].Format("%f",pPointtrue->x);
		//data[1].Format("%f",pPointtrue->y);
		//data[2].Format("%f",pPointtrue->z);
		//data[3].Format("%f",pPointtrue->x/20);
		//data[4].Format("%f",pPointtrue->y/20);
		//data[5].Format("%f",pPointtrue->z/20);
		string="<点>"+data[0]
			+","+data[1]
			+","+data[2]
			+","+data[3]
			+","+data[4]
			+","+data[5]
			+",2</点>";
			//fprintf(file,"%s\n",string);

		for(int j=1;j<=3;j++){
			    switch (j){
					case 1:s3.number(pPointtrue.x,'g',6);s1="X";break;
		            case 2:s3.number(pPointtrue.y,'g',6);s1="Y";break;	
		            case 3:s3.number(pPointtrue.z,'g',6);s1="Z";break;
			       // case 1:s3.Format("%f",pPointtrue->x);s1="X";break;
			       // case 2:s3.Format("%f",pPointtrue->y);s1="Y";break;	
				  //case 3:s3.Format("%f",pPointtrue->z);s1="Z";break;
				}
		      // m_ListCtrl->InsertItem(iPos,"");
    		  // m_ListCtrl->SetItemText(iPos,1,s1);
			  // m_ListCtrl->SetItemText(iPos,3,s3);
	  	   	   iPos++;
		}
		//num[iPos].Format("%d",iPos/3);
		num[iPos].number(iPos/3);
		//m_ListCtrl->SetItemText(iPos-3,0,num[iPos]);
	}
	//z向点标定
	    FIpPoint pPointtrue;
	   // pPointtrue = new CPoints;
		pPointtrue.x=0;
		pPointtrue.y=0;
		pPointtrue.z=height;
		pPointtrue.deg=0;
		m_pointList.push_back(pPointtrue);

		data[0].number(pPointtrue.x,'g',6);
		data[1].number(pPointtrue.y,'g',6);
		data[2].number(pPointtrue.z,'g',6);
		data[3].number(pPointtrue.x/20,'g',6);
		data[4].number(pPointtrue.y/20,'g',6);
		data[5].number(pPointtrue.z/20,'g',6);

		//data[0].Format("%f",pPointtrue->x);
		//data[1].Format("%f",pPointtrue->y);
		//data[2].Format("%f",pPointtrue->z);
		//data[3].Format("%f",pPointtrue->x/20);
		//data[4].Format("%f",pPointtrue->y/20);
		//data[5].Format("%f",pPointtrue->z/20);
		string="<点>"+data[0]
			+","+data[1]
			+","+data[2]
			+","+data[3]
			+","+data[4]
			+","+data[5]
			+",2</点>";
		//	fprintf(file,"%s\n",string);


		for(int k=1;k<=3;k++){
			    switch (k){
					case 1:s3.number(pPointtrue.x,'g',6);s1="X";break;
		            case 2:s3.number(pPointtrue.y,'g',6);s1="Y";break;	
		            case 3:s3.number(pPointtrue.z,'g',6);s1="Z";break;
			        //case 1:s3.Format("%f",pPointtrue->x);s1="X";break;
			       // case 2:s3.Format("%f",pPointtrue->y);s1="Y";break;	
					//case 3:s3.Format("%f",pPointtrue->z);s1="Z";break;
				}
		     //  m_ListCtrl->InsertItem(iPos,"");
    		  // m_ListCtrl->SetItemText(iPos,1,s1);
			  // m_ListCtrl->SetItemText(iPos,3,s3);
	  	   	   iPos++;
		}
		num[iPos].number(iPos/3);
		//num[iPos].Format("%d",iPos/3);
		//m_ListCtrl->SetItemText(iPos-3,0,num[iPos]);

		//gWindow->GetMainDoc()->setPointList(m_pointList);//Copy(PointsList);
		 gWindow->GetMainDoc()->SetIpPntList(m_pointList);
	//pMyDoc->PointsList.Copy(PointsList);
	//pMyDoc->LoadSphere();
	//UpdateData(false);

//	fclose(file);
}
AdjustProbe0::~AdjustProbe0()
{
}
NCFile0D::NCFile0D(double oldrad, int Ijp, double high){
	double LL=10;
	double rad;

	//测量起始半径
	double ddxx,ddyy,ddzz;			//测量起始点
	QString dxx(""),dyy(""),dzz("");//类型转换
	double ddeg1=2*M_PI/Ijp;			//步进角度
	/////////////////////baosheng-start///////////////////////////
	QString a,b;
	
	FTHP code1(b);
	a=code1.TH_GetHead();
	QString NCS=code1.TH_GetProfileString("NCS","NC_Type");	//机床类型
	QString RR=code1.TH_GetProfileString(NCS,NCS+"_CONST3");
	
	QString dR;	//R
	//dR.Format("%.4f",oldrad);
	dR = QString::number(oldrad,'f',4);
	a+=RR+"="+dR+"\n";
	
	a+="G01 G90 X0 Y0 Z0 F1000\n";
	a+=code1.TH_GetCenter();
	a+="G01 G90 X=R1 Y=R2 F1000\n";
	
	double ddii,ddjj;	//i,j
	double deg1=0;	//起始角度
	ddii=cos(deg1);
	ddjj=sin(deg1);
	for(int i=1;i<=Ijp;i++)
	{
		a+=code1.TH_GetProb(ddii,ddjj,0,oldrad*2);
		deg1+=ddeg1;
		ddii=cos(deg1);
		ddjj=sin(deg1);
	}

	QString dH;	//高度
	//dH.Format("%.4f",high);
	dH = QString::number(high,'f',4);

	a+="G01 G91 Z="+dH+" F1000\n";
	a+="G01 G90 X=R1+"+RR+"+5 F1000\n";
	a+=code1.TH_GetProb(0,0,-high,oldrad*2);
	a+="G01 G90 X=R1 Y=R2 F1000\n";

	a+=code1.TH_GetTail();
	QString nameWithPath = gAppPath+QString("/settings/nc/ProbeRingNC.mpf");
	code1.TH_ProbeFileWrite(a, nameWithPath);	
	/////////////////////NC  baosheng-end///////////////////////////
}
NCFile0D::~NCFile0D()
{
}
void FAdjustRingProbe::OnAdjustProbeQuit() 
{
	close();
	
}
void FAdjustRingProbe::OnAdjustProbeCreate() 
{
	double x=0;
	double y=0;
	double z=0;
	double r=0;
	double deg=0;
	int mij = m_Number-1;
	char   c = '\n';
	gWindow->GetMainDoc()->SetIpPntList(gWindow->GetMainDoc()->GetIpPntList());
	 FIpPntList RealPointsList;
	 RealPointsList=gWindow->GetMainDoc()->GetMesPntList();
	 FIpPntList m_pointList;
	 m_pointList=gWindow->GetMainDoc()->GetIpPntList();
	 FIpPntList PointsList;
	 PointsList=gWindow->GetMainDoc()->GetIpPntList();

	int m=RealPointsList.size();
	int n=PointsList.size();
	
	if (m!=n)
	{
		//MessageBox("测量点数据和理论点数据不匹配！","ERROR");
		QMessageBox::information(this,QString::fromLocal8Bit("测量点数据和理论点数据不匹配,ERROR!"),
		QString::fromLocal8Bit("测量点数据和理论点数据不匹配,ERROR!"),"OK");
	}
	else{		
		//生产标定文件
		QString nameWithPath = gAppPath+QString("/settings/nc/AdjustProbe0.txt");
		FILE * file=fopen(nameWithPath.toLocal8Bit().data(),"w");
		fprintf(file, "%i%c",mij,c);
		for (int i=0;i<m-1;i++)
		{
			x=PointsList[i].x-RealPointsList[i].x;
			y=PointsList[i].y-RealPointsList[i].y;
			z=0;
			r=pow((pow(x,2)+pow(y,2)+pow(z,2))*1.0,0.5);
			r=-1*r;	
			deg=PointsList[i].deg;
			fprintf(file, "%f %f%c",r,deg,c);
		}
		//z向补偿数据置于最后
		z=PointsList[m-1].z-RealPointsList[m-1].z;
		deg=PointsList[m-1].deg;
		x=0;
		y=0;
		r=pow((pow(x,2)+pow(y,2)+pow(z,2))*1.0,0.5);
		r=-1*r;	
		fprintf(file, "%f %f%c",r,deg,c);
		fclose(file);
		//MessageBox("环规标定文件生成！");
		QMessageBox::information(this,QString::fromLocal8Bit("环规标定文件生成！"),
		  ("环规标定文件生成！"),"OK");
	}
}