#include "FAccuracyEvaluation.h"
#include "qdir.h"
#include <String>
#include "FUtilities.h"
#include "GLU.h"
#include "MainDoc.h"
#include "FTHP.h"
#include "FMainWindow.h"
#include "QPixmap"
#include <QImageReader>
#include <QDebug>
#include <iostream>

extern FMainWindow* gWindow;

FAccuracyEvaluation::FAccuracyEvaluation(QWidget* parent)
	/*:QMdiSubWindow(parent)*/
{
    S=0;
	n= 8;
	
	this->setWindowTitle(QString::fromLocal8Bit("ƽ�������"));
	QPushButton* pOpenBtn = new QPushButton(QString::fromLocal8Bit("��"));
	QPushButton* pComputeBtn = new QPushButton(QString::fromLocal8Bit("����"));
	QPushButton* pExitBtn1 = new QPushButton(QString::fromLocal8Bit("�˳�"));

	QLabel *label1=new QLabel(this);
	label1->setText(QString::fromLocal8Bit("ƽ���"));
	s_flatnessLineEdit=new QLineEdit(this); 	 
	s_flatnessLineEdit->setText(QString::number(S));

	QLabel *label2=new QLabel(this);
	label2->setText(QString::fromLocal8Bit("�������"));
	n_pointLineEdit=new QLineEdit(this); 
	n_pointLineEdit->setText(QString::number(n));



	QLabel *label5=new QLabel(this);
	QPixmap img("images/Tools/pingmiandu.PNG");//б�߷���ͬ�Ͳ���ʾͼƬ
	label5->setPixmap(img);

	/*QTextCodec *code = QTextCodec::codecForName("gb2312");*/

	QGridLayout* mainGridLayout = new QGridLayout(this);
	QGroupBox *createFirstExclusiveGroup=new QGroupBox(QString::fromLocal8Bit("������"));
	QHBoxLayout *Hbox1 = new QHBoxLayout;               //QHBoxLayout��ˮƽ���֣���ˮƽ���������пؼ��������������С� 
                                                        //QVBoxLayout����ֱ���֣��ڴ�ֱ���������пؼ���������������
	Hbox1->addWidget(pOpenBtn);
	Hbox1->addWidget(pComputeBtn);
	Hbox1->addWidget(pExitBtn1);
	Hbox1->addStretch(1);
	createFirstExclusiveGroup->setLayout(Hbox1);

	QGroupBox *createSecondExclusiveGroup=new QGroupBox(QString::fromLocal8Bit("���"));
	QGridLayout* mainGridLayout1 = new QGridLayout;
	mainGridLayout1->addWidget(label1, 1, 0);  
	mainGridLayout1->addWidget(s_flatnessLineEdit, 1, 1);  
	mainGridLayout1->addWidget(label2, 2, 0);  
	mainGridLayout1->addWidget(n_pointLineEdit, 2, 1); 
	createSecondExclusiveGroup->setLayout(mainGridLayout1);

	QGroupBox *createThirdExclusiveGroup=new QGroupBox(QString::fromLocal8Bit("ͼ��"));
	QHBoxLayout *Hbox5 = new QHBoxLayout;

	Hbox5->addWidget(label5);
	Hbox5->addStretch(1);
	createThirdExclusiveGroup->setLayout(Hbox5);

	mainGridLayout->addWidget(createFirstExclusiveGroup, 1, 0);  
	mainGridLayout->addWidget(createSecondExclusiveGroup, 2, 0);  
	mainGridLayout->addWidget(createThirdExclusiveGroup, 3, 0);  
	this->setLayout(mainGridLayout);



	
	connect(pExitBtn1,SIGNAL(clicked()),this,SLOT(OnFlatQuit()));
	connect(pComputeBtn,SIGNAL(clicked()),this,SLOT(OnComputeS()));

}

void FAccuracyEvaluation::OnFlatQuit() 
{
	if (gWindow->GetMainDoc())
	{
		gWindow->GetMainDoc()->clear();
	}
	close();

}

FAccuracyEvaluation::~FAccuracyEvaluation(void)
{
}



std::vector<double> FAccuracyEvaluation:: ComputeCoef(double X[],double Y[],double Z[],int n){
	double Coef[3][3];//��ά�����ž���ϵ��
	//�����xi*yi��ϵ��
	for (int i=0;i<3;i++)
		for (int j=0;j<3;j++)
			Coef[i][j]=0;
	double B[3];
    	for (int j=0;j<3;j++)
			B[j]=0;

	      for (int j=0;j<n;j++)
	     {
		    Coef[0][0]+=X[j]*X[j];
			B[0]+=X[j]*Z[j];
			Coef[0][1]+=X[j]*Y[j];
			Coef[0][2]+=X[j];

			Coef[1][1]+=Y[j]*Y[j];
			Coef[1][2]+=Y[j];
			B[1]+=Y[j]*Z[j];

			B[2]+=Z[j];
	  
	     }
		  Coef[2][2]=n;
		  Coef[1][0]=Coef[0][1];
		  Coef[2][0]=Coef[0][2];
		  Coef[2][1]=Coef[1][2];

	vector <double>		Q;//�Ѽ��������ϵ�����������Q�в����أ�������֪����ô���ؾ��������һ��

		for (int i=0;i<3;i++)
		    for (int j=0;j<3;j++)
			   Q.push_back(Coef[i][j]);
		for (int i=0;i<3;i++)
			 Q.push_back(B[i]);

		return Q;
}

double FAccuracyEvaluation::ComputeHanglieshi(double A[3][3]){
	
    double s,s1,s2;
   
    s1=A[0][0]*A[1][1]*A[2][2]+A[0][1]*A[1][2]*A[2][0]+A[0][2]*A[1][0]*A[2][1];
    s2=A[0][2]*A[1][1]*A[2][0]+A[0][1]*A[1][0]*A[2][2]+A[0][0]*A[1][2]*A[2][1];
    s=s1-s2;
	return s;
}

std::vector<double> FAccuracyEvaluation::Cramer(double C[3][3], double B[3])
{
	double d,d1,d2,d3;
	d=ComputeHanglieshi(C);
	double D1[3][3];
	double D2[3][3];
	double D3[3][3];

	//��ʼ��D1,D2,D3
	for (int i=0;i<3;i++)
		for (int j=0;j<3;j++)
			D1[i][j]=0;
	for (int i=0;i<3;i++)
		for (int j=0;j<3;j++)
			D2[i][j]=0;
	for (int i=0;i<3;i++)
		for (int j=0;j<3;j++)
			D3[i][j]=0;
	
	//ʹD1=C����B�滻��һ�У�Ȼ���������ʽ
	for (int i=0;i<3;i++)
		for (int j=0;j<3;j++)
			D1[i][j]=C[i][j];
	for (int i=0;i<3;i++)
	{
		D1[i][0]=B[i];
	} 
	d1=ComputeHanglieshi(D1);

	//ʹD2=C����B�滻�ڶ��У�Ȼ���������ʽ
	for (int i=0;i<3;i++)
		for (int j=0;j<3;j++)
			D2[i][j]=C[i][j];
	for (int i=0;i<3;i++)
	{
		D2[i][1]=B[i];
	} 
	d2=ComputeHanglieshi(D2);

	//ʹD3=C����B�滻�����У�Ȼ���������ʽ
	for (int i=0;i<3;i++)
		for (int j=0;j<3;j++)
			D3[i][j]=C[i][j];
	for (int i=0;i<3;i++)
	{
		D3[i][2]=B[i];
	} 
	d3=ComputeHanglieshi(D3);

	double a0,a1,a2;
	a0=d1/d;
	a1=d2/d;
	a2=d3/d;
	vector<double> A;
	
	A.push_back(a0);
	A.push_back(a1);
	A.push_back(a2);

	return A;
}

double FAccuracyEvaluation::OnComputeS(){
	double X[12]={0,  9,  20,  8,   -3,  -4, -8, 31, 14,  5,  5,   4};
    double Y[12]={1,  -10, 8,   9,   15,  30, 5, -34, 5,  -29, -27,-5};
    double Z[12]={1,  2,  -27, -16, -11, -25, 4,  4,  -18, 25, 23,  2};
	/*double Z[12]={0,0,0,0,0,0,0,0,0,0,0,0};*/
	/*double X[12]={8.999,  9.01,  9.02,  8.998,   9.001,  8.989, 9.0, 8.99, 9.01,  8.999,  9.001,   9};*/
	/*double X[12]={0,0,0,0,0,0,0,0,0,0,0,0};*/


	int n=sizeof(X)/sizeof(X[0]);//��������Ԫ�ظ��������ռ���ɢ��ĸ���
	// ����һ����ά����C���ϵ��
	int e=0;
	for (int i=0;i<n;i++)  //�ж��ǲ�����XOYƽ����
	{
		if(Z[i]==Z[i+1]){e++;}
	}
	
	if(e==n-1)    //�������XOYƽ���ڣ�ƽ��Ⱦ�����
    {
		S=0;
	}else{    //�������XOYƽ���ڣ���С���˷�����ƽ�淽��

	double C[3][3];
	double B[3];
	double a0,a1,a2;

	vector <double> T1;//����һ����ʱvector��ŷ��صľ���Ԫ��
	T1=ComputeCoef( X, Y, Z,n);
	//��T1�е�Ԫ�ظ�ֵ��C�����Ծ�����ʽ�洢
	for (int i=0;i<3;i++)
		C[0][i]=T1[i];
	for (int i=0;i<3;i++)
		C[1][i]=T1[i+3];
	for (int i=0;i<3;i++)
		C[2][i]=T1[i+6];
	for (int i=0;i<3;i++)
		B[i]=T1[i+9];

	
	vector <double> T2;
	T2=Cramer(C, B);
	a0=T2[0];
	a1=T2[1];
	a2=T2[2];

	//cout<<a0<<endl;
	//cout<<a1<<endl;

	if(abs(a0)<=0.00001)   // ���|a0|<=0.00001������Ϊƽ����YOZ
	{
		double m=0,h=0;
		for(int i=0;i<n;i++)
		{
			if(X[i]<=m)
			{
				m=X[i];
			}
			if(Z[i]>=h)
			{
				h=X[i];
			}
		}
		S=h-m;
	}

	if(abs(a1)<=0.00001)    // ���|a1|<=0.00001������Ϊƽ����XOZ
	{
		double m=0,h=0;
		for(int i=0;i<n;i++)
		{
			if(Y[i]<=m)
			{
				m=Y[i];
			}
			if(Z[i]>=h)
			{
				h=Y[i];
			}
		}
		S=h-m;
	}
	else{

	double d,d1=0,d2=0;
	for(int i=0;i<n;i++){
		/*p=X[i]*a0+Y[i]*a1+a2-Z[i];*/
		d=(X[i]*a0+Y[i]*a1+a2-Z[i])/(sqrt(a0*a0+a1*a1+1));
		if(d<=d1){
			d1=d;
		 if(d>=d2)
			 d2=d;
		}
		}
	S=-d1+d2;
	}
	}

	s_flatnessLineEdit->setText(QString::number(S));
	n_pointLineEdit->setText(QString::number(n));

	return S;
}
