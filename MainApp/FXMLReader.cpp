#include "FXMLReader.h"
#include <iostream>
#include <QFile>
#include <QDomDocument>
#include "QTextCodec"
#include "QStringList"


using namespace std;

FXMLReader::FXMLReader( FMainDoc* pDoc)
	:m_pDoc(pDoc)
{
}

void FXMLReader::run()
{
	//std::cout	<<	"FXMLReader running...\n";
	//std::cout	<<	"FXMLReader thread:"	<<	QThread::currentThreadId()	<<	std::endl;
	//queryXMLSampleFile();
	//deleteLater();
}

void FXMLReader::load( QString path )
{
	m_path	=	path;
	start();
}

//void FXMLReader::queryXMLSampleFile()
//{
//	//std::cout	<<	"queryXMLSampleFile thread:"	<<	QThread::currentThreadId()	<<	std::endl;
//	//打开XML文件，并将其读入Dom中
//	QFile file(m_path);
//	if(!file.open(QFile::ReadOnly))
//	{
//		cout	<<	"fail to open file:"
//			<<	file.fileName().toStdString()	<<	endl;
//		return;
//	}
//	QDomDocument doc;
//	if(!doc.setContent(&file))
//	{
//		cout	<<	"fail to create document..."	<<	endl;
//		file.close();
//		return;
//	}
//	file.close();
//
//	//读取点的数据
//	QDomNodeList	nodeList = doc.elementsByTagName(QString::fromLocal8Bit("点"));
//	QDomNode		node;
//	QDomElement		ele;
//	QString			textStr;
//	for(int i=0;i<nodeList.size();i++)
//	{
//		node    =   nodeList.at(i);
//		if(node.isElement())
//		{
//			ele =   node.toElement();
//			textStr =   ele.toElement().text();
//			parsePointData(textStr);
//		}
//		//cout	<<	"点："	<<	textStr.toStdString()	<<	endl;
//	}
//
//	nodeList	=	doc.elementsByTagName(QString::fromLocal8Bit("点序号"));
//	for(int i=0;i<nodeList.size();i++)
//	{
//		node    =   nodeList.at(i);
//		if(node.isElement())
//		{
//			ele =   node.toElement();
//			textStr =   ele.toElement().text();
//			m_pointList[i].num	=	textStr.toInt();
//		}
//		//cout	<<	"点："	<<	textStr.toStdString()	<<	endl;
//	}
//	//将点的数据设置到当前文档中
//	m_pDoc->SetIpPntList(m_pointList);
//
//	/*QFile file1("E:\\src\\mypath.txt");
//	if(file1.open(QFile::WriteOnly|QFile::Truncate|QIODevice::Text))
//	{
//	QTextStream out(&file1);
//
//	for(int i = 0; i < m_pointList.size(); i++)
//	{
//	out <<  i <<  "\t"
//	<<  m_pointList[i].x <<  "\t"
//	<<  m_pointList[i].y <<  "\t"
//	<<  m_pointList[i].z <<  "\r\n";
//	}
//	file1.close();
//	}*/
//
//
//	//读取程序指令数据
//	nodeList	=	doc.elementsByTagName(QString::fromLocal8Bit("指令"));
//	for(int i=0;i<nodeList.size();i++)
//	{
//		node    =   nodeList.at(i);
//		if(node.isElement())
//		{
//			ele =   node.toElement();
//			textStr =   ele.toElement().text();
//			m_programList	<<	textStr;
//		}
//	}
//	/*for (int i=0;i<m_programList.size();i++)
//	{
//	cout	<<	"指令"	<<	i+1	<<
//	"："	<<	m_programList[i].toStdString()	<<	endl;
//	}*/
//
//	//读取测头运动轨迹信息
//	nodeList	=	doc.elementsByTagName(QString::fromLocal8Bit("线"));
//	for(int i=0;i<nodeList.size();i++)
//	{
//		node    =   nodeList.at(i);
//		if(node.isElement())
//		{
//			ele =   node.toElement();
//			textStr =   ele.toElement().text();
//			parsePathData(textStr);
//		}
//	}
//	m_pDoc->SetPathList(m_pathList);
//
//	//读取检测特征信息
//	QMap<int,QVector<FIpFeature*> >	featMap;
//	nodeList	=	doc.elementsByTagName(QString::fromLocal8Bit("检测特征"));
//	//int s = nodeList.size();
//	int globalId = 1;
//	for(int i=0;i<nodeList.size();i++)
//	{
//		FIpFeature*	feature = new FIpFeature; 
//		feature->SetGlobalID(globalId);
//		parseFeatureNode(nodeList.at(i), &feature);
//		/*for (int j = 0; j < feature.pntNumList.size();j++)
//		{
//			cout	<<	feature.pntNumList[j] << endl;
//		}*/
//
//		if (!featMap.contains(feature->GetType()))
//		{
//			featMap.insert(feature->GetType(), QVector<FIpFeature*>());
//		}
//		featMap[feature->GetType()].push_back(feature);
//		globalId++;
//	}
//	m_pDoc->setFeatureMap(featMap);
//
//}

//void FXMLReader::parseFeatureNode( QDomNode& node, FIpFeature** feature)
//{
//	QDomElement		ele;
//	ele	=	node.toElement();
//	QDomNodeList tmpList	=	ele.elementsByTagName(QString::fromLocal8Bit("标识"));
//	QString	tagText	=	tmpList.at(0).toElement().text();
//	(*feature)->SetTypeID(tagText.mid(0, 4).toInt());
//	(*feature)->SetType((*feature)->QString2TYPE(tagText.mid(4)));
//	//cout << tagText.toStdString() << endl;
//
//	tmpList	=	ele.elementsByTagName(QString::fromLocal8Bit("检测点"));
//	QString	numText	=	tmpList.at(0).toElement().text();
//	numText	=	numText.split(";")[0];
//	QStringList numList = numText.split(",");
//	for (int i = 0; i < numList.size(); i++)
//	{
//		(*feature)->m_pntNumList.push_back(numList[i].toInt());
//		(*feature)->m_selectedPntList.push_back(numList[i].toInt());
//	}
//
//	//根据检测类型的种类，将feature转化为特定的检测特征类
//	if ((*feature)->GetType() == FIpFeature::FN)
//	{
//		FPlaneFt* pPlaneFt = new FPlaneFt(**feature);
//		delete *feature;
//		*feature = pPlaneFt;
//		return;
//	}
//
//	if ((*feature)->GetType() == FIpFeature::PR)
//	{
//		FProfileFt* pProfileFt = new FProfileFt(**feature);
//		delete *feature;
//		*feature = pProfileFt;
//		return;
//	}
//
//	if ((*feature)->GetType() == FIpFeature::CY)
//	{
//		FCylindrFt* pCylindrFt = new FCylindrFt(**feature);
//		delete *feature;
//		*feature = pCylindrFt;
//		return;
//	}
//
//	//cout << numText.toStdString() << endl;
//}


void FXMLReader::parsePointData( QString pointData )
{
	FIpPoint point;
	QStringList	list	=	pointData.split(",");
	if (list.size()	==	7)
	{
		point.x	=	list[0].toDouble();
		point.y	=	list[1].toDouble();
		point.z	=	list[2].toDouble();
		point.i	=	list[3].toDouble();
		point.j	=	list[4].toDouble();
		point.k	=	list[5].toDouble();
		point.symbol	=	list[6].toInt();
	}
	m_pointList.push_back(point);
}

void FXMLReader::parsePathData( QString pathData )
{
	QStringList	list	=	pathData.trimmed().split(";");
	FIpPath thisPath;
	double xx(0), yy(0), zz(0);
	for ( int i = 0; i < list.size()-1; i++ )
	{
		QStringList	subList = list[i].split(",");
		double	x = subList[0].toDouble();
		xx	=	subList[0].toDouble();
		yy	=	subList[1].toDouble();
		zz	=	subList[2].toDouble();
		thisPath.AddPnt(FIpPoint(xx, yy, zz));
	}
	if (thisPath.GetSize() != 0)
		m_pathList.push_back(thisPath);
}





