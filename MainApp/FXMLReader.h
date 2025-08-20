#ifndef _FXMLREADER_H_
#define	_FXMLREADER_H_

#include <QVector>
#include <QStringList>
#include <QThread>
#include "FGuideDataType.h"
#include "MainDoc.h"

class	QDomNode;

class FXMLReader: public QThread
{
//	Q_OBJECT
public:
	explicit	FXMLReader(FMainDoc* pDoc);
	void		load(QString path);
protected:
	void		run();
private:
	//void		queryXMLSampleFile();
	void		parsePointData(QString pointData);
	void		parsePathData( QString pathData );
	//void		parseFeatureNode( QDomNode& node, FIpFeature** feature);
public:
	QString				m_path;			//文件地址路径
	//存放读入的数据
	FIpPntList			m_pointList;	//点表
	QStringList			m_programList;	//代码表
	FIpPathList			m_pathList;		//路径表
private:
	FMainDoc*		m_pDoc;
};

#endif