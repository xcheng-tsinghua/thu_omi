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
	QString				m_path;			//�ļ���ַ·��
	//��Ŷ��������
	FIpPntList			m_pointList;	//���
	QStringList			m_programList;	//�����
	FIpPathList			m_pathList;		//·����
private:
	FMainDoc*		m_pDoc;
};

#endif