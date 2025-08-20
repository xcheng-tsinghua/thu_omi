#ifndef _IOTYPE_H
#define _IOTYPE_H
/* ������Ԫ  Unit
 * int Id: ���
 * int Size: ��Ԫ�еĵ�ĸ���
 * char *UnitName: �õ�Ԫ������
 * Unit *Suc: ��һ����Ԫ��ָ��
 * Unit *Pred: ��һ����Ԫ��ָ��
 * Node **Elements: ���е������
 * int V: ��Ǳ���
*/
typedef struct IOUnit IOUnit;

/* ���ڵ� Node
* int Id: ���
* Unit *BelongTo: ָ��������ָ��
* Unit *Suc: ��һ����Ԫ��ָ��
* Unit *Pred: ��һ����Ԫ��ָ��
* double X: X����
* double Y: Y����
* double Z: Z����
* int V: ��Ǳ���
*/
typedef struct IONode IONode;

struct IOUnit {
	/* int Id: ���*/
	int Id;
	/* int Size: ��Ԫ�еĵ�ĸ���*/
	int Size;
	/* char *UnitName: �õ�Ԫ������*/
	char UnitName[20];
	/* Unit *Suc: ��һ����Ԫ��ָ��*/
	IOUnit *Suc;
	/* Unit *Pred: ��һ����Ԫ��ָ��*/
	IOUnit *Pred;
	/* Node **Elements: ���е������*/
	//Node **Elements;
	/* int V: ��Ǳ���*/
	int V;
};

struct IONode {
	/* ���*/
	int Id;
    /* ����������Id */
    int BelongId;
	/* ָ��������������ָ�� */
	IOUnit *BelongTo;
	/* ��һ���ڵ� */
	IONode *Suc;
	/* ��һ���ڵ� */
	IONode *Pred;
	/*X ����*/
	double X;
	/*Y ����*/
	double Y;
	/*Z ����*/
	double Z;
    /*������ X*/
    double nX;
    /*������ Y*/
    double nY;
    /*������ Z*/
    double nZ;
    /*���˵� X����*/
    double aX;
    /*���˵� Y����*/
    double aY;
    /*���˵� Z����*/
    double aZ;
	/*��Ǳ�����������ѭ���б�ʶ�Ѿ��������ˣ������ظ�*/
	int V;
    /*���ڱ���Ƿ��ǲ��*/
    int isIP;
};

#endif