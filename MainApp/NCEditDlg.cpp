#include "NCEditDlg.h"
#include <qlayout.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <qsettings.h>
#include <qfiledialog.h>
#include <qtextstream.h>
#include "np.h"

extern QSettings* gDlgSetting;

NCEditDlg::NCEditDlg(const QString& fpContent, const QString& fpSavePathDef, QWidget* parent) :QWidget(parent)
{
	// 初始化变量
	m_Content = fpContent;
	m_SavePathDef = fpSavePathDef;

	InitWidget();

}

NCEditDlg::NCEditDlg()
{
	InitWidget();
}

NCEditDlg::~NCEditDlg()
{

}

void NCEditDlg::setContent(const QString& fpContent)
{
	m_Content = fpContent;

	m_pTextEdit->clear();
	m_pTextEdit->setText(m_Content);

}

void NCEditDlg::setSavePathDef(const QString& fpSavePathDef)
{
	m_SavePathDef = fpSavePathDef;

}

void NCEditDlg::onSaveExit()
{
	if (m_SavePathDef.isEmpty())
	{
		MesgBox(QString::fromLocal8Bit("未设置默认保存路径，未能实现默认保存！"));
		return;
	}
	else
	{
		m_Content = m_pTextEdit->toPlainText();

		SaveStrToFile(m_Content, m_SavePathDef);
	}

	//delete m_pTextEdit;
	this->close();

}

void NCEditDlg::closeEvent(QCloseEvent* event)
{
	onSaveExit();

	QWidget::closeEvent(event);
}

void NCEditDlg::InitWidget()
{
	// 窗口
	this->setGeometry(950, 50, 900, 960);
	this->setWindowFlags(Qt::WindowStaysOnTopHint);
	this->setWindowTitle(QString::fromLocal8Bit("NC 编辑器"));
	this->setWindowIcon(QIcon("../icons/NCEdit.png"));

	QVBoxLayout* pMainLayout = new QVBoxLayout(this);
	QPushButton* pSaveExitBtn = new QPushButton(QString::fromLocal8Bit("保存退出"));
	QHBoxLayout* pBntLayout = new QHBoxLayout;
	pBntLayout->addStretch();
	pBntLayout->addWidget(pSaveExitBtn);

	m_pTextEdit = new QTextEdit;
	QFont font;
	font.setFamily("Calibri Light");
	font.setPointSize(10);
	font.setItalic(true);
	m_pTextEdit->setFont(font);
	pMainLayout->addWidget(m_pTextEdit);
	pMainLayout->addLayout(pBntLayout);

	m_pTextEdit->setText(m_Content);

	//connect(pSaveBtnSel, SIGNAL(clicked()), this, SLOT(onSaveSel()));
	//connect(pSaveBtnDef, SIGNAL(clicked()), this, SLOT(onSaveDef()));
	connect(pSaveExitBtn, SIGNAL(clicked()), this, SLOT(onSaveExit()));

	//滚动条
	m_pTextEdit->setStyleSheet(
		"QWidget"
		"{"
		"border: 1px solid rgb(232,232,236);"
		"background:rgb(247,249,254);"
		"}"
		"QScrollBar:vertical"
		"{"
		"width:22px;"
		"background:rgba(0,0,0,0%);"
		"margin:0px,0px,0px,0px;"
		"padding-top:18px;"
		"padding-bottom:18px;"
		"padding-right:5px;"
		"padding-left:5px;"
		"background:rgba(232,232,236,100%);"
		"}"
		// 滚动条两端变成椭圆
		"QScrollBar::handle:vertical"
		"{"
		"width:8px;"
		"background:rgba(0,0,0,25%);"
		"min-height:20;"
		"max-width:10;"
		"}"
		// 鼠标放到滚动条上的时候，颜色变深
		"QScrollBar::handle:vertical:hover"
		"{"
		"width:8px;"
		"background:rgba(0,0,0,50%);"
		"min-height:20;"
		"}"
		// 这个应该是设置下箭头的，3.png就是箭头
		"QScrollBar::add-line:vertical"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollDown.png);"
		"subcontrol-position:bottom;"
		"}"
		// 设置上箭头
		"QScrollBar::sub-line:vertical"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollUp.png);"
		"subcontrol-position:top;"
		"}"
		// 当鼠标放到下箭头上的时候
		"QScrollBar::add-line:vertical:hover"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollDown2.png);"
		"subcontrol-position:bottom;"
		"}"
		// 当鼠标放到下箭头上的时候
		"QScrollBar::sub-line:vertical:hover"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollUp2.png);"
		"subcontrol-position:top;"
		"}"
		// 当滚动条滚动的时候，上面的部分和下面的部分
		"QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical"
		"{"
		"background:rgba(232,232,236,100%);"
		"}"

		//横向
		"QScrollBar:horizontal"
		"{"
		"height:22px;"
		"background:rgba(0,0,0,0%);"
		"margin:0px,0px,0px,0px;"
		"padding-top:5px;"
		"padding-bottom:5px;"
		"padding-right:18px;"
		"padding-left:18px;"
		"background:rgba(232,232,236,100%);"
		"}"
		// 滚动条两端变成椭圆
		"QScrollBar::handle:horizontal"
		"{"
		"height:8px;"
		"background:rgba(0,0,0,25%);"
		"min-width:10;"
		"max-height:20;"
		"}"
		// 鼠标放到滚动条上的时候，颜色变深
		"QScrollBar::handle:horizontal:hover"
		"{"
		"height:8px;"
		"background:rgba(0,0,0,50%);"
		"min-width:20;"
		"}"
		// 设置左箭头的，3.png就是箭头
		"QScrollBar::sub-line:horizontal"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollLeft.png);"
		"subcontrol-position:left;"
		"}"
		// 设置右箭头
		"QScrollBar::add-line:horizontal"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollRight.png);"
		"subcontrol-position:right;"
		"}"
		// 当鼠标放到左箭头上的时候
		"QScrollBar::sub-line:horizontal:hover"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollLeft2.png);"
		"subcontrol-position:left;"
		"}"
		// 当鼠标放到右箭头上的时候
		"QScrollBar::add-line:horizontal:hover"
		"{"
		"height:18px;width:18px;"
		"border-image:url(../icons/StrollRight2.png);"
		"subcontrol-position:right;"
		"}"
		// 当滚动条滚动的时候，上面的部分和下面的部分
		"QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal"
		"{"
		"background:rgba(232,232,236,100%);"
		"}"
	);
}

void NCEditDlg::SaveStrToFile(QString fpText, QString fpFilePath)
{
	QFile file(fpFilePath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

	QTextStream out(&file);
	out << fpText << endl;
	file.close();

}
