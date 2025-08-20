/*!	
\brief	Create the main window UI, i.e. menus, toolbar, status bar, etc.
		Handle events from these UIs.
\author: jacky
\date    2014-03
*/
#ifndef F_MAINDOC_H
#define F_MAINDOC_H
/////////////////////////////////////////////////////////////////////////////
#include "../External/fkernel/Include/FKernelDefines.h"
#include "../External/fkernel/Include/FDoc.h"
#include "FUtilities.h"
#include "FGuideDataType.h"
#include "QObject"
#include <QtNetwork/QHostAddress>
#include "FInsFeature.h"
#include "SurfNearbyGraph_Node.h"
#include <set>

namespace FT{
	class FEntity;
	class FSphere;
}

class FMainWindow;
class MeasPt;

class FMainDoc : public	QObject, public FT::FDoc
{
	Q_OBJECT
public:
	FMainDoc(FMainWindow* pWnd);
	~FMainDoc();

	/*!
	\brief initialize the variables in doc and other global variables.
	\return	always true.
	*/
	bool	OnNewDocument();//�����������
	void	clear();		//���������ָ���ļ��������
	void	UpdateAllViews();	//Update doc and all views.
	bool	ImportProbe();		//import probe file
	void	CreateCalSphere(double radius);	//create the standard calibration sphere
	//////////////////////////// accessing methods //////////////////////////////////////////////
	//FMesPntVisual*			GetMesPntVis(){return m_pPntVisual;}

	// �������۲���
	void					SetIpPntList(FIpPntList pointList, double radius=3);//radius of inspection points, for display purpose

	// ��ȡ���۲���
	FIpPntList&				GetIpPntList(){return m_ipPntList;}

	// �����۲��������
	void					AddIpPnt(FIpPoint& pnt){m_ipPntList.push_back(pnt);}

	// ��ȡ����Ϊ _index �����۲��
	FIpPoint&				GetIpPnt(int _index){if(_index>=0 && _index<m_ipPntList.size()) return m_ipPntList[_index];}

	// ������۲���
	void					ClearIpPntList(){m_ipPntList.clear();}

	// ����ʵ�ʲ���
	void					SetMesPntList(FIpPntList pointList){m_mesPntList=pointList;}

	// ��ȡʵ�ʲ���
	FIpPntList&				GetMesPntList(){return m_mesPntList;}

	// ��ȡ����Ϊ _index ��ʵ�ʲ��
	FIpPoint&				GetMesPnt(int _index){if(_index>=0 && _index<m_mesPntList.size()) return m_mesPntList[_index];}

	// ��ȡԭʼ����
	FIpPntList&				GetOrigPntList() { return m_OrigPntList; }

	// ����ԭʼ����
	void					SetOrigPntList(FIpPntList pntList) { m_OrigPntList = pntList; }

	// ��ԭʼ���������
	void					AddOrigPnt(FIpPoint aPnt) { m_OrigPntList.push_back(aPnt); }

	// ��ȡ����Ϊ_index��ԭʼ���
	FIpPoint&				GetOrigPnt(int _index) { return m_OrigPntList[_index]; }

	// ��ȡ�����ڽ�ͼ
	vector<SurfNearbyGraph_Node*>& GetSurfNearbyGraph() { return m_SurfGraphNodeList; }

	// ��ȡ��ʾ����
	vector<MeasPt*>& getPntsShowList();

	// ��ȡ�����ʾ��Ĭ����ɫ
	void getPntsShowColorDefault(double& _r, double& _g, double& _b);

	// �����е���ɫ��ΪĬ��
	void setPntsShowColorDefault();

	void					SetPathList(FIpPathList pathList){m_ipPathList=pathList;}
	void					AddIpPath(FIpPath& path){m_ipPathList.push_back(path);}
	FIpPathList&			GetPathList() {return m_ipPathList;}
	void					ClearIpPathList(){m_ipPathList.clear();}
	void					clearMesPointList();
	void					addToMesPointList(double x_mes, double y_mes, double z_mes);
	void					addToMesPointList(int num, double x_mes, double y_mes, double z_mes);
	vector<double>          GetMeaErrList(){return m_errList;}
	void                    addToMesErrList(int num, double error);
	void					setIsMachineErrorCmps(bool b) { m_bIsMachineErrorCmps = b; }
	bool					getIsMachineErrorCmps() const { return m_bIsMachineErrorCmps; }
	void					setAniState(bool b) { m_bIsAniRunning = b; }
	bool					isAniRunning() const { return m_bIsAniRunning; }
	void					setHighlightPnt(int idx);
	void					setHighlightList(QVector<int> list);
	FFreeBody*				GetProbe(){return m_pProbe;}
	FSphere*				GetCalSphere(){return m_pCalSphere;}
	QHostAddress			GetIP() {return m_serverIP;}
	void					SetIP(QString ip) {m_serverIP = QHostAddress(ip);}

	void AddDispEnt(FEntity* pEnt);

	// ���ȫ����㣬�������۲�㡢ʵ�ʲ��
	void clearPntsAll();

	int						AdjustProbeFlag;     //��־������,��ʼֵΪ0������һ�α궨���1 
	//��ԭ��Ǩ�ƹ����ĺ���
	void					compute2(int num);

	//��׼��궨����
	void                    setProbeDia(double d) { m_d = d; }
	double                  getProbeDia() { return m_d; }
	void                    setRefDia(double d) { m_RefD = d; }
	double                  getRefDia() { return m_RefD; }
	void                    setNbLongitudePnt(int d) { m_NbLongitudePnt = d; }
	int						getNbLongitudePnt() { return m_NbLongitudePnt; }
	void                    setNbLatitudePnt(int d) { m_NbLatitudePnt = d; }
	int						getNbLatitudePnt() { return m_NbLatitudePnt; }

	void                    setrib(set<set<int> > b){m_rib=b;}
	set<set<int> >          getrib(){return m_rib;}
	void                    setribtop(vector<int> b){m_ribtop=b;}
	vector<int>             getribtop()const{return m_ribtop;}
	void                    setslot(set<set<int> > b){m_slot=b;}
	set<set<int> >          getslot()const{return m_slot;}
	void                    setprofile(vector<int> b){m_profile=b;}
	vector<int>             getprofile()const{return m_profile;}
	void                    sethole(set<set<int> > b){m_hole=b;}
	set<set<int> >          gethole()const{return m_hole;}

	void                    setAssemTree(QTreeWidgetItem* pTree) { m_pRoot = pTree; }
	QTreeWidgetItem*        getAssemTree() { return m_pRoot; }

	vector<FInsFeature*>& GetInsFeaList() { return m_InsFeatureList; }

	int     getPntNum() { return m_pntNum; }
	void    setPntNum(int num) { m_pntNum = num; }

	//functions for coordinate transformation

	vector<double>    getTransVec(int i);        ///i = 0, return m_move;i = 1 return m_rotx; i = 2 return m_roty; i =3 return m_rotz; i = 4 return m_total; 
	void    setTransVec(int i ,vector<double> transVec);


signals:
	void	resetPointDataSignal(bool,int,bool);     
	void	updateMesPointDataSignal(int);
	void    onUpdateErrorSignal(int);

private:		
	FMainWindow*		m_pMainWnd;				///main window handle
	PFEntityList		m_displayEnts; 			///list of display entities, e.g. features, models, etc.
	PFEntityList		m_selectEnts;			///list of selected entities, e.g. features, models, etc.		
	FFreeBody*			m_pProbe;				///probe model imported from hard disk
	FSphere*			m_pCalSphere;			///calibration sphere model

	// ������۲��
	FIpPntList			m_ipPntList;

	// �����ʾ���
	vector<MeasPt*> m_pntsShowList;

	// �����ʾ��Ĭ����ɫ
	double m_pntShowDef_R;
	double m_pntShowDef_G;
	double m_pntShowDef_B;

	// �����ʾ�ĸ�����ɫ
	double m_pntShowHig_R;
	double m_pntShowHig_G;
	double m_pntShowHig_B;

	// ���ʵ�ʲ��
	FIpPntList			m_mesPntList;

	FIpPathList			m_ipPathList;				///inspection paths read from guide file or TSP
	vector<double>      m_errList;              ///list for error data
	double              m_d;                    ///Diameter of inspection probe head
	double				m_RefD;					///Diameter of the reference sphere
	int					m_NbLongitudePnt;		///Number of longitude points on reference sphere
	int					m_NbLatitudePnt;		///Number of latitude points on reference sphere

	//�ӻ�����ȡ�Ĳ������ԭʼ����
	FIpPntList			m_OrigPntList;

	int                        m_pntNum;
	vector<FInsFeature*> m_InsFeatureList;//��ż������

	// ��������ڽ�ͼ
	vector<SurfNearbyGraph_Node*> m_SurfGraphNodeList;

	//FMesPntVisual*		m_pPntVisual;		
	QHostAddress		m_serverIP;
	QTreeWidgetItem*    m_pRoot;

	bool				m_bIsMachineErrorCmps;			//�Ƿ�ִ�л�������
	bool				m_bIsAniRunning;		//�����Ƿ�������
	set<set<int> >      m_rib;
	vector<int>         m_ribtop;
	set<set<int> >      m_slot;
	vector<int>         m_profile;
	set<set<int> >      m_hole;

	// ������
	// ����ϵ�仯���� added by Haoyu Jiang at 2021.03.23
	vector<double>              m_move;       //ͨ��vector�洢���ݷ������,����ֱ�ӵ��þ���ʹ��ָ��ָ���ָ��
	vector<double>              m_rotx;
	vector<double>              m_roty;
	vector<double>              m_rotz;
	vector<double>              m_total;

};
#endif
