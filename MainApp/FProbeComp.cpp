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
	//将测量值和理论值数据赋给本地变量，RealPointsList为测量值，PointsList为理论值
	x=m_ppDoc->GetMesPnt(num).x;
	y=m_ppDoc->GetMesPnt(num).y;
	z=m_ppDoc->GetMesPnt(num).z;
	mx=m_ppDoc->GetMesPnt(num).mx;   //测量点的法矢/旧版本中用到。新版本不用
	my=m_ppDoc->GetMesPnt(num).my;
	mz=m_ppDoc->GetMesPnt(num).mz;

	x0=m_ppDoc->GetIpPnt(num).x;    
	y0=m_ppDoc->GetIpPnt(num).y;
	z0=m_ppDoc->GetIpPnt(num).z;
	i=m_ppDoc->GetIpPnt(num).i;
	j=m_ppDoc->GetIpPnt(num).j;
	k=m_ppDoc->GetIpPnt(num).k;
	
	//////////////baosheng/////////////////////////
	
	degz=asin(k);	//与z轴夹角
	if ((1-k)>=0.00001)	//有xy投影
	{
		double ij=pow(pow(i,2)+pow(j,2),0.5);	//向量xy平面投影长度
		deg=asin(j/ij);
		if(i!=0){
			if(i>0){		//右半平面,j>=0第一象限，deg值不变
				if(j<0){	//右半平面,j<0第四象限，deg值+2pai
					deg+=2*M_PI;
				}
			}else{			//左半平面
				deg=M_PI-deg;
			}
		}else{
			if(j>=0){			//=0不可能出现，保留
				deg=M_PI/2;
			}else{
				deg=M_PI*3/2;
			}
		}
		
	} 
	else	//纯z向，xy无投影
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
	
	//读取标定文件,将数据存储于data变量中
    if (fopen(nameWithPath.toLocal8Bit().data(),"r")!=NULL) {
		FILE * file=fopen(nameWithPath.toLocal8Bit().data(),"r");
		fscanf(file, "%f %f %f  %f", &mk, &mij, &probedegree, &startdegree);
		while(fscanf(file, "%f %f %f", &dr, &ddeg, &ddegz) != EOF)  // 这里用到了误差的模和其两个角度
		{
			FIpPoint point = FIpPoint();
			point.r=dr;
			point.deg=ddeg;
			point.degz=ddegz;
			data.push_back(point);
		}
		fclose(file);
		
		
		//误差补偿算法
		/////////////baosheng///////////////
		double degreek=(0.5*M_PI)/(mk);	//z向步进   zsq
		double degreeij=2*M_PI/mij;	//xy平面步进
		double degreetmp=0;			//临时变量，用于寻找弧度区间
		int ktmp=0;					//临时变量，用于寻找点数区间
		if (deg-2*M_PI<0.00001&&deg-2*M_PI>-0.00001)
		{
			ktmp=mij;
		} 
		else
		{
			while(degreetmp<=deg)	//xy平面寻找区间
			{
				degreetmp+=degreeij;
				ktmp++;
			}
		}
		ij1=ktmp-1;		//区间下限
		ij2=ktmp;		//区间上限
		
		degreetmp=probedegree*M_PI/180;			//临时变量，用于寻找弧度区间
		ktmp=0;					//临时变量，用于寻找点数区间
		if (degz*180/M_PI+ probedegree <0.0001)
		{
			ktmp=mk-1;
		} 
		else if(degz>degreetmp){
			while (degz >= degreetmp)	//z平面寻找区间
			{
				degreetmp += degreek;
				ktmp++;
			}
		}
		else{
			while(degreetmp>=degz)	//z平面寻找区间
			{
				degreetmp-=degreek;
				ktmp++;
			}
		}
		k1=ktmp-1;		//区间下限
		k2=ktmp;		//区间上限
		/////////////baosheng///////////////
		
		double rk1, rk2, r;//双线性插值算法中运用变量

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
			//k1ij1点
			rk1ij1 = data[numtmp].r;
			degk1ij1 = data[numtmp].deg;
			degzk1ij1 = data[numtmp].degz;  //zsq


			//k2ij1点
			//numtmp=k2*mij+ij1;		//+1：第一行存m、n、z向范围等信息
			numtmp = (k2 - 1) * mij + ij1+1;    //zsq
			rk2ij1 = data[numtmp].r;
			degk2ij1 = data[numtmp].deg;
			degzk2ij1 = data[numtmp].degz;
			//k2ij2点
			if (ij2 >= mij)	//处理超限，大于情况不可能出现
			{
				//numtmp=k2*mij;		//取ij2=0点
				numtmp = (k2 - 1) * mij + 1;       //zsq
				rk2ij2 = data[numtmp].r;
				degk2ij2 = M_PI * 2;
				degzk2ij2 = data[numtmp].degz;
			}
			else
			{
				//numtmp=k2*mij+ij2;		//+1：第一行存m、n、z向范围等信息
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


			//给找出的四点赋值
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
			double numtmp = 0;		//临时变量，记录数据表行数
			//k1ij1点
			numtmp = (k1 - 1) * mij + ij1+1;		//+1：第一行存m、n、z向范围等信息
			rk1ij1 = data[numtmp].r;
			degk1ij1 = data[numtmp].deg;
			degzk1ij1 = data[numtmp].degz;
			//k1ij2点
			if (ij2 >= mij)	//处理超限，大于情况不可能出现
			{
				numtmp =( k1-1) * mij+1;		//取ij2=0点
				rk1ij2 = data[numtmp].r;
				degk1ij2 = M_PI * 2;
				degzk1ij2 = data[numtmp].degz;
			}
			else
			{
				numtmp = (k1 - 1) * mij + ij2+1;		//+1：第一行存m、n、z向范围等信息
				rk1ij2 = data[numtmp].r;
				degk1ij2 = data[numtmp].deg;
				degzk1ij2 = data[numtmp].degz;
			}

			//k2ij1点
			numtmp = (k2 - 1) * mij + ij1+1;		//+1：第一行存m、n、z向范围等信息
			rk2ij1 = data[numtmp].r;
			degk2ij1 = data[numtmp].deg;
			degzk2ij1 = data[numtmp].degz;
			//k2ij2点
			if (ij2 >= mij)	//处理超限，大于情况不可能出现
			{
				numtmp = (k2 - 1) * mij+1;		//取ij2=0点
				rk2ij2 = data[numtmp].r;
				degk2ij2 = M_PI * 2;
				degzk2ij2 = data[numtmp].degz;
			}
			else
			{
				numtmp = (k2 - 1) * mij + ij2+1;		//+1：第一行存m、n、z向范围等信息
				rk2ij2 = data[numtmp].r;
				degk2ij2 = data[numtmp].deg;
				degzk2ij2 = data[numtmp].degz;
			}


			//第一步线性插值找出中间半径
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
			//第一步线性插值找出补偿所需半径
			if ((degzk1ij1 - degzk2ij1) < 0.00001 && (degzk1ij1 - degzk2ij1) > -0.00001)
			{
				r = rk1;
			}
			else
			{
				r = (rk2 * (degz - degzk1ij1) + rk1 * (degzk2ij1 - degz)) / (degzk2ij1 - degzk1ij1);
			}
		}
		//补偿
		z += r * sin(degz);
		x += r * cos(degz) * cos(deg);
		y += r * cos(degz) * sin(deg);

		if (abs(x) <= 0.0001) { x = 0; }
		if (abs(y) <= 0.0001) { y = 0; }
		if (abs(z) <= 0.0001) { z = 0; }

		
		//double dm=computemachinex(mx);
		
		//计算误差
		uperror=m_ppDoc->GetMesPnt(num).uperror;
		downerror=m_ppDoc->GetMesPnt(num).downerror;
		result=(x0-x)*i+(y0-y)*j+(z0-z)*k;
		err=pow(pow(x-x0,2)+pow(y-y0,2)+pow(z-z0,2),0.5);
		if (abs(err) <= 0.0001) { err = 0; }
		//判断误差正负
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
		MessageBoxA(GetForegroundWindow(),"数据未补偿！请检查固定摆角标定文件是否存在！","ERROR",MB_OK);
    }
}

void FProbeComp :: errCompUpperSphere(int num)
{
	double x,y,z,x0,y0,z0,i,j,k,result,err,uperror,downerror,deg,degz;//deg,degz测量点
	double x1,x2,y1,y2,deg1,deg2,r1,r2;
	int symbol;
	//将测量值和理论值数据赋给本地变量，RealPointsList为测量值，PointsList为理论值
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
	
	degz=asin(k);	//与xt平面夹角
	if ((1-k)>=0.00001)	//有xy投影
	{
		double ij=pow(pow(i,2)+pow(j,2),0.5);	//向量xy平面投影长度
		deg=asin(j/ij);
		if(i!=0){
			if(i>0){		//右半平面,j>=0第一象限，deg值不变
				if(j<0){	//右半平面,j<0第四象限，deg值+2pai
					deg+=2*M_PI;
				}
			}else{			//左半平面
				deg=M_PI-deg;
			}
		}else{
			if(j>=0){			//=0不可能出现，保留
				deg=M_PI/2;
			}else{
				deg=M_PI*3/2;
			}
		}
		
	} 
	else	//纯z向，xy无投影
	{
		deg=0;
	}
	///////////////////baosheng/////////////////////////////
	
	
	float dr=0;
	float ddeg=0;
	float ddegz=0;//标定文件的
	float mij=0;
	float mk=0;
	int k1,k2,ij1,ij2;
	vector<FIpPoint> data;
	//FIpPoint * point;
	QString nameWithPath = gAppPath+QString("/settings/nc/ErrUpperSphere.txt");
	
	//读取标定文件,将数据存储于data变量中

    if (FILE* file = fopen(nameWithPath.toLocal8Bit().data(),"r")) {
		//FILE * file=fopen(nameWithPath.toLocal8Bit().data(),"r");
		fscanf(file, "%f %f", &mij, &mk);                         //mij为每圈点数，mk为圈数不含极点
		while(fscanf(file, "%f %f %f", &dr, &ddeg, &ddegz) != EOF)
		{
			FIpPoint point = FIpPoint();
			point.r=dr;
			point.deg=ddeg;
			point.degz=ddegz;
			data.push_back(point);
		}

		fclose(file);
		
		//误差补偿算法
		/////////////baosheng///////////////
		double degreek=(M_PI/2)/mk;	//z向步进    zsq
		double degreeij=2*M_PI/mij;	//xy平面步进   
		double degreetmp=0;			//临时变量，用于寻找弧度区间
		int ktmp=0;					//临时变量，用于寻找点数区间
		if (deg-2*M_PI<0.0001&&deg-2*M_PI>-0.0001)
		{
			ktmp=mij;
		} 
		else
		{
			while(degreetmp<=deg)	//xy平面寻找区间
			{
				degreetmp+=degreeij;
				ktmp++;
			}
		}
		ij1=ktmp-1;		//区间下限            //这两个值确定了在每圈的两个点，还需确定在第几圈
		ij2=ktmp;		//区间上限
		
		degreetmp=M_PI*0.5;			//临时变量，用于寻找弧度区间
		ktmp=0;					//临时变量，用于寻找点数区间
		
		if (degz*180/M_PI<0.0001)   //degz是否小于0.0001度，是则在赤道平面上
		{
			ktmp=mk;
		} 
		else if(degz>degreetmp){
			ktmp=1;
		}
		else{
			while(degreetmp>=degz)	//z平面寻找区间
			{
				degreetmp-=degreek;
				ktmp++;
			}
		}
		
		k1=ktmp-1;		//区间下限           //这两个值确定在第几圈
		k2=ktmp;		//区间上限
		/////////////baosheng///////////////
		double rk1, rk2, r;//双线性插值算法中运用变量

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
			//k1ij1点
			rk1ij1 = data[numtmp].r;
			degk1ij1 = data[numtmp].deg;
			degzk1ij1 = data[numtmp].degz;  //zsq


			//k2ij1点
			//numtmp=k2*mij+ij1;		//+1：第一行存m、n、z向范围等信息
			numtmp = (k2 - 1) * mij + ij1+1;    //zsq
			rk2ij1 = data[numtmp].r;
			degk2ij1 = data[numtmp].deg;
			degzk2ij1 = data[numtmp].degz;
			//k2ij2点
			if (ij2 >= mij)	//处理超限，大于情况不可能出现
			{
				//numtmp=k2*mij;		//取ij2=0点
				numtmp = (k2 - 1) * mij + 1;       //zsq
				rk2ij2 = data[numtmp].r;
				degk2ij2 = M_PI * 2;
				degzk2ij2 = data[numtmp].degz;
			}
			else
			{
				//numtmp=k2*mij+ij2;		//+1：第一行存m、n、z向范围等信息
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
			//给找出的四点赋值
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
			//double numtmp=0;		//临时变量，记录数据表行数
			////k1ij1点
			//numtmp=k1*mij+ij1;		//+1：第一行存m、n、z向范围等信息
			//rk1ij1=data[numtmp].r;
			//degk1ij1=data[numtmp].deg;
			//degzk1ij1=data[numtmp].degz;

			double numtmp = 0;
			//k1ij1点
			numtmp = (k1 - 1) * mij + ij1+1;
			rk1ij1 = data[numtmp].r;
			degk1ij1 = data[numtmp].deg;
			degzk1ij1 = data[numtmp].degz;  //zsq


			//k1ij2点
			if (ij2 >= mij)	//处理超限，大于情况不可能出现
			{
				//numtmp=k1*mij;		//取ij2=0点
				numtmp = (k1 - 1) * mij + 1;       //zsq

				rk1ij2 = data[numtmp].r;
				degk1ij2 = M_PI * 2;
				degzk1ij2 = data[numtmp].degz;
			}
			else
			{
				//numtmp=k1*mij+ij2;		//+1：第一行存m、n、z向范围等信息
				numtmp = (k1 - 1) * mij + ij2+1;    //zsq
				rk1ij2 = data[numtmp].r;
				degk1ij2 = data[numtmp].deg;
				degzk1ij2 = data[numtmp].degz;
			}

			//k2ij1点
			//numtmp=k2*mij+ij1;		//+1：第一行存m、n、z向范围等信息
			numtmp = (k2 - 1) * mij + ij1+1;    //zsq
			rk2ij1 = data[numtmp].r;
			degk2ij1 = data[numtmp].deg;
			degzk2ij1 = data[numtmp].degz;
			//k2ij2点
			if (ij2 >= mij)	//处理超限，大于情况不可能出现
			{
				//numtmp=k2*mij;		//取ij2=0点
				numtmp = (k2 - 1) * mij + 1;       //zsq
				rk2ij2 = data[numtmp].r;
				degk2ij2 = M_PI * 2;
				degzk2ij2 = data[numtmp].degz;
			}
			else
			{
				//numtmp=k2*mij+ij2;		//+1：第一行存m、n、z向范围等信息
				numtmp = (k2 - 1) * mij + ij2+1;    //zsq
				rk2ij2 = data[numtmp].r;
				degk2ij2 = data[numtmp].deg;
				degzk2ij2 = data[numtmp].degz;
			}



			//第一步线性插值找出中间半径
			
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
			//第一步线性插值找出补偿所需半径
			if ((degzk1ij1 - degzk2ij1) < 0.0001 && (degzk1ij1 - degzk2ij1) > -0.0001)
			{
				r = rk1;
			}
			else
			{
				r = (rk2 * (degz - degzk1ij1) + rk1 * (degzk2ij1 - degz)) / (degzk2ij1 - degzk1ij1);
			}
		}
		//补偿
		if (num==22)
		{
			int ald=1;
		}
		double adf=asin(0.999190);   //     87.69度
		double adfj=sin(1.53054);
		double adfg=cos(1.53054);
		double as1=r*sin(degz);		
		double as2=r*cos(degz)*cos(deg);
		double as3=r*cos(degz)*sin(deg);
			                       //xyz为补偿后的测量点坐标  
								  ///若后面导入的测量点去除了红宝石半径信息改为+
		z+= r * sin(degz);
		x+=r*cos(degz)*cos(deg);
		y+=r*cos(degz)*sin(deg);

		if (abs(x) <= 0.0001) { x = 0; }
		if (abs(y) <= 0.0001) { y = 0; }
		if (abs(z) <= 0.0001) { z = 0; }

		
		//计算误差
		uperror=m_ppDoc->GetMesPnt(num).uperror;
		downerror=m_ppDoc->GetMesPnt(num).downerror;
		result=(x0-x)*i+(y0-y)*j+(z0-z)*k;
		err=pow(pow(x-x0,2)+pow(y-y0,2)+pow(z-z0,2),0.5);
		if (abs(err) <= 0.0001) { err = 0; }

		//判断误差正负
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
		m_ppDoc->addToMesPointList(num+1,x,y,z); //zsq   num改成了+1
		m_ppDoc->addToMesErrList(num+1,err);     //zsq
		
		//进行误差判定，并对超差进行高亮显示
		if(err > m_ppDoc->GetMesPnt(num).uperror || err < m_ppDoc->GetMesPnt(num).downerror)
		{
			m_ppDoc->setHighlightPnt(num);
		}

	}
	else if (IsMind)
	{
		//MessageBoxA(GetForegroundWindow(),"数据未补偿！请检查上半球标定文件是否存在！","ERROR",MB_OK);
		IsMind = false;
	}

}
