#include <QDir>
#include <stdlib.h>
#include <String>
#include "FUtilities.h"
#include "FMainWindow.h"
#include "GLU.h"
#include "MainDoc.h"
#include "FTHP.h"
#include "FAdjustMachine.h"
#include <functional>

extern FMainWindow* gWindow;

FAdjustMachine::FAdjustMachine(QWidget* parent)
	/*:QMdiSubWindow(parent)*/
{
	this->setWindowTitle(QString::fromLocal8Bit("机床标定"));
	
	QPushButton* pstarXtBtn = new QPushButton(QString::fromLocal8Bit("生成X轴机床标定文件"));
	QPushButton* pstarYtBtn = new QPushButton(QString::fromLocal8Bit("生成Y轴机床标定文件"));
	QPushButton* pstarZtBtn = new QPushButton(QString::fromLocal8Bit("生成Z轴机床标定文件"));

	QPushButton* pExitBtn = new QPushButton(QString::fromLocal8Bit("退出"));
	QGroupBox *createFirstExclusiveGroup=new QGroupBox(QString::fromLocal8Bit("文件生成"));
	QGridLayout* mainGridLayout1 = new QGridLayout;
	mainGridLayout1->addWidget(pstarXtBtn, 1, 0);   
    mainGridLayout1->addWidget(pstarYtBtn, 2, 0);  
	mainGridLayout1->addWidget(pstarZtBtn, 3, 0);  
	createFirstExclusiveGroup->setLayout(mainGridLayout1);

	QGroupBox *createSecondExclusiveGroup=new QGroupBox(QString::fromLocal8Bit("其它"));
	QGridLayout* mainGridLayout2 = new QGridLayout;
	mainGridLayout2->addWidget(pExitBtn, 1, 0);  
	m_pUseOfMachineCompBnt = new QPushButton(QString::fromLocal8Bit("启用机床误差补偿(当前已关闭)"));
	mainGridLayout2->addWidget(m_pUseOfMachineCompBnt, 0, 0);
	gWindow->GetMainDoc()->setIsMachineErrorCmps(false);
	createSecondExclusiveGroup->setLayout(mainGridLayout2);

	//QTextCodec *code = QTextCodec::codecForName("gb2312");
	QGridLayout* mainGridLayout = new QGridLayout(this);

	QGroupBox* createThirdExclusiveGroup = new QGroupBox(QString::fromLocal8Bit("当前坐标系在标定坐标系下的坐标"));
	QGridLayout* mainGridLayout3 = new QGridLayout;
	QPushButton* pConfirmCoorBtn = new QPushButton(QString::fromLocal8Bit("设定坐标系"));

	m_pRealInCali_X = new QLineEdit;
	m_pRealInCali_Y = new QLineEdit;
	m_pRealInCali_Z = new QLineEdit;

	m_pRealInCali_X->setText("0");
	m_pRealInCali_Y->setText("0");
	m_pRealInCali_Z->setText("0");

	QLabel* InputLabel_X = new QLabel(QString::fromLocal8Bit("X坐标"));
	QLabel* InputLabel_Y = new QLabel(QString::fromLocal8Bit("Y坐标"));
	QLabel* InputLabel_Z = new QLabel(QString::fromLocal8Bit("Z坐标"));

	mainGridLayout3->addWidget(m_pRealInCali_X, 0, 1);
	mainGridLayout3->addWidget(m_pRealInCali_Y, 1, 1);
	mainGridLayout3->addWidget(m_pRealInCali_Z, 2, 1);

	mainGridLayout3->addWidget(InputLabel_X, 0, 0);
	mainGridLayout3->addWidget(InputLabel_Y, 1, 0);
	mainGridLayout3->addWidget(InputLabel_Z, 2, 0);

	mainGridLayout3->addWidget(pConfirmCoorBtn, 3, 0);

	createThirdExclusiveGroup->setLayout(mainGridLayout3);

    mainGridLayout->addWidget(createFirstExclusiveGroup, 1, 0);  
	mainGridLayout->addWidget(createThirdExclusiveGroup, 2, 0);
    mainGridLayout->addWidget(createSecondExclusiveGroup, 3, 0);  

    this->setFixedSize( this->width (),this->height ());
	this->setLayout(mainGridLayout);
    connect(pstarXtBtn,SIGNAL(clicked()),this,SLOT(OnMachinefilex()));
	connect(pstarYtBtn,SIGNAL(clicked()),this,SLOT(OnMachinefiley()));
	connect(pstarZtBtn,SIGNAL(clicked()),this,SLOT(OnMachinefilez()));
	connect(pExitBtn,SIGNAL(clicked()),this,SLOT(OnAdjustProbeQuit()));
	connect(pConfirmCoorBtn, SIGNAL(clicked()), this, SLOT(OnConfirmCoor()));
	connect(m_pUseOfMachineCompBnt, SIGNAL(clicked()), this, SLOT(OnOpenCloseMachineComp()));

	D_RealInCali = F3dPoint(0, 0, 0);
}

FAdjustMachine::~FAdjustMachine(void)
{

}

void FAdjustMachine::MachineErrComp(FIpPoint& inspPnt, FIpPoint TheoPnt)
{
	//根据激光干涉仪获得的数据计算坐标分量为x处的机床误差
	//Dir=1: X向误差
	//Dir=2: Y向误差
	//Dir=3: Z向误差
	function<double(double, int)> CompValCal = [&](double x, int Dir)->double
	{
		float a = 0;
		float b = 0;
		float r = 0;
		float dx1, dx2, mx1, mx2;
		int m, nmx1, nmx2;
		vector<gp_Pnt2d> data;
		gp_Pnt2d point;

		//读取标定文件,将数据存储于data变量中
		QString fname;
		switch (Dir)
		{
		case 1:fname = gAppPath + "/settings/nc/AjustMachineX.txt"; break;
		case 2:fname = gAppPath + "/settings/nc/AjustMachineY.txt"; break;
		case 3:fname = gAppPath + "/settings/nc/AjustMachineZ.txt"; break;
		default:
			QMessageBox::warning(
				QApplication::activeWindow(), QObject::tr("Caution"), QString::fromLocal8Bit("错误的Dir，数据未补偿"));
			return 0;
		}

		if (fopen(fname.toLocal8Bit().data(), "r") != NULL) {
			FILE* file = fopen(fname.toLocal8Bit().data(), "r");
			while (fscanf(file, "%f %f", &a, &b) != EOF)
			{
				point.SetX(a);
				point.SetY(b);

				data.push_back(point);
			}
			fclose(file);

			m = data.size();
			if (x > data[m - 1].X() || x < data[0].X() || m < 3)
			{
				QMessageBox::warning(
					QApplication::activeWindow(), QObject::tr("Caution"), QString::fromLocal8Bit("数据X轴坐标超出机床标定文件范围！"));
			}
			else
			{
				nmx1 = (x - data[0].X()) / (data[1].X() - data[0].X());
				nmx2 = nmx1 + 1;

				mx1 = data[nmx1].X();
				mx2 = data[nmx2].X();
				dx1 = data[nmx1].Y();
				dx2 = data[nmx2].Y();

				r = (dx2 * (x - mx1) + dx1 * (mx2 - x)) / (mx2 - mx1);
			}
		}
		else {
			QMessageBox::warning(
				QApplication::activeWindow(), QObject::tr("Caution"), QString::fromLocal8Bit("数据X轴未补偿！请检查X轴机床标定文件是否存在！"));
		}
		return r;
	};

	double i = TheoPnt.i;
	double j = TheoPnt.j;
	double k = TheoPnt.k;

	double rx = CompValCal(inspPnt.x + D_RealInCali.x, 1) / 1000;
	double ry = CompValCal(inspPnt.y + D_RealInCali.y, 2) / 1000;
	double rz = CompValCal(inspPnt.z + D_RealInCali.z, 3) / 1000;
	double castr = rx * i + ry * j + rz * k;
	double castx = castr * i;
	double casty = castr * j;
	double castz = castr * k;

	//这里本来是加上cast，个人觉得应该是减
	inspPnt.x -= castx;
	inspPnt.y -= casty;
	inspPnt.z -= castz;
}

void FAdjustMachine::OnMachinefilex()
{
	FIpPntList machinepiontlist;
	char line[255];
	QString aline;
	int number,count,target;
	float data1,data2,average;
	float data;
	QString fname = gAppPath+"/settings/X.rtl";
    if (fopen(fname.toLocal8Bit().data(),"r")!=NULL) {
		FILE * file=fopen(fname.toLocal8Bit().data(),"r");
		while(fgets(line,sizeof(line),file) != NULL)
		{
			aline=line;
			if (aline.left(12)=="Target-count")
			{
				QString s="Target-count: ";
				aline=aline.mid(s.size());
				//aline.TrimLeft("Target-count: ");
				number=(int)aline.toDouble();
				
				fgets(line,sizeof(line),file);  //选择下一行数据
				fgets(line,sizeof(line),file);
				aline=line;

				data1=aline.left(aline.indexOf(" ")).toFloat();
				aline=aline.remove(0,aline.indexOf(" ")+2);
				//data2=atof(aline.Left(aline.FindOneOf(" ")));
				data2=aline.left(aline.indexOf(" ")).toFloat();
				average=data2-data1;
				
				for(int i=0;i<number;i++){
					//CPoints * machinepiont = new CPoints;
					FIpPoint machinepiont;
					machinepiontlist.push_back(machinepiont);
					machinepiontlist[i].x=data1+average*i;
				}
			}
			
			if (aline.left(9)=="Run-count")
			{
				//aline.TrimLeft("Run-count:");
				QString s="Run-count:";
				aline=aline.mid(s.size(),-1);
				count=aline.toInt();
				
				fgets(line,sizeof(line),file);  //选择下一行数据
				fgets(line,sizeof(line),file);
				fgets(line,sizeof(line),file);
				for(int i=0;i<number*count;i++){
					fgets(line,sizeof(line),file);
					aline=line;
					//aline.Delete(0,4);
                    aline=aline.remove(0,4);
					target=aline.left(aline.indexOf(" ")).toFloat();
					//aline.Delete(0,aline.FindOneOf(" ")+1);
					aline=aline.remove(0,aline.indexOf(" ")+1);
					data=aline.toFloat();//target-1
					machinepiontlist[target-1].y+=(data/(count*1.0))*1.0;
					float aaa = machinepiontlist[target-1].y;
				}
			}
		}
		fclose(file);
		fname = gAppPath+"/settings/nc/AjustMachineX.txt";
		file=fopen(fname.toLocal8Bit().data(),"w");
		float x,y;
		char c='\n';
		for (int i=0;i<number;i++)
		{
			x=machinepiontlist[i].x;
			y=machinepiontlist[i].y;
			fprintf(file, "%f %f%c",x,y,c);
		}
		fclose(file);
		//MessageBox("X轴机床标定文件转换完成！");
		QMessageBox::information(this,QString::fromLocal8Bit("X轴机床标定文件转换完成！"),
		QString::fromLocal8Bit("X轴机床标定文件转换完成！"),"OK");

    }else{
		//MessageBox("请检查X轴机床标定文件是否存在！","ERROR");
		//QMessageBox::information(this,QString::fromLocal8Bit("请检查X轴机床标定文件是否存在！","ERROR"),
		//QString::fromLocal8Bit("请检查X轴机床标定文件是否存在！","ERROR");
		    QMessageBox customMsgBox;
			customMsgBox.setWindowTitle(QString::fromLocal8Bit("ERROR"));
			customMsgBox.setText(QString::fromLocal8Bit("请检查X轴机床标定文件是否存在！"));
			customMsgBox.exec();
    }
}
void FAdjustMachine::OnMachinefiley() 
{
	 //CTypedPtrArray<CObArray,CPoints*> machinepiontlist;
	FIpPntList machinepiontlist;
	char line[255];
	QString aline;
	int number,count,target;
	float data1,data2,average;
	float data;
	QString fname = gAppPath+"/settings/Y.rtl";
    if (fopen(fname.toLocal8Bit().data(),"r")!=NULL) {
		FILE * file=fopen(fname.toLocal8Bit().data(),"r");
		while(fgets(line,sizeof(line),file) != NULL)
		{
			aline=line;
			if (aline.left(12)=="Target-count")
			{
				QString s="Target-count: ";
				aline=aline.mid(s.size());
				//aline.TrimLeft("Target-count: ");
				number=(int)aline.toDouble();
				
				fgets(line,sizeof(line),file);  //选择下一行数据
				fgets(line,sizeof(line),file);
				aline=line;

				data1=aline.left(aline.indexOf(" ")).toFloat();
				aline=aline.remove(0,aline.indexOf(" ")+2);
				//data2=atof(aline.Left(aline.FindOneOf(" ")));
				data2=aline.left(aline.indexOf(" ")).toFloat();
				average=data2-data1;
				
				for(int i=0;i<number;i++){
					//CPoints * machinepiont = new CPoints;
					FIpPoint machinepiont;
					machinepiontlist.push_back(machinepiont);
					machinepiontlist[i].x=data1+average*i;
				}
			}
			
			if (aline.left(9)=="Run-count")
			{
				//aline.TrimLeft("Run-count:");
				QString s="Run-count:";
				aline=aline.mid(s.size(),-1);
				count=aline.toInt();
				
				fgets(line,sizeof(line),file);  //选择下一行数据
				fgets(line,sizeof(line),file);
				fgets(line,sizeof(line),file);
				for(int i=0;i<number*count;i++){
					fgets(line,sizeof(line),file);
					aline=line;
					//aline.Delete(0,4);
                    aline=aline.remove(0,4);
					target=aline.left(aline.indexOf(" ")).toFloat();
					//aline.Delete(0,aline.FindOneOf(" ")+1);
					aline=aline.remove(0,aline.indexOf(" ")+1);
					data=aline.toFloat();//target-1
					machinepiontlist[target-1].y+=(data/(count*1.0))*1.0;
					float aaa = machinepiontlist[target-1].y;
				}
			}
		}
		fclose(file);
		fname = gAppPath+"/settings/nc/AjustMachineY.txt";
		file=fopen(fname.toLocal8Bit().data(),"w");
		float x,y;
		char c='\n';
		for (int i=0;i<number;i++)
		{
			x=machinepiontlist[i].x;
			y=machinepiontlist[i].y;
			fprintf(file, "%f %f%c",x,y,c);
		}
		fclose(file);
		//MessageBox("X轴机床标定文件转换完成！");
		QMessageBox::information(this,QString::fromLocal8Bit("Y轴机床标定文件转换完成！"),
		QString::fromLocal8Bit("Y轴机床标定文件转换完成！"),"OK");

    }else{
		//MessageBox("请检查X轴机床标定文件是否存在！","ERROR");
		//QMessageBox::information(this,QString::fromLocal8Bit("请检查X轴机床标定文件是否存在！","ERROR"),
		//QString::fromLocal8Bit("请检查X轴机床标定文件是否存在！","ERROR");
		    QMessageBox customMsgBox;
			customMsgBox.setWindowTitle(QString::fromLocal8Bit("ERROR"));
			customMsgBox.setText(QString::fromLocal8Bit("请检查Y轴机床标定文件是否存在！"));
			customMsgBox.exec();
    }
	
}

void FAdjustMachine::OnMachinefilez() 
{
	 //CTypedPtrArray<CObArray,CPoints*> machinepiontlist;
	FIpPntList machinepiontlist;
	char line[255];
	QString aline;
	int number,count,target;
	float data1,data2,average;
	float data;
	QString fname = gAppPath+"/settings/Z.rtl";
    if (fopen(fname.toLocal8Bit().data(),"r")!=NULL) {
		FILE * file=fopen(fname.toLocal8Bit().data(),"r");
		while(fgets(line,sizeof(line),file) != NULL)
		{
			aline=line;
			if (aline.left(12)=="Target-count")
			{
				QString s="Target-count: ";
				aline=aline.mid(s.size());
				//aline.TrimLeft("Target-count: ");
				number=(int)aline.toDouble();
				
				fgets(line,sizeof(line),file);  //选择下一行数据
				fgets(line,sizeof(line),file);
				aline=line;

				data1=aline.left(aline.indexOf(" ")).toFloat();
				aline=aline.remove(0,aline.indexOf(" ")+2);
				//data2=atof(aline.Left(aline.FindOneOf(" ")));
				data2=aline.left(aline.indexOf(" ")).toFloat();
				average=data2-data1;
				
				for(int i=0;i<number;i++){
					//CPoints * machinepiont = new CPoints;
					FIpPoint machinepiont;
					machinepiontlist.push_back(machinepiont);
					machinepiontlist[i].x=data1+average*i;
				}
			}
			
			if (aline.left(9)=="Run-count")
			{
				//aline.TrimLeft("Run-count:");
				QString s="Run-count:";
				aline=aline.mid(s.size(),-1);
				count=aline.toInt();
				
				fgets(line,sizeof(line),file);  //选择下一行数据
				fgets(line,sizeof(line),file);
				fgets(line,sizeof(line),file);
				for(int i=0;i<number*count;i++){
					fgets(line,sizeof(line),file);
					aline=line;
					//aline.Delete(0,4);
                    aline=aline.remove(0,4);
					target=aline.left(aline.indexOf(" ")).toFloat();
					//aline.Delete(0,aline.FindOneOf(" ")+1);
					aline=aline.remove(0,aline.indexOf(" ")+1);
					data=aline.toFloat();//target-1
					machinepiontlist[target-1].y+=(data/(count*1.0))*1.0;
					float aaa = machinepiontlist[target-1].y;
				}
			}
		}
		fclose(file);
		fname = gAppPath+"/settings/nc/AjustMachineZ.txt";
		file=fopen(fname.toLocal8Bit().data(),"w");
		float x,y;
		char c='\n';
		for (int i=0;i<number;i++)
		{
			x=machinepiontlist[i].x;
			y=machinepiontlist[i].y;
			fprintf(file, "%f %f%c",x,y,c);
		}
		fclose(file);
		//MessageBox("X轴机床标定文件转换完成！");
		QMessageBox::information(this,QString::fromLocal8Bit("Z轴机床标定文件转换完成！"),
		QString::fromLocal8Bit("Z轴机床标定文件转换完成！"),"OK");

    }else{
		//MessageBox("请检查X轴机床标定文件是否存在！","ERROR");
		//QMessageBox::information(this,QString::fromLocal8Bit("请检查X轴机床标定文件是否存在！","ERROR"),
		//QString::fromLocal8Bit("请检查X轴机床标定文件是否存在！","ERROR");
		    QMessageBox customMsgBox;
			customMsgBox.setWindowTitle(QString::fromLocal8Bit("ERROR"));
			customMsgBox.setText(QString::fromLocal8Bit("请检查Z轴机床标定文件是否存在！"));
			customMsgBox.exec();
    }
}

void FAdjustMachine::OnConfirmCoor()
{
	D_RealInCali.x = m_pRealInCali_X->text().toDouble();
	D_RealInCali.y = m_pRealInCali_Y->text().toDouble();
	D_RealInCali.z = m_pRealInCali_Z->text().toDouble();
}

void FAdjustMachine::OnOpenCloseMachineComp()
{
	if (gWindow->GetMainDoc()->getIsMachineErrorCmps())
	{
		gWindow->GetMainDoc()->setIsMachineErrorCmps(false);
		m_pUseOfMachineCompBnt->setText(QString::fromLocal8Bit("启用机床误差补偿(当前已关闭)"));
	}
	else
	{
		gWindow->GetMainDoc()->setIsMachineErrorCmps(true);
		m_pUseOfMachineCompBnt->setText(QString::fromLocal8Bit("关闭机床误差补偿(当前已开启)"));
	}
}

void FAdjustMachine::GlobalRegistration()
{
	//实测数据
	FIpPntList dataafter = gWindow->GetMainDoc()->GetMesPntList();

	//理论数据
	FIpPntList datatrue = gWindow->GetMainDoc()->GetIpPntList();

	double i0, j0, k0, x, y, z, dxcastr, dxcastx, dxcasty, dxcastz;
	double datalisti1[200][3];
	double datalisti2[200][3];
	int m = dataafter.size();
	int s = 0;
	int r = 0;

	double sumyy1 = 0;
	double sumy1 = 0;
	double sumey1 = 0;
	double sume1 = 0;

	double sumxx2 = 0;
	double sumx2 = 0;
	double sumex2 = 0;
	double sume2 = 0;

	double k, b1, b2;
	double srdiv;

	for (int i = 0; i < m; i++) {
		i0 = datatrue[i].i;
		j0 = datatrue[i].j;
		if (i0 == 1 || i0 == -1)
		{
			datalisti1[r][1] = dataafter[i].x - datatrue[i].x;
			datalisti1[r][0] = dataafter[i].y;
			r++;
		}
		if (j0 == 1 || j0 == -1)
		{
			datalisti2[s][1] = dataafter[i].y - datatrue[i].y;
			datalisti2[s][0] = dataafter[i].x;
			s++;
		}
	}

	for (int i = 0; i < r; i++)
	{
		sumyy1 += datalisti1[i][0] * datalisti1[i][0];
		sumy1 += datalisti1[i][0];
		sumey1 += datalisti1[i][1] * datalisti1[i][0];
		sume1 += datalisti1[i][1];
	}

	for (int i = 0; i < s; i++)
	{
		sumxx2 += datalisti2[i][0] * datalisti2[i][0];
		sumx2 += datalisti2[i][0];
		sumex2 += datalisti2[i][1] * datalisti2[i][0];
		sume2 += datalisti2[i][1];
	}

	//srdiv=sumyy1+sumxx2-sumy1-sumx2;
	srdiv = s * r * (sumyy1 + sumxx2) - s * sumy1 * sumy1 - r * sumx2 * sumx2;
	if (srdiv<0.00001 && srdiv>-0.00001 || s == 0 || r == 0)
	{
		k = 0;
		b1 = 0;
		b2 = 0;

		QMessageBox::warning(QApplication::activeWindow(), QObject::tr("Caution"), QString::fromLocal8Bit("跳过XY轴误差消除"));
	}
	else
	{
		k = (s * r * sumex2 - s * r * sumey1 - r * sumx2 * sume2 + s * sumy1 * sume1) / srdiv;
		b1 = (sume2 - k * sumx2) / s;
		b2 = (sume1 + k * sumy1) / r;
	}
	double th = atan(k);
	double dc = th * 180 / 3.1415926;
	double dx = (b2 - k * b1) / (1 + k * k);
	double dy = (k * b2 + b1) / (1 + k * k);

	int num = 0;
	double kkk;

	double x0 = 0, y0 = 0, z0 = 0, dz = 0;

	for (int i = 0; i < m; i++) {
		kkk = datatrue[i].k;
		z = dataafter[i].z;
		z0 = datatrue[i].z;
		if ((1 - kkk) <= 0.01)
		{
			dz += (z - z0) * kkk;
			num++;
		}
	}
	if (num == 0) {
		dz = 0;
	}
	else {
		dz = dz / num;
	}

	double ii, jj, kk;
	double i00, j00;
	double x00, y00, z00;
	double l;
	double fen;
	double deg;
	double ij0;

	for (int i = 0; i < m; i++)
	{
		i0 = datatrue[i].i;
		j0 = datatrue[i].j;
		k0 = datatrue[i].k;
		x0 = datatrue[i].x;
		y0 = datatrue[i].y;
		z0 = datatrue[i].z;
		x = dataafter[i].x;
		y = dataafter[i].y;
		z = dataafter[i].z;
		ij0 = pow(pow(i0, 2) + pow(j0, 2), 0.5);	//向量xy平面投影长度
		if (ij0 == 0)
		{
			ii = 0;
			jj = 0;
		}
		else
		{
			deg = asin(j0 / ij0);
			if (i0 != 0) {
				if (i0 > 0) {		//右半平面,j>=0第一象限，deg值不变
					if (j0 < 0) {	//右半平面,j<0第四象限，deg值+2pai
						deg += 2 * PI;
					}
				}
				else {			//左半平面
					deg = PI - deg;
				}
			}
			else {
				if (j0 >= 0) {			//=0不可能出现，保留
					deg = PI / 2;
				}
				else {
					deg = PI * 3 / 2;
				}
			}

			ii = cos(deg + th) * ij0;
			jj = sin(deg + th) * ij0;
		}

		kk = k0;
		x00 = x0 * cos(th) - y0 * sin(th) + dx;
		y00 = x0 * sin(th) + y0 * cos(th) + dy;
		z00 = z0 + dz;

		fen = ii * i0 + jj * j0 + k0 * k0;
		if (fen != 0)
		{
			l = (ii * (x00 - x0) + jj * (y00 - y0) + kk * (z00 - z0)) / fen;
			x -= i0 * l;
			y -= j0 * l;
			z -= k0 * l;
			dataafter[i].x = x;
			dataafter[i].y = y;
			dataafter[i].z = z;
		}
		else
		{
			QMessageBox::warning(QApplication::activeWindow(), QObject::tr("Caution"), QString::fromLocal8Bit("补偿分母为0！"));
			//MessageBox(NULL, LPCWSTR("补偿分母为0！"), LPCWSTR("ERROR"), MB_YESNO);
		}
	}
}
