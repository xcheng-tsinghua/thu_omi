#include "../External/fkernel/Include/FKernelHeader.h"
#include "FUtilities.h"
#include "FMainWindow.h"
#include "MainView.h"
#include "FSubwindow.h"
#include "MainDoc.h"
#include "QMessageBox"
#include "iostream"
#include "FInsFeaTreeWnd.h"
#include "MeasPt.h"
#include <typeinfo>

FMainDoc::FMainDoc(FMainWindow* pWnd)
	:/*m_pPntVisual(nullptr), */m_bIsMachineErrorCmps(false),m_bIsAniRunning(false)
{
	m_pMainWnd = pWnd;
	m_pProbe = NULL;
	m_pCalSphere = NULL;
	m_gRange = 100;
	AdjustProbeFlag=0;
	m_serverIP = QHostAddress(QHostAddress::LocalHost);

	m_pntShowDef_R = 0;
	m_pntShowDef_G = 0;
	m_pntShowDef_B = 1;

	m_pntShowHig_R = 1;
	m_pntShowHig_G = 0;
	m_pntShowHig_B = 0;

}

FMainDoc::~FMainDoc()
{

}

bool FMainDoc::OnNewDocument()
{
	FT::FDoc::NewDoc();
	clear();
	UpdateAllViews();
	if (m_pMainWnd)
		m_pMainWnd->UpdateAllViews();
	return true;
}

bool FMainDoc::ImportProbe()
{
	if (m_pProbe)	return false;
	QString tStr = gAppPath+QString("/data/probe.stl");
	if(!QFile::exists(tStr)) return false;
	m_pProbe = FT::gImportSTLFile(tStr.toLocal8Bit().data());
	FForm tRot,tTran; 
	tTran.Translation(0,0,50);
	tRot.Rotation(FT::PI/2, F3dVector(1,0,0), F3dPoint(0,0,0));
	m_pProbe->DoTransform(tTran*tRot);	//rotation first
	m_pProbe->Update();
	return true;
}

void FMainDoc::CreateCalSphere(double radius)
{
	if(!m_pCalSphere)
	{
		m_pCalSphere = new FSphere(radius);
		m_pCalSphere->Update();
	}
	AddDispEnt(m_pCalSphere);
}
void FMainDoc::UpdateAllViews()
{
	FT::FDoc::Update();
	if(m_pMainWnd)
		m_pMainWnd->UpdateAllViews();
}

void FMainDoc::SetIpPntList(FIpPntList pointList, double radius/*=3*/)
{
	for (int i=0; i<pointList.size(); i++)
	{
		pointList[i].flag = 1;
		m_ipPntList.push_back(pointList[i]);
	}
	emit(resetPointDataSignal(true,0,false));	

	UpdateAllViews();
}

vector<MeasPt*>& FMainDoc::getPntsShowList()
{
	return m_pntsShowList;
}

void FMainDoc::getPntsShowColorDefault(double& _r, double& _g, double& _b)
{
	_r = m_pntShowDef_R;
	_g = m_pntShowDef_G;
	_b = m_pntShowDef_B;

}

void FMainDoc::setPntsShowColorDefault()
{
	for (auto ita = m_pntsShowList.begin(); ita != m_pntsShowList.end(); ++ita)
	{
		(*ita)->setColor(m_pntShowDef_R, m_pntShowDef_G, m_pntShowDef_B);
	}

	gWindow->UpdateModelView();

}

void FMainDoc::clearMesPointList()
{
	m_mesPntList.clear();
}

void FMainDoc::addToMesPointList(double x_mes, double y_mes, double z_mes)
{
	FIpPoint	p;
	p.x	=	x_mes;
	p.y	=	y_mes;
	p.z	=	z_mes;
	m_mesPntList.push_back(p);
	emit(updateMesPointDataSignal(1));
}

void FMainDoc::addToMesPointList( int num, double x_mes, double y_mes, double z_mes )
{
	while(m_mesPntList.size() < num)
	{
		FIpPoint	p0;
		p0.x	=	-9999;
		p0.y	=	-9999;
		p0.z	=	-9999;
		p0.num	=	m_mesPntList.size()+1;
		m_mesPntList.push_back(p0);
	}
	m_mesPntList[num-1].x	=	x_mes;
	m_mesPntList[num-1].y	=	y_mes;
	m_mesPntList[num-1].z	=	z_mes;
	m_mesPntList[num-1].num	=	num;
	
	emit(updateMesPointDataSignal(num));
}

void FMainDoc::addToMesErrList(int num, double error)
{
	while(m_errList.size() < num)
	{
		double m_error;
		m_error = error;
		m_errList.push_back(error);
	}

	emit(onUpdateErrorSignal(num));

}

void FMainDoc::setHighlightPnt( int idx )
{
	// ������ɫ
	setPntsShowColorDefault();

	if (idx >= 1 && idx <= m_pntsShowList.size())
	{
		m_pntsShowList[idx - 1]->setColor(m_pntShowHig_R, m_pntShowHig_G, m_pntShowHig_B);
	}

	gWindow->UpdateAllViews();

}

void FMainDoc::clear()
{
	m_ipPntList.clear();
	m_mesPntList.clear();
	m_ipPathList.clear();
	m_pntsShowList.clear();

	ClearDispEnts();

	m_bIsMachineErrorCmps	=	false;
	m_bIsAniRunning =	false;
	m_pProbe = NULL;
	gWindow->PointerInit();

	static_cast<FInsFeaTreeWnd*>(gWindow->GetSubWnd()[6])->m_pPntItemList.clear();

}

void FMainDoc::setHighlightList( QVector<int> list )
{
	// ������ɫ
	setPntsShowColorDefault();

	unordered_set<int> highLightIdx;

	for (auto ita = list.begin(); ita != list.end(); ++ita)
	{
		highLightIdx.insert(*ita);
	}

	int nPnts = m_pntsShowList.size();

	for (int i = 1; i <= nPnts; i++)
	{
		if (highLightIdx.count(i))
		{
			m_pntsShowList[i - 1]->setColor(m_pntShowHig_R, m_pntShowHig_G, m_pntShowHig_B);
		}
	}

	gWindow->UpdateModelView();

}

void FMainDoc::AddDispEnt(FEntity* pEnt)
{
	FDoc::AddDispEnt(pEnt);

	if (typeid(*pEnt) == typeid(MeasPt))
	{
		MeasPt* pMeasPt = dynamic_cast<MeasPt*>(pEnt);

		m_pntsShowList.push_back(pMeasPt);

		//cout << "������" << pMeasPt->getm_x() << "," << pMeasPt->getm_y() << "," << pMeasPt->getm_z() << endl;

	}

}

void FMainDoc::clearPntsAll()
{
	// ������۲���
	m_ipPntList.clear();

	// ���ʵ�ʲ���
	m_mesPntList.clear();

	// �����ʾ����
	//gWindow->GetMainDoc()->getPntsShowList()
	int nSize = m_pntsShowList.size();
	for (int i = 0; i < nSize; i++)
	{
		RemoveDispEnts(m_pntsShowList[i], true);
	}
	m_pntsShowList.clear();

	// ��ղ����Ϣ��
	emit(resetPointDataSignal(true, 0, false));
	UpdateAllViews();

}

void FMainDoc::compute2( int num )
{
	double pai=3.141592653589793238;
	double x,y,z,x0,y0,z0,i,j,k,result,err,uperror,downerror,deg,degz;
	double x1,x2,y1,y2,deg1,deg2,r1,r2;
	int symbol;
	//������ֵ������ֵ���ݸ������ر�����m_mesPntListΪ����ֵ��m_ipPntListΪ����ֵ
	x = m_mesPntList[num].x;
	y = m_mesPntList[num].y;
	z = m_mesPntList[num].z;
	x0 = m_ipPntList[num].x;
	y0 = m_ipPntList[num].y;
	z0 = m_ipPntList[num].z;
	i = m_ipPntList[num].i;
	j = m_ipPntList[num].j;
	k = m_ipPntList[num].k;

	//////////////baosheng/////////////////////////

	degz=asin(k);	//��z��н�
	if ((1-k)>=0.00001)	//��xyͶӰ
	{
		double ij=pow(pow(i,2)+pow(j,2),0.5);	//����xyƽ��ͶӰ����
		deg=asin(j/ij);
		if(i!=0){
			if(i>0){		//�Ұ�ƽ��,j>=0��һ���ޣ�degֵ����
				if(j<0){	//�Ұ�ƽ��,j<0�������ޣ�degֵ+2pai
					deg+=2*pai;
				}
			}else{			//���ƽ��
				deg=pai-deg;
			}
		}else{
			if(j>=0){			//=0�����ܳ��֣�����
				deg=pai/2;
			}else{
				deg=pai*3/2;
			}
		}

	} 
	else	//��z��xy��ͶӰ
	{
		deg=0;
	}
	///////////////////baosheng/////////////////////////////


	float dr=0;
	float ddeg=0;
	float ddegz=0;
	float mij=0;
	float mk=0;
	int k1,k2,ij1,ij2;
	QVector<FIpPoint*> data;
	FIpPoint* point;
	//CTypedPtrArray<CObArray,CPoints*> data;
	//CPoints * point;

	//��ȡ�궨�ļ�,�����ݴ洢��data������
	QString	dir = gAppPath + QString("/settings/nc/AdjustProbe2.txt");
	if (fopen(dir.toLocal8Bit().data(),"r")!=NULL) {
		FILE * file=fopen(dir.toLocal8Bit().data(),"r");
		fscanf(file, "%f %f", &mij, &mk);
		while(fscanf(file, "%f %f %f", &dr, &ddeg, &ddegz) != EOF)
		{
			point = new FIpPoint;
			point->r=dr;
			point->deg=ddeg;
			point->degz=ddegz;
			data.push_back(point);
		}
		fclose(file);


		//�����㷨
		/////////////baosheng///////////////
		double degreek=(pai/2)/(mk-1);	//z�򲽽�
		double degreeij=2*pai/mij;	//xyƽ�沽��
		double degreetmp=0;			//��ʱ����������Ѱ�һ�������
		int ktmp=0;					//��ʱ����������Ѱ�ҵ�������
		if (deg-2*pai<0.00001&&deg-2*pai>-0.00001)
		{
			ktmp=mij;
		} 
		else
		{
			while(degreetmp<=deg)	//xyƽ��Ѱ������
			{
				degreetmp+=degreeij;
				ktmp++;
			}
		}
		ij1=ktmp-1;		//��������
		ij2=ktmp;		//��������

		degreetmp=pai*0.5;			//��ʱ����������Ѱ�һ�������
		ktmp=0;					//��ʱ����������Ѱ�ҵ�������

		if (degz*180/pai<0.0001)
		{
			ktmp=mk-1;
		} 
		else if(degz>degreetmp){
			ktmp=1;
		}
		else{
			while(degreetmp>=degz)	//zƽ��Ѱ������
			{
				degreetmp-=degreek;
				ktmp++;
			}
		}

		k1=ktmp-1;		//��������
		k2=ktmp;		//��������
		/////////////baosheng///////////////


		//���ҳ����ĵ㸳ֵ
		double rk1ij1;
		double degk1ij1;
		double degzk1ij1;

		double rk1ij2;	
		double degk1ij2;
		double degzk1ij2;

		double rk2ij1;
		double degk2ij1;
		double degzk2ij1;

		double rk2ij2;	
		double degk2ij2;
		double degzk2ij2;
		///////////////////baosheng/////////////////////////
		double numtmp=0;		//��ʱ��������¼���ݱ�����
		//k1ij1��
		numtmp=k1*mij+ij1;		//+1����һ�д�m��n��z��Χ����Ϣ
		rk1ij1=data[numtmp]->r;
		degk1ij1=data[numtmp]->deg;
		degzk1ij1=data[numtmp]->degz;
		//k1ij2��
		if (ij2>=mij)	//�����ޣ�������������ܳ���
		{
			numtmp=k1*mij;		//ȡij2=0��
			rk1ij2=data[numtmp]->r;	
			degk1ij2=pai*2;
			degzk1ij2=data[numtmp]->degz;
		}
		else
		{
			numtmp=k1*mij+ij2;		//+1����һ�д�m��n��z��Χ����Ϣ
			rk1ij2=data[numtmp]->r;	
			degk1ij2=data[numtmp]->deg;
			degzk1ij2=data[numtmp]->degz;
		}

		//k2ij1��
		numtmp=k2*mij+ij1;		//+1����һ�д�m��n��z��Χ����Ϣ
		rk2ij1=data[numtmp]->r;
		degk2ij1=data[numtmp]->deg;
		degzk2ij1=data[numtmp]->degz;
		//k2ij2��
		if (ij2>=mij)	//�����ޣ�������������ܳ���
		{
			numtmp=k2*mij;		//ȡij2=0��
			rk2ij2=data[numtmp]->r;	
			degk2ij2=pai*2;
			degzk2ij2=data[numtmp]->degz;
		}
		else
		{
			numtmp=k2*mij+ij2;		//+1����һ�д�m��n��z��Χ����Ϣ
			rk2ij2=data[numtmp]->r;	
			degk2ij2=data[numtmp]->deg;
			degzk2ij2=data[numtmp]->degz;
		}



		//��һ�����Բ�ֵ�ҳ��м�뾶
		double rk1,rk2,r;
		if ((degk1ij1-degk1ij2)<0.0001&&(degk1ij1-degk1ij2)>-0.0001)
		{
			rk1=rk1ij1;
		} 
		else
		{
			rk1=(rk1ij2*(deg-degk1ij1)+rk1ij1*(degk1ij2-deg))/(degk1ij2-degk1ij1);
		}

		if ((degk2ij1-degk2ij2)<0.0001&&(degk2ij1-degk2ij2)>-0.0001)
		{
			rk2=rk2ij1;
		} 
		else
		{
			rk2=(rk2ij2*(deg-degk2ij1)+rk2ij1*(degk2ij2-deg))/(degk2ij2-degk2ij1);
		}
		//��һ�����Բ�ֵ�ҳ���������뾶
		if ((degzk1ij1-degzk2ij1)<0.0001&&(degzk1ij1-degzk2ij1)>-0.0001)
		{
			r=rk1;
		} 
		else
		{
			r=(rk2*(degz-degzk1ij1)+rk1*(degzk2ij1-degz))/(degzk2ij1-degzk1ij1);
		}

		//����
		if (num==22)
		{
			int ald=1;
		}
		double adf=asin(0.999190);
		double adfj=sin(1.53054);
		double adfg=cos(1.53054);
		double as1=r*sin(degz);		
		double as2=r*cos(degz)*cos(deg);
		double as3=r*cos(degz)*sin(deg);
		z-=r*sin(degz);		
		x-=r*cos(degz)*cos(deg);
		y-=r*cos(degz)*sin(deg);

		//�������
		uperror=m_mesPntList[num].uperror;
		downerror=m_mesPntList[num].downerror;
		result=(x0-x)*i+(y0-y)*j+(z0-z)*k;
		err=pow(pow(x-x0,2)+pow(y-y0,2)+pow(z-z0,2),0.5);
		//�ж��������
		if (result>=0)
		{
			symbol=1;
			m_mesPntList[num].err=err;
		} 
		else
		{
			symbol=-1;
			m_mesPntList[num].err=-1*err;
		}

		if ((err>uperror)||(err<-downerror))
		{
			if (err>uperror)
			{
				m_mesPntList[num].cot=err-uperror;
			} 
			else
			{
				m_mesPntList[num].cot=err-downerror;
			}

		} 
		else
		{
			m_mesPntList[num].cot=0;
		}
		m_mesPntList[num].x=x;
		m_mesPntList[num].y=y;
		m_mesPntList[num].z=z;

	}else{
		/*QMessageBox::warning(this,QString::fromLocal8Bit("����"),
			QString::fromLocal8Bit("����δ�����������ϰ���궨�ļ��Ƿ���ڣ�"),"OK");*/
		/*QMessageBox::critical(0, QString::fromLocal8Bit("����"),
			QString::fromLocal8Bit("����δ�����������ϰ���궨�ļ��Ƿ���ڣ�"),
			QMessageBox::Cancel);*/
		std::cout << "����δ�����������ϰ���궨�ļ��Ƿ���ڣ�" << std::endl;
		
	}

}

void FMainDoc::setTransVec(int i, vector<double> transVec)
{
	if (i == 0)
	{
		m_move.clear();
		for (int j = 0; j < transVec.size(); j++)
		{
			m_move.push_back(transVec.at(j));
		}

		if (m_move.size() != 16)
			return;
	}

	else if (i == 1)
	{
		m_rotx.clear();
		for (int j = 0; j < transVec.size(); j++)
		{
			m_rotx.push_back(transVec.at(j));
		}

		if (m_rotx.size() != 16)
			return;
	}

	else if (i == 2)
	{
		m_roty.clear();
		for (int j = 0; j < transVec.size(); j++)
		{
			m_roty.push_back(transVec.at(j));
		}

		if (m_roty.size() != 16)
			return;
	}

	else if (i == 3)
	{
		m_rotz.clear();
		for (int j = 0; j < transVec.size(); j++)
		{
			m_rotz.push_back(transVec.at(j));
		}

		if (m_rotz.size() != 16)
			return;
	}

	else if (i == 4)
	{
		m_total.clear();
		for (int j = 0; j < transVec.size(); j++)
		{
			m_total.push_back(transVec.at(j));
		}

		if (m_total.size() != 16)
			return;
	}

	else
		return;
}

vector<double> FMainDoc::getTransVec(int i)
{
	if (i == 0)
	{
		return m_move;
	}

	else if (i == 1)
	{
		return m_rotx;
	}

	else if (i == 2)
	{
		return m_roty;
	}

	else if (i == 3)
	{
		return m_rotz;
	}

	else if (i == 4)
	{
		return m_total;
	}

	else
		std::cout << "WRONG INPUT PARAMETER!" << endl;
}
