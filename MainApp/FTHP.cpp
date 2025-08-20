#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>
#include "FUtilities.h"
#include "FTHP.h"
#include "FMainWindow.h"
#include "np.h"
#include "NCEditDlg.h"
#include <memory>

extern QSettings* gDlgSetting;

FTHP::FTHP()
{
	InitDefaultStr();
}

FTHP::FTHP( QString codeIn )
	:code(codeIn)
{
	X	=	TH_GetValue("X");
	Y	=	TH_GetValue("Y");
	Z	=	TH_GetValue("Z");
	I	=	TH_GetValue("I");
	J	=	TH_GetValue("J");
	K	=	TH_GetValue("K");

	InitDefaultStr();

}

double FTHP::TH_GetValue( QString symbol )
{
	QString	section	=	code.mid(code.indexOf(symbol)+1);
	double	d	=	atof(section.toLocal8Bit());
	return	d;
}

QString FTHP::TH_GetProfileString( QString sAppName, QString sKeyName )
{
	QString	str;
	str	=	gAppSetting->value("/"+sAppName+"/"+sKeyName,"ERROR").toString();
	return	str;
}

QString FTHP::TH_GetBody( double X,double Y,double Z,double I,double J,double K )
{
	QString Meas[10];
	QString DataIn[3];
	QString qsX,qsY,qsZ,qsDX,qsDY,qsDZ,qsL,qsE,qsR;
	QString G	=	"";

	QString NCS	=	TH_GetProfileString("NCS","NC_Type");	//判断数控系统类型
	for(int i=0;i<10;i++)
	{
		Meas[i]	=	TH_GetProfileString(NCS,NCS+"_Meas"+QString("%1").arg(i));
	}
	if((NCS!="FDIA3")&&(NCS!="FDIA5"))
	{
		DataIn[0]=TH_GetProfileString(NCS,NCS+"_IN0");
		DataIn[1]=TH_GetProfileString(NCS,NCS+"_IN1");
		DataIn[2]=TH_GetProfileString(NCS,NCS+"_IN2");
	}
	double	Distance	=	gAppSetting->value("/PARA/Distance",0).toDouble();	//测量起始距离
	int		OverPass	=	gAppSetting->value("/PARA/OverPass",0).toDouble();	//过行程
	if((NCS!="FDIA3")&&(NCS!="FDIA5"))
	{
		qsX	=	QString::number(X-OverPass*I,'f',4);
		qsY	=	QString::number(Y-OverPass*J,'f',4);
		qsZ	=	QString::number(Z-OverPass*K,'f',4);
		G	+=	DataIn[0]+"="+qsX+"\n";
		G	+=	DataIn[1]+"="+qsY+"\n";
		G	+=	DataIn[2]+"="+qsZ+"\n";
		for(int i=0;i<10;i++)
		{
			if (Meas[i]!="None")
			{
				G	+=	Meas[i];
				G	+=	"\n";
			}
		}
	}
	else
	{
		qsX		=	QString::number(X,'f',4);
		qsY		=	QString::number(Y,'f',4);
		qsZ		=	QString::number(Z,'f',4);
		qsDX	=	QString::number(I,'f',4);
		qsDY	=	QString::number(J,'f',4);
		qsDZ	=	QString::number(K,'f',4);
		qsL		=	QString::number(Distance,'f',4);
		qsR		=	QString::number(Z+Distance*K,'f',4);
		qsE		=	QString::number(Distance,'f',4);
		G		+=	"G73 L"	+	qsL;
		G		+=	" E"	+	qsE;
		G		+=	" X"	+	qsX;
		G		+=	" Y"	+	qsY;
		G		+=	" Z"	+	qsZ;
		G		+=	" R"	+	qsR;
		G		+=	" DX"	+	qsDX;
		G		+=	" DY"	+	qsDY;
		G		+=	" DZ"	+	qsDZ	+	"\n";
	}
	return	G;
}

void FTHP::TH_NCFileWrite(QString sGCodeOut, QString fname)
{
	QFile file(fname);

	if (!file.open(QFile::WriteOnly|QFile::Truncate|QFile::Text))
	{
		QMessageBox::warning(0,QString::fromLocal8Bit("读入NC文件失败"),
			QString::fromLocal8Bit("无法打开NC程序进行写入，请检查路径是否正确！\n写入路径：")+ fname,"OK");
		return;
	}
	QTextStream	out(&file);
	out.setRealNumberNotation(QTextStream::FixedNotation); //ScientificNotation＃Scientific notation,FixedNotation＃定点符号,SmartNotation＃科学或定点符号
	out	<<	sGCodeOut;
	file.close();
}

void FTHP::setIsPauseBeforeMeas(bool _isPause)
{
	m_isPauseBeforeMeas = _isPause;
}

QString FTHP::ConvertPoint2Code(FIpPoint& pnt, bool bIpPnt/* =false */)
{
	//防止NC代码中出现科学计数法
	if (abs(pnt.x) < 1e-5)
	{
		pnt.x = 0;
	}
	if (abs(pnt.y) < 1e-5)
	{
		pnt.y = 0;
	}
	if (abs(pnt.z) < 1e-5)
	{
		pnt.z = 0;
	}

	QString pntStr;
	if (bIpPnt)	//inspection point
	{
		pntStr = FTHP::TH_GetBody(pnt.x, pnt.y, pnt.z, pnt.i, pnt.j, pnt.k);

		cout << "GetBody输出：" << pntStr.toStdString() << endl;
		pntStr +=	"\n";

		pntStr += m_NC_SaveToMachineToolLocalPnt;

	}
	else	//intermediate point
	{
		pntStr = QString("R20=%1\nR21=%2\nR22=%3\nSMOVE\n").arg(pnt.x).arg(pnt.y).arg(pnt.z);
	}
	return pntStr;
}

// OPCUA 五轴路径规划代码输出
void FTHP::TH_NCFileWrite(FIpPathList& pathlist, QString fname/*="NC.mpf"*/)
{
	m_NC_CodeBody.clear();

	// file body
	//handle the first path
	int nPath = pathlist.size();
	if(nPath<1)	return ;
	bool bIpPnt(false);	//whether it is an inspection (true) or intermediate point (false)
	int nPnt = pathlist[0].GetSize();
	for (int j=0; j<nPnt; j++)	//intermediate points
	{
		if (j == 2 || j == nPnt - 1)//edited by chengxi 测头移动至的第一个点设置为第一个测点的回退点到安全平面的点，第二个点为第一个测点的回退点
		{
			bIpPnt = true;
			m_newPnts.append(pathlist[0].GetPoint(j));
		}
			
		else
			bIpPnt = false;

		m_NC_CodeBody += ConvertPoint2Code(pathlist[0].GetPoint(j), bIpPnt);
	}

	//handle 1~(nPath-1) path
	for (int i=1; i<nPath-1; i++)
	{
		nPnt = pathlist[i].GetSize();
		if(nPnt==0)	continue;
		for (int j=1; j<nPnt; j++)	//intermediate points
		{
			if (j == nPnt - 1)
			{
				bIpPnt = true;
				m_newPnts.append(pathlist[i].GetPoint(j));
			}
				
			else
				bIpPnt = false;

			m_NC_CodeBody += ConvertPoint2Code(pathlist[i].GetPoint(j), bIpPnt);
		}
	}

	//handle the last path
	nPnt = pathlist[nPath-1].GetSize();
	for (int j=1; j<nPnt; j++)	//intermediate points
	{
		if (j == nPnt - 1)
		{
			bIpPnt = true;
			m_newPnts.append(pathlist[nPath - 1].GetPoint(j));
		}
		else
			bIpPnt = false;

		m_NC_CodeBody += ConvertPoint2Code(pathlist[nPath - 1].GetPoint(j), bIpPnt);

	}

	QString SaveStr = TH_GetHead() + m_NC_CodeBody + TH_GetTail();

	ProcessAndSaveNC(m_NC_SaveToMachineToolLocalHead + SaveStr + m_NC_SaveToMachineToolLocalLast, fname);

}

void FTHP::ProcessAndSaveNC(QString fpNCContent, QString fpFileSavePath)
{

	//fpNCContent.replace("\nSCENTER\n", m_SCENTER);
	//fpNCContent.replace("\nSCOMEND\n", m_SCOMEND);
	//fpNCContent.replace("\nSCOMSTART\n", m_SCOMSTART);
	//fpNCContent.replace("\nSMOVE\n", m_SMOVE);
	//fpNCContent.replace("\nSMEAS\n", m_SMEAS);
	//fpNCContent.replace("\nSPROBE\n", m_SPROBE);

	//// R变量代换
	//fpNCContent.replace("R11", "R11##");
	//fpNCContent.replace("R12", "R12##");
	//fpNCContent.replace("R13", "R13##");
	//fpNCContent.replace("R20", "R20##");
	//fpNCContent.replace("R21", "R21##");
	//fpNCContent.replace("R22", "R22##");
	//fpNCContent.replace("R40", "R40##");
	//fpNCContent.replace("R41", "R41##");
	//fpNCContent.replace("R43", "R43##");
	//fpNCContent.replace("R80", "R80##");
	//fpNCContent.replace("R90", "R90##");
	//fpNCContent.replace("R91", "R91##");
	//fpNCContent.replace("R92", "R92##");
	//fpNCContent.replace("R93", "R93##");
	//fpNCContent.replace("R94", "R94##");
	//fpNCContent.replace("R95", "R95##");
	//fpNCContent.replace("R96", "R96##");

	//fpNCContent.replace("R11##", "R4");
	//fpNCContent.replace("R12##", "R5");
	//fpNCContent.replace("R13##", "R6");
	//fpNCContent.replace("R20##", "R7");
	//fpNCContent.replace("R21##", "R8");
	//fpNCContent.replace("R22##", "R9");
	//fpNCContent.replace("R40##", "R10");
	//fpNCContent.replace("R41##", "R11");
	//fpNCContent.replace("R43##", "R12");
	//fpNCContent.replace("R80##", "R13");
	//fpNCContent.replace("R90##", "R14");
	//fpNCContent.replace("R91##", "R15");
	//fpNCContent.replace("R92##", "R16");
	//fpNCContent.replace("R93##", "R17");
	//fpNCContent.replace("R94##", "R18");
	//fpNCContent.replace("R95##", "R19");
	//fpNCContent.replace("R96##", "R20");

	//// 昌飞检测 NC 需要在首次测量前暂停十秒
	//if (m_isPauseBeforeMeas)
	//{
	//	int index = fpNCContent.indexOf("\nMeas=");

	//	if (index != -1) {
	//		// 使用 replace 替换第一个 "\nmeas"
	//		fpNCContent.insert(index, "\ng4f10");
	//	}

	//}

	m_pNCEditDlg->setContent(fpNCContent);
	m_pNCEditDlg->setSavePathDef(fpFileSavePath);

	m_pNCEditDlg->show();

}

void FTHP::InitDefaultStr()
{
	m_pNCEditDlg = new NCEditDlg;
	m_isPauseBeforeMeas = false;

	// 获取红宝石球半径
	double probeRadius = gWindow->GetMainDoc()->getProbeDia() / 2.0;

	if (abs(probeRadius) > 50)
	{
		//MesgBox(QString::fromLocal8Bit("读取到的半径为%1").arg(probeRadius));

		QMessageBox::warning(QApplication::activeWindow(), QString::fromLocal8Bit("错误！！！"), QString::fromLocal8Bit("未设定测针红宝石球直径！！！"), QMessageBox::Ok | QMessageBox::Cancel | QMessageBox::Abort);

		probeRadius = QInputDialog::getDouble(0, QString::fromLocal8Bit("输入测针红宝石球直径"), QString::fromLocal8Bit("测针红宝石球直径:"), 6, 0, 100, 5);
		gWindow->GetMainDoc()->setProbeDia(probeRadius);

		probeRadius /= 2.0;
	}

	// 106五轴保存方法
	m_NC_SaveToMachineToolLocalHead = "DEF STRING[100] _File\nDEF INT _I_ERROR\n_File = $P_PROGPATH << \"_N_InspData\" << \"_MPF\"\n ";
	m_NC_SaveToMachineToolLocalPnt = "WRITE(_I_ERROR,_File, R[90]<<\"	\"<<R[91]<<\"	\"<<R[92]<<\"	\"<<R[93])\n\n";
	m_NC_SaveToMachineToolLocalLast = "M30\n";

	// 昌飞保存方法
	//m_NC_SaveToMachineToolLocalHead = "DEF STRING[16] _FILE\nDEF INT _XWJ\nDEF STRING[100] BLZ[100]\n_FILE = \"OMIINSPDATA\"\n ";
	//m_NC_SaveToMachineToolLocalPnt = "WRITE(_XWJ,\"/_N_SPF_DIR/\"<<_FILE,R[14]<<\"	\"<<R[15]<<\"	\"<<R[16]<<\"	\"<<R[17])\n\n";
	//m_NC_SaveToMachineToolLocalLast = "M30\n";

	// 使用 arg 方法格式化字符串，替换目标为 %1，从而将测头红宝石球半径 probeRadius 写入该字符串。
	m_SCENTER = QString("\nG01 G91 X=R43+20 F=R40\nG01 Z=-R43 F=R40\nMeas=2 X-40 F=R41\nstopre\nR11=$AA_IW[X]\nG01 X5 F=R40\nG01 Z=R43 F=R40\nG01 X=-2*R43-20 F=R40\nG01 Z=-R43 F=R40\nMeas=2 X40 F=R41\nstopre\nR12=$AA_IW[X]\nG01 X-5 F=R40\nR1=(R11+ R12)/2\nG01 Z=R43 F=R40\nG90 G01 X=R1 F=R40\nG01 G91 Y=R43+20 F=R40\nG01 Z=-R43 F=R40\nMeas=2 Y-40 F=R41\nstopre\nR14=$AA_IW[Y]\nG01 Y5 F=R40\nG01 Z=R43 F=R40\nG01 Y=-2*R43-20 F=R40\nG01 Z=-R43 F=R40\nMeas=2 Y40 F=R41\nstopre\nR15=$AA_IW[Y]\nG01 Y-5 F=R40\nR2=(R14+R15)/2\nG01 Z=R43 F=R40\nG90 G01 X=R1 Y=R2 F=R40\nG01 G91\nMeas=2 Z-20 F=R41\nstopre\nR3=$AA_IW[Z]-R43-%1;0.283333\nG01 Z=10 F=R40\n\nG01 G90 X=R1+R43+10 F=R40\nG01 G90 Z=R3 F=R40\nG01 G91\nMeas=2 X-20 F=R41\nstopre\nR11=$AA_IW[X]\nG01 X5 F=R40\nG01 G90 Z=R3+R43+10 F=R40\nG01 G90 X=R1-R43-10 F=R40\nG01 G90 Z=R3 F=R40\nG01 G91\nMeas=2 X20 F=R41\nstopre\nR12=$AA_IW[X]\nG01 X-5 F=R40\nR1=(R11+ R12)/2\nG01 G90 Z=R3+R43+10 F=R40\nG01 G90 X=R1 F=R40\nG01 G90 Y=R2+R43+10 F=R40\nG01 G90 Z=R3 F=R40\nG01 G91\nMeas=2 Y-20 F=R41\nstopre\nR14=$AA_IW[Y]\nG01 Y5 F=R40\nG01 G90 Z=R3+R43+10 F=R40\nG01 G90 Y=R2-R43-10 F=R40\nG01 G90 Z=R3 F=R40\nG01 G91\nMeas=2 Y20 F=R41\nstopre\nR15=$AA_IW[Y]\nG01 Y-5 F=R40\nR2=(R14+R15)/2\nG01 G90 Z=R3+R43+10 F=R40\nG90 G01 X=R1 Y=R2 F=R40\nG01 G91\nMeas=2 Z-20 F=R41\nstopre\nR3=$AA_IW[Z]-R43-%1;0.283333\nG01 Z=10 F=R40\n").arg(probeRadius);

	m_SCOMEND = "\nR90=9999\ng4f0.3\nstopre\n";

	m_SCOMSTART = "\nm66\nR90=9999\ng4f0.3\nstopre\nR90=8888\ng4f0.3\nstopre\n;commu data clear:R90=-1,R91 to R96=0\nR91=0\ng4f0.3\nstopre\nR92=0\ng4f0.3\nstopre\nR93=0\ng4f0.3\nstopre\nR94=0\ng4f0.3\nstopre\nR95=0\ng4f0.3\nstopre\nR96=0\ng4f0.3\nstopre\nR90=-1\ng4f0.3\nstopre\n;trasfer the workpiece coordination\nR91=$p_uifr[1,x,tr]\ng4f0.3\nstopre\nR92=$p_uifr[1,y,tr]\ng4f0.3\nstopre\nR93=$p_uifr[1,z,tr]\ng4f0.3\nstopre\nR90=0\ng4f0.3\nstopre\n;commu data clear:R90=-1,R91 to R96=0.R188-R198=0\nR91=0\ng4f0.3\nstopre\nR92=0\ng4f0.3\nstopre\nR93=0\ng4f0.3\nstopre\nR94=0\ng4f0.3\nstopre\nR95=0\ng4f0.3\nstopre\nR96=0\ng4f0.3\nstopre\nR90=-1\ng4f0.3\nstopre\n\n";

	m_SMEAS = "\nR11=$AA_IW[X]\nR12=$AA_IW[Y]\nR13=$AA_IW[Z]\nG90 G01\nMeas=2 X=R20 Y=R21 Z=R22 F=R41\nstopre\nR91=$AA_IW[X]\ng4f0.3\nstopre\nR92=$AA_IW[Y]\ng4f0.3\nstopre\nR93=$AA_IW[Z]\ng4f0.3\nstopre\nR94=$AA_IM[X]\ng4f0.3\nstopre\nR95=$AA_IM[Y]\ng4f0.3\nstopre\nR96=$AA_IM[Z]\ng4f0.3\nstopre\nG90 G01 X=R11 Y=R12 Z=R13 F=R40\nR90=R80\ng4f0.3\nstopre\nR80=R80+1\n";

	m_SMOVE = "\nIF R20<>9999 GOTOF LN1\nR20=$AA_IW[X]\nLN1:\nIF R21<>9999 GOTOF LN2\nR21=$AA_IW[Y]\nLN2:\nIF R22<>9999 GOTOF LN3\nR22=$AA_IW[Z]\nLN3:\nSTOPRE\nG90 G01\nMeas=2 X=R20  Y=R21  Z=R22  F=R40\nSTOPRE\nIF R20<>$AA_IW[X] GOTOF END\nIF R21<>$AA_IW[Y] GOTOF END\nIF R22<>$AA_IW[Z] GOTOF END\nSTOPRE\nR20=9999\nR21=9999\nR22=9999\nEND:\n";

	//m_SMEAS = "\nR11=$AA_IW[X]\nR12=$AA_IW[Y]\nR13=$AA_IW[Z]\nG90 G01\nMeas=2 X=R20 Y=R21 Z=R22 F=R41\nstopre\nR91=$AA_IW[X]\ng4f0.3\nstopre\nR92=$AA_IW[Y]\ng4f0.3\nstopre\nR93=$AA_IW[Z]\ng4f0.3\nstopre\ng4f0.3\nstopre\nG90 G01 X=R11 Y=R12 Z=R13 F=R40\nR90=R80\ng4f0.3\nstopre\nR80=R80+1\n";

	//m_SMOVE = "\nSTOPRE\nG90 G01\nMeas=2 X=R20  Y=R21  Z=R22  F=R40\nSTOPRE\n";

	m_SPROBE = "\nR11=$AA_IW[X]\nR12=$AA_IW[Y]\nR13=$AA_IW[Z]\nG91 G01\nMeas=2 X=R20 Y=R21 Z=R22 F=R41\nstopre\nR91=$AA_IW[X]-R1\ng4f0.3\nstopre\nR92=$AA_IW[Y]-R2\ng4f0.3\nstopre\nR93=$AA_IW[Z]-R3\ng4f0.3\nstopre\nR90=R80\ng4f0.3\nstopre\nG90 G01 X=R11 Y=R12 Z=R13 F=R40\nR80=R80+1\n";

}

QString FTHP::TH_GetHead()	//解释后得到程序头，已修改，去掉了第一行的M66和第二行的TRAORI
{
	QString Head[10],Reg[3],Constant[3];
	QString G;

	QString NCS=this->TH_GetProfileString("NCS","NC_Type");	//判断数控系统类型
	Head[0]=this->TH_GetProfileString(NCS,NCS+"_Head0");
	Head[1]=this->TH_GetProfileString(NCS,NCS+"_Head1");
	Head[2]=this->TH_GetProfileString(NCS,NCS+"_Head2");
	Head[3]=this->TH_GetProfileString(NCS,NCS+"_Head3");
	Head[4]=this->TH_GetProfileString(NCS,NCS+"_Head4");
	Head[5]=this->TH_GetProfileString(NCS,NCS+"_Head5");
	Head[6]=this->TH_GetProfileString(NCS,NCS+"_Head6");
	Head[7]=this->TH_GetProfileString(NCS,NCS+"_Head7");
	Head[8]=this->TH_GetProfileString(NCS,NCS+"_Head8");
	Head[9]=this->TH_GetProfileString(NCS,NCS+"_Head9");

	Reg[0]=this->TH_GetProfileString(NCS,NCS+"_Const0");
	Reg[1]=this->TH_GetProfileString(NCS,NCS+"_Const1");
	Reg[2]=this->TH_GetProfileString(NCS,NCS+"_Const2");
	Constant[0]=this->TH_GetProfileString("PARA","PARA_MoveSPD");	//移动速度
	Constant[1]=this->TH_GetProfileString("PARA","PARA_MeasSPD");	//测量速度
	Constant[2]=this->TH_GetProfileString("PARA","PARA_TouchSPD");	//粗测速度

	//added by chengxi <加上测头开启指令>
	G = gDlgSetting->value("ProbeOnCmd").toString() + "\n";
	for(int i=0;i<10;i++)
	{
		if (Head[i]!="None")
		{
			//初始
			//G+=Head[i];
			//G+="\n";

			if (i > 1)//去掉第一行的M66和第二行的TRAORI
			{
				G += Head[i];
				G += "\n";
			}

		}
	}

	if((NCS!="FDIA3")&&(NCS!="FDIA5"))
	{
		for(int i=0;i<3;i++)
		{
			G+=Reg[i]+"="+Constant[i]+"\n";

		}
	}

	return G;
}

QString FTHP::TH_GetCenter()	//寻找标定中心程序
{
	QString G;
	QString NCS=this->TH_GetProfileString("NCS","NC_Type");	//判断数控系统类型
	if((NCS!="FDIA3")&&(NCS!="FDIA5"))
	{
		G=this->TH_GetProfileString(NCS,NCS+"_Center")+"\n";
	}
	else G="";
	return G;
}

QString FTHP::TH_GetProb(double ii,double jj,double kk,double D)	//解释后得到程序头
{
	QString Prob[10];
	QString DataIn[3];
	QString csX,csY,csZ,csDX,csDY,csDZ,csL,csE,csR;
	QString G;

	QString NCS=this->TH_GetProfileString("NCS","NC_Type");	//判断数控系统类型
	Prob[0]=this->TH_GetProfileString(NCS,NCS+"_Prob0");
	Prob[1]=this->TH_GetProfileString(NCS,NCS+"_Prob1");
	Prob[2]=this->TH_GetProfileString(NCS,NCS+"_Prob2");
	Prob[3]=this->TH_GetProfileString(NCS,NCS+"_Prob3");
	Prob[4]=this->TH_GetProfileString(NCS,NCS+"_Prob4");
	Prob[5]=this->TH_GetProfileString(NCS,NCS+"_Prob5");
	Prob[6]=this->TH_GetProfileString(NCS,NCS+"_Prob6");
	Prob[7]=this->TH_GetProfileString(NCS,NCS+"_Prob7");
	Prob[8]=this->TH_GetProfileString(NCS,NCS+"_Prob8");
	Prob[9]=this->TH_GetProfileString(NCS,NCS+"_Prob9");
	if((NCS!="FDIA3")&&(NCS!="FDIA5"))
	{
		DataIn[0]=this->TH_GetProfileString(NCS,NCS+"_IN0");
		DataIn[1]=this->TH_GetProfileString(NCS,NCS+"_IN1");
		DataIn[2]=this->TH_GetProfileString(NCS,NCS+"_IN2");
	}
	//double Distance=GetPrivateProfileInt("PARA","Distance",0,"D:\\src\\THP_Lite.ini");	//测量起始距离
	//QSettings configIniRead("THP_Lite.ini", QSettings::IniFormat);  
	//QString ipResult = configIniRead.value("PARA/Distance").toString();  
	QString ipResult = gAppSetting->value("PARA/Distance").toString(); 
	double Distance= ipResult.toDouble();
	//QString ipResult2 =configIniRead.value("PARA/OverPass").toString();  
	QString ipResult2 = gAppSetting->value("PARA/OverPass").toString();  
	int OverPass= (int)ipResult2.toDouble();
	G="";

	if((NCS!="FDIA3")&&(NCS!="FDIA5"))
	{
		csX=QString::number(-(Distance+OverPass)*ii,'f',4);
		csY=QString::number(-(Distance+OverPass)*jj,'f',4);
		csZ=QString::number(-(Distance+OverPass)*kk,'f',4);
		G+=DataIn[0]+"="+csX+"\n";
		G+=DataIn[1]+"="+csY+"\n";
		G+=DataIn[2]+"="+csZ+"\n";
		for(int i=0;i<10;i++)
		{
			if (Prob[i]!="None")
			{
				G+=Prob[i];
				G+="\n";
			}
		}
	}
	else
	{
		csX=QString::number(D/2*ii,'f',4);
		csY=QString::number(D/2*jj,'f',4);
		csZ=QString::number(D/2*kk,'f',4);
		csDX=QString::number(ii,'f',4);
		csDY=QString::number(jj,'f',4);
		csDZ=QString::number(kk,'f',4);
		csL=QString::number(Distance,'f',4);
		csR=QString::number((D/2+Distance)*kk,'f',4);
		csE=QString::number(Distance,'f',4);

		G+="G73 L"+csL;
		G+=" E"+csE;
		G+=" X"+csX;
		G+=" Y"+csY;
		G+=" Z"+csZ;
		G+=" R"+csR;
		G+=" DX"+csDX;
		G+=" DY"+csDY;
		G+=" DZ"+csDZ+"\n";
	}
	return G;
}

QString FTHP::TH_GetTail()	//程序尾
{
	QString Tail[10];
	QString G;

	QString NCS=this->TH_GetProfileString("NCS","NC_Type");	//判断数控系统类型
	Tail[0]=this->TH_GetProfileString(NCS,NCS+"_Tail0");
	Tail[1]=this->TH_GetProfileString(NCS,NCS+"_Tail1");
	Tail[2]=this->TH_GetProfileString(NCS,NCS+"_Tail2");
	Tail[3]=this->TH_GetProfileString(NCS,NCS+"_Tail3");
	Tail[4]=this->TH_GetProfileString(NCS,NCS+"_Tail4");
	Tail[5]=this->TH_GetProfileString(NCS,NCS+"_Tail5");
	Tail[6]=this->TH_GetProfileString(NCS,NCS+"_Tail6");
	Tail[7]=this->TH_GetProfileString(NCS,NCS+"_Tail7");
	Tail[8]=this->TH_GetProfileString(NCS,NCS+"_Tail8");
	Tail[9]=this->TH_GetProfileString(NCS,NCS+"_Tail9");

	G="";

	//added by chengxi<程序尾加上测头关闭指令>
	G += gDlgSetting->value("ProbeOffCmd").toString() + "\n";

	for(int i=0;i<10;i++)
	{
		if (Tail[i]!="None")
		{
			//原始
			//G+=Tail[i];
			//G+="\n";

			if (i != 1)//去掉文件尾的TRAORI
			{
				G += Tail[i];
				G += "\n";
			}
		}
	}
	return G;
}

void FTHP::TH_ProbeFileWrite(QString csGCodeOut, QString fname)
{
	QString nameWithPath = gAppPath + "/" + fname;
	QFile	file(nameWithPath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;
	QTextStream out(&file);
	out << csGCodeOut << endl;
	file.close();

	//QFile file(fname);
	//if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	//	return;
	//QTextStream out(&file);
	//out <<csGCodeOut << endl;    
	//file.close();

}