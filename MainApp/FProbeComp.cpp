#include "FProbeComp.h"

FProbeComp :: FProbeComp(FMainDoc * m_Doc)
{
	this->m_ppDoc = m_Doc;
	IsMind = true;
}

FProbeComp :: ~FProbeComp()
{

}

void FProbeComp :: errCompFixedAngle(int num)
{
	double x,y,z,mx,my,mz,x0,y0,z0,i,j,k,result,err,uperror,downerror,deg,degz;  
	double x1,x2,y1,y2,deg1,deg2,r1,r2;
	int symbol;
	//������ֵ������ֵ���ݸ������ر�����RealPointsListΪ����ֵ��PointsListΪ����ֵ
	x=m_ppDoc->GetMesPnt(num).x;
	y=m_ppDoc->GetMesPnt(num).y;
	z=m_ppDoc->GetMesPnt(num).z;
	mx=m_ppDoc->GetMesPnt(num).mx;   //������ķ�ʸ/�ɰ汾���õ����°汾����
	my=m_ppDoc->GetMesPnt(num).my;
	mz=m_ppDoc->GetMesPnt(num).mz;

	x0=m_ppDoc->GetIpPnt(num).x;    
	y0=m_ppDoc->GetIpPnt(num).y;
	z0=m_ppDoc->GetIpPnt(num).z;
	i=m_ppDoc->GetIpPnt(num).i;
	j=m_ppDoc->GetIpPnt(num).j;
	k=m_ppDoc->GetIpPnt(num).k;
	
	//////////////baosheng/////////////////////////
	
	degz=asin(k);	//��z��н�
	if ((1-k)>=0.00001)	//��xyͶӰ
	{
		double ij=pow(pow(i,2)+pow(j,2),0.5);	//����xyƽ��ͶӰ����
		deg=asin(j/ij);
		if(i!=0){
			if(i>0){		//�Ұ�ƽ��,j>=0��һ���ޣ�degֵ����
				if(j<0){	//�Ұ�ƽ��,j<0�������ޣ�degֵ+2pai
					deg+=2*M_PI;
				}
			}else{			//���ƽ��
				deg=M_PI-deg;
			}
		}else{
			if(j>=0){			//=0�����ܳ��֣�����
				deg=M_PI/2;
			}else{
				deg=M_PI*3/2;
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
	float startdegree=0;
	float probedegree = 0;
	int k1,k2,ij1,ij2;
	vector<FIpPoint> data;
	//FIpPoint * point;
	QString nameWithPath = gAppPath+QString("/settings/nc/ErrAngleSphere.txt");
	
	//��ȡ�궨�ļ�,�����ݴ洢��data������
    if (fopen(nameWithPath.toLocal8Bit().data(),"r")!=NULL) {
		FILE * file=fopen(nameWithPath.toLocal8Bit().data(),"r");
		fscanf(file, "%f %f %f  %f", &mk, &mij, &probedegree, &startdegree);
		while(fscanf(file, "%f %f %f", &dr, &ddeg, &ddegz) != EOF)  // �����õ�������ģ���������Ƕ�
		{
			FIpPoint point = FIpPoint();
			point.r=dr;
			point.deg=ddeg;
			point.degz=ddegz;
			data.push_back(point);
		}
		fclose(file);
		
		
		//�����㷨
		/////////////baosheng///////////////
		double degreek=(0.5*M_PI)/(mk);	//z�򲽽�   zsq
		double degreeij=2*M_PI/mij;	//xyƽ�沽��
		double degreetmp=0;			//��ʱ����������Ѱ�һ�������
		int ktmp=0;					//��ʱ����������Ѱ�ҵ�������
		if (deg-2*M_PI<0.00001&&deg-2*M_PI>-0.00001)
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
		
		degreetmp=probedegree*M_PI/180;			//��ʱ����������Ѱ�һ�������
		ktmp=0;					//��ʱ����������Ѱ�ҵ�������
		if (degz*180/M_PI+ probedegree <0.0001)
		{
			ktmp=mk-1;
		} 
		else if(degz>degreetmp){
			while (degz >= degreetmp)	//zƽ��Ѱ������
			{
				degreetmp += degreek;
				ktmp++;
			}
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
		
		double rk1, rk2, r;//˫���Բ�ֵ�㷨�����ñ���

		if (k1 == 0)
		{
			double rk1ij1;      //r_k1_ij1
			double degk1ij1;     //deg_k1_ij1
			double degzk1ij1;     //degz_k1_ij1

			double rk2ij1;
			double degk2ij1;
			double degzk2ij1;

			double rk2ij2;
			double degk2ij2;
			double degzk2ij2;

			double numtmp = 0;
			//k1ij1��
			rk1ij1 = data[numtmp].r;
			degk1ij1 = data[numtmp].deg;
			degzk1ij1 = data[numtmp].degz;  //zsq


			//k2ij1��
			//numtmp=k2*mij+ij1;		//+1����һ�д�m��n��z��Χ����Ϣ
			numtmp = (k2 - 1) * mij + ij1+1;    //zsq
			rk2ij1 = data[numtmp].r;
			degk2ij1 = data[numtmp].deg;
			degzk2ij1 = data[numtmp].degz;
			//k2ij2��
			if (ij2 >= mij)	//�����ޣ�������������ܳ���
			{
				//numtmp=k2*mij;		//ȡij2=0��
				numtmp = (k2 - 1) * mij + 1;       //zsq
				rk2ij2 = data[numtmp].r;
				degk2ij2 = M_PI * 2;
				degzk2ij2 = data[numtmp].degz;
			}
			else
			{
				//numtmp=k2*mij+ij2;		//+1����һ�д�m��n��z��Χ����Ϣ
				numtmp = (k2 - 1) * mij + ij2+1;    //zsq
				rk2ij2 = data[numtmp].r;
				degk2ij2 = data[numtmp].deg;
				degzk2ij2 = data[numtmp].degz;
			}
			double wi1, wi2, hi3, S1, S2, S3;
			wi2 = (degk2ij2 - deg) / degreeij;
			wi1 = 1 - wi2;
			hi3 = (degz + degreek - M_PI / 2) / degreek;
			if (hi3 == 1)
			{
				S1 = 0;
				S2 = 0;
				S3 = 1;
			}
			else if (wi2 == 0)
			{
				S1 = 1;
				S2 = 0;
				S3 = 0;
			}
			else if (wi1 == 0)
			{
				S1 = 0;
				S2 = 1;
				S3 = 0;
			}
			else
			{
				S3 = hi3;
				S2 = (1 - S3) / (11 + wi1 / wi2);
				S1 = 1 - S2 - S3;

			}
			r = S3 * rk1ij1 + S2 * rk2ij1 + S1 * rk2ij2;

		}
		else
		{


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
			double numtmp = 0;		//��ʱ��������¼���ݱ�����
			//k1ij1��
			numtmp = (k1 - 1) * mij + ij1+1;		//+1����һ�д�m��n��z��Χ����Ϣ
			rk1ij1 = data[numtmp].r;
			degk1ij1 = data[numtmp].deg;
			degzk1ij1 = data[numtmp].degz;
			//k1ij2��
			if (ij2 >= mij)	//�����ޣ�������������ܳ���
			{
				numtmp =( k1-1) * mij+1;		//ȡij2=0��
				rk1ij2 = data[numtmp].r;
				degk1ij2 = M_PI * 2;
				degzk1ij2 = data[numtmp].degz;
			}
			else
			{
				numtmp = (k1 - 1) * mij + ij2+1;		//+1����һ�д�m��n��z��Χ����Ϣ
				rk1ij2 = data[numtmp].r;
				degk1ij2 = data[numtmp].deg;
				degzk1ij2 = data[numtmp].degz;
			}

			//k2ij1��
			numtmp = (k2 - 1) * mij + ij1+1;		//+1����һ�д�m��n��z��Χ����Ϣ
			rk2ij1 = data[numtmp].r;
			degk2ij1 = data[numtmp].deg;
			degzk2ij1 = data[numtmp].degz;
			//k2ij2��
			if (ij2 >= mij)	//�����ޣ�������������ܳ���
			{
				numtmp = (k2 - 1) * mij+1;		//ȡij2=0��
				rk2ij2 = data[numtmp].r;
				degk2ij2 = M_PI * 2;
				degzk2ij2 = data[numtmp].degz;
			}
			else
			{
				numtmp = (k2 - 1) * mij + ij2+1;		//+1����һ�д�m��n��z��Χ����Ϣ
				rk2ij2 = data[numtmp].r;
				degk2ij2 = data[numtmp].deg;
				degzk2ij2 = data[numtmp].degz;
			}


			//��һ�����Բ�ֵ�ҳ��м�뾶
			double rk1, rk2, r;
			if ((degk1ij1 - degk1ij2) < 0.00001 && (degk1ij1 - degk1ij2) > -0.00001)
			{
				rk1 = rk1ij1;
			}
			else
			{
				rk1 = (rk1ij2 * (deg - degk1ij1) + rk1ij1 * (degk1ij2 - deg)) / (degk1ij2 - degk1ij1);
			}

			if ((degk2ij1 - degk2ij2) < 0.00001 && (degk2ij1 - degk2ij2) > -0.00001)
			{
				rk2 = rk2ij1;
			}
			else
			{
				rk2 = (rk2ij2 * (deg - degk2ij1) + rk2ij1 * (degk2ij2 - deg)) / (degk2ij2 - degk2ij1);
			}
			//��һ�����Բ�ֵ�ҳ���������뾶
			if ((degzk1ij1 - degzk2ij1) < 0.00001 && (degzk1ij1 - degzk2ij1) > -0.00001)
			{
				r = rk1;
			}
			else
			{
				r = (rk2 * (degz - degzk1ij1) + rk1 * (degzk2ij1 - degz)) / (degzk2ij1 - degzk1ij1);
			}
		}
		//����
		z += r * sin(degz);
		x += r * cos(degz) * cos(deg);
		y += r * cos(degz) * sin(deg);

		if (abs(x) <= 0.0001) { x = 0; }
		if (abs(y) <= 0.0001) { y = 0; }
		if (abs(z) <= 0.0001) { z = 0; }

		
		//double dm=computemachinex(mx);
		
		//�������
		uperror=m_ppDoc->GetMesPnt(num).uperror;
		downerror=m_ppDoc->GetMesPnt(num).downerror;
		result=(x0-x)*i+(y0-y)*j+(z0-z)*k;
		err=pow(pow(x-x0,2)+pow(y-y0,2)+pow(z-z0,2),0.5);
		if (abs(err) <= 0.0001) { err = 0; }
		//�ж��������
		if (result>=0)
		{
			symbol=1;
			m_ppDoc->GetMesPnt(num).err=err;
		} 
		else
		{
			symbol=-1;
			err=-1*err;
			m_ppDoc->GetMesPnt(num).err=err;
		}
		
		if ((err>uperror)||(err<-1*downerror))
		{
			if (err>uperror)
			{
				m_ppDoc->GetMesPnt(num).cot=err-uperror;
			} 
			else
			{
				m_ppDoc->GetMesPnt(num).cot=err-downerror;
			}
			
		} 
		else
		{
			m_ppDoc->GetMesPnt(num).cot=0;
		}
		m_ppDoc->GetMesPnt(num).x=x;
		m_ppDoc->GetMesPnt(num).y=y;
		m_ppDoc->GetMesPnt(num).z=z;
		m_ppDoc->addToMesPointList(num+1,x,y,z);
		m_ppDoc->addToMesErrList(num+1,err);
		
		if(err > m_ppDoc->GetMesPnt(num).uperror || err < m_ppDoc->GetMesPnt(num).downerror)
		{
			m_ppDoc->setHighlightPnt(num);
		}

	}else{
		MessageBoxA(GetForegroundWindow(),"����δ����������̶��ڽǱ궨�ļ��Ƿ���ڣ�","ERROR",MB_OK);
    }
}

void FProbeComp :: errCompUpperSphere(int num)
{
	double x,y,z,x0,y0,z0,i,j,k,result,err,uperror,downerror,deg,degz;//deg,degz������
	double x1,x2,y1,y2,deg1,deg2,r1,r2;
	int symbol;
	//������ֵ������ֵ���ݸ������ر�����RealPointsListΪ����ֵ��PointsListΪ����ֵ
	//cout << "num-1=" << num << endl;

	x=m_ppDoc->GetMesPnt(num).x;
	y=m_ppDoc->GetMesPnt(num).y;
	z=m_ppDoc->GetMesPnt(num).z;

	/*cout << "xyz=" << x <<"   "<< y << "   " << z << "   " << endl;*/

	x0=m_ppDoc->GetIpPnt(num).x;
	y0=m_ppDoc->GetIpPnt(num).y;
	z0=m_ppDoc->GetIpPnt(num).z;
	i=m_ppDoc->GetIpPnt(num).i;
	j=m_ppDoc->GetIpPnt(num).j;
	k=m_ppDoc->GetIpPnt(num).k;
	
	//////////////baosheng/////////////////////////
	
	degz=asin(k);	//��xtƽ��н�
	if ((1-k)>=0.00001)	//��xyͶӰ
	{
		double ij=pow(pow(i,2)+pow(j,2),0.5);	//����xyƽ��ͶӰ����
		deg=asin(j/ij);
		if(i!=0){
			if(i>0){		//�Ұ�ƽ��,j>=0��һ���ޣ�degֵ����
				if(j<0){	//�Ұ�ƽ��,j<0�������ޣ�degֵ+2pai
					deg+=2*M_PI;
				}
			}else{			//���ƽ��
				deg=M_PI-deg;
			}
		}else{
			if(j>=0){			//=0�����ܳ��֣�����
				deg=M_PI/2;
			}else{
				deg=M_PI*3/2;
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
	float ddegz=0;//�궨�ļ���
	float mij=0;
	float mk=0;
	int k1,k2,ij1,ij2;
	vector<FIpPoint> data;
	//FIpPoint * point;
	QString nameWithPath = gAppPath+QString("/settings/nc/ErrUpperSphere.txt");
	
	//��ȡ�궨�ļ�,�����ݴ洢��data������

    if (FILE* file = fopen(nameWithPath.toLocal8Bit().data(),"r")) {
		//FILE * file=fopen(nameWithPath.toLocal8Bit().data(),"r");
		fscanf(file, "%f %f", &mij, &mk);                         //mijΪÿȦ������mkΪȦ����������
		while(fscanf(file, "%f %f %f", &dr, &ddeg, &ddegz) != EOF)
		{
			FIpPoint point = FIpPoint();
			point.r=dr;
			point.deg=ddeg;
			point.degz=ddegz;
			data.push_back(point);
		}

		fclose(file);
		
		//�����㷨
		/////////////baosheng///////////////
		double degreek=(M_PI/2)/mk;	//z�򲽽�    zsq
		double degreeij=2*M_PI/mij;	//xyƽ�沽��   
		double degreetmp=0;			//��ʱ����������Ѱ�һ�������
		int ktmp=0;					//��ʱ����������Ѱ�ҵ�������
		if (deg-2*M_PI<0.0001&&deg-2*M_PI>-0.0001)
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
		ij1=ktmp-1;		//��������            //������ֵȷ������ÿȦ�������㣬����ȷ���ڵڼ�Ȧ
		ij2=ktmp;		//��������
		
		degreetmp=M_PI*0.5;			//��ʱ����������Ѱ�һ�������
		ktmp=0;					//��ʱ����������Ѱ�ҵ�������
		
		if (degz*180/M_PI<0.0001)   //degz�Ƿ�С��0.0001�ȣ������ڳ��ƽ����
		{
			ktmp=mk;
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
		
		k1=ktmp-1;		//��������           //������ֵȷ���ڵڼ�Ȧ
		k2=ktmp;		//��������
		/////////////baosheng///////////////
		double rk1, rk2, r;//˫���Բ�ֵ�㷨�����ñ���

		if (k1==0)
		{
			double rk1ij1;      //r_k1_ij1
			double degk1ij1;     //deg_k1_ij1
			double degzk1ij1;     //degz_k1_ij1

			double rk2ij1;
			double degk2ij1;
			double degzk2ij1;

			double rk2ij2;
			double degk2ij2;
			double degzk2ij2;

			double numtmp = 0;
			//k1ij1��
			rk1ij1 = data[numtmp].r;
			degk1ij1 = data[numtmp].deg;
			degzk1ij1 = data[numtmp].degz;  //zsq


			//k2ij1��
			//numtmp=k2*mij+ij1;		//+1����һ�д�m��n��z��Χ����Ϣ
			numtmp = (k2 - 1) * mij + ij1+1;    //zsq
			rk2ij1 = data[numtmp].r;
			degk2ij1 = data[numtmp].deg;
			degzk2ij1 = data[numtmp].degz;
			//k2ij2��
			if (ij2 >= mij)	//�����ޣ�������������ܳ���
			{
				//numtmp=k2*mij;		//ȡij2=0��
				numtmp = (k2 - 1) * mij + 1;       //zsq
				rk2ij2 = data[numtmp].r;
				degk2ij2 = M_PI * 2;
				degzk2ij2 = data[numtmp].degz;
			}
			else
			{
				//numtmp=k2*mij+ij2;		//+1����һ�д�m��n��z��Χ����Ϣ
				numtmp = (k2 - 1) * mij + ij2+1;    //zsq
				rk2ij2 = data[numtmp].r;
				degk2ij2 = data[numtmp].deg;
				degzk2ij2 = data[numtmp].degz;
			}
			
			double wi1, wi2, hi3, S1, S2, S3;
			wi2= (degk2ij2 - deg) / degreeij;
			wi1 = 1 - wi2;
			hi3 = (degz + degreek - M_PI / 2) / degreek;
			if (hi3 == 1)
			{
				S1 = 0; 
				S2 = 0;
				S3 = 1;
			}
			else if (wi2 == 0) 
			{
				S1 = 1;
				S2 = 0;
				S3 = 0;
			}
			else if (wi1==0)
			{
				S1 = 0;
				S2 = 1;
				S3 = 0;
			}
			else
			{
				S3 = hi3;
				S2 = (1 - S3) / (11 + wi1 / wi2);
				S1 = 1 - S2 - S3;

			}
			r = S3 * rk1ij1 + S2 * rk2ij1 + S1* rk2ij2; 

		}
		else
		{
			//���ҳ����ĵ㸳ֵ
			double rk1ij1;      //r_k1_ij1
			double degk1ij1;     //deg_k1_ij1
			double degzk1ij1;     //degz_k1_ij1

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
			//double numtmp=0;		//��ʱ��������¼���ݱ�����
			////k1ij1��
			//numtmp=k1*mij+ij1;		//+1����һ�д�m��n��z��Χ����Ϣ
			//rk1ij1=data[numtmp].r;
			//degk1ij1=data[numtmp].deg;
			//degzk1ij1=data[numtmp].degz;

			double numtmp = 0;
			//k1ij1��
			numtmp = (k1 - 1) * mij + ij1+1;
			rk1ij1 = data[numtmp].r;
			degk1ij1 = data[numtmp].deg;
			degzk1ij1 = data[numtmp].degz;  //zsq


			//k1ij2��
			if (ij2 >= mij)	//�����ޣ�������������ܳ���
			{
				//numtmp=k1*mij;		//ȡij2=0��
				numtmp = (k1 - 1) * mij + 1;       //zsq

				rk1ij2 = data[numtmp].r;
				degk1ij2 = M_PI * 2;
				degzk1ij2 = data[numtmp].degz;
			}
			else
			{
				//numtmp=k1*mij+ij2;		//+1����һ�д�m��n��z��Χ����Ϣ
				numtmp = (k1 - 1) * mij + ij2+1;    //zsq
				rk1ij2 = data[numtmp].r;
				degk1ij2 = data[numtmp].deg;
				degzk1ij2 = data[numtmp].degz;
			}

			//k2ij1��
			//numtmp=k2*mij+ij1;		//+1����һ�д�m��n��z��Χ����Ϣ
			numtmp = (k2 - 1) * mij + ij1+1;    //zsq
			rk2ij1 = data[numtmp].r;
			degk2ij1 = data[numtmp].deg;
			degzk2ij1 = data[numtmp].degz;
			//k2ij2��
			if (ij2 >= mij)	//�����ޣ�������������ܳ���
			{
				//numtmp=k2*mij;		//ȡij2=0��
				numtmp = (k2 - 1) * mij + 1;       //zsq
				rk2ij2 = data[numtmp].r;
				degk2ij2 = M_PI * 2;
				degzk2ij2 = data[numtmp].degz;
			}
			else
			{
				//numtmp=k2*mij+ij2;		//+1����һ�д�m��n��z��Χ����Ϣ
				numtmp = (k2 - 1) * mij + ij2+1;    //zsq
				rk2ij2 = data[numtmp].r;
				degk2ij2 = data[numtmp].deg;
				degzk2ij2 = data[numtmp].degz;
			}



			//��һ�����Բ�ֵ�ҳ��м�뾶
			
			if ((degk1ij1 - degk1ij2) < 0.0001 && (degk1ij1 - degk1ij2) > -0.0001)
			{
				rk1 = rk1ij1;
			}
			else
			{
				rk1 = (rk1ij2 * (deg - degk1ij1) + rk1ij1 * (degk1ij2 - deg)) / (degk1ij2 - degk1ij1);
				//rk1 = rk1ij2 * (deg - degk1ij1) / degreeij + rk1ij1 * (1 - (deg - degk1ij1)/ degreeij);
				

			}

			if ((degk2ij1 - degk2ij2) < 0.0001 && (degk2ij1 - degk2ij2) > -0.0001)
			{
				rk2 = rk2ij1;
			}
			else
			{
				rk2 = (rk2ij2 * (deg - degk2ij1) + rk2ij1 * (degk2ij2 - deg)) / (degk2ij2 - degk2ij1);
				//rk2 =rk2ij2 * (deg - degk2ij1)/degreeij + rk2ij1 * (1- (deg - degk2ij1)/degreeij);
				
			}
			//��һ�����Բ�ֵ�ҳ���������뾶
			if ((degzk1ij1 - degzk2ij1) < 0.0001 && (degzk1ij1 - degzk2ij1) > -0.0001)
			{
				r = rk1;
			}
			else
			{
				r = (rk2 * (degz - degzk1ij1) + rk1 * (degzk2ij1 - degz)) / (degzk2ij1 - degzk1ij1);
			}
		}
		//����
		if (num==22)
		{
			int ald=1;
		}
		double adf=asin(0.999190);   //     87.69��
		double adfj=sin(1.53054);
		double adfg=cos(1.53054);
		double as1=r*sin(degz);		
		double as2=r*cos(degz)*cos(deg);
		double as3=r*cos(degz)*sin(deg);
			                       //xyzΪ������Ĳ���������  
								  ///�����浼��Ĳ�����ȥ���˺챦ʯ�뾶��Ϣ��Ϊ+
		z+= r * sin(degz);
		x+=r*cos(degz)*cos(deg);
		y+=r*cos(degz)*sin(deg);

		if (abs(x) <= 0.0001) { x = 0; }
		if (abs(y) <= 0.0001) { y = 0; }
		if (abs(z) <= 0.0001) { z = 0; }

		
		//�������
		uperror=m_ppDoc->GetMesPnt(num).uperror;
		downerror=m_ppDoc->GetMesPnt(num).downerror;
		result=(x0-x)*i+(y0-y)*j+(z0-z)*k;
		err=pow(pow(x-x0,2)+pow(y-y0,2)+pow(z-z0,2),0.5);
		if (abs(err) <= 0.0001) { err = 0; }

		//�ж��������
		if (result>=0)
		{
			symbol=1;
			m_ppDoc->GetMesPnt(num).err=err;
		} 
		else
		{
			symbol=-1;
			m_ppDoc->GetMesPnt(num).err=-1*err;
		}
		
		if ((err>uperror)||(err<-downerror))
		{
			if (err>uperror)
			{
				m_ppDoc->GetMesPnt(num).cot=err-uperror;
			} 
			else
			{
				m_ppDoc->GetMesPnt(num).cot=err-downerror;
			}

		} 
		else
		{
			m_ppDoc->GetMesPnt(num).cot=0;
		}
		m_ppDoc->GetMesPnt(num).x=x;
		m_ppDoc->GetMesPnt(num).y=y;
		m_ppDoc->GetMesPnt(num).z=z;
		m_ppDoc->addToMesPointList(num+1,x,y,z); //zsq   num�ĳ���+1
		m_ppDoc->addToMesErrList(num+1,err);     //zsq
		
		//��������ж������Գ�����и�����ʾ
		if(err > m_ppDoc->GetMesPnt(num).uperror || err < m_ppDoc->GetMesPnt(num).downerror)
		{
			m_ppDoc->setHighlightPnt(num);
		}

	}
	else if (IsMind)
	{
		//MessageBoxA(GetForegroundWindow(),"����δ�����������ϰ���궨�ļ��Ƿ���ڣ�","ERROR",MB_OK);
		IsMind = false;
	}

}
