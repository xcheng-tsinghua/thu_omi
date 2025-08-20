#include "InsFeaUnitPainter_Rep.h"

InsFeaUnitPainter_Rep::InsFeaUnitPainter_Rep(EvaluData Evalu, QWidget* parent):QWidget(parent)
{
	//黑框总宽度
	Width = 1213;//原始带模型截图的为750

	//边缘宽度
	Marginal = 5;

	//最上方检测特征名高度
	InsFeaNameH = 15;//字体设置时高度
	RealNameH = 40;//实际字高

	//逐点比较字号
	PntCompWordH = 8;

	//下排总览窗口高度
	OvewHigh = 45;

	//设定每一排逐点比较高度
	PntCompHigh = 85;

	//左侧用户输入信息栏宽度
	UserInfoW = 65;

	//检测类型高度
	TypeH = 25;

	//每排逐点比较点数
	NbPntperRow = 7;

	//计算有几排逐点比较，一排NbPntperRow个
	NbRow = Evalu.ThoMesPntList.size() / NbPntperRow;
	if (Evalu.ThoMesPntList.size() % NbPntperRow != 0)
	{
		NbRow += 1;
	}

	//计算黑框总高度，总高度等于逐点比较高度加下排总览高度
	Height = NbRow * PntCompHigh + OvewHigh;

	setFixedSize(Width + 2 * Marginal, Height + 2 * Marginal + RealNameH); //固定大小

	p_Evalu = Evalu;
}

InsFeaUnitPainter_Rep::~InsFeaUnitPainter_Rep()
{

}

void InsFeaUnitPainter_Rep::paintEvent(QPaintEvent*)
{
	QPainter painter(this);// 创建QPainter一个对象
	QPen pen;

	//检测特征名
	QFont InsFeaNameFont("宋体", InsFeaNameH, QFont::Normal, false);
	painter.setFont(InsFeaNameFont);
	painter.setPen(Qt::black);
	painter.drawText(5, RealNameH, QString::fromLocal8Bit("检测特征")+QString::number(p_Evalu.p_InsFea->InsFeatureindex));

	//画外框
	painter.setPen(QPen(Qt::black, 2));
	QPointF points1[5] = { QPointF(Marginal, Marginal + RealNameH), QPointF(Width + Marginal, Marginal + RealNameH), QPointF(Width + Marginal, Height + Marginal + RealNameH), QPointF(Marginal, Height + Marginal + RealNameH),QPointF(Marginal, Marginal + RealNameH) };
	painter.drawPolyline(points1, 5);

	//画左边用户输入信息栏
	QPointF UserinfoLine1[2] = { QPointF(UserInfoW + Marginal, Marginal + RealNameH), QPointF(UserInfoW + Marginal, Height + Marginal + RealNameH) };
	painter.drawPolyline(UserinfoLine1, 2);//画竖线

	//如果是检测公差，不用画第二三条横线，检测尺寸才需要
	painter.setPen(Qt::black);
	QPointF UserinfoLine2[2] = { QPointF(Marginal, Marginal + RealNameH + TypeH), QPointF(Marginal + UserInfoW, Marginal + RealNameH + TypeH) };
	painter.drawPolyline(UserinfoLine2, 2);

	int EachH = (NbRow * PntCompHigh + OvewHigh - TypeH) / 3;//左侧横线除第一行外每行行高
	if (p_Evalu.p_InsFea->type == DISTANCE || p_Evalu.p_InsFea->type == DIAMETER)
	{
		QPointF UserinfoLine3[2] = { QPointF(Marginal, Marginal + RealNameH + TypeH + EachH), QPointF(Marginal + UserInfoW, Marginal + RealNameH + TypeH + EachH) };
		painter.drawPolyline(UserinfoLine3, 2);

		QPointF UserinfoLine4[2] = { QPointF(Marginal, Marginal + RealNameH + TypeH + 2 * EachH), QPointF(Marginal + UserInfoW, Marginal + RealNameH + TypeH + 2 * EachH) };
		painter.drawPolyline(UserinfoLine4, 2);
	}

	//画最下方总览栏横线
	QPointF OvewLine[2] = { QPointF(Marginal + UserInfoW, Height + Marginal + RealNameH - OvewHigh), QPointF(Width + Marginal, Height + Marginal + RealNameH - OvewHigh) };
	painter.drawPolyline(OvewLine, 2);

	//写左侧用户输入信息
	//写第一行检测类型
	QFont InsTypeFont("Microsoft YaHei", PntCompWordH * 1.4, QFont::Normal, false);
	painter.setFont(InsTypeFont);
	painter.setPen(Qt::black);
	QString insType;
	if (p_Evalu.p_InsFea->type == PARALLELISM)
	{
		insType = QString::fromLocal8Bit("平行度");
	}
	else if (p_Evalu.p_InsFea->type == ROUNDNESS)
	{
		insType = QString::fromLocal8Bit("圆度");
	}
	else if (p_Evalu.p_InsFea->type == DISTANCE)
	{
		if (p_Evalu.p_InsFea->DirState == 1)
		{
			insType = QString::fromLocal8Bit("X向距离");
		}
		else if (p_Evalu.p_InsFea->DirState == 2)
		{
			insType = QString::fromLocal8Bit("Y向距离");
		}
		else if (p_Evalu.p_InsFea->DirState == 3)
		{
			insType = QString::fromLocal8Bit("Z向距离");
		}
		else
		{
			insType = QString::fromLocal8Bit("距离");
		}
	}
	else if (p_Evalu.p_InsFea->type == VERTICALITY)
	{
		insType = QString::fromLocal8Bit("垂直度");
	}
	else if (p_Evalu.p_InsFea->type == DIAMETER)
	{
		insType = QString::fromLocal8Bit("直径");
	}
	else if (p_Evalu.p_InsFea->type == AUXIALITY)
	{
		insType = QString::fromLocal8Bit("同轴度");
	}
	else if (p_Evalu.p_InsFea->type == FLATNESS)
	{
		insType = QString::fromLocal8Bit("平面度");
	}
	else
	{
		insType = QString::fromLocal8Bit("未知");
	}
	painter.drawText(Marginal + 2, Marginal + RealNameH + TypeH - 5, insType);

	//写第二行用户输入
	QFont InsTypeFontUser("Microsoft YaHei", PntCompWordH * 1.2, QFont::Normal, false);
	painter.setFont(InsTypeFontUser);
	painter.setPen(Qt::black);

	int DevName_x = 4;//左侧用户输入信息汉字左右偏移距离
	int DevValue_x = 2;//左侧用户输入信息数字左右偏移距离
	int DevValue_y = -3;//左侧用户输入信息数字上下偏移距离
	int UpMv = 17;//左侧用户输入栏上下两排字间距
	int Dev_Sig = 10;//左侧只有一栏时，内容上下偏移量
	int Dev_Tri = -2;

	if (p_Evalu.p_InsFea->type == DISTANCE)
	{
		painter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + EachH - UpMv + Dev_Tri, insType + QString::fromLocal8Bit("："));
		painter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + EachH + Dev_Tri, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));

		painter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + 2 * EachH - UpMv + Dev_Tri, QString::fromLocal8Bit("上偏差："));
		painter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + 2 * EachH + Dev_Tri, tr("+") + QString::number(p_Evalu.p_InsFea->FInsUpDeviationValue) + tr("mm"));

		painter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + 3 * EachH - UpMv + Dev_Tri, QString::fromLocal8Bit("下偏差："));
		painter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + 3 * EachH + Dev_Tri, tr("-") + QString::number(p_Evalu.p_InsFea->FInsDownDeviationValue) + tr("mm"));
	}
	else if (p_Evalu.p_InsFea->type == DIAMETER)
	{
		painter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + EachH - UpMv + Dev_Tri, QString::fromLocal8Bit("直径："));
		painter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + EachH + Dev_Tri, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));

		painter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + 2 * EachH - UpMv + Dev_Tri, QString::fromLocal8Bit("上偏差："));
		painter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + 2 * EachH + Dev_Tri, tr("+") + QString::number(p_Evalu.p_InsFea->FInsUpDeviationValue) + tr("mm"));

		painter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + 3 * EachH - UpMv + Dev_Tri, QString::fromLocal8Bit("下偏差："));
		painter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + 3 * EachH + Dev_Tri, tr("-") + QString::number(p_Evalu.p_InsFea->FInsDownDeviationValue) + tr("mm"));
	}
	else if (p_Evalu.p_InsFea->type == PARALLELISM)
	{
		painter.drawText(DevName_x + Marginal, Dev_Sig + Marginal + RealNameH + TypeH + 1.5 * EachH - UpMv, QString::fromLocal8Bit("平行度："));
		painter.drawText(DevValue_x + Marginal, Dev_Sig + DevValue_y + Marginal + RealNameH + TypeH + 1.5 * EachH, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));
	}
	else if (p_Evalu.p_InsFea->type == ROUNDNESS)
	{
		painter.drawText(DevName_x + Marginal, Dev_Sig + Marginal + RealNameH + TypeH + 1.5 * EachH - UpMv, QString::fromLocal8Bit("圆度："));
		painter.drawText(DevValue_x + Marginal, Dev_Sig + DevValue_y + Marginal + RealNameH + TypeH + 1.5 * EachH, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));
	}
	else if (p_Evalu.p_InsFea->type == VERTICALITY)
	{
		painter.drawText(DevName_x + Marginal, Dev_Sig + Marginal + RealNameH + TypeH + 1.5 * EachH - UpMv, QString::fromLocal8Bit("垂直度："));
		painter.drawText(DevValue_x + Marginal, Dev_Sig + DevValue_y + Marginal + RealNameH + TypeH + 1.5 * EachH, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));
	}
	else if (p_Evalu.p_InsFea->type == AUXIALITY)
	{
		painter.drawText(DevName_x + Marginal, Dev_Sig + Marginal + RealNameH + TypeH + 1.5 * EachH - UpMv, QString::fromLocal8Bit("同轴度："));
		painter.drawText(DevValue_x + Marginal, Dev_Sig + DevValue_y + Marginal + RealNameH + TypeH + 1.5 * EachH, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));
	}
	else if (p_Evalu.p_InsFea->type == FLATNESS)
	{
		painter.drawText(DevName_x + Marginal, Dev_Sig + Marginal + RealNameH + TypeH + 1.5 * EachH - UpMv, QString::fromLocal8Bit("平面度："));
		painter.drawText(DevValue_x + Marginal, Dev_Sig + DevValue_y + Marginal + RealNameH + TypeH + 1.5 * EachH, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));
	}
	else
	{
		cout << "检测特征类型出错，位于InsFeaUnitPainter_Rep.cpp,165" << endl;
	}
	
	//画逐点误差比较框
	int PntCompW = (Width - UserInfoW) / NbPntperRow;//小框宽度
	int perRowH = PntCompHigh / 5;//逐点比较每行高度
	int DevAll_x = 2;//逐点比较信息相对于所在局部小格区域原点左右向偏移距离
	int DevAll_y = -3;//逐点比较信息相对于所在局部小格区域原点上下向偏移距离
	for (int i = 0; i < p_Evalu.ThoMesPntList.size(); i++)
	{
		//判断该框在第几排第几列，一排NbPntperRow个,行列从零开始
		int nRow = i / NbPntperRow;
		int nCol = i % NbPntperRow;

		//画最外框
		painter.setPen(QPen(Qt::black, 2));
		QPointF OutLines[5] = { QPointF(Marginal + UserInfoW + nCol * PntCompW, Marginal + RealNameH + nRow * PntCompHigh), QPointF(Marginal + UserInfoW + PntCompW + nCol * PntCompW, Marginal + RealNameH + nRow * PntCompHigh), QPointF(Marginal + UserInfoW + PntCompW + nCol * PntCompW, Marginal + RealNameH + PntCompHigh + nRow * PntCompHigh), QPointF(Marginal + UserInfoW + nCol * PntCompW, Marginal + RealNameH + PntCompHigh + nRow * PntCompHigh),QPointF(Marginal + UserInfoW + nCol * PntCompW, Marginal + RealNameH + nRow * PntCompHigh) };
		painter.drawPolyline(OutLines, 5);

		//画小框中横线
		painter.setPen(Qt::black);
		for (int j = 1; j <= 4; j++)
		{
			QPointF pRowLine[2] = { QPointF(Marginal + UserInfoW + nCol * PntCompW, Marginal + RealNameH + nRow * PntCompHigh + j * perRowH), QPointF(Marginal + UserInfoW + PntCompW + nCol * PntCompW, Marginal + RealNameH + nRow * PntCompHigh + j * perRowH) };
			painter.drawPolyline(pRowLine, 2);
		}

		//画小框中竖线，这里的0.3表示对于某一点的信息比较框，点名那一列占30%，剩余两列65%
		QPointF pColLine[2] = { QPointF(Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, Marginal + RealNameH + nRow * PntCompHigh), QPointF(Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, Marginal + RealNameH + PntCompHigh + nRow * PntCompHigh) };
		painter.drawPolyline(pColLine, 2);

		QPointF pColLine2[2] = { QPointF(Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, Marginal + RealNameH + nRow * PntCompHigh), QPointF(Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, Marginal + RealNameH + PntCompHigh + nRow * PntCompHigh - perRowH) };
		painter.drawPolyline(pColLine2, 2);

		//逐点比较框第一列字
		QFont PntCopFont("Microsoft YaHei", PntCompWordH, QFont::Normal, false);
		painter.setFont(PntCopFont);
		painter.setPen(Qt::black);
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + perRowH, QString::fromLocal8Bit("点")+QString::number(i));
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 2 * perRowH, QString::fromLocal8Bit("X"));
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 3 * perRowH, QString::fromLocal8Bit("Y"));
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 4 * perRowH, QString::fromLocal8Bit("Z"));
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 5 * perRowH, QString::fromLocal8Bit("误差"));

		//第二列，这里的0.3表示对于某一点的信息比较框，点名那一列占30%，剩余两列65%
		//painter.setPen(Qt::blue);
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + perRowH, QString::fromLocal8Bit("理论"));
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 2 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].first.X()));
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 3 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].first.Y()));
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 4 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].first.Z()));

		painter.setPen(Qt::black);//第二列最后一排，与第二列其它排不一样，15表示单独为这一列设置的向左偏移量
		painter.drawText(30 + Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 5 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].first.Distance(p_Evalu.ThoMesPntList[i].second)));

		//第三列
		//painter.setPen(Qt::red);
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + perRowH, QString::fromLocal8Bit("实际"));
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 2 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].second.X()));
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 3 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].second.Y()));
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 4 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].second.Z()));

	}

	//写最下面两行评价
	//第一行
	QFont EvaluFont("Microsoft YaHei", 10, QFont::Normal, false);
	painter.setFont(EvaluFont);
	painter.setPen(Qt::black);
	QString EvaluCont;
	if (p_Evalu.p_InsFea->type == DISTANCE)
	{
		EvaluCont = insType + QString::fromLocal8Bit("：") + QString::number(p_Evalu.DimensionValue) + QString::fromLocal8Bit("mm ，偏差：") + QString::number(p_Evalu.Deviation) + QString::fromLocal8Bit("mm");

		//EvaluCont = insType + QString::fromLocal8Bit("：") + QString::number(p_Evalu.DimensionValue) + QString::fromLocal8Bit("mm ，上偏差：+") + QString::number(p_Evalu.upDeviation) + QString::fromLocal8Bit("mm ，下偏差：") + QString::number(p_Evalu.downDeviation) + tr("mm");

	}
	else if (p_Evalu.p_InsFea->type == DIAMETER)
	{
		EvaluCont = QString::fromLocal8Bit("直径：") + QString::number(p_Evalu.DimensionValue) + QString::fromLocal8Bit("mm ，偏差：") + QString::number(p_Evalu.Deviation) + QString::fromLocal8Bit("mm");

		//EvaluCont = QString::fromLocal8Bit("直径：") + QString::number(p_Evalu.DimensionValue) + QString::fromLocal8Bit("mm ，上偏差：+") + QString::number(p_Evalu.upDeviation) + QString::fromLocal8Bit("mm ，下偏差：") + QString::number(p_Evalu.downDeviation) + tr("mm");
	}
	else if (p_Evalu.p_InsFea->type == PARALLELISM)
	{
		EvaluCont = QString::fromLocal8Bit("平行度：") + QString::number(p_Evalu.ToleranceValue) + tr("mm");
	}
	else if (p_Evalu.p_InsFea->type == ROUNDNESS)
	{
		EvaluCont = QString::fromLocal8Bit("圆度：") + QString::number(p_Evalu.ToleranceValue) + tr("mm");
	}
	else if (p_Evalu.p_InsFea->type == VERTICALITY)
	{
		EvaluCont = QString::fromLocal8Bit("垂直度：") + QString::number(p_Evalu.ToleranceValue) + tr("mm");
	}
	else if (p_Evalu.p_InsFea->type == AUXIALITY)
	{
		EvaluCont = QString::fromLocal8Bit("同轴度：") + QString::number(p_Evalu.ToleranceValue) + tr("mm");
	}
	else if (p_Evalu.p_InsFea->type == FLATNESS)
	{
		EvaluCont = QString::fromLocal8Bit("平面度：") + QString::number(p_Evalu.ToleranceValue) + tr("mm");
	}
	else
	{
		cout << "检测特征类型出错，位于InsFeaUnitPainter_Rep.cpp,258" << endl;
	}

	int DevRes_x = 5;//检测结果合格不合格相对于局部区域原点x轴偏移量
	int DevRes_y = -6;//检测结果合格不合格相对于局部区域原点y轴偏移量
	int Inval_Res = 25;//检测结果两排之间间距
	painter.drawText(DevRes_x + UserInfoW + Marginal, Height + Marginal + RealNameH - Inval_Res, EvaluCont);

	//第二行
	painter.drawText(DevRes_x + UserInfoW + Marginal, Height + Marginal + RealNameH + DevRes_y, QString::fromLocal8Bit("检测结果："));

	if (p_Evalu.qualify)
	{
		painter.setPen(Qt::green);
		painter.drawText(DevRes_x + UserInfoW + Marginal + 100, Height + Marginal + RealNameH + DevRes_y, QString::fromLocal8Bit("合格"));
	}
	else
	{
		painter.setPen(Qt::red);
		painter.drawText(DevRes_x + UserInfoW + Marginal + 100, Height + Marginal + RealNameH + DevRes_y, QString::fromLocal8Bit("不合格"));
	}
}



