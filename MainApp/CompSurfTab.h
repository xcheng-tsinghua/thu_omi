/*
使用镜像曲面的方法实现加工误差补偿
理论：孙震
实现：张博洋
修改：程曦
*/

#pragma once
#include <qwidget.h>
#include "../External/fKernel/include/Fkerneldefines.h"
#include <Geom_BSplineSurface.hxx>
#include <numeric>

class CompSurfTab :public QWidget
{
	Q_OBJECT
public:
	CompSurfTab(QWidget* parent = NULL);
	~CompSurfTab(void);

	struct ConPnt  //重构点
	{
		double uPara, vPara, ErrorDist;
		gp_Pnt MirrorInsec, ErrorInsec;
		gp_Vec Nvector;
		ConPnt(double p1, double p2) : uPara(p1), vPara(p2) {}
		ConPnt() : uPara(0), vPara(0) {}
	};

	void setSurface(FSurface* _surf);

private slots:
	void onRecstMeasFace();

	// 生成误差补偿后的曲面
	void onGenCompadSurf();

	void onSaveCompensatedModel();
	void onFaceSel();

private:
	// 曲面选择工具
	//FFeatureTool* m_pFaceSelTool;

	// 显示选择的曲面名
	QLabel* m_pSelFaceNameLabel;

	//窗口界面成员变量
	QLineEdit* m_pMachiningAllowance;

	// 由被测点重构获得的曲面
	Handle(Geom_BSplineSurface) m_pRecstBSplSurf;
	FSurface* m_pRecstFace;

	// 补偿后的曲面
	Handle(Geom_BSplineSurface) m_pCompsedBSplSurf;
	FSurface* m_pCompsedFace;

	// 鼠标点选的曲面
	Handle(Geom_BSplineSurface) m_pSelBSplSurf;
	FSurface* m_pSelFSurf;

	// 构造镜像曲面（误差补偿曲面）时采点的分段数
	int m_ErrEvaluSecUV;

	// 保存补偿模型时，是否仅输出补偿后的单个曲面
	// == false 时输出全部面（补偿面的理论模型替换为实际模型）
	bool m_is_out_compsurf_only;


};




