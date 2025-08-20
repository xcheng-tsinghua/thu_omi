#include "ErrCompsteDlg.h"
#include <qicon.h>
#include "CompSurfTab.h"
#include "CompCylinTab.h"
#include "CompWThickTab.h"

ErrCompsteDlg::ErrCompsteDlg(QWidget* parent) : QTabWidget(parent)
{
	// ��������
	setWindowIcon(QIcon("../icons/ErrorComp.png"));
	setGeometry(1350, 120, 500, 500);
	setWindowTitle(QString::fromLocal8Bit("����"));
	setWindowFlags(Qt::WindowStaysOnTopHint);

	m_pCompSurfTab = new CompSurfTab;
	m_pCompCylinTab = new CompCylinTab;
	m_pCompWThickTab = new CompWThickTab;

	addTab(m_pCompSurfTab, QString::fromLocal8Bit("װ�в���"));
	addTab(m_pCompCylinTab, QString::fromLocal8Bit("��λ����"));
	addTab(m_pCompWThickTab, QString::fromLocal8Bit("�ں񲹳�"));

}

ErrCompsteDlg::~ErrCompsteDlg(void)
{
	delete m_pCompSurfTab;
	delete m_pCompCylinTab;
	delete m_pCompWThickTab;

}
