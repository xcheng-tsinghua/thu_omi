#include "DataTransferTool.h"
#include <Eigen\Dense>
#include "../MainApp/MainDoc.h"
#include "qinputdialog.h"
#include "MainApp/FInsFeaTreeWnd.h"
#include "MainApp/FAdjustMachine.h"
#include "../MainApp/np.h"
#include "../MainApp/MeasPt.h"

using namespace Eigen;

extern FMainWindow* gWindow;
extern QSettings* gDlgSetting;

CSDataTrans::CSDataTrans()
{
	pMc = NULL;
}

CSDataTrans::~CSDataTrans()

{

}

void  CSDataTrans:: txtFileReader(FMainDoc* m_ppDoc,int mode)
{
	ifstream infile;
	infile.open("C:/Users/Public/Data.txt");
	assert(infile.is_open());

	string s;
	vector<string> vec;

	while(getline(infile,s))
	{
		vec.push_back(s);
		//cout<<s<<endl;
	}

	infile.close();

	char pattern = '\t';

	for(int i = 0; i < vec.size(); i++)
	{
		stringstream input(vec.at(i));
		string temp;
		vector<string> res;

		while(getline(input,temp,pattern))
		{
			res.push_back(temp);
			//cout<<temp<<endl;
		}

		num = atoi(res.at(0).c_str());
		mes_x = sci2db(res.at(1));
		mes_y = sci2db(res.at(2));
		mes_z = sci2db(res.at(3));

		if(mode == 0)
		{
			//cout << "No."<<num<<" x="<<mes_x<<" y="<<mes_y<<" z="<<mes_z<<endl;
			m_ppDoc->addToMesPointList(num,mes_x,mes_y,mes_z);
		}
		if(mode == 1)
		{
			FProbeComp mc = FProbeComp(m_ppDoc);
			m_ppDoc->addToMesPointList(num,mes_x,mes_y,mes_z);
			mc.errCompUpperSphere(num-1);
		}
		if(mode == 2)
		{
			FProbeComp mc = FProbeComp(m_ppDoc);
			m_ppDoc->addToMesPointList(num,mes_x,mes_y,mes_z);
			mc.errCompFixedAngle(num-1);
		}


	}


}

bool CSDataTrans::txtFileReader(FMainDoc* m_ppDoc, int mode, double m_dd)
{
	ifstream infile;

	QString defaultGetDir = gDlgSetting->value("SelMeasFilePath").toString();
	QString filename = QFileDialog::getOpenFileName(NULL, QString::fromLocal8Bit("选择测量数据文件"), defaultGetDir, QString("NC files (*.txt)"));

	if (filename.isEmpty()) return false;

	infile.open(filename.toStdString());
	cout << filename.toStdString() << endl;
	//infile.open("C:/Users/Public/Data.txt");
	/*ofstream outfile_Data2("C:/Users/Public/Data2.txt");*/
	//assert(infile.is_open());

	if(!infile.is_open()) return false;

	gDlgSetting->setValue("SelMeasFilePath", filename);

	string s;
	vector<string> vec;
	//cout << "m_dd=" << m_dd << endl;
	while (getline(infile, s))
	{
		vec.push_back(s);
		cout << s << endl;
	}

	infile.close();

	char pattern = '\t';
	/*cout << "vec.size=" << vec.size() << endl;*/

	for (int i = 0; i < vec.size(); i++)
	{
		stringstream input(vec.at(i));
		string temp;
		vector<string> res;

		while (getline(input, temp, pattern))
		{
			res.push_back(temp);
			//cout<<temp<<endl;
		}


		num = atoi(res.at(0).c_str());
		cout << "num= " << num << endl;
		mes_x = sci2db(res.at(1));
		mes_y = sci2db(res.at(2));
		mes_z = sci2db(res.at(3));

		FIpPntList  theoPntList;   //zsq
		FMainDoc* pDoc = gWindow->GetMainDoc();
		theoPntList = gWindow->GetMainDoc()->GetIpPntList();
		int theoNum = theoPntList.size();

		FIpPoint temPnt;
		temPnt.i = theoPntList[i].i;
		temPnt.j = theoPntList[i].j;
		temPnt.k = theoPntList[i].k;
		mes_x = mes_x - m_dd / 2 * temPnt.i;
		mes_y = mes_y - m_dd / 2 * temPnt.j;
		mes_z = mes_z - m_dd / 2 * temPnt.k;
		if (abs(mes_x) <= 0.0001) { mes_x = 0; }
		if (abs(mes_y) <= 0.0001) { mes_y = 0; }
		if (abs(mes_z) <= 0.0001) { mes_z = 0; }

		/*cout << "i=" << temPnt.i << " j=" << temPnt.j << " k=" << temPnt.k << endl;*/


	/*	cout << "No."<<num<<" x="<<mes_x<<" y="<<mes_y<<" z="<<mes_z<<endl;*/
		if (mode == 0)
		{
			//cout << "No."<<num<<" x="<<mes_x<<" y="<<mes_y<<" z="<<mes_z<<endl;
			m_ppDoc->addToMesPointList(num, mes_x, mes_y, mes_z);
		}
		if (mode == 1)
		{
			/*cout << "No."<<num<<" x="<<mes_x<<" y="<<mes_y<<" z="<<mes_z<<endl;*/
			FProbeComp mc = FProbeComp(m_ppDoc);
			m_ppDoc->addToMesPointList(num, mes_x, mes_y, mes_z);
			mc.errCompUpperSphere(num - 1);
		}
		if (mode == 2)
		{
			FProbeComp mc = FProbeComp(m_ppDoc);
			m_ppDoc->addToMesPointList(num, mes_x, mes_y, mes_z);
			mc.errCompFixedAngle(num - 1);
		}
	}

	return true;
}

void CSDataTrans::txtFileReader(QString name)
{
	MatrixXd totMat(4, 4);
	vector<double> totVec;
	totVec = gWindow->GetMainDoc()->getTransVec(4);
	int num = 0;
	if (totVec.size() == 16)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				totMat(i, j) = totVec.at(num);
				num++;
			}
		}
	}

	else if (totVec.size() == 0)
	{
		totMat.setIdentity();
	}

	else
	{
		std::cout << "Wrong size of the transfer matrix! Check the file!" << endl;
		return;
	}

	totMat(0, 3) = -totMat(0, 3);
	totMat(1, 3) = -totMat(1, 3);
	totMat(2, 3) = -totMat(2, 3);

	MatrixXd inv_Tot(4, 4);
	inv_Tot = totMat.inverse();
	
	ifstream infile;
	infile.open(name.toStdString());
	string s;
	vector<string> vec;

	if (!pMc)
	{
		FProbeComp* pMc = new FProbeComp(gWindow->GetMainDoc());
	}

	while (getline(infile, s))
	{
		vec.push_back(s);
	}

	infile.close();

	bool IsReadFromMpf;

	if (isSuffixXXX(name.toStdString(), "txt"))
	{
		IsReadFromMpf = false;
	}
	else if (isSuffixXXX(name.toStdString(), "MPF") || isSuffixXXX(name.toStdString(), "mpf"))
	{
		IsReadFromMpf = true;
	}
	else
	{
		MesgBox(QString::fromLocal8Bit("仅支持后缀为 mpf/MPF txt 文件"));
		return;
	}

	char pattern = '\t';

	if (gWindow->GetMainDoc()->GetIpPntList().size() == 0)
	{
		QMessageBox::information(NULL, QString::fromLocal8Bit("EROOR"),QString::fromLocal8Bit("未检测到理论测点，请确认已生成理论测点后再试"), "OK");
		return;
	}

	int NbMeas = vec.size();
	int NbTheo = gWindow->GetMainDoc()->GetIpPntList().size();


	if (vec.size() != gWindow->GetMainDoc()->GetIpPntList().size())
	{
		QMessageBox::information(NULL, QString::fromLocal8Bit("EROOR"), QString::fromLocal8Bit("导入的实际测点数与理论测点数不同，请检查！"), "OK");
		return;
	}

	for (int i = 0; i < vec.size(); i++)
	{
		stringstream input(vec.at(i));
		string temp;
		vector<string> res;

		while (getline(input, temp, pattern))
		{
			res.push_back(temp);
			//cout<<temp<<endl;
		}

		num = atoi(res.at(0).c_str());

		if (IsReadFromMpf)
		{
			mes_x = sci2dbEX(res.at(1));
			mes_y = sci2dbEX(res.at(2));
			mes_z = sci2dbEX(res.at(3));
		}
		else
		{
			mes_x = sci2db(res.at(1));
			mes_y = sci2db(res.at(2));
			mes_z = sci2db(res.at(3));
		}
		
		FIpPoint temPnt = gWindow->GetMainDoc()->GetIpPnt(i);
		m_dd = gWindow->GetMainDoc()->getProbeDia();

		if (abs(m_dd) > 100)
		{
			QMessageBox::warning(QApplication::activeWindow(),QString::fromLocal8Bit("错误！！！"), QString::fromLocal8Bit("未设定测针红宝石球直径！！！"), QMessageBox::Ok | QMessageBox::Cancel | QMessageBox::Abort);

			m_dd = QInputDialog::getDouble(0, QString::fromLocal8Bit("输入测针红宝石球直径"), QString::fromLocal8Bit("测针红宝石球直径:"), 6, 0, 100, 5);
			gWindow->GetMainDoc()->setProbeDia(m_dd);
		}

		mes_x = mes_x - m_dd / 2 * temPnt.i;
		mes_y = mes_y - m_dd / 2 * temPnt.j;
		mes_z = mes_z - m_dd / 2 * temPnt.k;
		if (abs(mes_x) <= 0.0001) { mes_x = 0; }
		if (abs(mes_y) <= 0.0001) { mes_y = 0; }
		if (abs(mes_z) <= 0.0001) { mes_z = 0; }

		Vector4d transV(mes_x, mes_y, mes_z, 1);

		Vector4d oriV = inv_Tot * transV;

		FIpPoint tmpPnt = FIpPoint(oriV[0], oriV[1], oriV[2]);

		if (gWindow->GetMainDoc()->getIsMachineErrorCmps())
		{
			if (!gWindow->GetMachineErrCompDlg())
			{
				QMessageBox::information(NULL, QString::fromLocal8Bit("EROOR"), QString::fromLocal8Bit("未开启机床误差补偿窗口进行操作，未进行误差补偿"), "OK");
			}
			else
			{
				FIpPoint tmpTheoPnt = gWindow->GetMainDoc()->GetIpPntList()[num - 1];

				function<double(double, double)> SquareCond = [&](double Val1, double Val2)->double {return (Val1 - Val2) * (Val1 - Val2); };

				if (sqrt(SquareCond(tmpPnt.x, tmpTheoPnt.x) + SquareCond(tmpPnt.y, tmpTheoPnt.y) + SquareCond(tmpPnt.z, tmpTheoPnt.z)) > 5)
				{
					QMessageBox::information(NULL, QString::fromLocal8Bit("EROOR"), QString::fromLocal8Bit("理论点和对应实际点距离超过5mm，请检查程序，未进行误差补偿"), "OK");
				}
				else
				{
					gWindow->GetMachineErrCompDlg()->MachineErrComp(tmpPnt, tmpTheoPnt);
				}
			}
		}
		 
		gWindow->GetMainDoc()->addToMesPointList(num, tmpPnt.x, tmpPnt.y, tmpPnt.z);

		pMc->errCompUpperSphere(num - 1);

	}

	//static_cast<FInsFeaTreeWnd*>(gWindow->GetSubWnd()[6])->onUpdateMeasPnts();
}

void CSDataTrans::importMesData(QString _filename)
{
	ifstream infile;
	infile.open(_filename.toStdString());
	string s;
	vector<string> vec;

	bool IsReadFromMpf;

	if (isSuffixXXX(_filename.toStdString(), "txt"))
	{
		IsReadFromMpf = false;
	}
	else if (isSuffixXXX(_filename.toStdString(), "MPF") || isSuffixXXX(_filename.toStdString(), "mpf"))
	{
		IsReadFromMpf = true;
	}
	else
	{
		MesgBox(QString::fromLocal8Bit("仅支持后缀为 mpf/MPF txt 文件"));
		return;
	}

	while (getline(infile, s))
	{
		vec.push_back(s);
	}

	infile.close();

	char pattern = '\t';

	if (gWindow->GetMainDoc()->GetIpPntList().size() == 0)
	{
		QMessageBox::information(NULL, QString::fromLocal8Bit("EROOR"), QString::fromLocal8Bit("未检测到理论测点，请确认已生成理论测点后再试"), "OK");
		return;
	}

	int NbMeas = vec.size();
	int NbTheo = gWindow->GetMainDoc()->GetIpPntList().size();

	if (vec.size() != gWindow->GetMainDoc()->GetIpPntList().size())
	{
		QMessageBox::information(NULL, QString::fromLocal8Bit("EROOR"), QString::fromLocal8Bit("导入的实际测点数与理论测点数不同，请检查！"), "OK");
		return;
	}

	for (int i = 0; i < vec.size(); i++)
	{
		stringstream input(vec.at(i));
		string temp;
		vector<string> res;
		double x_read, y_read, z_read;

		while (getline(input, temp, pattern))
		{
			res.push_back(temp);
		}

		int num = atoi(res.at(0).c_str());

		if (IsReadFromMpf)
		{
			x_read = sci2dbEX(res.at(1));
			y_read = sci2dbEX(res.at(2));
			z_read = sci2dbEX(res.at(3));
		}
		else
		{
			x_read = sci2db(res.at(1));
			y_read = sci2db(res.at(2));
			z_read = sci2db(res.at(3));
		}

		if (abs(x_read) <= 0.0001) { x_read = 0; }
		if (abs(y_read) <= 0.0001) { y_read = 0; }
		if (abs(z_read) <= 0.0001) { z_read = 0; }

		// 点的 num 编号从 1 开始，非零
		gWindow->GetMainDoc()->addToMesPointList(num, x_read, y_read, z_read);

		double x, y, z, x0, y0, z0, result, err, uperror, downerror, deg, degz;//deg,degz测量点
		double x1, x2, y1, y2, deg1, deg2, r1, r2;
		int symbol;

		FIpPoint cMesPnt = gWindow->GetMainDoc()->GetMesPnt(num - 1);
		x = cMesPnt.x;
		y = cMesPnt.y;
		z = cMesPnt.z;

		FIpPoint cTheoPnt = gWindow->GetMainDoc()->GetIpPnt(num - 1);
		x0 = cTheoPnt.x;
		y0 = cTheoPnt.y;
		z0 = cTheoPnt.z;

		if (abs(x) <= 0.0001) { x = 0; }
		if (abs(y) <= 0.0001) { y = 0; }
		if (abs(z) <= 0.0001) { z = 0; }

		//计算误差
		uperror = cMesPnt.uperror;
		downerror = cMesPnt.downerror;

		err = pow(pow(x - x0, 2) + pow(y - y0, 2) + pow(z - z0, 2), 0.5);
		if (abs(err) <= 0.0001) { err = 0; }

		gWindow->GetMainDoc()->addToMesErrList(num, err);

	}

	//static_cast<FInsFeaTreeWnd*>(gWindow->GetSubWnd()[6])->onUpdateMeasPnts();

}

void CSDataTrans::importTheoData(QString name)
{
	vector<TopoDS_Face> TDSFaceList;
	if (gWindow->GetOccConverter() != NULL && gWindow->GetOccConverter()->GetTopoDS_FaceList().size() != 0)
	{
		TDSFaceList = gWindow->GetOccConverter()->GetTopoDS_FaceList();
	}

	ifstream infile;
	infile.open(name.toStdString());
	string s;
	vector<string> vec;
	FIpPntList pntList;

	while (getline(infile, s))
	{
		vec.push_back(s);
		//cout<<s<<endl;
	}

	infile.close();

	char pattern = '\t';

	for (int i = 0; i < vec.size(); i++)
	{
		stringstream input(vec.at(i));
		string temp;
		vector<string> res;

		while (getline(input, temp, pattern))
		{
			res.push_back(temp);
			//cout<<temp<<endl;
		}

		double ip_x, ip_y, ip_z, ip_i, ip_j, ip_k;

		if (res.size() != 4 && res.size() != 7)
		{
			QMessageBox::information(gWindow, QString::fromLocal8Bit("理论测量点数据不正确,ERROR!"),
				QString::fromLocal8Bit("理论测点数据文件应为4列或7列，请检查选中的txt文件！"), "OK");
			return;
		}

		else if (res.size() == 4)
		{
			ip_x = atof(res.at(1).c_str());
			ip_y = atof(res.at(2).c_str());
			ip_z = atof(res.at(3).c_str());

			// 计算法线
			// 先获取全部型面
			if (gWindow->GetOccConverter() == NULL || gWindow->GetOccConverter()->GetTopoDS_FaceList().size() == 0)
			{
				MesgBox2("未导入模型，无法计算法线");
				return;
			}

			// 找到该点所属的型面
			gp_Vec cNorm = gp_Vec(-999, -999, -999);
			gp_Pnt cPnt = gp_Pnt(ip_x, ip_y, ip_z);
			bool isNormCal = false;
			for (auto ita = TDSFaceList.begin(); ita != TDSFaceList.end(); ++ita)
			{
				if (isPointInShape(cPnt, *ita, 0.01))
				{
					cNorm = NormalAt(*ita, cPnt);
					isNormCal = true;
					break;
				}
			}

			// 判断是否找到
			if (!isNormCal)
			{
				MesgBox2("存在点未找到所在曲面，已跳过该点");
				continue;
			}

			ip_i = cNorm.X();
			ip_j = cNorm.Y();
			ip_k = cNorm.Z();

			FIpPoint ip = FIpPoint(ip_x, ip_y, ip_z);
			ip.i = ip_i; ip.j = ip_j; ip.k = ip_k;

			pntList.append(ip);
			MeasPt* tmpReadPt = new MeasPt(ip_x, ip_y, ip_z, ip_i, ip_j, ip_k);
			gWindow->GetMainDoc()->AddDispEnt(tmpReadPt);

		}

		else if (res.size() == 7)
		{
			ip_x = atof(res.at(1).c_str());
			ip_y = atof(res.at(2).c_str());
			ip_z = atof(res.at(3).c_str());
			FIpPoint ip = FIpPoint(ip_x, ip_y, ip_z);
			ip_i = atof(res.at(4).c_str());
			ip_j = atof(res.at(5).c_str());
			ip_k = atof(res.at(6).c_str());

			ip.i = ip_i; ip.j = ip_j; ip.k = ip_k;

			pntList.append(ip);
			MeasPt* tmpReadPt = new MeasPt(ip_x, ip_y, ip_z, ip_i, ip_j, ip_k);
			gWindow->GetMainDoc()->AddDispEnt(tmpReadPt);

		}

	}

	gWindow->GetMainDoc()->ClearIpPntList();
	gWindow->GetMainDoc()->SetIpPntList(pntList);

}



//double CSDataTrans::sci2db(const std::string &strSci)
//{
//	int    iPower       = 0;  //幂
//	double dMntsa       = 0;  //尾数
//	double dCoefficient = 1;  //系数
//
//	std::string strPower, strMntsa;
//
//	if (std::string::npos == strSci.find("E"))
//	{
//		return atof(strSci.c_str());
//	}
//
//	strMntsa = strSci.substr(0, strSci.find("E"));
//	strPower = strSci.substr(strSci.find("E") + 1);
//
//	dMntsa = atof(strMntsa.c_str());
//	iPower = atoi(strPower.c_str());
//
//	while (iPower != 0)
//	{
//		if (iPower > 0)
//		{
//			dCoefficient *= 10;
//			iPower--;
//		}
//		else
//		{
//			dCoefficient *= 0.1;
//			iPower++;
//		}
//	}
//
//	return dMntsa * dCoefficient;
//}

//double CSDataTrans::sci2dbEX(const std::string& strSci)
//{
//	int    iPower = 0;  //幂
//	double dMntsa = 0;  //尾数
//	double dCoefficient = 1;  //系数
//
//	std::string strPower, strMntsa;
//
//	if (std::string::npos == strSci.find("EX"))
//	{
//		return atof(strSci.c_str());
//	}
//
//	strMntsa = strSci.substr(0, strSci.find("EX"));
//	strPower = strSci.substr(strSci.find("EX") + 2);
//
//	dMntsa = atof(strMntsa.c_str());
//	iPower = atoi(strPower.c_str());
//
//	while (iPower != 0)
//	{
//		if (iPower > 0)
//		{
//			dCoefficient *= 10;
//			iPower--;
//		}
//		else
//		{
//			dCoefficient *= 0.1;
//			iPower++;
//		}
//	}
//
//	return dMntsa * dCoefficient;
//}
