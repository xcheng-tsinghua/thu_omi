/*
ת��������NC�������
*/

#ifndef _FTHP_H_
#define	_FTHP_H_
#include <QString>
#include "FGuideDataType.h"

class NCEditDlg;

class FTHP
{
public:
	FTHP();
	FTHP(QString codeIn);
	double	TH_GetValue(QString symbol);
	static	QString TH_GetBody(double X,double Y,double Z,double I,double J,double K);
	/*!
	\brief	Get value from THP_Lite.ini via gAppSettings.
	\param	[i]sAppName	triangular mesh model.
	\param	[i]sKeyName	start point of the line.
	\return	a string of value corresponding to sAppName+sKeyName.*/
	static	QString TH_GetProfileString(QString sAppName, QString sKeyName);
	/*!
	\brief	Convert path into a NC file. 
	\param	[i]sGCodeOut	a string containing G code, out to file directly.
	\param	[i]pathlist		list inspection path, needs to convert to G code.
	\return	void.*/	
	static	void TH_NCFileWrite(QString sGCodeOut, QString fname="NC.mpf");

	//Ŀǰʹ�õ�NC���ɴ���
	void TH_NCFileWrite(FIpPathList& pathlist, QString fname="NC.mpf");

	void ProcessAndSaveNC(QString fpNCContent, QString fpFileSavePath);

	void InitDefaultStr();

	//���ͺ�õ�����ͷ�����޸ģ�ȥ���˵�һ�е�M66�͵ڶ��е�TRAORI
	QString TH_GetHead();

	QString TH_GetCenter();
	QString TH_GetProb(double ii,double jj,double kk,double D);

	//���ͺ�õ�����β�����޸ģ�ȥ�����ļ�β��TRAORI
	QString TH_GetTail();

	// �����㵽���صĳ���ͷ�����β
	QString GetSaveToMachineToolLocalHead() { return m_NC_SaveToMachineToolLocalHead; };
	QString GetSaveToMachineToolLocalPnt() { return m_NC_SaveToMachineToolLocalPnt; };
	QString GetSaveToMachineToolLocalLast() { return m_NC_SaveToMachineToolLocalLast; };

	void TH_ProbeFileWrite(QString csGCodeOut, QString fname);

	/*! added by Haoyu Jiang on 2021.03.30
	\brief   function to return the new inspection points order after path planning

	         m_newPnts are generated at function TH_NCFileWrite()
	*/
	FIpPntList TH_GetPntsOrder() { return m_newPnts; }

	//added by chengxi<set the probe on and off command>
	//void SetProbeOnCommand(QString probeOnString) { m_ProbeOn = probeOnString; };
	//void SetProbeOffCommand(QString probeOffString) { m_ProbeOff = probeOffString; };

	// ��ȡȥ������ͷ�ͳ���β�Ĳ���NC
	QString GetNCCodeBody() { return m_NC_CodeBody; };

	void setIsPauseBeforeMeas(bool _isPause);

protected:
	/*!
	\brief	Convert a point to G code string, w.r.t. intermediate or inspection point. 
	\param	[i]pnt		given point.
	\param	[i]bIpPnt	true for inspection point, default is false.
	\return	string of corresponding G code.*/	
	QString	ConvertPoint2Code(FIpPoint& pnt, bool bIpPnt=false);

	FIpPntList m_newPnts;
private:
	//���ݳ�Ա
	QString	code;
	double	X, Y, Z, I, J, K;

	// ȥ������ͷ�ͳ���β�Ĳ���NC
	QString m_NC_CodeBody;

	// ����ͷ
	QString m_NC_SaveToMachineToolLocalHead;

	// ��¼������
	QString m_NC_SaveToMachineToolLocalPnt;

	// ����β���������
	QString m_NC_SaveToMachineToolLocalLast;

	// NC �ı��༭��
	NCEditDlg* m_pNCEditDlg;

	// NC ����ʹ�õĺ�
	QString m_SCENTER;
	QString m_SCOMEND;
	QString m_SCOMSTART;
	QString m_SMEAS;
	QString m_SMOVE;
	QString m_SPROBE;

	// ���ɼ��NC��MEAS֮ǰ��Ҫ��ͣ10��
	// �궨��������ͣ
	bool m_isPauseBeforeMeas;

};

#endif