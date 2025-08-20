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
	bool	OnNewDocument();//清除所有数据
	void	clear();		//仅清除测量指导文件相关数据
	void	UpdateAllViews();	//Update doc and all views.
	bool	ImportProbe();		//import probe file
	void	CreateCalSphere(double radius);	//create the standard calibration sphere
	//////////////////////////// accessing methods //////////////////////////////////////////////
	//FMesPntVisual*			GetMesPntVis(){return m_pPntVisual;}

	// 设置理论测点表
	void					SetIpPntList(FIpPntList pointList, double radius=3);//radius of inspection points, for display purpose

	// 获取理论测点表
	FIpPntList&				GetIpPntList(){return m_ipPntList;}

	// 向理论测点表加入测点
	void					AddIpPnt(FIpPoint& pnt){m_ipPntList.push_back(pnt);}

	// 获取索引为 _index 的理论测点
	FIpPoint&				GetIpPnt(int _index){if(_index>=0 && _index<m_ipPntList.size()) return m_ipPntList[_index];}

	// 清空理论测点表
	void					ClearIpPntList(){m_ipPntList.clear();}

	// 设置实际测点表
	void					SetMesPntList(FIpPntList pointList){m_mesPntList=pointList;}

	// 获取实际测点表
	FIpPntList&				GetMesPntList(){return m_mesPntList;}

	// 获取索引为 _index 的实际测点
	FIpPoint&				GetMesPnt(int _index){if(_index>=0 && _index<m_mesPntList.size()) return m_mesPntList[_index];}

	// 获取原始测点表
	FIpPntList&				GetOrigPntList() { return m_OrigPntList; }

	// 设置原始测点表
	void					SetOrigPntList(FIpPntList pntList) { m_OrigPntList = pntList; }

	// 向原始测点表加入测点
	void					AddOrigPnt(FIpPoint aPnt) { m_OrigPntList.push_back(aPnt); }

	// 获取索引为_index的原始测点
	FIpPoint&				GetOrigPnt(int _index) { return m_OrigPntList[_index]; }

	// 获取曲面邻接图
	vector<SurfNearbyGraph_Node*>& GetSurfNearbyGraph() { return m_SurfGraphNodeList; }

	// 获取显示测点表
	vector<MeasPt*>& getPntsShowList();

	// 获取测点显示的默认颜色
	void getPntsShowColorDefault(double& _r, double& _g, double& _b);

	// 将所有点颜色置为默认
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

	// 清空全部测点，包含理论测点、实际测点
	void clearPntsAll();

	int						AdjustProbeFlag;     //标志，符号,初始值为0，经过一次标定后变1 
	//照原样迁移过来的函数
	void					compute2(int num);

	//标准球标定数据
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

	// 存放理论测点
	FIpPntList			m_ipPntList;

	// 存放显示测点
	vector<MeasPt*> m_pntsShowList;

	// 测点显示的默认颜色
	double m_pntShowDef_R;
	double m_pntShowDef_G;
	double m_pntShowDef_B;

	// 测点显示的高亮颜色
	double m_pntShowHig_R;
	double m_pntShowHig_G;
	double m_pntShowHig_B;

	// 存放实际测点
	FIpPntList			m_mesPntList;

	FIpPathList			m_ipPathList;				///inspection paths read from guide file or TSP
	vector<double>      m_errList;              ///list for error data
	double              m_d;                    ///Diameter of inspection probe head
	double				m_RefD;					///Diameter of the reference sphere
	int					m_NbLongitudePnt;		///Number of longitude points on reference sphere
	int					m_NbLatitudePnt;		///Number of latitude points on reference sphere

	//从机床读取的测点坐标原始数据
	FIpPntList			m_OrigPntList;

	int                        m_pntNum;
	vector<FInsFeature*> m_InsFeatureList;//存放检测特征

	// 存放曲面邻接图
	vector<SurfNearbyGraph_Node*> m_SurfGraphNodeList;

	//FMesPntVisual*		m_pPntVisual;		
	QHostAddress		m_serverIP;
	QTreeWidgetItem*    m_pRoot;

	bool				m_bIsMachineErrorCmps;			//是否执行机床误差补偿
	bool				m_bIsAniRunning;		//动画是否在运行
	set<set<int> >      m_rib;
	vector<int>         m_ribtop;
	set<set<int> >      m_slot;
	vector<int>         m_profile;
	set<set<int> >      m_hole;

	// 已弃用
	// 坐标系变化矩阵 added by Haoyu Jiang at 2021.03.23
	vector<double>              m_move;       //通过vector存储数据方便调用,避免直接调用矩阵使用指向指针的指针
	vector<double>              m_rotx;
	vector<double>              m_roty;
	vector<double>              m_rotz;
	vector<double>              m_total;

};
#endif
