#include "FGuideDataType.h"
#include "FMainWindow.h"
#include "iostream"

extern FMainWindow* gWindow;

FIpPoint::FIpPoint()
{
	x=0;	y=0;	z=0;
	mx=0;	my=0;	mz=0;
	i=0;	j=0;	k=0;
	r=0;	a=0;	cot=0;
	err=0;	uperror=0.04;	downerror=-0.04;
	symbol=0;
}

FIpPoint::FIpPoint(double _x, double _y, double _z)
{
	x=_x;	y=_y;	z=_z;
	mx=0;	my=0;	mz=0;
	i=0;	j=0;	k=0;
	r=0;	a=0;	cot=0;
	err=0;	uperror=0.04;	downerror=-0.04;
	symbol=0;
}

FIpPoint::FIpPoint(gp_Pnt _loc, gp_Vec _dir)
{
	x = _loc.X();	y = _loc.Y();	z = _loc.Z();
	mx = 0;	my = 0;	mz = 0;
	i = _dir.X();	j = _dir.Y();	k = _dir.Z();
	r = 0;	a = 0;	cot = 0;
	err = 0;	uperror = 0.04;	downerror = -0.04;
	symbol = 0;
}

bool FIpPoint::isVaild() const
{
	if( fabs(x+9999) < 1e-3 && fabs(y+9999) < 1e-3 
		&& fabs(z+9999) < 1e-3)
		return false;
	else
		return true;
}

FIpPath::FIpPath()
{

}
