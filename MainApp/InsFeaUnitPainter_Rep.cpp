#include "InsFeaUnitPainter_Rep.h"

InsFeaUnitPainter_Rep::InsFeaUnitPainter_Rep(EvaluData Evalu, QWidget* parent):QWidget(parent)
{
	//�ڿ��ܿ��
	Width = 1213;//ԭʼ��ģ�ͽ�ͼ��Ϊ750

	//��Ե���
	Marginal = 5;

	//���Ϸ�����������߶�
	InsFeaNameH = 15;//��������ʱ�߶�
	RealNameH = 40;//ʵ���ָ�

	//���Ƚ��ֺ�
	PntCompWordH = 8;

	//�����������ڸ߶�
	OvewHigh = 45;

	//�趨ÿһ�����Ƚϸ߶�
	PntCompHigh = 85;

	//����û�������Ϣ�����
	UserInfoW = 65;

	//������͸߶�
	TypeH = 25;

	//ÿ�����Ƚϵ���
	NbPntperRow = 7;

	//�����м������Ƚϣ�һ��NbPntperRow��
	NbRow = Evalu.ThoMesPntList.size() / NbPntperRow;
	if (Evalu.ThoMesPntList.size() % NbPntperRow != 0)
	{
		NbRow += 1;
	}

	//����ڿ��ܸ߶ȣ��ܸ߶ȵ������Ƚϸ߶ȼ����������߶�
	Height = NbRow * PntCompHigh + OvewHigh;

	setFixedSize(Width + 2 * Marginal, Height + 2 * Marginal + RealNameH); //�̶���С

	p_Evalu = Evalu;
}

InsFeaUnitPainter_Rep::~InsFeaUnitPainter_Rep()
{

}

void InsFeaUnitPainter_Rep::paintEvent(QPaintEvent*)
{
	QPainter painter(this);// ����QPainterһ������
	QPen pen;

	//���������
	QFont InsFeaNameFont("����", InsFeaNameH, QFont::Normal, false);
	painter.setFont(InsFeaNameFont);
	painter.setPen(Qt::black);
	painter.drawText(5, RealNameH, QString::fromLocal8Bit("�������")+QString::number(p_Evalu.p_InsFea->InsFeatureindex));

	//�����
	painter.setPen(QPen(Qt::black, 2));
	QPointF points1[5] = { QPointF(Marginal, Marginal + RealNameH), QPointF(Width + Marginal, Marginal + RealNameH), QPointF(Width + Marginal, Height + Marginal + RealNameH), QPointF(Marginal, Height + Marginal + RealNameH),QPointF(Marginal, Marginal + RealNameH) };
	painter.drawPolyline(points1, 5);

	//������û�������Ϣ��
	QPointF UserinfoLine1[2] = { QPointF(UserInfoW + Marginal, Marginal + RealNameH), QPointF(UserInfoW + Marginal, Height + Marginal + RealNameH) };
	painter.drawPolyline(UserinfoLine1, 2);//������

	//����Ǽ�⹫����û��ڶ��������ߣ����ߴ����Ҫ
	painter.setPen(Qt::black);
	QPointF UserinfoLine2[2] = { QPointF(Marginal, Marginal + RealNameH + TypeH), QPointF(Marginal + UserInfoW, Marginal + RealNameH + TypeH) };
	painter.drawPolyline(UserinfoLine2, 2);

	int EachH = (NbRow * PntCompHigh + OvewHigh - TypeH) / 3;//�����߳���һ����ÿ���и�
	if (p_Evalu.p_InsFea->type == DISTANCE || p_Evalu.p_InsFea->type == DIAMETER)
	{
		QPointF UserinfoLine3[2] = { QPointF(Marginal, Marginal + RealNameH + TypeH + EachH), QPointF(Marginal + UserInfoW, Marginal + RealNameH + TypeH + EachH) };
		painter.drawPolyline(UserinfoLine3, 2);

		QPointF UserinfoLine4[2] = { QPointF(Marginal, Marginal + RealNameH + TypeH + 2 * EachH), QPointF(Marginal + UserInfoW, Marginal + RealNameH + TypeH + 2 * EachH) };
		painter.drawPolyline(UserinfoLine4, 2);
	}

	//�����·�����������
	QPointF OvewLine[2] = { QPointF(Marginal + UserInfoW, Height + Marginal + RealNameH - OvewHigh), QPointF(Width + Marginal, Height + Marginal + RealNameH - OvewHigh) };
	painter.drawPolyline(OvewLine, 2);

	//д����û�������Ϣ
	//д��һ�м������
	QFont InsTypeFont("Microsoft YaHei", PntCompWordH * 1.4, QFont::Normal, false);
	painter.setFont(InsTypeFont);
	painter.setPen(Qt::black);
	QString insType;
	if (p_Evalu.p_InsFea->type == PARALLELISM)
	{
		insType = QString::fromLocal8Bit("ƽ�ж�");
	}
	else if (p_Evalu.p_InsFea->type == ROUNDNESS)
	{
		insType = QString::fromLocal8Bit("Բ��");
	}
	else if (p_Evalu.p_InsFea->type == DISTANCE)
	{
		if (p_Evalu.p_InsFea->DirState == 1)
		{
			insType = QString::fromLocal8Bit("X�����");
		}
		else if (p_Evalu.p_InsFea->DirState == 2)
		{
			insType = QString::fromLocal8Bit("Y�����");
		}
		else if (p_Evalu.p_InsFea->DirState == 3)
		{
			insType = QString::fromLocal8Bit("Z�����");
		}
		else
		{
			insType = QString::fromLocal8Bit("����");
		}
	}
	else if (p_Evalu.p_InsFea->type == VERTICALITY)
	{
		insType = QString::fromLocal8Bit("��ֱ��");
	}
	else if (p_Evalu.p_InsFea->type == DIAMETER)
	{
		insType = QString::fromLocal8Bit("ֱ��");
	}
	else if (p_Evalu.p_InsFea->type == AUXIALITY)
	{
		insType = QString::fromLocal8Bit("ͬ���");
	}
	else if (p_Evalu.p_InsFea->type == FLATNESS)
	{
		insType = QString::fromLocal8Bit("ƽ���");
	}
	else
	{
		insType = QString::fromLocal8Bit("δ֪");
	}
	painter.drawText(Marginal + 2, Marginal + RealNameH + TypeH - 5, insType);

	//д�ڶ����û�����
	QFont InsTypeFontUser("Microsoft YaHei", PntCompWordH * 1.2, QFont::Normal, false);
	painter.setFont(InsTypeFontUser);
	painter.setPen(Qt::black);

	int DevName_x = 4;//����û�������Ϣ��������ƫ�ƾ���
	int DevValue_x = 2;//����û�������Ϣ��������ƫ�ƾ���
	int DevValue_y = -3;//����û�������Ϣ��������ƫ�ƾ���
	int UpMv = 17;//����û����������������ּ��
	int Dev_Sig = 10;//���ֻ��һ��ʱ����������ƫ����
	int Dev_Tri = -2;

	if (p_Evalu.p_InsFea->type == DISTANCE)
	{
		painter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + EachH - UpMv + Dev_Tri, insType + QString::fromLocal8Bit("��"));
		painter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + EachH + Dev_Tri, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));

		painter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + 2 * EachH - UpMv + Dev_Tri, QString::fromLocal8Bit("��ƫ�"));
		painter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + 2 * EachH + Dev_Tri, tr("+") + QString::number(p_Evalu.p_InsFea->FInsUpDeviationValue) + tr("mm"));

		painter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + 3 * EachH - UpMv + Dev_Tri, QString::fromLocal8Bit("��ƫ�"));
		painter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + 3 * EachH + Dev_Tri, tr("-") + QString::number(p_Evalu.p_InsFea->FInsDownDeviationValue) + tr("mm"));
	}
	else if (p_Evalu.p_InsFea->type == DIAMETER)
	{
		painter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + EachH - UpMv + Dev_Tri, QString::fromLocal8Bit("ֱ����"));
		painter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + EachH + Dev_Tri, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));

		painter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + 2 * EachH - UpMv + Dev_Tri, QString::fromLocal8Bit("��ƫ�"));
		painter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + 2 * EachH + Dev_Tri, tr("+") + QString::number(p_Evalu.p_InsFea->FInsUpDeviationValue) + tr("mm"));

		painter.drawText(DevName_x + Marginal, Marginal + RealNameH + TypeH + 3 * EachH - UpMv + Dev_Tri, QString::fromLocal8Bit("��ƫ�"));
		painter.drawText(DevValue_x + Marginal, DevValue_y + Marginal + RealNameH + TypeH + 3 * EachH + Dev_Tri, tr("-") + QString::number(p_Evalu.p_InsFea->FInsDownDeviationValue) + tr("mm"));
	}
	else if (p_Evalu.p_InsFea->type == PARALLELISM)
	{
		painter.drawText(DevName_x + Marginal, Dev_Sig + Marginal + RealNameH + TypeH + 1.5 * EachH - UpMv, QString::fromLocal8Bit("ƽ�жȣ�"));
		painter.drawText(DevValue_x + Marginal, Dev_Sig + DevValue_y + Marginal + RealNameH + TypeH + 1.5 * EachH, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));
	}
	else if (p_Evalu.p_InsFea->type == ROUNDNESS)
	{
		painter.drawText(DevName_x + Marginal, Dev_Sig + Marginal + RealNameH + TypeH + 1.5 * EachH - UpMv, QString::fromLocal8Bit("Բ�ȣ�"));
		painter.drawText(DevValue_x + Marginal, Dev_Sig + DevValue_y + Marginal + RealNameH + TypeH + 1.5 * EachH, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));
	}
	else if (p_Evalu.p_InsFea->type == VERTICALITY)
	{
		painter.drawText(DevName_x + Marginal, Dev_Sig + Marginal + RealNameH + TypeH + 1.5 * EachH - UpMv, QString::fromLocal8Bit("��ֱ�ȣ�"));
		painter.drawText(DevValue_x + Marginal, Dev_Sig + DevValue_y + Marginal + RealNameH + TypeH + 1.5 * EachH, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));
	}
	else if (p_Evalu.p_InsFea->type == AUXIALITY)
	{
		painter.drawText(DevName_x + Marginal, Dev_Sig + Marginal + RealNameH + TypeH + 1.5 * EachH - UpMv, QString::fromLocal8Bit("ͬ��ȣ�"));
		painter.drawText(DevValue_x + Marginal, Dev_Sig + DevValue_y + Marginal + RealNameH + TypeH + 1.5 * EachH, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));
	}
	else if (p_Evalu.p_InsFea->type == FLATNESS)
	{
		painter.drawText(DevName_x + Marginal, Dev_Sig + Marginal + RealNameH + TypeH + 1.5 * EachH - UpMv, QString::fromLocal8Bit("ƽ��ȣ�"));
		painter.drawText(DevValue_x + Marginal, Dev_Sig + DevValue_y + Marginal + RealNameH + TypeH + 1.5 * EachH, QString::number(p_Evalu.p_InsFea->FInsToleranceValue) + tr("mm"));
	}
	else
	{
		cout << "����������ͳ���λ��InsFeaUnitPainter_Rep.cpp,165" << endl;
	}
	
	//��������ȽϿ�
	int PntCompW = (Width - UserInfoW) / NbPntperRow;//С����
	int perRowH = PntCompHigh / 5;//���Ƚ�ÿ�и߶�
	int DevAll_x = 2;//���Ƚ���Ϣ��������ھֲ�С������ԭ��������ƫ�ƾ���
	int DevAll_y = -3;//���Ƚ���Ϣ��������ھֲ�С������ԭ��������ƫ�ƾ���
	for (int i = 0; i < p_Evalu.ThoMesPntList.size(); i++)
	{
		//�жϸÿ��ڵڼ��ŵڼ��У�һ��NbPntperRow��,���д��㿪ʼ
		int nRow = i / NbPntperRow;
		int nCol = i % NbPntperRow;

		//�������
		painter.setPen(QPen(Qt::black, 2));
		QPointF OutLines[5] = { QPointF(Marginal + UserInfoW + nCol * PntCompW, Marginal + RealNameH + nRow * PntCompHigh), QPointF(Marginal + UserInfoW + PntCompW + nCol * PntCompW, Marginal + RealNameH + nRow * PntCompHigh), QPointF(Marginal + UserInfoW + PntCompW + nCol * PntCompW, Marginal + RealNameH + PntCompHigh + nRow * PntCompHigh), QPointF(Marginal + UserInfoW + nCol * PntCompW, Marginal + RealNameH + PntCompHigh + nRow * PntCompHigh),QPointF(Marginal + UserInfoW + nCol * PntCompW, Marginal + RealNameH + nRow * PntCompHigh) };
		painter.drawPolyline(OutLines, 5);

		//��С���к���
		painter.setPen(Qt::black);
		for (int j = 1; j <= 4; j++)
		{
			QPointF pRowLine[2] = { QPointF(Marginal + UserInfoW + nCol * PntCompW, Marginal + RealNameH + nRow * PntCompHigh + j * perRowH), QPointF(Marginal + UserInfoW + PntCompW + nCol * PntCompW, Marginal + RealNameH + nRow * PntCompHigh + j * perRowH) };
			painter.drawPolyline(pRowLine, 2);
		}

		//��С�������ߣ������0.3��ʾ����ĳһ�����Ϣ�ȽϿ򣬵�����һ��ռ30%��ʣ������65%
		QPointF pColLine[2] = { QPointF(Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, Marginal + RealNameH + nRow * PntCompHigh), QPointF(Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, Marginal + RealNameH + PntCompHigh + nRow * PntCompHigh) };
		painter.drawPolyline(pColLine, 2);

		QPointF pColLine2[2] = { QPointF(Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, Marginal + RealNameH + nRow * PntCompHigh), QPointF(Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, Marginal + RealNameH + PntCompHigh + nRow * PntCompHigh - perRowH) };
		painter.drawPolyline(pColLine2, 2);

		//���ȽϿ��һ����
		QFont PntCopFont("Microsoft YaHei", PntCompWordH, QFont::Normal, false);
		painter.setFont(PntCopFont);
		painter.setPen(Qt::black);
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + perRowH, QString::fromLocal8Bit("��")+QString::number(i));
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 2 * perRowH, QString::fromLocal8Bit("X"));
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 3 * perRowH, QString::fromLocal8Bit("Y"));
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 4 * perRowH, QString::fromLocal8Bit("Z"));
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 5 * perRowH, QString::fromLocal8Bit("���"));

		//�ڶ��У������0.3��ʾ����ĳһ�����Ϣ�ȽϿ򣬵�����һ��ռ30%��ʣ������65%
		//painter.setPen(Qt::blue);
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + perRowH, QString::fromLocal8Bit("����"));
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 2 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].first.X()));
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 3 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].first.Y()));
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 4 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].first.Z()));

		painter.setPen(Qt::black);//�ڶ������һ�ţ���ڶ��������Ų�һ����15��ʾ����Ϊ��һ�����õ�����ƫ����
		painter.drawText(30 + Marginal + UserInfoW + nCol * PntCompW + 0.3 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 5 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].first.Distance(p_Evalu.ThoMesPntList[i].second)));

		//������
		//painter.setPen(Qt::red);
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + perRowH, QString::fromLocal8Bit("ʵ��"));
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 2 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].second.X()));
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 3 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].second.Y()));
		painter.drawText(DevAll_x + Marginal + UserInfoW + nCol * PntCompW + 0.65 * PntCompW, DevAll_y + Marginal + RealNameH + nRow * PntCompHigh + 4 * perRowH, QString::number(p_Evalu.ThoMesPntList[i].second.Z()));

	}

	//д��������������
	//��һ��
	QFont EvaluFont("Microsoft YaHei", 10, QFont::Normal, false);
	painter.setFont(EvaluFont);
	painter.setPen(Qt::black);
	QString EvaluCont;
	if (p_Evalu.p_InsFea->type == DISTANCE)
	{
		EvaluCont = insType + QString::fromLocal8Bit("��") + QString::number(p_Evalu.DimensionValue) + QString::fromLocal8Bit("mm ��ƫ�") + QString::number(p_Evalu.Deviation) + QString::fromLocal8Bit("mm");

		//EvaluCont = insType + QString::fromLocal8Bit("��") + QString::number(p_Evalu.DimensionValue) + QString::fromLocal8Bit("mm ����ƫ�+") + QString::number(p_Evalu.upDeviation) + QString::fromLocal8Bit("mm ����ƫ�") + QString::number(p_Evalu.downDeviation) + tr("mm");

	}
	else if (p_Evalu.p_InsFea->type == DIAMETER)
	{
		EvaluCont = QString::fromLocal8Bit("ֱ����") + QString::number(p_Evalu.DimensionValue) + QString::fromLocal8Bit("mm ��ƫ�") + QString::number(p_Evalu.Deviation) + QString::fromLocal8Bit("mm");

		//EvaluCont = QString::fromLocal8Bit("ֱ����") + QString::number(p_Evalu.DimensionValue) + QString::fromLocal8Bit("mm ����ƫ�+") + QString::number(p_Evalu.upDeviation) + QString::fromLocal8Bit("mm ����ƫ�") + QString::number(p_Evalu.downDeviation) + tr("mm");
	}
	else if (p_Evalu.p_InsFea->type == PARALLELISM)
	{
		EvaluCont = QString::fromLocal8Bit("ƽ�жȣ�") + QString::number(p_Evalu.ToleranceValue) + tr("mm");
	}
	else if (p_Evalu.p_InsFea->type == ROUNDNESS)
	{
		EvaluCont = QString::fromLocal8Bit("Բ�ȣ�") + QString::number(p_Evalu.ToleranceValue) + tr("mm");
	}
	else if (p_Evalu.p_InsFea->type == VERTICALITY)
	{
		EvaluCont = QString::fromLocal8Bit("��ֱ�ȣ�") + QString::number(p_Evalu.ToleranceValue) + tr("mm");
	}
	else if (p_Evalu.p_InsFea->type == AUXIALITY)
	{
		EvaluCont = QString::fromLocal8Bit("ͬ��ȣ�") + QString::number(p_Evalu.ToleranceValue) + tr("mm");
	}
	else if (p_Evalu.p_InsFea->type == FLATNESS)
	{
		EvaluCont = QString::fromLocal8Bit("ƽ��ȣ�") + QString::number(p_Evalu.ToleranceValue) + tr("mm");
	}
	else
	{
		cout << "����������ͳ���λ��InsFeaUnitPainter_Rep.cpp,258" << endl;
	}

	int DevRes_x = 5;//������ϸ񲻺ϸ�����ھֲ�����ԭ��x��ƫ����
	int DevRes_y = -6;//������ϸ񲻺ϸ�����ھֲ�����ԭ��y��ƫ����
	int Inval_Res = 25;//���������֮����
	painter.drawText(DevRes_x + UserInfoW + Marginal, Height + Marginal + RealNameH - Inval_Res, EvaluCont);

	//�ڶ���
	painter.drawText(DevRes_x + UserInfoW + Marginal, Height + Marginal + RealNameH + DevRes_y, QString::fromLocal8Bit("�������"));

	if (p_Evalu.qualify)
	{
		painter.setPen(Qt::green);
		painter.drawText(DevRes_x + UserInfoW + Marginal + 100, Height + Marginal + RealNameH + DevRes_y, QString::fromLocal8Bit("�ϸ�"));
	}
	else
	{
		painter.setPen(Qt::red);
		painter.drawText(DevRes_x + UserInfoW + Marginal + 100, Height + Marginal + RealNameH + DevRes_y, QString::fromLocal8Bit("���ϸ�"));
	}
}



