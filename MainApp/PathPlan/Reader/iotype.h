#ifndef _IOTYPE_H
#define _IOTYPE_H
/* 特征单元  Unit
 * int Id: 序号
 * int Size: 单元中的点的个数
 * char *UnitName: 该单元的名字
 * Unit *Suc: 上一个单元的指针
 * Unit *Pred: 下一个单元的指针
 * Node **Elements: 所有点的数组
 * int V: 标记变量
*/
typedef struct IOUnit IOUnit;

/* 测点节点 Node
* int Id: 序号
* Unit *BelongTo: 指向所属的指针
* Unit *Suc: 上一个单元的指针
* Unit *Pred: 下一个单元的指针
* double X: X坐标
* double Y: Y坐标
* double Z: Z坐标
* int V: 标记变量
*/
typedef struct IONode IONode;

struct IOUnit {
	/* int Id: 序号*/
	int Id;
	/* int Size: 单元中的点的个数*/
	int Size;
	/* char *UnitName: 该单元的名字*/
	char UnitName[20];
	/* Unit *Suc: 上一个单元的指针*/
	IOUnit *Suc;
	/* Unit *Pred: 下一个单元的指针*/
	IOUnit *Pred;
	/* Node **Elements: 所有点的数组*/
	//Node **Elements;
	/* int V: 标记变量*/
	int V;
};

struct IONode {
	/* 编号*/
	int Id;
    /* 所属特征的Id */
    int BelongId;
	/* 指向所属的特征的指针 */
	IOUnit *BelongTo;
	/* 上一个节点 */
	IONode *Suc;
	/* 下一个节点 */
	IONode *Pred;
	/*X 坐标*/
	double X;
	/*Y 坐标*/
	double Y;
	/*Z 坐标*/
	double Z;
    /*法向量 X*/
    double nX;
    /*法向量 Y*/
    double nY;
    /*法向量 Z*/
    double nZ;
    /*回退点 X坐标*/
    double aX;
    /*回退点 Y坐标*/
    double aY;
    /*回退点 Z坐标*/
    double aZ;
	/*标记变量，用于在循环中标识已经遍历过了，避免重复*/
	int V;
    /*用于标记是否是测点*/
    int isIP;
};

#endif